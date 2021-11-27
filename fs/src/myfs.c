#include "myfs.h"

u8_t* my_vdrive; //虚拟磁盘起始地址
useropen open_file_list[MAX_OPEN_FILE];
int curdir_fd;
u8_t current_dir[80];
u8_t* data_start_ptr;  //数据区起始地址
u8_t buff[vDRIVE_SIZE];  //文件系统缓冲区
void parse_command()
{

}
void startsys()
{
    my_vdrive=(u8_t*) malloc(vDRIVE_SIZE); //内存空间
    FILE * fp;
    if(fp= fopen(FILENAME,"r")!=NULL)
    {
        fread(buff,vDRIVE_SIZE,1,fp);
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
        my_format(fp);
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

    strcpy(open_file_list[0].dir,"/root");
    open_file_list[0].rw_ptr=0;
    open_file_list[0].fcbstate = 0;
    open_file_list[0].topenfile = 1;

    data_start_ptr=((block0*)my_vdrive)->startblock_ptr;
    curdir_fd=0;


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

    fat1=(block0*)FAT1_PTR;
    for(int i=0;i<5;i++)
    {
        fat1[i].index=FULL_BLOCK;
    }
    for(int i=5;i<1000;i++)
    {
        fat1[i].index=FREE_BLOCK;
    }

    fat2=(block0*)FAT2_PTR;
    for(int i=0;i<5;i++)
    {
        fat2[i].index=FULL_BLOCK;
    }
    for(int i=5;i<1000;i++)
    {
        fat2[i].index=FREE_BLOCK;
    }

    fat1[5].index=FULL_BLOCK;
    fat2[5].index=FULL_BLOCK;

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

void my_cd()
{

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
void my_create()
{

}
void my_rm()
{

}

int my_open(char* filedir)
{
    char* opendir=(char*) malloc(80);
    char* absolute_dir=(char*) malloc(80);

    strcpy(opendir,filedir);
    fcb* fcb_buff[MAX_TEXT_SIZE];
    char* dir_and_filename;
    char* exname;
    char* filename;
    dir_and_filename= strtok(filedir,".");
    exname= strtok(NULL,".");

    int big_length=strlen(dir_and_filename);
    int name_index=big_length;
    while(name_index>0){

        if(dir_and_filename[name_index-1]!='/'){
            name_index--;
        }
        else
        {
            break;
        }
    }
    filename=dir_and_filename+name_index;

    if(strcmp(exname,"dir")){
        printf("can not open directory file!\n");
        return 0;
    }

    int dir_start_block;
    int dir_length;
    int dir_fd;

    if(dir_and_filename[0]=='/') { //如果是绝对路径
        dir_fd=0;
    }
    else{
        dir_fd=curdir_fd;

        strcpy(open_file_list[dir_fd].dir,absolute_dir);
        strncat(absolute_dir,opendir,strlen(opendir));
    }
    dir_length=open_file_list[dir_fd].length;
    dir_start_block=open_file_list[dir_fd].first_block;
    char* dir= strtok(dir_and_filename,"/");
    fat* fat1=FAT1_PTR;
    int fcb_index=-1;
    int next_block_flag=0;

    while(strcmp(dir,filename)!=0){  // 第一个/后面不是文件名，则找到这个目录

        do_read(dir_start_block,dir_length,(char*)fcb_buff);

        for(int i=0; i < dir_length ; i++){ //在fcb列表中找到目录fcb
            if(strcmp(fcb_buff[i]->filename,dir)==0&&fcb_buff[i]->attribute==0){
                fcb_index=i;
                break;
            }
        }
        if(fcb_index==-1){ //没找到目录文件的fcb

            printf("file not found!\n");
            return 0;
        }
        else{
            dir_start_block=fcb_buff[fcb_index]->first_block;
            dir_length=fcb_buff[fcb_index]->length;
        }
        dir= strtok(NULL,"/");
        memset(fcb_buff,0,MAX_TEXT_SIZE);
    }
    //定位完了，此时从dir_buff中查找filename  dir_buff就是需要打开的文件的目录位置
    int file_index;
    for(int i=0;i<dir_length;i++){
        if(strcmp(fcb_buff[i]->filename,filename)==0&& strcmp(fcb_buff[i]->exname,exname)==0&&fcb_buff[i]->attribute==1){
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
        strcpy(open_file_list[new_fd].filename,fcb_buff[file_index]->filename);
        strcpy(open_file_list[new_fd].exname,fcb_buff[file_index]->exname);
        open_file_list[new_fd].time=fcb_buff[file_index]->time;
        open_file_list[new_fd].date=fcb_buff[file_index]->date;
        open_file_list[new_fd].first_block=fcb_buff[file_index]->first_block;
        open_file_list[new_fd].length=fcb_buff[file_index]->length;
        strcpy(open_file_list[new_fd].dir,absolute_dir);
        open_file_list[new_fd].rw_ptr=0;
        open_file_list[new_fd].fcbstate=0;
        open_file_list[new_fd].topenfile=1;
    }
    free(opendir);
    free(absolute_dir);
    free(fcb_buff);
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
        printf("file not found!\n");
        return -1;
    }
    printf("waiting command for write method:\n1:截断写\n2:覆盖写\n3:追加写\n");
    char method[10];
    int write_method=0;
    fgets(method,10,stdin);
    if(strcmp(method,"1")==0)
    {
        write_method=1;
    }
    else if(strcmp(method,"2"))
    {
        write_method=2;
    }
    else if(strcmp(method,"3"))
    {
        write_method=3;
    }
    char* write_txt=(char*) malloc(MAX_TEXT_SIZE);
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
            write_block=write_block;
            break;
        case 3:
            while (fat1[write_block].index!=FULL_BLOCK){
                write_block=fat1[write_block].index;
            }
            write_ptr=(open_file_list[fd].rw_ptr+open_file_list[fd].length)%BLOCK_SIZE-1;
            break;
    }

    int write_len=0;
    int textptr=0;
    while(tot_len > 0)
    {
        char* buff=(char*) malloc(BLOCK_SIZE);
        memcpy(buff,write_block,BLOCK_SIZE);
        if(tot_len + write_ptr > BLOCK_SIZE) {
            write_len = BLOCK_SIZE - write_ptr;
            tot_len -= write_len;
            memcpy(buff+write_ptr,text+textptr, write_len);
            textptr+=write_len;
            write_ptr=0;
            memcpy(write_block,buff,BLOCK_SIZE);
            if (fat1[write_block].index == FULL_BLOCK)
            {
                int new_block;
                new_block = get_free_block();
                if (!new_block) {
                    printf("no free block, drive is full,fail to write!\n");
                    return 0;
                }
                fat1[write_block].index=new_block;
                fat1[new_block].index=FULL_BLOCK;
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
        free(buff);
    }

}

int do_read(int start_block, int tot_len, char* text)
{
    int read_length=tot_len;
    fat* fat1=(fat*)FAT1_PTR;
    int offset=0;

    while(offset>=BLOCK_SIZE)
    {
        offset-=BLOCK_SIZE;
        start_block=fat1[start_block].index;
    }

    int cur_block=start_block;
    int textptr;
    int copy_len;
    while(tot_len > 0){
        char* buff=(char*) malloc(BLOCK_SIZE);
        if(fat1[cur_block].index!=FULL_BLOCK){
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
        free(buff);
    }
    return read_length;
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
    do_read(open_file_list[fd].first_block,len,text);
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
        return 0;
    }
}

