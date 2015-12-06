GOAL=$(MAKECMDGOALS)

$(GOAL): $(GOAL).as 
	nasm -f elf $(GOAL).as
	gcc -m32 $(GOAL).o $(GOAL).c
#ld -m elf_i386 -lc -dynamic-linker /lib/ld-linux.so.2 -o $(GOAL) $(GOAL).o
