linux:
	make -s -C main linux
	make -s -C wrap linux
mac:
	make -s -C main mac
	make -s -C wrap mac
win+api:
	make -s -C main win
	make -s -C wrap1 win
	make -s -C wrap win+api
win+sdl:
	make -s -C main win
	make -s -C wrap1 win
	make -s -C wrap win+sdl
binary:
	make -s -C main binary
	make -s -C wrap1 binary
	make -s -C wrap binary
crosscompile:
	make -s -C main crosscompile
	make -s -C wrap1 crosscompile
	make -s -C wrap crosscompile
qemu:
	cp ../../../core/core.bin test.img    #first64k
	dd if=color.bin of=test.img bs=65536 seek=1 conv=notrunc    #real bin
	echo -n 9 | dd of=test.img bs=1 seek=1048575 conv=notrunc    #fill 0
	echo "test" | dd of=test.img bs=1 seek=448 conv=notrunc    #test sign
	../../../tool/qemu $(shell pwd)/test.img
kvm:
	cp ../../../core/core.bin test.img    #first64k
	dd if=color.bin of=test.img bs=65536 seek=1 conv=notrunc    #real bin
	echo -n 9 | dd of=test.img bs=1 seek=1048575 conv=notrunc    #fill 0
	echo "test" | dd of=test.img bs=1 seek=448 conv=notrunc    #test sign
	../../../tool/kvm $(shell pwd)/test.img
clean:
	rm -f */*.o */*.a *.bin *.out *.exe
