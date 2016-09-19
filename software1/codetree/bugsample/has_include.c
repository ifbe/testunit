#if __has_include("stdint.h)
#endif

// expected-error@+1 {{expected "FILENAME" or <FILENAME>}} expected-warning@+1 {{missing terminating '"' character}} expected-error@+1 {{token is not a valid binary operator in a preprocessor subexpression}}
#if __has_include(stdint.h")
#endif
