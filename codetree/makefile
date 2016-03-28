.PHONY:all
all:
	################code2seed##################
	gcc code2seed.c \
	explain/purec.c \
	explain/cpp.c \
	explain/struct.c \
	-o code2seed.exe
	################seed2hash####################
	gcc seed2hash.c -o seed2hash.exe
	################seed+hash=tree####################
	gcc hash2tree.c -o hash2tree.exe
.PHONY:fun
fun:
	################seed2table2txt##################
	gcc fun/seed2table.c -o seed2table.exe
	gcc fun/table2txt.c -o table2txt.exe
install:
	-cp code2seed.exe /opt/bin/
	-cp seed2hash.exe /opt/bin/
	-cp hash2tree.exe /opt/bin/
uninstall:
	-rm -f /opt/bin/code2seed.exe
	-rm -f /opt/bin/seed2hash.exe
	-rm -f /opt/bin/hash2tree.exe
clean:
	rm -f *.exe *.out
	rm -f *.seed *.tree
