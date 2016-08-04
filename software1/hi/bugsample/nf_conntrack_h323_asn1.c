#if H323_TRACE
#define TAB_SIZE 4
#define IFTHEN(cond, act) if(cond){act;}
#ifdef __KERNEL__
#define PRINT printk
#else
#define PRINT printf
#endif
#define FNAME(name) name,
#else
#define IFTHEN(cond, act)
#define PRINT(fmt, args...)
#define FNAME(name)
#endif
