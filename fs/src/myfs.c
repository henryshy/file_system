#include "myfs.h"

u8_t* my_vdrive; //虚拟磁盘起始地址
useropen open_file_list[MAX_OPEN_FILE];
int curfd;
void parse_command()
{

}
void startsys()
{
    my_vdrive=(u8_t*) calloc(1,vDRIVE_SIZE); //内存空间
    FILE * fp= fopen("123.txt","wt+");
    if(fp!=NULL)
    {
        fread(my_vdrive,vDRIVE_SIZE,1,fp);
        fclose(fp);
        if(memcmp(my_vdrive,"myownsys",8)==0)
        {
            printf("starting file system\n");
        }
        else
        {
            printf("first loading drive, starting format\n");
            my_format();
        }
    }
    else
    {
        printf("disk not found, exiting file system!\n");
        exit(0);
    }
    block0* boot_block=(block0*)my_vdrive;
    fcb* root_fcb=(fcb*) getPtr_of_vDrive(ROOT_BLOCK_INDEX);

    strcpy(open_file_list[0].filename,boot_block->root_dir_name);
    strcpy(open_file_list[0].exname,"dir");
    open_file_list[0].time=root_fcb->time;
    open_file_list[0].date=root_fcb->date;
    open_file_list[0].first_block=boot_block->root_block;
    open_file_list[0].length=root_fcb->length;

    strcpy(open_file_list[0].dir,"/");
    strcat(open_file_list[0].dir,boot_block->root_dir_name);
    open_file_list[0].rw_ptr=0;
    open_file_list[0].fcbstate = 0;
    open_file_list[0].topenfile = 1;
    open_file_list[0].attribute=0;

    curfd=0;

}
void exitsys()
{
    for(int i=0;i<MAX_OPEN_FILE;i++){
        if(open_file_list[i].topenfile!=0&&open_file_list[i].attribute==1){
            my_close(i);
        }
    }
    FILE *fp = fopen(FILENAME, "w+");
    fwrite(my_vdrive, vDRIVE_SIZE, 1, fp);
    fclose(fp);
}
void  my_format()
{
    block0* boot_block=(block0*)my_vdrive;

    strcpy(boot_block->id,"myownsys");
    strcpy(boot_block->information,"123123123");
    boot_block->root_block=ROOT_BLOCK_INDEX;
    strcpy(boot_block->root_dir_name, "root");
    boot_block->startblock_ptr=getPtr_of_vDrive(ROOT_BLOCK_INDEX+1);

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

    fcb *root1 = (fcb*)getPtr_of_vDrive(ROOT_BLOCK_INDEX);
    strcpy(root1->filename, ".");
    strcpy(root1->exname, "dir");

    time_t rawTime = time(NULL);
    struct tm *time = localtime(&rawTime);

    root1->time = time->tm_hour * 2048 + time->tm_min * 32 + time->tm_sec / 2;
    root1->date = (time->tm_year - 100) * 512 + (time->tm_mon + 1) * 32 + (time->tm_mday);
    root1->first_block = ROOT_BLOCK_INDEX;
    root1->attribute = 0;
    root1->length = 2 * sizeof(fcb);
    root1->is_open=1;
    root1->free=1;

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
    char* absolute_dir=(char*)  calloc(1,80); //文件的实际绝对路径
    char* dir_and_filename=(char*)  calloc(1,80); //去除.和后缀名
    char* exname=(char*)  calloc(1,80);//后缀名
    char* filename=(char*)  calloc(1,80);//文件名

    int ret= name_split(dirname,dir_and_filename,exname,filename);
    if(ret==-1){
        printf("cd fail!\n");
        return -1;
    }

    fcb* fcb_buff=(fcb*) calloc(1,MAX_TEXT_SIZE);

    int fcb_count;
    int fcb_index=-1;
    ret=go_to_dir(dir_and_filename,filename,fcb_buff);
    if(ret==-1){
        printf("cd fail!\n");
    }
    else
    {
        fcb_count=ret/sizeof (fcb);
    }
    for(int i=0;i<fcb_count;i++){
        if(fcb_buff[i].attribute==1&& strcmp(fcb_buff[i].filename,filename)==0){
            fcb_index=i;
            break;
        }
    }
    if(fcb_index==-1){
        printf("no such directory, cd fail!\n");
        return -1;
    }

    if(dirname[0]=='/'){
        strcpy(absolute_dir,dirname);
    }
    else
    {
        strcpy(absolute_dir,open_file_list[curfd].dir);
        strncat(absolute_dir,'/',1);
        strncat(absolute_dir,dir_and_filename, strlen(dir_and_filename));
    }

    strcpy(open_file_list[curfd].filename,fcb_buff[fcb_index].filename);
    strcpy(open_file_list[curfd].exname,fcb_buff[fcb_index].exname);
    strcpy(open_file_list[curfd].dir,absolute_dir);
    open_file_list[curfd].rw_ptr=0;
    open_file_list[curfd].fcbstate=1;
    open_file_list[curfd].topenfile=1;
    open_file_list[curfd].time=fcb_buff[fcb_index].time;
    open_file_list[curfd].date=fcb_buff[fcb_index].date;
    open_file_list[curfd].first_block=fcb_buff[fcb_index].first_block;
    open_file_list[curfd].length=fcb_buff[fcb_index].length;
    open_file_list[curfd].attribute=fcb_buff[fcb_index].attribute;
}
void my_mkdir(char* dirname)
{

}
void my_rmdir(char *dirname)
{

}
void my_ls()
{
    if(open_file_list[curfd].attribute==1){
        printf("can not ls in a non directory file, ls fail!\n");
        return;
    }
    fcb* fcb_buff=(fcb*) calloc(1, MAX_TEXT_SIZE/ sizeof (fcb));
    do_read(0,open_file_list[curfd].first_block,open_file_list[curfd].length,(char*)fcb_buff);
    int fcb_count=(int)(open_file_list[curfd].length/sizeof (fcb));
    for(int i=0;i<fcb_count;i++){
        if(fcb_buff[i].free == 1){
            //目录文件
            //同理,年份占7位,月份占4位,日期占5位
            //小时占5位,分钟占6位,秒占5位
            if(fcb_buff[i].attribute == 0){
                printf("%s\t%dB\t<DIR>\t%d/%d/%d\t%02d:%02d:%02d\n",
                       fcb_buff[i].filename, fcb_buff[i].length,
                       (fcb_buff[i].date >> 9) + 2000,
                       (fcb_buff[i].date >> 5) & 0x000f,
                       (fcb_buff[i].date) & 0x001f,
                       (fcb_buff[i].time >> 11),
                       (fcb_buff[i].time >> 5) & 0x003f,
                       (fcb_buff[i].time) & 0x001f * 2);
            }
            else{
                unsigned int length = fcb_buff[i].length;
                if(length != 0)length -= 1;
                printf("%s.%s\t%dB\t<File>\t%d/%d/%d\t%02d:%02d:%02d\n",
                       fcb_buff[i].filename,
                       fcb_buff[i].exname,
                       length,
                       (fcb_buff[i].date >> 9) + 2000,
                       (fcb_buff[i].date >> 5) & 0x000f,
                       (fcb_buff[i].date) & 0x001f,
                       (fcb_buff[i].time >> 11),
                       (fcb_buff[i].time >> 5) & 0x003f,
                       (fcb_buff[i].time) & 0x001f * 2);
            }
        }
    }

}
int my_create(char* filedir) //创建数据文件
{
    if(open_file_list[curfd].attribute == 1){
        printf("can not create in a file, create fail!\n");
        return -1;
    }

    char* dir_and_filename=(char*)  calloc(1,80); //去除.和后缀名
    char* exname=(char*)  calloc(1,80);//后缀名
    char* filename=(char*)  calloc(1,80);//文件名

    int ret=name_split(filedir, dir_and_filename, exname, filename);
    if(ret==-1){
        printf("create fail!\n");
        return -1;
    }
    if(strcmp(exname,"dir")==0){
        printf("can not create file with extend name \".dir\", create fail!\n");
        return -1;
    }
    if(strcmp(filename,"..")==0|| strcmp(filename,"")==0){
        printf("invalid filename, create fail!\n");
        return -1;
    }

    if(strcmp(exname,"")==0){
        printf("can not create file with no extend name, create fail!\n");
        return -1;
    }
    fcb* fcb_buff=(fcb*) calloc(1, MAX_TEXT_SIZE);

    int fcb_count=-1;
    ret= go_to_dir(dir_and_filename, filename, fcb_buff) / sizeof (fcb);
    if(ret==-1){
        printf("no such dir, create fail!\n");
        return -1;
    }
    else
    {
        fcb_count=ret;
    }

    for(int i=0; i < fcb_count; i++){
        if(strcmp(fcb_buff[i].filename, filename) == 0 && strcmp(fcb_buff[i].exname, exname) == 0){
            printf("file already exists, create fail!\n");
            return -1;
        }
    }
    int fcb_index=fcb_count;

    int free_block=get_free_block();
    if(free_block==-1){
        printf("disk is full, create fail!\n");
        return -1;
    }
    fat* fat1=FAT1_PTR;
    fat* fat2=FAT2_PTR;

    fat1[free_block].index=END_OF_FILE;
    memcpy(fat2,fat1,BLOCK_SIZE*2);

    strcpy(fcb_buff[fcb_index].filename, filename);
    strcpy(fcb_buff[fcb_index].exname, exname);
    time_t rawtime = time(NULL);
    struct tm* time = localtime(&rawtime);
    fcb_buff[fcb_index].date = (time->tm_year - 100) * 512 + (time->tm_mon + 1) * 32 + (time->tm_mday);
    fcb_buff[fcb_index].time = (time->tm_hour) * 2048 + (time->tm_min) * 32 + (time->tm_sec) / 2;
    fcb_buff[fcb_index].first_block = free_block;
    fcb_buff[fcb_index].free = 1;
    fcb_buff[fcb_index].length = 0;
    fcb_buff[fcb_index].attribute = 1;

    open_file_list[curfd].length+=sizeof (fcb);

    if(fcb_buff[1].first_block == ROOT_BLOCK_INDEX)  //如果当前目录是根目录，则..项的fcb也要修改
    {
        fcb_buff[0].length+=sizeof (fcb);
        fcb_buff[1].length+=sizeof (fcb);
    }
    else
    {
        fcb_buff[0].length+=sizeof (fcb);
    }

    do_write(fcb_buff[0].first_block, 0, (char*)fcb_buff, fcb_buff[0].length);


    open_file_list[curfd].rw_ptr=0;
    open_file_list[curfd].fcbstate=1;

    free(dir_and_filename);
    free(exname);
    free(filename);
}
int my_rm(char* filedir) //只能删除数据文件
{
    if(open_file_list[curfd].attribute==1){
        printf("can not rm in a non-dir file, rm fail!\n");
        return -1;
    }
    char* dir_and_filename=(char*)  calloc(1,80); //去除.和后缀名
    char* exname=(char*)  calloc(1,80);//后缀名
    char* filename=(char*)  calloc(1,80);//文件名

    int ret=name_split(filedir, dir_and_filename, exname, filename);
    if(ret==-1){
        printf("rm fail!\n");
        return -1;
    }
    if(strcmp(filename,"..")==0|| strcmp(filename,"")==0){
        printf("invalid filename, open fail!\n");
        return -1;
    }
    if(strcmp(exname,"dir")==0){
        printf("can not rm file with extend name \".dir\", rm fail!\n");
        return -1;
    }
    if(strcmp(exname,"")==0){
        printf("no such file, rm fail!\n");
        return -1;
    }

    fcb* fcb_buff=(fcb*) calloc(1,MAX_TEXT_SIZE);
    int fcb_count=-1;
    ret= go_to_dir(dir_and_filename, filename, fcb_buff) / sizeof (fcb);

    if(ret==-1){
        printf("rm fail!\n");
        return -1;
    }
    else
    {
        fcb_count=ret/sizeof (fcb);
    }

    int fcb_index=-1;

    for(int i=0; i < fcb_count; i++){
        if(strcmp(fcb_buff[i].filename,filename)==0&& strcmp(fcb_buff[i].exname,exname)==0&&fcb_buff[i].attribute==0){
            fcb_index=i;
            break;
        }
    }
    if(fcb_index==-1){
        printf("no such file ,rm fail!\n");
        return -1;
    }

    if(fcb_buff[fcb_index].is_open==1){
        printf("can not rm a opened file, close it before rm!\n");
        printf("close fail!\n");
        return -1;
    }
    for(int i=fcb_index; i < fcb_count; i++){
        memcpy((fcb*)(fcb_buff+i),(fcb*)(fcb_buff+i+1),sizeof (fcb));
    }

    do_write(fcb_buff[0].first_block,0,(char*)fcb_buff,(fcb_count-1)*sizeof (fcb));

    return 1;
}
int check_name(char* name,int length){
    for(int i=0;i<length-1;i++){
        if(name[i]==':'||name[i]=='?'||name[i]=='$'||name[i]==' '){
            return -1;
        }
    }
    return 1;
}
int name_split(char* filedir,char* dir_and_filename,char* exname,char* filename)
{
    int check;
    check=check_name(filedir, strlen(filedir));
    if(check==-1){
        printf("invalid char detected!\n");
        return -1;
    }


    int point_index= -1;
    int i= strlen(filedir);
    int ex_length=0;
    int point_mark=0;
    int split_mark=0;
    while(i>=0) {
        if (point_mark==0&&split_mark==0) {
            if (filedir[i] == '/') {
                point_index= strlen(filedir);
                break;
            }
            else if (filedir[i] == '.')
            {
                point_mark++;
                point_index=i;
            }
        }
        else if(point_mark>0){
            if (filedir[i] == '/') {
                break;
            }
            else if (filedir[i] == '.')
            {
                if(point_index-1==i) {
                    point_index = strlen(filedir);
                }
                else{
                    break;
                }
                break;
            }
        }
        i--;
    }
    ex_length= strlen(filedir)-point_index;
    memcpy(dir_and_filename,filedir,point_index);
    memcpy(exname,filedir+point_index+1,ex_length);
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
    int fcb_count;
    int start_block;

    if(dir_and_filename[0]=='/') { //如果是绝对路径
        start_block=ROOT_BLOCK_INDEX;
        fcb_count=((fcb*)getPtr_of_vDrive(((block0*)my_vdrive)->root_block))->length/sizeof (fcb);
    }
    else{
        start_block=open_file_list[curfd].first_block;
        fcb_count=open_file_list[curfd].length/sizeof (fcb);
    }

    char* dir= strtok(dir_and_filename,"/");
    int fcb_index=-1;
    if(strcmp(dir,filename)==0){
        memset(fcb_buff,0,MAX_TEXT_SIZE);
        do_read(0, start_block, fcb_count*sizeof (fcb), fcb_buff);
    }
    while(strcmp(dir,filename)!=0){  //定位到文件目录
        memset(fcb_buff,0,MAX_TEXT_SIZE);
        do_read(0, start_block, fcb_count, fcb_buff);

        for(int i=0; i < fcb_count ; i++){ //在fcb列表中找到目录fcb
            if(strcmp(fcb_buff[i].filename,dir)==0&&strcmp(fcb_buff[i].exname,"dir")==0&&fcb_buff[i].attribute==0){
                fcb_index=i;
                break;
            }
        }
        if(fcb_index==-1){ //没找到目录文件的fcb
            printf("no such dir!\n");
            return -1;
        }
        else{
            fcb_count=fcb_buff[fcb_index].length;
            start_block=fcb_buff[fcb_index].first_block;
        }
        dir= strtok(NULL,"/");


    }
    return fcb_count*sizeof (fcb);
}
int my_open(char* filedir)
{

    char* absolute_dir=(char*)  calloc(1,80); //文件的实际绝对路径
    char* dir_and_filename=(char*)  calloc(1,80); //去除.和后缀名
    char* exname=(char*)  calloc(1,80);//后缀名
    char* filename=(char*)  calloc(1,80);//文件名

    int ret=name_split(filedir, dir_and_filename, exname, filename);

    if(ret==-1){
        printf("open fail!\n");
        return -1;
    }
    if(strcmp(filename,"..")==0|| strcmp(filename,"")==0){
        printf("invalid filename, open fail!\n");
        return -1;
    }

    if(strcmp(exname,"")==0){
        printf("no such file, open fail!\n");
        return -1;
    }

    fcb* fcb_buff=(fcb*) calloc(1,MAX_TEXT_SIZE);


    if(strcmp(exname,"dir")==0){
        printf("can not open file with extend name \".dir\", open faile!\n");
        return -1;
    }
    int fcb_count=-1;
    ret=go_to_dir(dir_and_filename, filename, fcb_buff)/sizeof (fcb);
    if(ret==-1){
        printf("open fail!\n");
        return -1;
    }
    else
    {
        fcb_count=ret;
    }

    if(dir_and_filename[0]=='/') { //如果是绝对路径
        strncat(absolute_dir,dir_and_filename,strlen(dir_and_filename));
    }
    else{
        strcpy(absolute_dir,open_file_list[curfd].dir);
        strcat(absolute_dir,"/");
        strncat(absolute_dir,filedir,strlen(filedir));
    }
    //定位完了，此时从dir_buff中查找filename  dir_buff就是需要打开的文件的目录文件的所有内容
    int fcb_index=-1;
    for(int i=0; i < fcb_count;i++){
        if(strcmp(fcb_buff[i].filename,filename)==0&& strcmp(fcb_buff[i].exname,exname)==0&&fcb_buff[i].attribute==1){
            fcb_index=i;
            break;
        }
    }
    if(fcb_index == -1){
        printf("file not found, open fail!\n");
        return -1;
    }
    //找到了文件则继续往下

    //将文件属性写上已打开
    fcb_buff[fcb_index].is_open=1;
    do_write(fcb_buff[0].first_block,fcb_index*sizeof(fcb),(char*)(fcb_buff+fcb_index),sizeof (fcb));

    int new_fd=get_free_fd();
    if(new_fd==-1){
        printf("can not open, open file list is full!\n");
        return -1;
    }
    else{
        strcpy(open_file_list[new_fd].filename,fcb_buff[fcb_index].filename);
        strcpy(open_file_list[new_fd].exname,fcb_buff[fcb_index].exname);
        open_file_list[new_fd].time=fcb_buff[fcb_index].time;
        open_file_list[new_fd].date=fcb_buff[fcb_index].date;
        open_file_list[new_fd].first_block=fcb_buff[fcb_index].first_block;
        open_file_list[new_fd].length=fcb_buff[fcb_index].length;
        strcpy(open_file_list[new_fd].dir,absolute_dir);
        open_file_list[new_fd].attribute=1;
        open_file_list[new_fd].rw_ptr=0;
        open_file_list[new_fd].fcbstate=1;
        open_file_list[new_fd].topenfile=1;
        curfd=new_fd;
    }
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
int my_close(int fd)
{
    if(fd<0||fd>MAX_OPEN_FILE||open_file_list[fd].topenfile==0){
        printf("no such file, close fail!\n");
        return -1;
    }
    if(open_file_list[fd].attribute==0){
        printf("can not close a directory file, close fail!\n");
        return -1;
    }

    int father_fd=-1;
    char* father_dir=(char*) calloc(1,80);
    char* file_dir=(char*) calloc(1,80);
    strcpy(file_dir,open_file_list[fd].dir);

    for(int i=0;i<MAX_OPEN_FILE;i++){
        if(open_file_list[i].topenfile==1&&i!=fd&&open_file_list[i].attribute==0){
            strcpy(father_dir,open_file_list[i].dir);
            strcat(father_dir,"/");
            strcat(father_dir,open_file_list[fd].filename);
            strcat(father_dir,".");
            strcat(father_dir,open_file_list[fd].exname);
            if(strcmp(father_dir,file_dir)==0){
                father_fd=i;
                break;
            }
        }
        else
        {
            continue;
        }
    }
    free(father_dir);
    free(file_dir);
    if(father_fd==-1){
        printf("father dir not found, close fail!\n");
        return -1;
    }
    int fcb_index=-1;
    int fcb_count=open_file_list[father_fd].length/sizeof (fcb);
    if(open_file_list[fd].fcbstate==1){
        fcb* fcb_buff=(fcb*) calloc(1,MAX_TEXT_SIZE/sizeof (fcb));
        do_read(0,open_file_list[father_fd].first_block,open_file_list[father_fd].length,(char*)fcb_buff);
        for(int i=0;i<fcb_count;i++){
            if(strcmp(fcb_buff[i].filename,open_file_list[fd].filename)==0&& strcmp(fcb_buff[i].exname,open_file_list[fd].exname)==0){
                fcb_index=i;
                break;
            }
        }
        fcb_buff[fcb_index].is_open=0;
        fcb_buff[fcb_index].length=open_file_list[fd].length;
        do_write(fcb_buff[0].first_block,fcb_index*sizeof (fcb),(char*)(fcb_buff+fcb_index),sizeof (fcb));
    }
    memset(&open_file_list[fd],0,sizeof (useropen));
    curfd=father_fd;
    return father_fd;
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
    getchar();

    if(write_method<0|write_method>3){
        printf("write method not found, write fail!\n");
        return -1;
    }
    char* write_txt=(char*) calloc(1,MAX_TEXT_SIZE);
    int write_length=0;

    printf("input text:\n");

    int ch;

    while (ch!=EOF){
        ch=getchar();
        write_txt[write_length++]=ch;

    }

    write_length= strlen(write_txt);
    int offset=open_file_list[fd].rw_ptr;
    int write_block=open_file_list[fd].first_block;
    fat* fat1=(fat*)FAT1_PTR;


    switch (write_method) {
        case 1:
            open_file_list[fd].length=0;
            open_file_list[fd].rw_ptr=0;
            do_write(write_block,0,write_txt,write_length);
            open_file_list[fd].length=write_length;
            break;
        case 2:
            open_file_list[fd].length=open_file_list[fd].rw_ptr+write_length>open_file_list[fd].length?(open_file_list[fd].rw_ptr+write_length):open_file_list[fd].length;

            while(offset>=BLOCK_SIZE){
                offset-=BLOCK_SIZE;
                write_block=fat1[write_block].index;
            }
            do_write(write_block,offset,write_txt,write_length);
            break;
        case 3:
            while (fat1[write_block].index != END_OF_FILE){
                write_block=fat1[write_block].index;
            }
            offset=(open_file_list[fd].length)%BLOCK_SIZE;
            do_write(write_block,offset,write_txt,write_length);
            open_file_list[fd].length+=write_length-1;
            break;
    }

    free(write_txt);
    return write_length;
}

int do_write(int start_block,int offset, char* text, int tot_len)
{
    fat* fat1=FAT1_PTR;

    int write_len=0;
    int textptr=0;
    char* block_buff=(char*) calloc(1, BLOCK_SIZE);
    while(tot_len > 0)
    {
        if(offset>=BLOCK_SIZE){
            offset-=BLOCK_SIZE;
            start_block=fat1[start_block].index;
            continue;
        }
        memcpy(block_buff, (char*)getPtr_of_vDrive(start_block), BLOCK_SIZE);
        if(tot_len + offset > BLOCK_SIZE) {
            write_len = BLOCK_SIZE - offset;
            tot_len -= write_len;
            memcpy((char*)(block_buff + offset), (char*)(text + textptr), write_len);
            textptr+=write_len;
            offset=0;
            memcpy((char*)getPtr_of_vDrive(start_block), block_buff, BLOCK_SIZE);
            if (fat1[start_block].index == END_OF_FILE)
            {
                int new_block;
                new_block = get_free_block();
                if (!new_block) {
                    printf("no free block, drive is full,fail to write!\n");
                    return 0;
                }
                fat1[start_block].index=new_block;
                fat1[new_block].index=END_OF_FILE;
            }
            start_block=fat1[start_block].index;
        }
        else
        {
            write_len=tot_len;
            tot_len-=write_len;
            memcpy((char*)(block_buff + offset), (char*)(text + textptr), write_len);
            memcpy((char*)getPtr_of_vDrive(start_block), block_buff, BLOCK_SIZE);
        }
        memset(block_buff, 0, BLOCK_SIZE);
    }
    free(block_buff);
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
    char* block_buff=(char*) calloc(1, BLOCK_SIZE);
    while(tot_len > 0){

        if(fat1[cur_block].index != END_OF_FILE){
            memcpy(block_buff, (char*)getPtr_of_vDrive(cur_block), BLOCK_SIZE);
            cur_block=fat1[cur_block].index;
            copy_len=BLOCK_SIZE-offset;
        }
        else
        {
            memcpy(block_buff, (char*)getPtr_of_vDrive(cur_block), BLOCK_SIZE);
            copy_len=tot_len;
        }
        memcpy((char*)(text+textptr), (char*)(block_buff + offset), copy_len);
        textptr+=copy_len;
        tot_len-=copy_len;
        offset=0;
        rw_offset+=copy_len;
        memset(block_buff, 0, BLOCK_SIZE);
    }
    free(block_buff);
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
    int block_index=-1;
    for(i=7;i<=1000;i++){
        if(fat1[i].index==FREE_BLOCK){
            block_index=i;
            break;
        }
    }
    if(block_index!=-1)
    {
        return block_index;
    }
    else{
        return -1;
    }
}

