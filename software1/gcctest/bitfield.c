#include<stdio.h>
#define u8 unsigned char
#define u64 unsigned long long
struct leaf1
{
	u64 left:56;
	u64 type:8;
	u64 right:56;
	u64 len:8;
	u64 parent:56;
	u64 lock:8;
	u64 child:56;
	u64 flag:8;
};
struct leaf2
{
	u64 left:56;
	u8 type;
	u64 right:56;
	u8 len;
	u64 parent:56;
	u8 lock;
	u64 child:56;
	u8 flag;
};
void print(u8* buf, int len)
{
	int j;
	for(j=0;j<len;j++)
	{
		printf("%02x ",buf[j]);
		if((j%16)==15)printf("\n");
	}
	printf("\n");
}
int main()
{
	int j;
	u8* buf;
	struct leaf1 l1;
	struct leaf2 l2;
	printf("%lx,%lx\n",sizeof(l1), sizeof(l2));

	l1.type = l2.type = 1;
	l1.len = l2.len = 2;
	l1.lock = l2.lock = 3;
	l1.flag = l2.flag = 4;
	l1.left = l2.left = 0x1122334455667788;
	l1.right = l2.right = 0x33445566778899aa;
	l1.parent = l2.parent = 0x5566778899aabbcc;
	l1.child = l2.child = 0xffeeddccbbaa9988;
	print(&l1, 0x20);
	print(&l2, 0x20);
}
