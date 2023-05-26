#include <stdio.h>
#include <memory.h>
#include <stddef.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <elf.h>

int main(int argc, char* argv[])
{
	// 参数错误
	if (argc < 2)
	{
		printf("invalid arguments\n");
		exit(0);
	}

	// 打开文件
	FILE *fp;
	fp = fopen(argv[1], "r");
	if (NULL == fp)
	{
		printf("fail to open the file");
		exit(0);
	}

	// 解析head
	Elf64_Ehdr elf_head;
	int shnum, a;

	// 读取 head 到elf_head
	a = fread(&elf_head, sizeof(Elf64_Ehdr), 1, fp);   //fread参数1：读取内容存储地址，参数2：读取内容大小，参数3：读取次数，参数4：文件读取引擎
	if (0 == a)
	{
		printf("fail to read head\n");
		exit(0);
	}

	// 判断elf文件类型
	if (elf_head.e_ident[0] != 0x7F ||
			elf_head.e_ident[1] != 'E' ||
			elf_head.e_ident[2] != 'L' ||
			elf_head.e_ident[3] != 'F')
	{
		printf("Not a ELF file\n");
		exit(0);
	}

	// 解析section 分配内存 section * 数量
	// e_shnum = section num
	Elf64_Shdr *shdr = (Elf64_Shdr*)malloc(sizeof(Elf64_Shdr) * elf_head.e_shnum);
	if (NULL == shdr)
	{
		printf("shdr malloc failed\n");
		exit(0);
	}

	// 设置fp偏移量 offset，e_shoff含义
	// if use SEEK_SET, the offset is calculated from the start of file
	// e_shoff is the offset to section header
	a = fseek(fp, elf_head.e_shoff, SEEK_SET); //fseek调整指针的位置，采用参考位置+偏移量
	if (0 != a)
	{
		printf("\nfaile to fseek\n");
		exit(0);
	}

	// 读取section 到 shdr, 大小为shdr * 数量
	a = fread(shdr, sizeof(Elf64_Shdr) * elf_head.e_shnum, 1, fp);
	if (0 == a)
	{
		printf("\nfail to read section\n");
		exit(0);
	}

	// 重置指针位置到文件流开头
	rewind(fp);

	// 将fp指针移到 字符串表偏移位置处
	fseek(fp, shdr[elf_head.e_shstrndx].sh_offset, SEEK_SET);

	// 第e_shstrndx项是字符串表 定义 字节 长度 char类型 数组
	// Ehdr.e_shstrndx is a index number of shstrtab. that means if
	// the index of shstrtab is 13 in section header,this value is 13
	// so we can get shstrtab addr
	char shstrtab[shdr[elf_head.e_shstrndx].sh_size];
	char *temp = shstrtab;

	// 读取内容
	a = fread(shstrtab, shdr[elf_head.e_shstrndx].sh_size, 1, fp);
	if (0 == a)
	{
		printf("\nfaile to read\n");
	}

	// 遍历查找所有的shstrname
	// remember the sh_name is the offset value from shstrtab
	for (int i = 0; i < elf_head.e_shnum; i++)
	{
		temp = shstrtab;
		temp = temp + shdr[i].sh_name;
		if (strcmp(temp, ".symtab") == 0){
			// first move fp to the start of file
			rewind(fp);
			Elf64_Sym* sym = (Elf64_Sym*) malloc(shdr[i].sh_size);
			printf("节的名称: %s\n", temp);
			printf("节首的偏移: %x\n", shdr[i].sh_offset);
			printf("节的大小: %x\n", shdr[i].sh_size);
		}
		if (strcmp(temp, ".symtab") != 0) continue;//该section名称
		printf("节的名称: %s\n", temp);
		printf("节首的偏移: %x\n", shdr[i].sh_offset);
		printf("节的大小: %x\n", shdr[i].sh_size);
		uint8_t *sign_data=(uint8_t*)malloc(sizeof(uint8_t)*shdr[i].sh_size);
		// 依据此段在文件中的偏移读取出
		fseek(fp, shdr[i].sh_offset, SEEK_SET);
		fread(sign_data, sizeof(uint8_t)*shdr[i].sh_size, 1, fp);
	}
	return 0;
}


