/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>
#include <memory/paddr.h>

#include <elf.h>

void init_rand();
void init_log(const char *log_file);
void init_mem();
void init_difftest(char *ref_so_file, long img_size, int port);
void init_device();
void init_sdb();
void init_disasm(const char *triple);

// adding func for ftrace
typedef struct {
	Elf64_Sym *addr;
	uint64_t size;
	uint64_t num;
} symbol;

typedef struct{
	char name[20];
	uint64_t p_start;
	uint64_t p_end;
} Functable;

typedef struct{
	Functable *func;
	uint64_t fun_num;
	// add more if need
} TraceVar;

typedef struct{
	uint64_t p_start;
	uint64_t p_end;
} ProgramRange;

static TraceVar tracevar = {NULL,0};
static ProgramRange program_range = {0,0};

int ParseElf(int argc, char* ElfFile);
int FuncTableInit(symbol *sym, symbol* strtab,TraceVar *_tracevar);
bool CheckNotInstRange(uint64_t addr);

// adding func for ftrace

static void welcome() {
  Log("Trace: %s", MUXDEF(CONFIG_TRACE, ANSI_FMT("ON", ANSI_FG_GREEN), ANSI_FMT("OFF", ANSI_FG_RED)));
  IFDEF(CONFIG_TRACE, Log("If trace is enabled, a log file will be generated "
        "to record the trace. This may lead to a large log file. "
        "If it is not necessary, you can disable it in menuconfig"));
  Log("Build time: %s, %s", __TIME__, __DATE__);
  printf("Welcome to %s-NEMU!\n", ANSI_FMT(str(__GUEST_ISA__), ANSI_FG_YELLOW ANSI_BG_RED));
  printf("For help, type \"help\"\n");
  //Log("Exercise: Please remove me in the source code and compile NEMU again.");
  //assert(0);
}

#ifndef CONFIG_TARGET_AM
#include <getopt.h>

void sdb_set_batch_mode();

static char *log_file = NULL;
static char *diff_so_file = NULL;
static char *img_file = NULL;
static int difftest_port = 1234;

static long load_img() {
  if (img_file == NULL) {
    Log("No image is given. Use the default build-in image.");
    return 4096; // built-in image size
  }

  FILE *fp = fopen(img_file, "rb");
  Assert(fp, "Can not open '%s'", img_file);

  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);

  Log("The image is %s, size = %ld", img_file, size);

  fseek(fp, 0, SEEK_SET);
  int ret = fread(guest_to_host(RESET_VECTOR), size, 1, fp);
  assert(ret == 1);

  fclose(fp);
  return size;
}

static int parse_args(int argc, char *argv[]) {
  const struct option table[] = {
    {"batch"    , no_argument      , NULL, 'b'},
    {"log"      , required_argument, NULL, 'l'},
    {"diff"     , required_argument, NULL, 'd'},
    {"port"     , required_argument, NULL, 'p'},
    {"help"     , no_argument      , NULL, 'h'},
	{"ftrace"   , required_argument, NULL, 'f'},
    {0          , 0                , NULL,  0 },
  };
  int o;
  // : means it needs args
  while ( (o = getopt_long(argc, argv, "-bhl:f:d:p:", table, NULL)) != -1) {
    switch (o) {
      case 'b': sdb_set_batch_mode(); break;
      case 'p': sscanf(optarg, "%d", &difftest_port); break;
      case 'l': log_file = optarg; break;
      case 'd': diff_so_file = optarg; break;
	  case 'f': ParseElf(argc,optarg); break;
      case 1: img_file = optarg; return 0;
      default:
        printf("Usage: %s [OPTION...] IMAGE [args]\n\n", argv[0]);
        printf("\t-b,--batch              run with batch mode\n");
        printf("\t-l,--log=FILE           output log to FILE\n");
        printf("\t-d,--diff=REF_SO        run DiffTest with reference REF_SO\n");
        printf("\t-p,--port=PORT          run DiffTest with port PORT\n");
        printf("\n");
        exit(0);
    }
  }
  return 0;
}

void init_monitor(int argc, char *argv[]) {
  /* Perform some global initialization. */

  /* Parse arguments. */
  parse_args(argc, argv);

  /* Set random seed. */
  init_rand();

  /* Open the log file. */
  init_log(log_file);

  /* Initialize memory. */
  init_mem();

  /* Initialize devices. */
  IFDEF(CONFIG_DEVICE, init_device());

  /* Perform ISA dependent initialization. */
  init_isa();

  /* Load the image to memory. This will overwrite the built-in image. */
  long img_size = load_img();

  /* Initialize differential testing. */
  init_difftest(diff_so_file, img_size, difftest_port);

  /* Initialize the simple debugger. */
  init_sdb();

#ifndef CONFIG_ISA_loongarch32r
  IFDEF(CONFIG_ITRACE, init_disasm(
    MUXDEF(CONFIG_ISA_x86,     "i686",
    MUXDEF(CONFIG_ISA_mips32,  "mipsel",
    MUXDEF(CONFIG_ISA_riscv32, "riscv32",
    MUXDEF(CONFIG_ISA_riscv64, "riscv64", "bad")))) "-pc-linux-gnu"
  ));
#endif

  /* Display welcome message. */
  welcome();
}
#else // CONFIG_TARGET_AM
static long load_img() {
  extern char bin_start, bin_end;
  size_t size = &bin_end - &bin_start;
  Log("img size = %ld", size);
  memcpy(guest_to_host(RESET_VECTOR), &bin_start, size);
  return size;
}

void am_init_monitor() {
  init_rand();
  init_mem();
  init_isa();
  load_img();
  IFDEF(CONFIG_DEVICE, init_device());
  welcome();
}
#endif


int ParseElf(int argc, char* ElfFile){
  // if argc < 2, means no args transferred to main function!
	printf("filestr:%s\n",ElfFile);
	if (argc < 2)
	{
		printf("invalid arguments\n");
		assert(0);
	}
	FILE *fp;
	fp = fopen(ElfFile, "r");
	if (NULL == fp)
	{
		printf("fail to open the file");
		assert(0);
	}

  // parse the ELfHeader to get basic information of ELF.
  // most importantly, we need to know the Elf_type(32 or 64. But this time we use 64, so if using 32, need to modify this code!) 
  // and section header numbers and section header string tab offset.
	Elf64_Ehdr elf_head;
	int a;

	a = fread(&elf_head, sizeof(Elf64_Ehdr), 1, fp);
	if (0 == a)
	{
		printf("fail to read head\n");
		assert(0);
	}
	// check if is ELF file 
	// this is the magic number
	if (elf_head.e_ident[0] != 0x7F ||
			elf_head.e_ident[1] != 'E' ||
			elf_head.e_ident[2] != 'L' ||
			elf_head.e_ident[3] != 'F')
	{
		printf("Not a ELF file\n");
		assert(0);
	}

	// parse Ehdr for Phdr to know the size of program segment
	
	// comment it since we found .text section is the key to know the size of inst
	//Elf64_Phdr *phdr = (Elf64_Phdr *)malloc(sizeof(Elf64_Phdr) * elf_head.e_phnum);
	//a = fseek(fp, elf_head.e_phoff, SEEK_SET); 
	//a = fread(phdr, sizeof(Elf64_Phdr) * elf_head.e_phnum, 1, fp);
	//if (0 == a)
	//{
	//	printf("\nfail to read ph section\n");
	//	assert(0);
	//}
	//for(int i=0;i<elf_head.e_phnum;i++){
	//	printf("No.%d\n",i);
	//	printf("type:%x, vaddr:%lx, paddr:%lx, fsize:%lu, msize:%lu\n",phdr[i].p_type,phdr[i].p_vaddr,
	//	phdr[i].p_paddr,phdr[i].p_filesz,phdr[i].p_memsz);
	//}

	// e_shnum = section num
	// after we get header info, we know the numbers of Section headers
	// elf_head.e_shnum is the number
	Elf64_Shdr *shdr = (Elf64_Shdr*)malloc(sizeof(Elf64_Shdr) * elf_head.e_shnum);
	if (NULL == shdr)
	{
		printf("shdr malloc failed\n");
		assert(0);
	}
	// e_shoff is the offset to section header
	// if use SEEK_SET, the offset is calculated from the start of file
	a = fseek(fp, elf_head.e_shoff, SEEK_SET); 
	if (0 != a)
	{
		printf("\nfaile to fseek\n");
		assert(0);
	}
	// read all the sections headers' information
	a = fread(shdr, sizeof(Elf64_Shdr) * elf_head.e_shnum, 1, fp);
	if (0 == a)
	{
		printf("\nfail to read section\n");
		assert(0);
	}
	// repoint the fp to the start of the ELF file
	rewind(fp);

	// move the fp to the section header string table
	// elf_head.e_shstrndx is got from ELF header info!! 
	// so section header section could be indexed from this index 
	fseek(fp, shdr[elf_head.e_shstrndx].sh_offset, SEEK_SET);

	// Ehdr.e_shstrndx is a index number of shstrtab. that means if
	// the index of shstrtab is 13 in section header,this value is 13
	// so we can get shstrtab addr
	char shstrtab[shdr[elf_head.e_shstrndx].sh_size];
	char *temp = shstrtab;
	a = fread(shstrtab, shdr[elf_head.e_shstrndx].sh_size, 1, fp);
	if (0 == a)
	{
		printf("\nfaile to read\n");
	}

	// travesally search the shstrname
	// remember the sh_name is the offset value from shstrtab
	// init a struct to store symtab
	symbol sym;
	symbol strtab;
	for (int i = 0; i < elf_head.e_shnum; i++)
	{
		temp = shstrtab;
		temp = temp + shdr[i].sh_name;
		// find symtab
		if (strcmp(temp, ".symtab") == 0){
			// move the fp to the addr of symtab section
			fseek(fp, shdr[i].sh_offset, SEEK_SET);
			sym.addr = (Elf64_Sym*) malloc(shdr[i].sh_size);
			a = fread(sym.addr, sizeof(uint8_t)*shdr[i].sh_size, 1, fp);
			if (0 == a)
			{
				printf("\nfaile to read\n");
			}
			sym.size = shdr[i].sh_size;
			sym.num = sym.size/sizeof(Elf64_Sym);
		}
		// find strtab
		else if (strcmp(temp, ".strtab") == 0) {
			// strtab is different with shstrtab!!!
			// section header strtab is consist of the names of section header
			// while strtab is consist of the names of the elements of .symtab
			// move the fp to the addr of strtab section
			fseek(fp, shdr[i].sh_offset, SEEK_SET);
			strtab.addr = (Elf64_Sym*) malloc(shdr[i].sh_size);
			a = fread(strtab.addr, sizeof(uint8_t)*shdr[i].sh_size, 1, fp);
			if (0 == a)
			{
				printf("\nfaile to read\n");
			}
			strtab.size = shdr[i].sh_size;
			strtab.num = 1;
		}
		// find .text
		else if (strcmp(temp, ".text") == 0) {
			program_range.p_start = shdr[i].sh_addr;
			program_range.p_end = program_range.p_start + shdr[i].sh_size;
		}
	}

	FuncTableInit(&sym,&strtab,&tracevar);

	// free from malloc
	if(shdr != NULL) {
		free(shdr);
		shdr = NULL;
	}
	if(sym.addr != NULL) {
		free(sym.addr);
		sym.addr = NULL;
	}
	if(strtab.addr !=NULL) {
		free(strtab.addr);
		strtab.addr = NULL;
	}
	
	//check!
	for(int i=0;i<tracevar.fun_num;i++){
		printf("No.%d FuncName:%-15s, P_start = %lx, P_end = %lx\n",i,tracevar.func[i].name,
		tracevar.func[i].p_start,tracevar.func[i].p_end);
	}

	return 0;
}


int FuncTableInit(symbol *sym, symbol* strtab,TraceVar* _tracevar){
	uint64_t offset = 0;
	int num_fun = 0;
	for(uint64_t i=0; i< sym->num;i++){
		// type is lower 4 bits in st_info
		// this macro will help extract the info
		if(ELF64_ST_TYPE(sym->addr[i].st_info) == STT_FUNC){
			num_fun++;
		}
	}
	if(num_fun == 0){
		printf("There is no Func symbol!\n");
		return -1;
	}
	Functable *functable = (Functable*)malloc(sizeof(Functable) * num_fun);
	if(functable == NULL){
		printf("Malloc failed when functable\n");
		assert(0);
	}

	num_fun =0;
	for(uint64_t i=0; i< sym->num;i++){
		if(ELF64_ST_TYPE(sym->addr[i].st_info) == STT_FUNC){
			offset = sym->addr[i].st_name;
			strcpy(functable[num_fun].name,(char*)((uint64_t)strtab->addr + offset));
			functable[num_fun].p_start = sym->addr[i].st_value;
			functable[num_fun].p_end = sym->addr[i].st_value + sym->addr[i].st_size;
			num_fun++;
		}
	}

	_tracevar->func = functable;
	_tracevar->fun_num = num_fun;
	return 0;
}

bool CheckNotInstRange(uint64_t addr){
	if(addr >= program_range.p_start && addr < program_range.p_end){
		return 0;
	}
	return 1;
}