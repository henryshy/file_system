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
    root1->empty = 1;
    root1->length = 2 * sizeof(fcb);

    //root2 指向根目录区的第二个fcb,即特殊目录项..,因为根目录区没有上级目录,所以指向自己
    fcb* root2 = root1 + 1;
    memcpy(root2, root1, sizeof(fcb));
    strcpy(root2->filename, "..");

    for(int i=2; i < (int)(BLOCK_SIZE / sizeof(fcb)); i++){
        root2 ++;
        strcpy(root2->filename, "");
        root2->empty = 0;
    }

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
int my_open(char* filename)
{
    int isopen=1;
    int index;

    for(int i=0;i<MAX_OPEN_FILE;i++)
    {
        if(open_file_list[i].filename==filename)
        {
            isopen=0;
            index=i;
            break;
        }
    }
    if(!isopen)
    {


    }
    else
    {
        return 0;
    }
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

int do_read(int fd, int tot_len, char* text)
{
    int start_block=open_file_list[fd].first_block;
    int readlen=open_file_list[fd].length;
    fat* fat1=(fat*)FAT1_PTR;
    int offset=open_file_list[fd].rw_ptr;

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
    return readlen;
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
    do_read(fd,len,text);
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

