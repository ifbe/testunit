.PHONY:all
all:
	################code->seed->hash##################
	gcc code2seed.c \
	explain/purec.c \
	explain/cpp.c \
	explain/struct.c \
	explain/header.c \
	-o code2seed.exe
	################seed+hash=tree####################
	gcc seed2tree.c -o seed2tree.exe
.PHONY:fun
fun:
	################seed2table2txt##################
	gcc fun/seed2table.c -o seed2table.exe
	gcc fun/table2txt.c -o table2txt.exe
install:
	-cp code2seed.exe /bin/
	-cp seed2tree.exe /bin/
uninstall:
	-rm -f /bin/code2seed.exe
	-rm -f /bin/seed2tree.exe
clean:
	rm -f *.exe *.out
	rm -f *.seed *.tree
	rm -f *.hash *.table
