#include <proc.h>
#include <elf.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif


// import
size_t ramdisk_read(void *buf, size_t offset, size_t len);
uintptr_t sys_open(uintptr_t name);
uintptr_t sys_read(uintptr_t fd,uintptr_t buf,uintptr_t len);
uintptr_t sys_lseek(uintptr_t fd,uintptr_t offset,uintptr_t whence);
void* new_page(size_t nr_page);



static bool Elf_MagicCheck(Elf64_Ehdr elf_head);
uintptr_t Load_img(const char* ElfFile);


uintptr_t loader(PCB *pcb, const char *filename) {

  uintptr_t ret = Load_img(filename);
  if(ret == 0){
    printf("Load img failed!\n");
    assert(0);
  }
  return ret;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = 0x%lx", entry);
  //Log("Jump to entry = %p", (void *)entry);
  ((void(*)())entry) ();
}


uintptr_t Load_img(const char* ElfFile){

  // open file to get pid
  uintptr_t index = sys_open((uintptr_t)ElfFile);
  // parse the ELfHeader to get basic information of ELF.
  // most importantly, we need to know the Elf_type(32 or 64. But this time we use 64, so if using 32, need to modify this code!) 
  // and section header numbers and section header string tab offset.
	Elf64_Ehdr elf_head;
  // move the pointer to the start of file!
  if(sys_lseek(index,0,0) == -1){
    printf("error!\n");
    assert(0);
  }
  sys_read(index,(uintptr_t)&elf_head,sizeof(elf_head));
  //ramdisk_read( (void *)&elf_head, file_table[index].disk_offset, sizeof(Elf64_Ehdr));
	// check if is ELF file 
	// this is the magic number
  if(false == Elf_MagicCheck(elf_head)) return -1;
	// parse Ehdr for Phdr to know the size of program segment
  // we do not use malloc since malloc and newpage function both use the same heap area!!
  // and they do not communicate!!! so just use newpage func here
	//Elf64_Phdr *phdr = (Elf64_Phdr *)malloc(sizeof(Elf64_Phdr) * elf_head.e_phnum);

	Elf64_Phdr *phdr = (Elf64_Phdr *)new_page(1);
  printf("phdr addr = %lx\n",(uintptr_t)phdr);
  
  // move the pointer to the phoff!
  if(sys_lseek(index,elf_head.e_phoff,0) == -1){
    printf("error!\n");
    assert(0);
  }
  sys_read(index,(uintptr_t)phdr,sizeof(Elf64_Phdr) * elf_head.e_phnum);
  //ramdisk_read( (void *)phdr, file_table[index].disk_offset + elf_head.e_phoff, sizeof(Elf64_Phdr) * elf_head.e_phnum);
  // for now, we get the phdr info
	for(int i=0;i<elf_head.e_phnum;i++){
    if(phdr[i].p_type == PT_LOAD){

      
      /*
      printf("No.%d\n",i);
      printf("type:%d, offset:%lx, vaddr:%lx, paddr:%lx, fsize:%lx, msize:%lx\n",phdr[i].p_type, phdr[i].p_offset ,phdr[i].p_vaddr,
      phdr[i].p_paddr,phdr[i].p_filesz,phdr[i].p_memsz);
      */
      
      uintptr_t p_addr = phdr[i].p_paddr;
      // move the pointer to the offset!
      if(sys_lseek(index,phdr[i].p_offset,0) == -1){
          printf("error!\n");
        assert(0);
      }
      sys_read(index,p_addr,phdr[i].p_filesz);
      //ramdisk_read( (void *)p_addr, file_table[index].disk_offset + phdr[i].p_offset, phdr[i].p_filesz);
      // zero set from filesz to memsz
      memset((void *)(p_addr+phdr[i].p_filesz), 0, phdr[i].p_memsz - phdr[i].p_filesz);

    }
	}

  free(phdr);
  return elf_head.e_entry;

}

static bool Elf_MagicCheck(Elf64_Ehdr elf_head){
	if (elf_head.e_ident[0] != 0x7F ||
			elf_head.e_ident[1] != 'E' ||
			elf_head.e_ident[2] != 'L' ||
			elf_head.e_ident[3] != 'F' ||
      // add to check if ELF64
      elf_head.e_ident[4] != ELFCLASS64	
      )
	{
		printf("Not a ELF-64 file\n");
    return false;
	}
  if(elf_head.e_machine != EM_RISCV	){
		printf("Not a RISCV-ISA\n");
    return false;
  }
  return true;
}