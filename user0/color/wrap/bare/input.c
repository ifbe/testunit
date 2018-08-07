#define u64 unsigned long long
#define u8 unsigned char
static u8 translatetable[0x80]={
0x1c,0xd,	//回车
0x1,0x1b,	//esc
0xe,0x8,	//删除
0x39,' ',	//空格
0x0b,'0',
0x02,'1',
0x03,'2',
0x04,'3',
0x05,'4',
0x06,'5',
0x07,'6',
0x08,'7',
0x09,'8',
0x0a,'9',
0x1e,'a',
0x30,'b',
0x2e,'c',
0x20,'d',
0x12,'e',
0x21,'f',
0x22,'g',
0x23,'h',
0x17,'i',
0x24,'j',
0x25,'k',
0x26,'l',
0x32,'m',
0x31,'n',
0x18,'o',
0x19,'p',
0x10,'q',
0x13,'r',
0x1f,'s',
0x14,'t',
0x16,'u',
0x2f,'v',
0x11,'w',
0x2d,'x',
0x15,'y',
0x2c,'z',
0x4e,'+',
0x4a,'-',
0x37,'*',
0x35,'/',
0x1a,'[',
0x1b,']',
0x33,',',
0x34,'.',
0x0d,'=',
0x7e,'~',
0xff,0xff,
};




char convert(char old)
{
    char* p;
    u64 rsi;
    char new=0x20;
    int i;

    rsi=(u64)&translatetable;
    for(i=0;i<51;i++)
    {
        p=(char*)rsi;
        if(*p==old)
        {
            rsi++;
            p=(char*)rsi;
            new=*p;
            break;
        }
        rsi+=2;
    }
    return new;
}




int compare()
{
	if( (*(u64*)0xff8) == (*(u64*)0xfe8) ) return 0;
	return -1;
}
void sleep()
{
	asm("hlt");
}
int waitevent()
{
	//比较，没有收到就继续hlt，等从hlt出来继续比较
	while(1)
	{
		if(compare() != 0) break;
		else sleep();
	}

	//得到dequeue pointer指向的数据
	u64 dequeue=*(u64*)0xff8;
	u8 val=*(u8*)dequeue;

	//处理dequeue pointer
	dequeue+=1;
	if( dequeue >= 0xfe8 ) dequeue=0x800;
	*(u64*)0xff8=dequeue;
	
	//返回翻译之后的数据
	return (int)convert(val);
}