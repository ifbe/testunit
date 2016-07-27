#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifndef O_BINARY
	//mingw64 compatiable
	#define O_BINARY 0x0
#endif




//c
int c_explain(int,int);
int c_start(char*,int);
int c_stop(int);
int c_init(char*,char*);
int c_kill();
//cpp
int cpp_explain(int,int);
int cpp_start(char*,int);
int cpp_stop(int);
int cpp_init(char*,char*);
int cpp_kill();
//class
int class_explain(int,int);
int class_start(char*,int);
int class_stop(int);
int class_init(char*,char*);
int class_kill();
//none(example)
int none_explain(int,int);
int none_start(char*,int);
int none_stop(int);
int none_init(char*,char*);
int none_kill();
//dts
int dts_explain(int,int);
int dts_start(char*,int);
int dts_stop(int);
int dts_init(char*,char*);
int dts_kill();
//go
int go_explain(int,int);
int go_start(char*,int);
int go_stop(int);
int go_init(char*,char*);
int go_kill();
//include
int include_explain(int,int);
int include_start(char*,int);
int include_stop(int);
int include_init(char*,char*);
int include_kill();
//java
int java_explain(int,int);
int java_start(char*,int);
int java_stop(int);
int java_init(char*,char*);
int java_kill();
//js
int js_explain(int,int);
int js_start(char*,int);
int js_stop(int);
int js_init(char*,char*);
int js_kill();
//json
int json_explain(int,int);
int json_start(char*,int);
int json_stop(int);
int json_init(char*,char*);
int json_kill();
//perl
int perl_explain(int,int);
int perl_start(char*,int);
int perl_stop(int);
int perl_init(char*,char*);
int perl_kill();
//php
int php_explain(int,int);
int php_start(char*,int);
int php_stop(int);
int php_init(char*,char*);
int php_kill();
//python
int python_explain(int,int);
int python_start(char*,int);
int python_stop(int);
int python_init(char*,char*);
int python_kill();
//ruby
int ruby_explain(int,int);
int ruby_start(char*,int);
int ruby_stop(int);
int ruby_init(char*,char*);
int ruby_kill();
//struct
int struct_explain(int,int);
int struct_start(char*,int);
int struct_stop(int);
int struct_init(char*,char*);
int struct_kill();
