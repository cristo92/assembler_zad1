GOAL=$(MAKECMDGOALS)

Zad1: Zad1.c Zad1_test.c zad1_old.c Zad1.as
	nasm -f elf Zad1.as
	gcc -m32 -c -o zad1_old.o zad1_old.c
	gcc -m32 -c -o Zad1_c.o Zad1.c
	gcc -m32 -o Zad1 Zad1_c.o zad1_old.o Zad1.o Zad1_test.c

#$(GOAL): $(GOAL).as 
#	nasm -f elf $(GOAL).as
#	gcc -m32 zad1_old.o $(GOAL).o $(GOAL).c
#ld -m elf_i386 -lc -dynamic-linker /lib/ld-linux.so.2 -o $(GOAL) $(GOAL).o
