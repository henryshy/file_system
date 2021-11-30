#include "myfs.h"

u8_t* my_vdrive; //虚拟磁盘起始地址
useropen open_file_list[MAX_OPEN_FILE];
int curfd;
u8_t current_dir[80];
u8_t* data_start_ptr;  //数据区起始地址
u8_t buff[vDRIVE_SIZE];  //文件系统缓冲区
void parse_command()
{

}
void startsys()
{
    my_vdrive=(u8_t*) calloc(1,vDRIVE_SIZE); //内存空间
    FILE * fp= fopen(FILENAME,"rw");
    if(fp!=NULL)
    {
        fread(buff,vDRIVE_SIZE,1,fp);
        fclose(fp);
        if(memcmp(buff,"myownsys",8)==0)
        {
            memcpy(my_vdrive,buff,vDRIVE_SIZE);
            printf("starting file system\n");
        }
        else
        {
            printf("first loading drive, starting format\n");
            my_format();
            memcpy(my_vdrive,buff,vDRIVE_SIZE);
        }
    }
    else
    {
        printf("first loading drive, starting format\n");
        my_format();
        memcpy(my_vdrive,buff,vDRIVE_SIZE);
    }

    fcb* root_fcb;
    root_fcb=(fcb*)(my_vdrive+5*BLOCK_SIZE);
    strcpy(open_file_list[0].filename,root_fcb->filename);
    strcpy(open_file_list[0].exname,root_fcb->exname);
    open_file_list[0].time=root_fcb->time;
    open_file_list[0].date=root_fcb->date;
    open_file_list[0].first_block=root_fcb->first_block;
    open_file_list[0].length=root_fcb->length;

    strcpy(open_file_list[0].dir,"/");
    open_file_list[0].rw_ptr=0;
    open_file_list[0].fcbstate = 0;
    open_file_list[0].topenfile = 1;
    open_file_list[0].attribute=0;
    data_start_ptr=((block0*)my_vdrive)->startblock_ptr;
    curfd=0;

}
void exitsys()
{

}
void  my_format()
{
    block0* boot_block=(block0*)my_vdrive;

    strcpy(boot_block->id,"myownsys");
    strcpy(boot_block->information,"123123123");
    boot_block->root=5;
    boot_block->startblock_ptr=getPtr_of_vDrive(6);

    fat* fat1;
    fat* fat2;

    fat1=(fat*)FAT1_PTR;
    for(int i=0;i<5;i++)
    {
        fat1[i].index=END_OF_FILE;
    }
    for(int i=5;i<1000;i++)
    {
        fat1[i].index=FREE_BLOCK;
    }

    fat2=(fat*)FAT2_PTR;
    for(int i=0;i<5;i++)
    {
        fat2[i].index=END_OF_FILE;
    }
    for(int i=5;i<1000;i++)
    {
        fat2[i].index=FREE_BLOCK;
    }

    fat1[5].index=END_OF_FILE;
    fat2[5].index=END_OF_FILE;

    fcb *root1 = (fcb*)getPtr_of_vDrive(6);
    strcpy(root1->filename, ".");
    strcpy(root1->exname, "dir");

    time_t rawTime = time(NULL);
    struct tm *time = localtime(&rawTime);

    root1->time = time->tm_hour * 2048 + time->tm_min * 32 + time->tm_sec / 2;
    root1->date = (time->tm_year - 100) * 512 + (time->tm_mon + 1) * 32 + (time->tm_mday);
    root1->first_block = 5;
    root1->attribute = 1;
    root1->length = 2 * sizeof(fcb);

    //root2 指向根目录区的第二个fcb,即特殊目录项..,因为根目录区没有上级目录,所以指向自己
    fcb* root2 = root1 + 1;
    memcpy(root2, root1, sizeof(fcb));
    strcpy(root2->filename, "..");

    FILE *fp = fopen(FILENAME, "w");
    fwrite(my_vdrive, vDRIVE_SIZE, 1, fp);
    fclose(fp);

}

int my_cd(char* dirname)
{
    if(open_file_list[curfd].attribute==1){
        printf("can not cd in data file, cd fail!\n");
        return -1;
    }
    char* cd_dir=(char*) calloc(1,80); //输入的文件路径
    char* absolute_dir=(char*)  calloc(1,80); //文件的实际绝对路径
    char* name_before_point=(char*)  calloc(1,80); //去除.和后缀名
    char* exname=(char*)  calloc(1,80);//后缀名
    char* filename=(char*)  calloc(1,80);//文件名

    int ret= name_split(dirname,cd_dir,name_before_point,exname,filename,DIR_FILE_NAME);
    if(ret==-1){
        printf("cd fail!\n");
        return -1;
    }

    fcb* fcb_buff=(fcb*) calloc(1,MAX_TEXT_SIZE);

    if(dirname[0]='/'){

    }
    else
    {

    }
}
void my_mkdir(char* dirname)
{

}
void my_rmdir()
{

}
void my_ls()
{

}
int my_create(char* filedir)
{
    if(open_file_list[curfd].attribute == 1){
        printf("can not create in a file!\n");
        return -1;
    }

    char* opendir=(char*) calloc(1,80); //输入的文件路径
    char* dir_and_filename=(char*)  calloc(1,80); //去除.和后缀名
    char* exname=(char*)  calloc(1,80);//后缀名
    char* filename=(char*)  calloc(1,80);//文件名

    int ret=name_split(filedir, opendir, dir_and_filename, exname, filename, DATA_FILE_NAME);
    if(ret==-1){
        printf("create fail!\n");
        return -1;
    }
    if(strcmp(exname,"dir")==0){
        printf("can not create file with extend name \".dir\", create fail!\n");
        return -1;
    }
    if(strcmp(filename,"..")==0){
        printf("invalid filename, create fail!\n");
        return -1;
    }

    fcb* buff=(fcb*) calloc(1,MAX_TEXT_SIZE);

    int dir_lengrh=go_to_dir(dir_and_filename,filename,buff);

    for(int i=0;i<dir_lengrh;i++){
        if(strcmp(buff[i].filename,filename)==0&& strcmp(buff[i].exname,exname)==0){
            printf("file already exists, create fail!\n");
            return -1;
        }
    }
    int fcb_index=0;
    for(int i=0;i<dir_lengrh;i++){
        if(buff[i].free==0){
            fcb_index=i;
            break;
        }
    }
    int free_block=get_free_block();
    if(free_block==-1){
        printf("disk is full, create fail!\n");
        return -1;
    }
    fat* fat1=FAT1_PTR;
    fat* fat2=FAT2_PTR;

    fat1[free_block].index=END_OF_FILE;
    memcpy(fat2,fat1,BLOCK_SIZE*2);

    strcpy(buff[fcb_index].filename,filename);
    strcpy(buff[fcb_index].exname,exname);
    time_t rawtime = time(NULL);
    struct tm* time = localtime(&rawtime);
    buff[fcb_index].date = (time->tm_year-100)*512 + (time->tm_mon+1)*32 + (time->tm_mday);
    buff[fcb_index].time = (time->tm_hour)*2048 + (time->tm_min)*32 + (time->tm_sec) / 2;
    buff[fcb_index].first_block = free_block;
    buff[fcb_index].free = 1;
    buff[fcb_index].length = 0;
    buff[fcb_index].attribute = 1;
    open_file_list[curfd].length+=sizeof (fcb);

    do_write(curfd,(char*)(buff+fcb_index),sizeof(fcb),1);

    open_file_list[curfd].rw_ptr=0;
    open_file_list[curfd].fcbstate=1;

}
void my_rm()
{

}
int check_name(char* name,int length){
    for(int i=0;i<length-1;i++){
        if(name[i]==':'||name[i]=='?'||name[i]=='$'||name[i]==' '){
            return -1;
        }
    }
    return 1;
}
int name_split(char* filedir,char* opendir,char* dir_and_filename,char* exname,char* filename,int flag)
{
    int check;
    check=check_name(filedir, strlen(filedir));
    if(check==-1){
        printf("invalid char detected!\n");
        return -1;
    }
    int point_index= strlen(filedir)-1;
    int ex_length=0;
    int mark=0;

    while(point_index>=0){
        char cur_char=opendir[point_index];
        if(cur_char=='/'){
            mark=1;
            break;
        }
        if(cur_char=='.'){
            break;
        }
        else
        {
            point_index--;
            ex_length++;
        }
    }
    if(flag == DATA_FILE_NAME){
        if(point_index<=0||point_index>=strlen(filedir)-1||mark==1){
            printf("format error!\n");
            return -1;
        }
        else{
            memcpy(dir_and_filename,filedir,point_index);
            memcpy(exname,filedir+point_index+1,ex_length);
        }
    }
    else if(flag == DIR_FILE_NAME){
        if(ex_length>0){
            printf("format error!\n");
            return -1;
        }
    }
    int big_length=strlen(dir_and_filename);
    int name_index=big_length-1;
    int name_length=0;
    while(name_index>=0){

        if(dir_and_filename[name_index]!='/'){
            name_index--;
            name_length++;
        }
        else
        {
            break;
        }
    }
    memcpy(filename,dir_and_filename+name_index+1,name_length);
    return 1;
}
int go_to_dir(char* dir_and_filename,char* filename,fcb* fcb_buff){
    int dir_length;
    int start_block;

    if(dir_and_filename[0]=='/') { //如果是绝对路径
        start_block=ROOT_BLOCK_INDEX;
        dir_length=MAX_BLOCK_FCB_NUM;
    }
    else{
        start_block=open_file_list[curfd].first_block;
        dir_length=open_file_list[curfd].length;
    }

    char* dir= strtok(dir_and_filename,"/");
    int fcb_index=-1;

    while(strcmp(dir,filename)!=0){  //定位到文件目录

        do_read(0,start_block,dir_length,fcb_buff);

        for(int i=0; i < dir_length ; i++){ //在fcb列表中找到目录fcb
            if(strcmp(fcb_buff[i].filename,dir)==0&&strcmp(fcb_buff[i].exname,"dir")==0&&fcb_buff[i].attribute==0){
                fcb_index=i;
                break;
            }
        }
        if(fcb_index==-1){ //没找到目录文件的fcb
            printf("no such file!\n");
            return 0;
        }
        else{
            dir_length=fcb_buff[fcb_index].length;
            start_block=fcb_buff[fcb_index].first_block;
        }
        dir= strtok(NULL,"/");

        memset(fcb_buff,0,MAX_TEXT_SIZE);
    }
    return dir_length;
}
int my_open(char* filedir)
{

    char* opendir=(char*) calloc(1,80); //输入的文件路径
    char* absolute_dir=(char*)  calloc(1,80); //文件的实际绝对路径
    char* dir_and_filename=(char*)  calloc(1,80); //去除.和后缀名
    char* exname=(char*)  calloc(1,80);//后缀名
    char* filename=(char*)  calloc(1,80);//文件名

    int ret=name_split(filedir, opendir, dir_and_filename, exname, filename, DATA_FILE_NAME);
    if(ret==-1){
        printf("open fail!\n");
        return -1;
    }
    if(strcmp(filename,"..")==0){
        printf("invalid filename, open fail!\n");
        return -1;
    }

    fcb* fcb_buff=(fcb*) calloc(1,MAX_TEXT_SIZE);


    if(strcmp(exname,"dir")){
        printf("can not open file with extend name \".dir\", open faile!\n");
        return -1;
    }

    int dir_length=go_to_dir(dir_and_filename,filename,fcb_buff);

    if(dir_and_filename[0]=='/') { //如果是绝对路径
        strncat(absolute_dir,dir_and_filename,strlen(dir_and_filename));
    }
    else{
        strcpy(absolute_dir,open_file_list[curfd].dir);
        strcat(absolute_dir,"/");
        strncat(absolute_dir,opendir,strlen(opendir));
    }
    //定位完了，此时从dir_buff中查找filename  dir_buff就是需要打开的文件的目录文件的所有内容
    int file_index;
    for(int i=0;i<dir_length;i++){
        if(strcmp(fcb_buff[i].filename,filename)==0&& strcmp(fcb_buff[i].exname,exname)==0&&fcb_buff[i].attribute==1){
            file_index=i;
            break;
        }
    }
    int new_fd=get_free_fd();
    if(new_fd==-1){
        printf("can not open, open file list is full!\n");
        return 0;
    }
    else{
        strcpy(open_file_list[new_fd].filename,fcb_buff[file_index].filename);
        strcpy(open_file_list[new_fd].exname,fcb_buff[file_index].exname);
        open_file_list[new_fd].time=fcb_buff[file_index].time;
        open_file_list[new_fd].date=fcb_buff[file_index].date;
        open_file_list[new_fd].first_block=fcb_buff[file_index].first_block;
        open_file_list[new_fd].length=fcb_buff[file_index].length;
        strcpy(open_file_list[new_fd].dir,absolute_dir);
        open_file_list[new_fd].attribute=1;
        open_file_list[new_fd].rw_ptr=0;
        open_file_list[new_fd].fcbstate=0;
        open_file_list[new_fd].topenfile=1;
    }
    free(opendir);
    free(absolute_dir);
    free(fcb_buff);
    free(exname);
    free(filename);
    free(dir_and_filename);
}
int get_free_fd(){
    for(int i=0;i<MAX_OPEN_FILE;i++){
        if(open_file_list[i].topenfile==0){
            return i;
        }
    }
    return -1;
}
void my_close()
{

}
int my_write(int fd)
{
    if(fd < 0 || fd >= MAX_OPEN_FILE){
        printf("file not found, write fail!\n");
        return -1;
    }
    if(open_file_list[fd].topenfile==0){
        printf("file not opened, write fail!\n");
        return -1;
    }
    printf("waiting command for write method:\n1:截断写\n2:覆盖写\n3:追加写\n");
    char method[10]={'\0'};
    int write_method=0;
    scanf("%d",&write_method);

    if(write_method<0|write_method>3){
        printf("write method not found, write fail!\n");
        return -1;
    }
    char* write_txt=(char*) calloc(1,MAX_TEXT_SIZE);
    char write_length;

    printf("input text:\n");
    fgets(write_txt,MAX_TEXT_SIZE,stdin);
    write_length= strlen(write_txt);

    do_write(fd,write_txt,write_length,write_method);

    free(write_txt);
    return write_length;
}

int do_write(int fd, char* text, int tot_len, int write_method)
{
    u32_t write_ptr;
    fat* fat1=FAT1_PTR;
    int offset=open_file_list[fd].rw_ptr;
    int write_block=open_file_list[fd].first_block;
    switch (write_method) {
        case 1:
            open_file_list[fd].length=0;
            open_file_list[fd].rw_ptr=0;
            write_ptr=0;
            break;
        case 2:
            while(offset>=BLOCK_SIZE){
                offset-=BLOCK_SIZE;
                write_block=fat1[write_block].index;
            }
            write_ptr=offset;

            break;
        case 3:
            while (fat1[write_block].index != END_OF_FILE){
                write_block=fat1[write_block].index;
            }
            write_ptr=(open_file_list[fd].rw_ptr+open_file_list[fd].length)%BLOCK_SIZE-1;
            break;
    }

    int write_len=0;
    int textptr=0;
    char* buff=(char*) calloc(1,BLOCK_SIZE);
    while(tot_len > 0)
    {

        memcpy(buff,write_block,BLOCK_SIZE);
        if(tot_len + write_ptr > BLOCK_SIZE) {
            write_len = BLOCK_SIZE - write_ptr;
            tot_len -= write_len;
            memcpy(buff+write_ptr,text+textptr, write_len);
            textptr+=write_len;
            write_ptr=0;
            memcpy(write_block,buff,BLOCK_SIZE);
            if (fat1[write_block].index == END_OF_FILE)
            {
                int new_block;
                new_block = get_free_block();
                if (!new_block) {
                    printf("no free block, drive is full,fail to write!\n");
                    return 0;
                }
                fat1[write_block].index=new_block;
                fat1[new_block].index=END_OF_FILE;
            }
            write_block=fat1[write_block].index;
        }
        else
        {
            write_len=tot_len;
            tot_len-=write_len;
            memcpy(buff+write_ptr,text+textptr, tot_len);
            memcpy(write_block,buff,BLOCK_SIZE);
        }
        memset(buff,0,BLOCK_SIZE);
    }
    free(buff);
}

int do_read(int offset,int start_block, int tot_len, char* text)
{
    fat* fat1=(fat*)FAT1_PTR;

    int rw_offset=0;


    while(offset>=BLOCK_SIZE)
    {
        offset-=BLOCK_SIZE;
        start_block=fat1[start_block].index;
    }

    int cur_block=start_block;
    int textptr=0;
    int copy_len;
    char* buff=(char*) calloc(1,BLOCK_SIZE);
    while(tot_len > 0){

        if(fat1[cur_block].index != END_OF_FILE){
            memcpy(buff,(char*)getPtr_of_vDrive(cur_block),BLOCK_SIZE);
            cur_block=fat1[cur_block].index;
            copy_len=BLOCK_SIZE-offset;
            memcpy(text+textptr,buff+offset,copy_len);
            textptr+=copy_len;
            tot_len-=copy_len;
            offset=0;
        }
        else
        {
            memcpy(buff,(char*)getPtr_of_vDrive(cur_block),BLOCK_SIZE);
            copy_len=tot_len;
            memcpy(text+textptr,buff+offset,copy_len);
            textptr+=copy_len;
            tot_len-=copy_len;
            offset=0;
        }
        rw_offset+=copy_len;
        memset(buff,0,BLOCK_SIZE);
    }
    free(buff);
    return rw_offset;
}
int my_read(int fd,int len)
{
    if(fd >= MAX_OPEN_FILE || fd < 0){
        printf("file not found!\n");
        return -1;
    }
    open_file_list[fd].rw_ptr = 0;
    char text[MAX_TEXT_SIZE] = "\0";

    if(len > open_file_list[fd].length)
    {
        len = open_file_list[fd].length;
    }
    int rw_offset=do_read(open_file_list[fd].rw_ptr,open_file_list[fd].first_block,len,text);
    open_file_list[fd].rw_ptr+=rw_offset;
    printf("%s\n",text);
    return 1;
}
int get_free_block()
{
    fat* fat1=FAT1_PTR;
    int i;
    for(i=6;i<1000;i++){
        if(fat1[i].index=FREE_BLOCK){
            break;
        }
    }
    if(i!=1000)
    {
        return i;
    }
    else{
        return -1;
    }
}

