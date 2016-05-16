#define DEVINDEX_HASHBITS 8
#define DEVINDEX_HASHSIZE (1U << DEVINDEX_HASHBITS)
static struct hlist_head fib_info_devhash[DEVINDEX_HASHSIZE];

#ifdef CONFIG_IP_ROUTE_MULTIPATH
u32 fib_multipath_secret __read_mostly;

#define for_nexthops(fi) {						\
	int nhsel; const struct fib_nh *nh;				\
	for (nhsel = 0, nh = (fi)->fib_nh;				\
	     nhsel < (fi)->fib_nhs;					\
	     nh++, nhsel++)

#define change_nexthops(fi) {						\
	int nhsel; struct fib_nh *nexthop_nh;				\
	for (nhsel = 0,	nexthop_nh = (struct fib_nh *)((fi)->fib_nh);	\
	     nhsel < (fi)->fib_nhs;					\
	     nexthop_nh++, nhsel++)

#else /* CONFIG_IP_ROUTE_MULTIPATH */

/* Hope, that gcc will optimize it to get rid of dummy loop */

#define for_nexthops(fi) {						\
	int nhsel; const struct fib_nh *nh = (fi)->fib_nh;		\
	for (nhsel = 0; nhsel < 1; nhsel++)

#define change_nexthops(fi) {						\
	int nhsel;							\
	struct fib_nh *nexthop_nh = (struct fib_nh *)((fi)->fib_nh);	\
	for (nhsel = 0; nhsel < 1; nhsel++)

#endif /* CONFIG_IP_ROUTE_MULTIPATH */

#define endfor_nexthops(fi) }
