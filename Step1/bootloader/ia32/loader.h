
#ifndef __LOADER__
#define __LOADER__

/* The offset of the end of the partition table.  */
#define STAGE1_PARTEND		0x1fe

/* The signature for bootloader.  */
#define STAGE1_SIGNATURE	0xaa55

#define ARCH_IA32

#define DIRECT_BOOT_EAX_MAGIC	0x6A6F7362
#define	SYSXBOOT_EAX_MAGIC	0x910DFAA0

// Explicitly-sized versions of integer types
typedef __signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;

#define X86_INST_ATTR	static __inline __attribute__((always_inline, no_instrument_function))

X86_INST_ATTR
uint8_t inb(uint16_t port) {
  uint8_t data;
  __asm __volatile("inb %w1,%0" : "=a" (data) : "d" (port));
  return data;
}

X86_INST_ATTR
void insb(uint16_t port, void *addr, int cnt) {
  __asm __volatile("cld\n\trepne\n\tinsb"			:
		   "+D" (addr), "+c" (cnt)		:
		   "d" (port)				:
		   "memory", "cc");
}

X86_INST_ATTR
uint16_t inw(uint16_t port) {
  uint16_t data;
  __asm __volatile("inw %w1,%0" : "=a" (data) : "d" (port));
  return data;
}

X86_INST_ATTR
void insw(uint16_t port, void *addr, int cnt) {
  __asm __volatile("cld\n\trepne\n\tinsw"			:
		   "+D" (addr), "+c" (cnt)		:
		   "d" (port)				:
		   "memory", "cc");
}

X86_INST_ATTR
uint32_t inl(uint16_t port) {
  uint32_t data;
  __asm __volatile("inl %w1,%0" : "=a" (data) : "d" (port));
  return data;
}

X86_INST_ATTR
void insl(uint16_t port, void *addr, int cnt) {
  __asm __volatile("cld\n\trepne\n\tinsl"			:
		   "+D" (addr), "+c" (cnt)		:
		   "d" (port)				:
		   "memory", "cc");
}

X86_INST_ATTR
void outb(uint16_t port, uint8_t data) {
  __asm __volatile("outb %0,%w1" : : "a" (data), "d" (port));
}

X86_INST_ATTR
void outsb(uint16_t port, const void *addr, int cnt) {
  __asm __volatile("cld\n\trepne\n\toutsb"		:
		   "+S" (addr), "+c" (cnt)		:
		   "d" (port)				:
		   "cc");
}

X86_INST_ATTR
void outw(uint16_t port, uint16_t data) {
  __asm __volatile("outw %0,%w1" : : "a" (data), "d" (port));
}

X86_INST_ATTR
void outsw(uint16_t port, const void *addr, int cnt) {
  __asm __volatile("cld\n\trepne\n\toutsw"		:
		   "+S" (addr), "+c" (cnt)		:
		   "d" (port)				:
		   "cc");
}

X86_INST_ATTR
void outsl(uint16_t port, const void *addr, int cnt) {
  __asm __volatile("cld\n\trepne\n\toutsl"		:
		   "+S" (addr), "+c" (cnt)		:
		   "d" (port)				:
		   "cc");
}

X86_INST_ATTR
void outl(uint16_t port, uint32_t data) {
  __asm __volatile("outl %0,%w1" : : "a" (data), "d" (port));
}

#define ELF_MAGIC_BE		0x7F454C46	/* "\x7FELF" in big endian */
#define ELF_MAGIC_LE		0x464C457F	/* "\x7FELF" in little endian */

// e_type values
#define ELF_TYPE_EXEC		2
#define ELF_TYPE_DYNAMIC	3
#define ELF_TYPE_CORE		4

// e_ident indexes
#define EI_CLASS		0
#define EI_DATA			1
#define EI_VERSION		2
#define EI_ABI			3

// e_ident[EI_CLASS]
#define ELF_CLASS_32		1
#define ELF_CLASS_64		2

// e_machine values
#define ELF_MACH_SPARC		2
#define ELF_MACH_386		3
#define ELF_MACH_486		6
#define ELF_MACH_AMD64		62

// p_type values
#define ELF_PROG_LOAD		1
#define ELF_PROG_INTERP		3

// p_flags
#define ELF_PF_X		0x01
#define ELF_PF_W		0x02
#define ELF_PF_R		0x04

// sh_type
#define ELF_SHT_NULL		0
#define ELF_SHT_PROGBITS	1
#define ELF_SHT_SYMTAB		2
#define ELF_SHT_STRTAB		3

// sh_name
#define ELF_SHN_UNDEF		0

// Symbol table index
#define ELF_STN_UNDEF		0

// macros for st_info
#define ELF_ST_BIND(i)		((i) >> 4)
#define ELF_ST_TYPE(i)		((i) & 0xF)
#define ELF_ST_INFO(b, t)	((b) << 4 | ((t) & 0xF))

// auxiliary table entry types
#define ELF_AT_NULL		0		/* End of vector */
#define ELF_AT_IGNORE		1		/* Entry should be ignored */
#define ELF_AT_EXECFD		2		/* File descriptor of program */
#define ELF_AT_PHDR		3		/* Program headers for program */
#define ELF_AT_PHENT		4		/* Size of program header entry */
#define ELF_AT_PHNUM		5		/* Number of program headers */
#define ELF_AT_PAGESZ		6		/* System page size */
#define ELF_AT_BASE		7		/* Base address of interpreter */
#define ELF_AT_FLAGS		8		/* Flags */
#define ELF_AT_ENTRY		9		/* Entry point of program */
#define ELF_AT_NOTELF		10		/* Program is not ELF */
#define ELF_AT_UID		11		/* Real uid */
#define ELF_AT_EUID		12		/* Effective uid */
#define ELF_AT_GID		13		/* Real gid */
#define ELF_AT_EGID		14		/* Effective gid */
#define ELF_AT_CLKTCK		17		/* Frequency of times() */

typedef struct {
	uint32_t e_magic;
	uint8_t e_ident[12];
	uint16_t e_type;
	uint16_t e_machine;
	uint32_t e_version;
	uint32_t e_entry;
	uint32_t e_phoff;
	uint32_t e_shoff;
	uint32_t e_flags;
	uint16_t e_ehsize;
	uint16_t e_phentsize;
	uint16_t e_phnum;
	uint16_t e_shentsize;
	uint16_t e_shnum;
	uint16_t e_shstrndx;
} Elf32_Ehdr;

typedef struct {
	uint32_t p_type;
	uint32_t p_offset;
	uint32_t p_vaddr;
	uint32_t p_paddr;
	uint32_t p_filesz;
	uint32_t p_memsz;
	uint32_t p_flags;
	uint32_t p_align;
} Elf32_Phdr;

typedef struct {
	uint32_t sh_name;
	uint32_t sh_type;
	uint32_t sh_flags;
	uint32_t sh_addr;
	uint32_t sh_offset;
	uint32_t sh_size;
	uint32_t sh_link;
	uint32_t sh_info;
	uint32_t sh_addralign;
	uint32_t sh_entsize;
} Elf32_Shdr;

typedef struct {
	uint32_t st_name;
	uint32_t st_value;
	uint32_t st_size;
	uint8_t st_info;
	uint8_t st_other;
	uint16_t st_shndx;
} Elf32_Sym;

#endif //  __LOADER__
