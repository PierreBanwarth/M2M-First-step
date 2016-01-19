
AS=gcc
ASFLAGS=-m32 -g -Os -fno-builtin -nostdinc -falign-jumps=1 -falign-loops=1 -falign-functions=1

CC=gcc
CFLAGS=-m32 -g -Os \
	-fno-builtin -nostdinc \
	-falign-jumps=1 -falign-loops=1 -falign-functions=1 -fnothrow-opt \
	-fno-stack-protector -fno-builtin-printf -fno-asynchronous-unwind-tables \
	-fno-threadsafe-statics -fno-asynchronous-unwind-tables

LD=ld
LDFLAGS= -nostdlib -N -m elf_i386

OBJCOPY=objcopy

all : tags hda.img
	@echo "  Launch with:"
	@echo "    $> qemu-system-i386 -serial stdio -hda $@ "
	@echo 
	@echo "  Debug with:"
	@echo "    $> qemu-system-i386 -serial stdio -S -s -hda hda.img "
	@echo ""
	@echo "  And with gdb (look at .gdbinit):"
	@echo "    $> gdb boot.elf"
	@echo "  Or with emacs:"
	@echo "    M-x gdb"
	@echo "    Rn gdb (like this): gdb -i=mi boot.elf"

#---------------------------------------------------------------------------
# Run/Debug targets
# Hardware config: one UART and one IDE disk.
#---------------------------------------------------------------------------

run: tags hda.img
	qemu-system-i386 -serial stdio -hda hda.img

dbg: tags hda.img
	qemu-system-i386 -serial stdio -hda hda.img -s -S

#---------------------------------------------------------------------------
# Run/Debug targets
# Hardware config: two UARTs and one IDE disk.
# For debug, uncomment DEBUG.
#---------------------------------------------------------------------------

CHARDEV_COM2=-chardev socket,id=com2,host=127.0.0.1,port=5555,server,telnet 
SERIAL_COM2=-serial chardev:com2
#DEBUG=-s -S
run2: tags hda.img
	qemu-system-i386 -hda hda.img -serial stdio $(CHARDEV_COM2) $(SERIAL_COM2) $(DEBUG)
	 
#---------------------------------------------------------------------------
# Produce the disk image to boot qemu from.
#---------------------------------------------------------------------------

hda.img: boot.mbr kernel.elf
	dd if=/dev/zero of=hda.img bs=512 count=32
	dd if=boot.mbr of=hda.img bs=512 count=1
	dd if=kernel.elf of=hda.img bs=512 seek=1

#---------------------------------------------------------------------------
# Generates Emacs tags
# Only necessary for emacs users.
#---------------------------------------------------------------------------
tags: 
	find . -name "*.[S|c|h]" -exec etags --append \{\} \;

#---------------------------------------------------------------------------
# Generates Master Boot Record (mbr)
#---------------------------------------------------------------------------
boot.mbr: boot.elf 
	$(OBJCOPY) -S -O binary $< $@
	exec echo -n -e "\x55\xAA" | dd of=$@ seek=510 bs=1 conv=notrunc

boot.elf: boot.o loader.o
	$(LD) $(LDFLAGS) -e start -Ttext 0x7c00 -o $@ $+

# Older command formats with earlier GCC toolchains...
#	$(LDL) --verbose -m elf_i386 -e start -Ttext 0x7c00 --oformat binary -o $@ $+
#	$(LDL) -m elf_i386 -e start -Ttext 0x7c00 --oformat binary -b elf32-i386 -o $@ $+

boot.o: boot.S 
	$(AS) -c $(ASFLAGS) -o $@ $<

loader.o: loader.c loader.h
	$(CC) -c $(CFLAGS) -o $@ $<

#----------------------------------------------------------------------
# Compiling and linking a simple kernel, as an ELF executable.
#
# Notice the linking with no standard libraries and as an i386 elf.
# Very important: do not link with the -N option, our simple bootloader 
# would no longer be able to load it. 
#
# The reason why is not understood so far, it is just like this.
# This is a bit confusing since the -N option is necessary to link 
# the bootloader itself. Oh well...
#----------------------------------------------------------------------

kernel.elf: main.o crt0.o 
	$(LD) -nostdlib -m elf_i386 -e crt0 -Tldscript -o $@ $+

crt0.o: crt0.S 
	$(AS) -c $(ASFLAGS) -o $@ $<

main.o: main.c
	$(CC) -c $(CFLAGS) -o $@ $<

#----------------------------------------------------------------------

clean:
	rm -f boot.mbr boot.elf boot.o loader.o console.o crt0.o kernel.elf hda.img
	rm TAGS

