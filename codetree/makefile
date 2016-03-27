all:
	################code2seed##################
	gcc code2seed.c \
	explain/purec.c \
	explain/cpp.c \
	explain/struct.c \
	-o code2seed.exe
	################seed2table####################
	gcc seed2table.c -o seed2table.exe
	################seed2tree####################
	gcc seed2tree.c -o seed2tree.exe
clean:
	rm -f *.exe *.out
	rm -f *.seed *.tree
