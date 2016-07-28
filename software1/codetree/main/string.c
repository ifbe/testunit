#include<stdio.h>




char* eatdir(char* p)
{
        int j=0;
        int k=0;
        while(1)
        {
                if(p[k]==0x0)break;
                if(p[k]=='/')j=k+1;

                k++;
        }
        return p+j;
}
int match(char* first,char* second)
{
        int j=0;
        int k=0;
        while(1)
        {
                if( (first[j]==0) && (second[k]==0) )
                {
                        break;
                }
printf("%c,%c\n",first[j],second[k]);
                if(first[j]=='*')
                {
                        j++;
                        if(first[j]==0)return 1;
                }
                else if(second[k]=='*')
                {
                        k++;
                        if(second[k]==0)return 1;
                }
                else if(
                        (first[j]=='?') |
                        (second[k]=='?') |
                        (first[j]==second[k]) )
                {
                        j++;
                        k++;
                }
                else return 0;
        }

        return 1;
}
