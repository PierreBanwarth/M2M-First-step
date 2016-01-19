/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Author: Olivier Gruber (olivier dot gruber at acm dot org)
*/

#include "loader.h"

#define ELF_EHDR Elf32_Ehdr
#define ELF_PHDR Elf32_Phdr
#define ELF_MACH ELF_MACH_386

#define ELF_OFFSET      512  // second block on disk
#define SECTSIZE	512
#define ELFHDR		((ELF_EHDR *) 0x5000)	// scratch space

static 
void waitdisk(void) {
  // wait for disk reaady
  while ((inb(0x1F7) & 0xC0) != 0x40)
    /* do nothing */ ;
}

static 
void readsect(void *dst, uint32_t offset) {
  // wait for disk to be ready
  waitdisk();
  
  outb(0x1F2, 1);		// count = 1
  outb(0x1F3, offset);
  outb(0x1F4, offset >> 8);
  outb(0x1F5, offset >> 16);
  outb(0x1F6, (offset >> 24) | 0xE0);
  outb(0x1F7, 0x20);		// cmd 0x20 - read sectors
  
  // wait for disk to be ready
  waitdisk();
  
  // read a sector
  insl(0x1F0, dst, SECTSIZE / 4);
}

/*
 * Read 'count' bytes at 'offset' from kernel into virtual address 'va'.
 * Might copy more than asked
 */
static 
void readseg(uint32_t va, uint32_t count, uint32_t offset) {
  uint32_t end_va;
  
  va &= 0xFFFFFF;
  end_va = va + count;
  
  // round down to sector boundary
  va &= ~(SECTSIZE - 1);
  
  // translate from bytes to sectors, and kernel starts at sector 1
  offset = (offset / SECTSIZE);
  
  // If this is too slow, we could read lots of sectors at a time.
  // We'd write more to memory than asked, but it doesn't matter --
  // we load in increasing order.
  while (va < end_va) {
    readsect((uint8_t *) va, offset);
    va += SECTSIZE;
    offset++;
  }
}

uint32_t load_elf() {
  ELF_PHDR *ph;
  int i;

  /*
   * Read the ELF header... 
   * We don't really know how big it is...
   * So let's read an entire sector,
   * hoping it is enough.
   */
  readseg((uint32_t) ELFHDR, SECTSIZE , ELF_OFFSET);
  
  if (ELFHDR->e_magic != ELF_MAGIC_LE ||	/* Invalid ELF */
      ELFHDR->e_machine != ELF_MACH)		/* Wrong machine */
    goto bad;
  
  /*
   * load each program segment (ignores ph flags)
   * where they are supposed to be in memory.
   */
  ph = (ELF_PHDR *) ((uint8_t *) ELFHDR + ELFHDR->e_phoff);
  for (i = ELFHDR->e_phnum; i != 0; i--) {
    readseg(ph->p_vaddr, ph->p_memsz, ph->p_offset + ELF_OFFSET);
    ph = (ELF_PHDR *) ((uint8_t *) ph + ELFHDR->e_phentsize);
    /* Check that ph does not move passed the end of
     * the single sector we read earlier. If it does,
     * we haven't read enough, and it is bad.
     */
    if ((int)ph+sizeof(ELF_PHDR)>((int)ELFHDR+SECTSIZE))
      goto bad;
  }
  return ELFHDR->e_entry & 0xFFFFFF;

 bad:
  outw(0x8A00, 0x8A00);
  outw(0x8A00, 0x8AE0);
  for (;;)
    ;
  return 0; // keep compiler happy.
}

void diskboot(uint32_t _eax, uint32_t _ebx) {
  
  uint32_t entry_point;
  entry_point = load_elf();

  /* 
   * call the entry point from the ELF header, passing in
   * our args.
   */
  uint32_t eax = _eax;
  uint32_t ebx = _ebx;
  uint32_t ecx = entry_point;
  __asm__("jmp *%%ecx": :"a"(eax), "b"(ebx), "c"(ecx));
  
  // note: this function never returns!
}
