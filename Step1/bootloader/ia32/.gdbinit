set non-stop off
symbol boot.elf
br *0x7c00
br protcseg
br diskboot
target remote:1234
