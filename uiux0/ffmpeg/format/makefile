all:
	gcc test.c -lavformat -lavutil
	install_name_tool -add_rpath /usr/local/Cellar/ffmpeg/7.1_3/lib/ a.out
