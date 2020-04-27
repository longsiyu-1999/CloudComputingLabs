#include<stdio.h>
#include<unistd.h>
#include<getopt.h>

int main(int argc, char *argv[])
{
    int opt;
    int digit_optind = 0;
    int option_index = 0;
    char *string = "a:b:d";
//单个字符为参数选项
//选项字符后面跟':'，表示该选项必须具备参数，参数与选项之间以空格分隔
//选项字符后面跟'::',表示该选项必须具备参数，参数紧跟选项，二者连接在一起

    static struct option long_options[] =
    {  
        {  "ip",required_argument,NULL,'r'},
        {"port",required_argument,NULL,'r'},
        {    NULL,                 0,NULL,  0},
    };
//name：选项字符串
//has_args：三种参数类型，no_argument表示无参数(0)，required_argument表示需要参数(1)，optional_argument表示参数可选(2)
//flag：如果为NULL，getopt_long()返回该结构val字段中的数值；如果不为NULL，getopt_long()会使得它所指向的变量中填入val字段中的数值，并且getopt_long()返回0；通常flag设置为NULL，val设置为与该长选项对应的短选项
//val：发现了长选项时的返回值，或者flag不是NULL时载入*flag中的值。典型情况下，若flag不是NULL，那么val是个真/假值，譬如1 或0；另一方面，如果flag是NULL，那么val通常是字符常量，若长选项与短选项一致，那么该字符常量应该与optstring中出现的这个选项的参数相同。 
    while((opt =getopt_long_only(argc,argv,string,long_options,&option_index))!= -1)
    {  
        printf("opt = %c\t\t",        opt);
        printf("optarg = %s\t\t",     optarg);
        printf("optind = %d\t\t",     optind);
        printf("argv[optind] =%s\t\t",argv[optind]);
        printf("option_index = %d\n", option_index);
    }  
}