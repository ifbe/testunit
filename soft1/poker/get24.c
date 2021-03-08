#include<stdio.h>
static char table[4] = {'+','-','*','/'};
static char* valuetable[15] = {
      0, "A", "2", "3", "4", "5", "6", "7",
    "8", "9","10", "J", "Q", "K"
};
int ascii2value(char* p)
{
    if(('2'<=p[0])&&(p[0]<='9'))return p[0]-'0';
    if('1' == p[0])return 10;
    if('J' == p[0])return 11;
    if('Q' == p[0])return 12;
    if('K' == p[0])return 13;
    if('A' == p[0])return 1;
    return 0;
}
int calc(int a,int b,int symbol)
{
    switch(symbol){
        case 0:return a+b;
        case 1:return a-b;
        case 2:return a*b;
        case 3:{
            if(0==(a%b))return a/b;
            else return 0;
        }
    }
    return 0;
}
int checksymbol(int a,int b,int c,int d)
{
    int i,j,k,ret;
    for(i=0;i<4;i++){
        for(j=0;j<4;j++){
            for(k=0;k<4;k++){
                ret = calc(a,b,i);
                if(0 == ret)continue;
                ret = calc(ret,c,j);
                if(0 == ret)continue;
                ret = calc(ret,d,k);
                if(0 == ret)continue;
                if(24 == ret){
                    printf("%s%c%s%c%s%c%s\n",
                       valuetable[a],table[i],
                       valuetable[b],table[j],
                       valuetable[c],table[k],
                       valuetable[d]
                    );
                    return 24;
                }
            }
        }
    }
    return 0;
}
int checkvalue(int a,int b,int c,int d)
{
    //printf("%d,%d,%d,%d",a,b,c,d);
    int ret;
    int x[4] = {a,b,c,d};
    for(a=0;a<4;a++){
        for(b=0;b<4;b++){
            if(a==b)continue;
            for(c=0;c<4;c++){
                if(a==c)continue;
                if(b==c)continue;
                for(d=0;d<4;d++){
                    if(a==d)continue;
                    if(b==d)continue;
                    if(c==d)continue;
                    //printf("%d,%d,%d,%d\n",x[a],x[b],x[c],x[d]);
                    ret = checksymbol(x[a],x[b],x[c],x[d]);
                    if(24==ret)return 24;
                }
            }
        }
    }
    printf("NONE");
    return 0;
}
int main(){
    char a[8]={0};
    char b[8]={0};
    char c[8]={0};
    char d[8]={0};
    scanf("%s %s %s %s",a,b,c,d);
    //printf("%s,%s,%s,%s",a,b,c,d);
    if(('o' == a[0])|('O'==a[0])|
       ('o' == b[0])|('O'==b[0])|
       ('o' == c[0])|('O'==c[0])|
       ('o' == d[0])|('O'==d[0]))
    {
        printf("ERROR");
        return 0;
    }
 
    checkvalue(ascii2value(a),ascii2value(b),ascii2value(c),ascii2value(d));
    return 0;
}
