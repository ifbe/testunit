.PHONY:all
all:
	################code->seed->hash##################
	gcc code2hashseed.c \
	explain/purec.c \
	explain/cpp.c \
	explain/struct.c \
	-o code2hashseed.exe
	################seed+hash=tree####################
	gcc hashseed2tree.c -o hashseed2tree.exe
.PHONY:fun
fun:
	################seed2table2txt##################
	gcc fun/seed2table.c -o seed2table.exe
	gcc fun/table2txt.c -o table2txt.exe
install:
	-cp code2hashseed.exe /opt/bin/
	-cp hashseed2tree.exe /opt/bin/
uninstall:
	-rm -f /opt/bin/code2hashseed.exe
	-rm -f /opt/bin/hashseed2tree.exe
clean:
	rm -f *.exe *.out
	rm -f *.seed *.tree
