all:
	gcc test.c -lX11
ifeq (${XDG_SESSION_TYPE}, wayland)
	@echo warning: this wont work in wayland, only xorg
endif
