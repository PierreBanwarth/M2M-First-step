
# Under 64bit linux, you need the -m32 flag
# to force the gnu compiler to produce a 32bit executable.
# If your host environment is a 32bit linux, then you may omit that flag.
# However, you must always retain the -static flag.
CFLAGS= -m32 -static
GCC=gcc

all: hello initrd.hello

#	echo Please copy initrd.hello under MiniDist/boot/
#	echo Also copy hello under /MiniDist/
#	echo Finally, make sure that you have an entry in MiniDist/boot/grub/menu.lst for it!

clean:
	rm hello initrd.hello *~

# Produce the most simple root file system, 
# using cpio, and solely containing the hello program.
initrd.hello: hello
	echo hello | cpio -o --format=newc > initrd.hello
	file initrd.hello

# Compile the hello program in order to be able to 
# execute it as the init program launch by a Linux kernel.
hello: hello.c
	$(GCC) $(CFLAGS) -o hello hello.c
	file hello

