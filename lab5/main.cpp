//
//  main.cpp
//  FileSystem
//
//  Created by 张宗灏 on 2019/6/6.
//  Copyright © 2019 张宗灏. All rights reserved.
//

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>

#define MEM_D_SIZE 1024*1024    //总磁盘空间为1M
#define DISKSIZE 1024            //磁盘块的大小1K
#define DISK_NUM 1024            //磁盘块数目1K
#define FATSIZE  DISK_NUM*sizeof(FATitem)    //FAT表大小
#define ROOT_DISK_NO FATSIZE/DISKSIZE+1                //根目录起始盘块号
#define ROOT_DISK_SIZE sizeof(Direct)        //根目录大小
#define DIR_MAXSIZE  1024        //路径最大长度为1KB
#define MSD   5                    //最大子目录数5
#define MOFN  5                    //最大文件深度为5
#define MAX_WRITE 1024*128        //最大写入文字长度128KB

using namespace std;

typedef struct fatitem{  // size 8
    int item;     //存放文件下一个磁盘的指针
    char em_disk; //磁盘块是否空闲标志位 0空闲
}FATitem;

typedef struct direct{
    struct FCB{
        char name[9];   //文件/目录名 8位
        char property;  //属性 1目录 0普通文件
        int size;       //文件/目录字节数、盘块数)
        int firstdisk;  //文件/目录 起始盘块号
        int next;       //子目录起始盘块号
        int sign;       //1是根目录 0不是根目录
        
    }Directitem[MSD+2];
    
}Direct;

struct opentable{
    struct openttableitem
    {
        char name[9]; //文件名
        int firstdisk; //起始盘块号
        int size;  //文件的大小
    }openitem[MOFN];
    int cur_size;  //当前文件的数目
};

FATitem *fat;       //FAT表
Direct *root;       //根目录
Direct *cur_dir;    //当前目录
struct opentable UserOpentable;
int  flag=-1;
char *bufferdir;    //记录当前路径的名称
char *fdisk;        //虚拟磁盘起始地址

void InitFile();
void Format();
void Enter();
void EXIT();
void DISPLAYInstruction();
void Display();

int create(char *name);
int open(char *name);
int close(char *name);
int write(int flag,char *buf,int len);
int read(int flag,char *buf);
int destroy(char *name);

int mkdir(char *name);
int rmdir(char *name);
void dir();
int cd(char *name);


int main(){
    FILE *fp;
    char ch;
    char a[100];
    char code[11][10];
    char name[10];
    int i,flag,r_size;
    char *contect;
    
    contect = (char *)malloc(MAX_WRITE*sizeof(char));
    
    if((fp=fopen("disk.dat","rb"))==NULL){
        cout << "You have not Format,Do you want Format?(y/n)" << endl;
        cout << "Input:";
        cin >> ch;
        
        if(ch=='y'){
            InitFile();
            cout << "Successfully Format! " << endl;
        }
        else
            return 0;
    }
    
    Enter();
    DISPLAYInstruction();
    
    strcpy(code[0],"exit");
    strcpy(code[1],"create");
    strcpy(code[2],"open");
    strcpy(code[3],"close");
    strcpy(code[4],"write");
    strcpy(code[5],"read");
    strcpy(code[6],"destroy");
    strcpy(code[7],"mkdir");
    strcpy(code[8],"rmdir");
    strcpy(code[9],"dir");
    strcpy(code[10],"cd");
    
    while(true){
        Display();
        cin >> a;
        for(i=0;i<11;i++){
            if(!strcmp(code[i],a))
                break;
        }
        
        switch(i){
            case 0: //退出
                free(contect);
                EXIT();
                return 0;
            case 1: //创建文件
                cin >> name;
                flag = create(name);
                if(flag==-1){
                    cout << "Error:"<< endl;
                    cout << " The length is too long !" << endl;
                }
                else if(flag==-2){
                    cout << "Error:"<< endl;
                    cout << " The Direct item is already full !" << endl;
                }
                else if(flag==-3){
                    cout << "Error:"<< endl;
                    cout << " The number of openfile is too much !" << endl;
                }
                else if(flag==-4){
                    cout << "Error:"<< endl;
                    cout << " The name is already in the Direct !" << endl;
                }
                else if(flag==-5){
                    cout << "Error:"<< endl;
                    cout << " The disk space is full!" << endl;
                }
                else{
                    cout << "Successfully create a file! " << endl;
                }
                break;
                
            case 2://打开文件
                cin >> name;
                flag = open(name);
                if(flag == -1){
                    cout << "Error:"<< endl;
                    cout << " The open file not exit!" << endl;
                }
                else if(flag == -2){
                    cout << "Error:"<< endl;
                    cout << " The file have already opened!" << endl;
                }
                else if(flag == -3){
                    cout << "Error:"<< endl;
                    cout << " The number of open file is too much!" << endl;
                }
                else if(flag == -4){
                    cout << "Error:"<< endl;
                    cout << " It is a Direct,can not open for read or write!" << endl;
                }
                else{
                    cout << "Successfully opened! " << endl;
                }
                break;
                
            case 3://关闭文件
                cin >> name;
                flag = close(name);
                if(flag == -1){
                    cout << "Error:"<< endl;
                    cout << " The file is not opened !" << endl;
                }
                else{
                    cout << "Successfully closed!" << endl;
                }
                break;
                
            case 4://写文件
                if(flag ==-1){
                    cout << "Error:"<< endl;
                    cout << " The file is not opened !" << endl;
                }
                else{
                    cout << "Please input:";
                    cin >> contect;
                    flag=write(flag,contect,strlen(contect));
                    if(flag == 0){
                        cout << "Successfully write!" << endl;
                    }
                    else{
                        cout << "Error:"<< endl;
                        cout << " The disk size is not enough!" << endl;
                    }
                }
                break;
                
            case 5://读文件
                if(flag ==-1){
                    cout << "Error:"<< endl;
                    cout << " The file is not opened !" << endl;
                }
                else{
                    flag = read(flag,contect);
                    if(flag == 0){
                            cout << contect[i];
                        cout << contect;
                        printf("\t\n");
                    }
                }
                break;
                
            case 6://删除文件
                cin >> name;
                flag = destroy(name);
                if(flag == -1){
                    cout << "Error:"<< endl;
                    cout << " The file not exit!" << endl;
                }
                else if(flag == -2){
                    cout << "Error:"<< endl;
                    cout << " The file is opened,please first close it !" << endl;
                }
                else if(flag == -3){
                    cout << "Error:"<< endl;
                    cout << " The delete is not file !" << endl;
                }
                else{
                    cout << "Successfully delete!" << endl;
                }
                break;
                
            case 7://创建子目录
                scanf("%s",name);
                flag = mkdir(name);
                if(flag == -1) {
                    cout << "Error:"<< endl;
                    cout << " The length of name is to long!" << endl;
                }
                else if(flag == -2){
                    cout << "Error:"<< endl;
                    cout << " The Direct item is already full !" << endl;
                }
                else if(flag == -3){
                    cout << "Error:"<< endl;
                    cout << " The name is already in the Direct !" << endl;
                }
                else if(flag == -4){
                    cout << "Error:"<< endl;
                    cout << " ..' or '.' can not as the name of the Direct !" << endl;
                }
                else if(flag == -5){
                    cout << "Error:"<< endl;
                    cout << " The disk space is full !" << endl;
                }
                else if(flag == 0){
                    cout << "Successfully make dircet!" << endl;
                }
                break;
                
            case 8://删除子目录
                scanf("%s",name);
                flag = rmdir(name);
                if(flag == -1){
                    cout << "Error:"<< endl;
                    cout << " The Direct is not exist!" << endl;
                }
                else if(flag == -2){
                    cout << "Error:"<< endl;
                    cout << " The Direct has son Direct ,please first remove the son dircct!" << endl;
                }
                else if(flag == -3){
                    cout << "Error:"<< endl;
                    cout << " This is not Direct !" << endl;
                }
                else if(flag == 0){
                    cout << "Successfully remove dircet!" << endl;
                }
                break;
                
            case 9://显示当前子目录
                dir();
                break;
                
            case 10://更改当前目录
                cin >> name;
                flag = cd(name);
                if(flag == -1){
                    cout << "Error:"<< endl;
                    cout << " The path no correct!" << endl;
                }
                else if(flag == -2){
                    cout << "Error:"<< endl;
                    cout << " The opened is not Direct!" << endl;
                }
                
                break;
                
            default:
                cout << "Error:"<< endl;
                cout << " The command is wrong!" << endl;
        }
    }
    return 0;
}

void InitFile(){
    fdisk = (char *)malloc(MEM_D_SIZE*sizeof(char)); //申请内存空间
    Format();
}

void Format(){
    int i;
    FILE *fp;
    
    fat = (FATitem *)(fdisk+DISKSIZE);
    fat[0].item=-1;
    fat[0].em_disk='1';
    
    for(i=1;i<ROOT_DISK_NO-1;i++){
        fat[i].item=i+1;
        fat[i].em_disk='1';
    }
    
    fat[ROOT_DISK_NO].item=-1;
    fat[ROOT_DISK_NO].em_disk='1';
    
    for(i=ROOT_DISK_NO+1;i<DISK_NUM;i++){
        fat[i].item = -1;
        fat[i].em_disk = '0';
    }
    
    root = (Direct *)(fdisk+DISKSIZE+FATSIZE);
    
    root->Directitem[0].sign = 1;
    root->Directitem[0].firstdisk = ROOT_DISK_NO;
    strcpy(root->Directitem[0].name,".");
    root->Directitem[0].next = root->Directitem[0].firstdisk;
    root->Directitem[0].property = '1';
    root->Directitem[0].size = ROOT_DISK_SIZE;
    
    root->Directitem[1].sign = 1;
    root->Directitem[1].firstdisk = ROOT_DISK_NO;
    strcpy(root->Directitem[1].name,"..");
    root->Directitem[1].next = root->Directitem[0].firstdisk;
    root->Directitem[1].property = '1';
    root->Directitem[1].size = ROOT_DISK_SIZE;
    
    if((fp = fopen("disk.dat","wb"))==NULL){
        cout << "Error:"<< endl;
        cout << " Can`t open file!" << endl;
        exit(0);
    }
    for(i=2;i<MSD+2;i++){
        root->Directitem[i].sign = 0;
        root->Directitem[i].firstdisk = -1;
        strcpy(root->Directitem[i].name,"");
        root->Directitem[i].next = -1;
        root->Directitem[i].property = '0';
        root->Directitem[i].size = 0;
    }
    
    
    if((fp = fopen("disk.dat","wb"))==NULL){
        cout << "Error:"<< endl;
        cout << " Can`t open file!" << endl;
        exit(0);
    }
    if(fwrite(fdisk,MEM_D_SIZE,1,fp)!=1){
        cout << "Error:"<< endl;
        cout << " File writing error!" << endl;
    }
    
    fclose(fp);
}

void Enter(){
    FILE *fp;
    int i;
    
    fdisk = (char *)malloc(MEM_D_SIZE*sizeof(char));
    if((fp=fopen("disk.dat","rb"))==NULL){
        cout << "Error:"<< endl;
        cout << " Can`t open file!" << endl;
        return;
    }
    if(!fread(fdisk,MEM_D_SIZE,1,fp)){
        printf("Error:\nCannot read file\n");
        cout << "Error:"<< endl;
        cout << " Can`t read file!" << endl;
        exit(0);
    }
    fat = (FATitem *)(fdisk+DISKSIZE);
    root = (Direct *)(fdisk+DISKSIZE+FATSIZE);
    fclose(fp);
    
    for(i=0;i<MOFN;i++){//初始化用户打开表
        strcpy(UserOpentable.openitem[i].name,"");
        UserOpentable.openitem[i].firstdisk = -1;
        UserOpentable.openitem[i].size = 0;
    }
    UserOpentable.cur_size = 0;
    
    cur_dir = root;
    bufferdir = (char *)malloc(DIR_MAXSIZE*sizeof(char));
    strcpy(bufferdir,"Root:");
}

void EXIT(){
    FILE *fp;
    int i;
    
    if((fp=fopen("disk.dat","wb"))==NULL){
        cout << "Error:"<< endl;
        cout << " Can`t read file!" << endl;
        return;
    }
    if(!fwrite(fdisk,MEM_D_SIZE,1,fp)) {
        cout << "Error:"<< endl;
        cout << " File writing error!" << endl;
    }
    fclose(fp);
    
    free(fdisk);
    free(bufferdir);
    return;
}

int create(char *name){
    int i,j;
    
    if(strlen(name)>8)
        return -1;
    
    for(j=2;j<MSD+2;j++){
        if(!strcmp(cur_dir->Directitem[j].name,name))
            break;
    }
    if(j<MSD+2)
        return -4;
    
    for(i=2;i<MSD+2;i++){
        if(cur_dir->Directitem[i].firstdisk==-1)
            break;
    }
    if(i>=MSD+2)
        return -2;
    if(UserOpentable.cur_size>=MOFN)
        return -3;
    
    
    for(j=ROOT_DISK_NO+1;j<DISK_NUM;j++) {
        if(fat[j].em_disk=='0')
            break;
    }
    if(j>=DISK_NUM)
        return -5;
    fat[j].em_disk = '1';
    
    strcpy(cur_dir->Directitem[i].name,name);
    cur_dir->Directitem[i].firstdisk = j;
    cur_dir->Directitem[i].size = 0;
    cur_dir->Directitem[i].next = j;
    cur_dir->Directitem[i].property = '0';
    
    flag = open(name);
    return 0;
}

int open(char *name){
    int i, j;
    
    for(i=2;i<MSD+2;i++) {
        if(!strcmp(cur_dir->Directitem[i].name,name))
            break;
    }
    if(i>=MSD+2)
        return -1;
    
    if(cur_dir->Directitem[i].property=='1')
        return -4;
    
    for(j=0;j<MOFN;j++){
        if(!strcmp(UserOpentable.openitem[j].name,name))
            break;
    }
    if(j<MOFN)
        return -2;
    
    
    if(UserOpentable.cur_size>=MOFN)
        return -3;
    
    for(j=0;j<MOFN;j++){
        if(UserOpentable.openitem[j].firstdisk==-1)
            break;
    }

    UserOpentable.openitem[j].firstdisk = cur_dir->Directitem[i].firstdisk;
    strcpy(UserOpentable.openitem[j].name,name);
    UserOpentable.openitem[j].size = cur_dir->Directitem[i].size;
    UserOpentable.cur_size++;

    return j;
}

int close(char *name){
    int i;
    
    for(i=0;i<MOFN;i++){
        if(!strcmp(UserOpentable.openitem[i].name,name))
            break;
    }
    if(i>=MOFN)
        return -1;
    
    strcpy(UserOpentable.openitem[i].name,"");
    UserOpentable.openitem[i].firstdisk = -1;
    UserOpentable.openitem[i].size = 0;
    UserOpentable.cur_size--;
    
    return 0;
}

int write(int flag, char *buf, int len){
    char *first;
    int item, i, j, k;
    int ilen1, ilen2, modlen, temp;
    
    char Space = 32;
    char Endter= '\n';
    
    for(i=0;i<len;i++){
        if(buf[i] == '$')
            buf[i] = Space;
        else if(buf[i] == '#')
            buf[i] = Endter;
    }
    
    item = UserOpentable.openitem[flag].firstdisk;
    
    for(i=2;i<MSD+2;i++){
        if(cur_dir->Directitem[i].firstdisk==item)
            break;
    }
    temp = i;

    while(fat[item].item!=-1){
        item =fat[item].item;
    }
    
    first = fdisk+item*DISKSIZE+UserOpentable.openitem[flag].size%DISKSIZE;
    
    if(DISKSIZE-UserOpentable.openitem[flag].size%DISKSIZE>len){
        strcpy(first,buf);
        UserOpentable.openitem[flag].size = UserOpentable.openitem[flag].size+len;
        cur_dir->Directitem[temp].size = cur_dir->Directitem[temp].size+len;
    }
    else{
        for(i=0;i<(DISKSIZE-UserOpentable.openitem[flag].size%DISKSIZE);i++){
            first[i] = buf [i];
        }
        
        ilen1 = len-(DISKSIZE-UserOpentable.openitem[flag].size%DISKSIZE);
        ilen2 = ilen1/DISKSIZE;
        modlen = ilen1%DISKSIZE;
        if(modlen>0)
            ilen2 = ilen2+1;
        
        for(j=0;j<ilen2;j++){
            for(i=ROOT_DISK_NO+1;i<DISK_NUM;i++){
                if(fat[i].em_disk=='0')
                    break;
            }
            if(i>=DISK_NUM)
                return -1;
            first = fdisk+i*DISKSIZE;
            if(j==ilen2-1){
                for(k=0;k<len-(DISKSIZE-UserOpentable.openitem[flag].size%DISKSIZE)-j*DISKSIZE;k++)
                    first[k] = buf[k];
            }
            else{
                for(k=0;k<DISKSIZE;k++)
                    first[k] =buf[k];
            }
            
            fat[item].item = i;
            fat[i].em_disk = '1';
            fat[i].item = -1;
        }
        
        UserOpentable.openitem[flag].size = UserOpentable.openitem[flag].size+len;
        cur_dir->Directitem[temp].size = cur_dir->Directitem[temp].size+len;
    }
    return 0;
}

int read(int flag, char *buf){
    int len = UserOpentable.openitem[flag].size;
    char *first;
    int i, j, item;
    int ilen1, modlen;
    
    item = UserOpentable.openitem[flag].firstdisk;
    
    ilen1 = len/DISKSIZE;
    modlen = len%DISKSIZE;
    if(modlen!=0)
        ilen1 = ilen1+1;
    
    first = fdisk+item*DISKSIZE;
    
    for(i=0;i<ilen1;i++){
        if(i==ilen1-1){
            for(j=0;j<len-i*DISKSIZE;j++)
                buf[i*DISKSIZE+j] = first[j];
        }
        else{
            for(j=0;j<len-i*DISKSIZE;j++)
                buf[i*DISKSIZE+j] = first[j];
            item = fat[item].item;
            first = fdisk+item*DISKSIZE;
        }
    }
    return 0;
}

int destroy(char *name){
    int i,cur_item,item,temp;
    
    for(i=2;i<MSD+2;i++){
        if(!strcmp(cur_dir->Directitem[i].name,name))
            break;
    }
    cur_item = i;
    
    if(i>=MSD+2)
        return -1;
    
    if(cur_dir->Directitem[cur_item].property!='0')
        return -3;
    
    for(i=0;i<MOFN;i++){
        if(!strcmp(UserOpentable.openitem[i].name,name))
            return -2;
    }
    item = cur_dir->Directitem[cur_item].firstdisk;
    while(item!=-1){
        temp = fat[item].item;
        fat[item].item = -1;
        fat[item].em_disk = '0';
        item = temp;
    }
    
    cur_dir->Directitem[cur_item].sign = 0;
    cur_dir->Directitem[cur_item].firstdisk = -1;
    strcpy(UserOpentable.openitem[cur_item].name,"");
    cur_dir->Directitem[cur_item].next = -1;
    cur_dir->Directitem[cur_item].property = '0';
    cur_dir->Directitem[cur_item].size = 0;
    
    return 0;
}

int mkdir(char *name){
    int i,j;
    Direct *cur_mkdir;
    
    if(!strcmp(name,"."))
        return -4;
    if(!strcmp(name,".."))
        return -4;
    if(strlen(name)>8)
        return -1;
    
    for(i=2;i<MSD+2;i++){
        if(cur_dir->Directitem[i].firstdisk==-1)
            break;
    }
    if(i>=MSD+2)
        return -2;
    for(j=2;j<MSD+2;j++) {
        if(!strcmp(cur_dir->Directitem[j].name,name))
            break;
    }
    if(j<MSD+2)
        return(-3);
    for(j=ROOT_DISK_NO+1;j<DISK_NUM;j++){
        if(fat[j].em_disk=='0')
            break;
    }
    if(j>=DISK_NUM)
        return(-5);
    fat[j].em_disk='1';
    
    strcpy(cur_dir->Directitem[i].name,name);
    cur_dir->Directitem[i].firstdisk=j;
    cur_dir->Directitem[i].size=ROOT_DISK_SIZE;
    cur_dir->Directitem[i].next=j;
    cur_dir->Directitem[i].property='1';
    
    cur_mkdir=(Direct *)(fdisk+cur_dir->Directitem[i].firstdisk*DISKSIZE);
    
    cur_mkdir->Directitem[0].sign=0;
    cur_mkdir->Directitem[0].firstdisk=cur_dir->Directitem[i].firstdisk;
    strcpy(cur_mkdir->Directitem[0].name,".");
    cur_mkdir->Directitem[0].next=cur_mkdir->Directitem[0].firstdisk;
    cur_mkdir->Directitem[0].property='1';
    cur_mkdir->Directitem[0].size=ROOT_DISK_SIZE;
    
    cur_mkdir->Directitem[1].sign=cur_dir->Directitem[0].sign;
    cur_mkdir->Directitem[1].firstdisk=cur_dir->Directitem[0].firstdisk;
    strcpy(cur_mkdir->Directitem[1].name,"..");
    cur_mkdir->Directitem[1].next=cur_mkdir->Directitem[1].firstdisk;
    cur_mkdir->Directitem[1].property='1';
    cur_mkdir->Directitem[1].size=ROOT_DISK_SIZE;
    for(i=2;i<MSD+2;i++) {
        cur_mkdir->Directitem[i].sign=0;
        cur_mkdir->Directitem[i].firstdisk=-1;
        strcpy(cur_mkdir->Directitem[i].name,"");
        cur_mkdir->Directitem[i].next=-1;
        cur_mkdir->Directitem[i].property='0';
        cur_mkdir->Directitem[i].size=0;
    }
    return 0;
}

int rmdir(char *name){
    int i,j,item;
    Direct *temp_dir;
    
    for(i=2;i<MSD+2;i++){
        if(!strcmp(cur_dir->Directitem[i].name,name))
            break;
    }
    if(i>=MSD+2)
        return(-1);
    if(cur_dir->Directitem[i].property!='1')
        return(-3);
    
    temp_dir=(Direct *)(fdisk+cur_dir->Directitem[i].next*DISKSIZE);
    for(j=2;j<MSD+2;j++){
        if(temp_dir->Directitem[j].next!=-1)
            break;
    }
    if(j<MSD+2)
        return(-2);
    
    item=cur_dir->Directitem[i].firstdisk;
    fat[item].em_disk='0';
    
    cur_dir->Directitem[i].sign=0;
    cur_dir->Directitem[i].firstdisk=-1;
    strcpy(cur_dir->Directitem[i].name,"");
    cur_dir->Directitem[i].next=-1;
    cur_dir->Directitem[i].property='0';
    cur_dir->Directitem[i].size=0;
    
    return 0;
}

void dir(){
    int i;
    for(i=2;i<MSD+2;i++){
        if(cur_dir->Directitem[i].firstdisk!=-1) {
            printf("%s\t",cur_dir->Directitem[i].name);
            if(cur_dir->Directitem[i].property=='0')
                printf("%d\t\t\n",cur_dir->Directitem[i].size);
            else
                printf("\t<目录>\t\n");
        }
    }
}

int cd(char *name){
    int i,j,item;
    char *str;
    char *temp,*point,*point1;
    Direct *temp_dir;
    temp_dir=cur_dir;
    str=name;
    
    if(!strcmp("\\\\",name)) {
        cur_dir = root;
        strcpy(bufferdir,"Root:");
        return 0;
    }
    
    temp = (char *)malloc(DIR_MAXSIZE*sizeof(char));
    for(i=0;i<(int)strlen(str);i++)
        temp[i]=str[i];
    temp[i]='\0';
    
    for(j=0;j<MSD+2;j++){
        if(!strcmp(temp_dir->Directitem[j].name,temp))
            break;
    }
    
    free(temp);

    if(j>=MSD+2)
        return(-1);
    
    item=temp_dir->Directitem[j].firstdisk;
    
    temp_dir=(Direct *)(fdisk+item*DISKSIZE);
    
    
    if(!strcmp("..",name)){
        if(cur_dir->Directitem[j-1].sign!=1) {
            point=strchr(bufferdir,'\\');
            
            while(point!=NULL){
                point1=point+1;
                point=strchr(point1,'\\');
            }
            *(point1-1)='\0';
        }
        
    }
    
    else{
        bufferdir = strcat(bufferdir,"\\");
        bufferdir = strcat(bufferdir,name);
    }
    cur_dir=temp_dir; 
    return 0;
}

void Display(){
    cout << bufferdir << ">";
}

void DISPLAYInstruction(){
    cout << "*********************************************************" << endl;
    cout << "**********************文件系统说明*************************" << endl;
    cout << "*\t命令格式              说明                            *" << endl;
    cout << "*\tcd 目录名            更改当前目录                      *" << endl;
    cout << "*\tmkdir 目录名         创建子目录                        *" << endl;
    cout << "*\trmdir 目录名         删除子目录                        *" << endl;
    cout << "*\tdir                 显示当前目录的子目录                *" << endl;
    cout << "*\tcreate 文件名        创建文件                          *" << endl;
    cout << "*\tdel 文件名           删除文件                          *" << endl;
    cout << "*\topen 文件名          打开文件                          *" << endl;
    cout << "*\tclose 文件名         关闭文件                          *" << endl;
    cout << "*\tread                读文件                            *" << endl;
    cout << "*\twrite               写文件                            *" << endl;
    cout << "*\texit                退出系统                          *" << endl;
    cout << "*********************************************************" << endl;
    
}



