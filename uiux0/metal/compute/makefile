all:compute.out
	swiftc main.swift -o a.out
%.air: %.metal
	xcrun -sdk macosx metal -O2 -std=osx-metal1.1 -c -o $@ $<

%.metal-ar: %.air
	xcrun -sdk macosx metal-ar r $@ $<

%.out: %.metal-ar
	xcrun -sdk macosx metallib -o $@ $<
