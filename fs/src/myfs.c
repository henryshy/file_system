#include "myfs.h"

char* my_vdrive; //虚拟磁盘起始地址

useropen open_file_list[MAX_OPEN_FILE];
useropen cur_dir;
char* buff;
void parse_command() {
    char cmd[15][10] = {"mkdir", "rmdir", "ls", "cd", "create", "rm", "open", "close", "write", "read", "exit", "help","printopen"};
    char command[50];
    while (1) {
        printf("current dir: %s > ", cur_dir.dir);
        fgets(command, 50, stdin);

        if (strcmp(command, "") == 0) {
            continue;
        }
        command[strlen(command) - 1] = '\0';
        char *sp;
        int indexOfCmd = -1;
        if (strcmp(command, "")) {       // 不是空命令
            sp = strtok(command, " ");  // 把空格前的命令分离出来
            //printf("%s\n",sp);
            for (int i = 0; i < 15; i++) {
                if (strcmp(sp, cmd[i]) == 0) {
                    indexOfCmd = i;
                    break;
                }
            }
            int ret=0;
            switch (indexOfCmd) {
                case 0:         // mkdir
                    sp = strtok(NULL, " ");
                    //printf("%s\n",sp);
                    if (sp != NULL)
                        my_mkdir(sp);
                    else
                        error("mkdir");
                    break;
                case 1:         // rmdir
                    sp = strtok(NULL, " ");
                    if (sp != NULL)
                        my_rmdir(sp);
                    else
                        error("rmdir");
                    break;
                case 2:         // ls
                    my_ls();
                    break;
                case 3:         // cd
                    sp = strtok(NULL, " ");
                    if (sp != NULL) {
                        ret = my_cd(sp);
                        if (ret == -1) {
                            continue;
                        }
                    } else {
                        error("cd");
                    }
                    break;
                case 4:         // create
                    sp = strtok(NULL, " ");
                    if (sp != NULL) {
                        ret = my_create(sp);
                        if (ret == -1) {
                            continue;
                        }
                    } else {
                        error("cd");
                    }
                    break;
                case 5:         // rm
                    sp = strtok(NULL, " ");
                    if (sp != NULL) {
                        ret = my_rm(sp);
                        if (ret == -1) {
                            continue;
                        }
                    } else {
                        error("rm");
                    }
                    break;
                case 6:         // open
                    sp = strtok(NULL, " ");
                    if (sp != NULL) {
                        my_open(sp);
                    }
                    else{
                        error("open");
                    }
                    break;
                case 7:         // close
                    sp = strtok(NULL, " ");
                    if (sp != NULL) {
                        int fd;
                        char *left;
                        fd = strtol(sp, &left, 20);
                        if(strcmp(left,"")==0) {
                            my_close(fd);
                        }
                        else
                        {
                            error("close");
                        }
                    }
                    else{
                        error("close");
                    }
                    break;
                case 8:         // write
                    sp = strtok(NULL, " ");
                    if (sp != NULL) {
                        int fd;
                        char *left;
                        fd = strtol(sp, &left, 20);
                        if(strcmp(left,"")==0) {
                            my_write(fd);
                        }
                        else
                        {
                            error("write");
                        }
                    }
                    else{
                        error("write");
                    }
                    break;
                case 9:         // read
                    sp = strtok(NULL, " ");
                    if (sp != NULL) {
                        int fd;
                        char *left;
                        fd = strtol(sp, &left, 20);
                        if(strcmp(left,"")==0) {
                            my_read(fd);
                        }
                        else
                        {
                            error("read");
                        }
                    }
                    else{
                        error("read");
                    }
                    break;
                case 10:        // exit
                    exitsys();
                    printf("exiting file system\n");
                    exit(0);
                case 11:        // help
                    show_help();
                    break;
                case 12:
                    print_opended();
                    break;
                default:
                    printf("command not found\n", sp);
                    break;
            }
        }
    }
}
void print_opended(){
    printf("filefd\tfilename\texname\tsize\n");
    for(int i=0;i<MAX_OPEN_FILE;i++){
        if(open_file_list[i].topenfile==0){
            continue;
        }
        printf("%d\t%s\t%s\t%dB\n",i,open_file_list[i].filename,open_file_list[i].exname,open_file_list[i].length);
    }
}
void show_help()
{
    printf("mkdir: make a new directory, example: mkdir /dir\n");
    printf("rmdir: remove an existing directory, example: rmdir /dir\n");
    printf("ls: print all files under current directory\n");
    printf("cd: change current directory to another, example: cd /dir\n");
    printf("create: create a non-directory file, example: create /text.txt\n");
    printf("rm: remove a non-directory file example: rm /text.txt\n");
    printf("open: open an existing file for further options, example: open /text.txt\n");
    printf("read: read an opened file and print it on the screen, example: read 1\n");
    printf("write: write an opened file, three different write styles are supported, example: write 1\n");
    printf("exit: exit file system and save all the changes, example: exit\n");
}
void error(char *command){
    printf("%s : argument error\n", command);
    printf("use command help for guidance\n");
}
void startsys()
{
    my_vdrive=(u8_t*) calloc(1,vDRIVE_SIZE); //内存空间

    buff=(char*) calloc(1,MAX_TEXT_SIZE);
    FILE * fp= fopen(FILENAME,"r");
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
    inode* inode_ptr=INODE_PTR;
    fcb* root_fcb=(fcb*) getPtr_of_vDrive(ROOT_BLOCK_INDEX);

    strcpy(cur_dir.filename,boot_block->root_dir_name);
    strcpy(cur_dir.exname,"dir");
    cur_dir.time=inode_ptr[root_fcb[0].inode_index].time;
    cur_dir.date=inode_ptr[root_fcb[0].inode_index].date;
    cur_dir.first_block=boot_block->root_block;
    cur_dir.length=inode_ptr[root_fcb[0].inode_index].length;

    strcpy(cur_dir.dir,"");
    strcat(cur_dir.dir,boot_block->root_dir_name);
    cur_dir.rw_ptr=0;
    cur_dir.fcbstate = 0;
    cur_dir.topenfile = 1;
    cur_dir.attribute=0;

}
void exitsys()
{
    for(int i=0;i<MAX_OPEN_FILE;i++){
        if(open_file_list[i].topenfile!=0&&open_file_list[i].attribute==1){
            my_close(i);
        }
    }
    FILE *fp = fopen(FILENAME, "w");
    fwrite((char*)my_vdrive, vDRIVE_SIZE, 1, fp);
    fclose(fp);

    free(buff);
    free(my_vdrive);
}
void my_format()
{
//初始化引导块
    block0* boot_block=(block0*)my_vdrive;

    strcpy(boot_block->id,"myownsys");
    strcpy(boot_block->information,"123123123");
    boot_block->root_block=ROOT_BLOCK_INDEX;
    strcpy(boot_block->root_dir_name, "");
//初始化fat1和fat2
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
//初始化inode[0]为根目录的inode
    inode* inode_ptr=INODE_PTR;
    time_t rawTime = time(NULL);
    struct tm *time = localtime(&rawTime);

    strcpy(inode_ptr[0].exname, "dir");
    inode_ptr[0].time = time->tm_hour * 2048 + time->tm_min * 32 + time->tm_sec / 2;
    inode_ptr[0].date = (time->tm_year - 100) * 512 + (time->tm_mon + 1) * 32 + (time->tm_mday);
    inode_ptr[0].first_block = ROOT_BLOCK_INDEX;
    inode_ptr[0].attribute = 0;
    inode_ptr[0].length = 2 * sizeof(fcb);
    inode_ptr[0].free=1;
//为根目录新建两个特殊的fcb：.和.. inode都指向inode[0]
    fcb *root1 = (fcb*)getPtr_of_vDrive(ROOT_BLOCK_INDEX);
    root1->inode_index=0;
    strcpy(root1->filename, ".");

    //root2 指向根目录区的第二个fcb,即特殊目录项..,因为根目录区没有上级目录,所以指向自己
    fcb* root2 = root1 + 1;

    strcpy(root2->filename, "..");
    root2->inode_index=0;
//将初始化好的内存区域写回文件
    FILE *fp = fopen(FILENAME, "w+");
    fwrite(my_vdrive, vDRIVE_SIZE, 1, fp);
    fclose(fp);

}
void my_ls()
{

    fcb* fcb_buff=(fcb*) buff;
    memset((char*)buff,0,MAX_TEXT_SIZE);
    inode* inode_ptr=INODE_PTR;
    do_read(0,cur_dir.first_block,cur_dir.length,(char*)fcb_buff);
    int fcb_count=(int)(cur_dir.length/sizeof (fcb));
    fcb_buff=(fcb*) buff;
    for(int i=0;i<fcb_count;i++){
        if(inode_ptr[fcb_buff[i].inode_index].free == 1){
            //目录文件
            //同理,年份占7位,月份占4位,日期占5位
            //小时占5位,分钟占6位,秒占5位
            if(inode_ptr[fcb_buff[i].inode_index].attribute == 0){
                if(strcmp(fcb_buff[i].filename,"..")!=0&&strcmp(fcb_buff[i].filename,".")!=0) {
                    printf("%s\t%dB\t<DIR>\t%d/%d/%d\t%02d:%02d:%02d\n",
                           fcb_buff[i].filename,
                           (inode_ptr[fcb_buff[i].inode_index].length),
                           (inode_ptr[fcb_buff[i].inode_index].date >> 9) + 2000,
                           (inode_ptr[fcb_buff[i].inode_index].date >> 5) & 0x000f,
                           (inode_ptr[fcb_buff[i].inode_index].date) & 0x001f,
                           (inode_ptr[fcb_buff[i].inode_index].time >> 11),
                           (inode_ptr[fcb_buff[i].inode_index].time >> 5) & 0x003f,
                           (inode_ptr[fcb_buff[i].inode_index].time) & 0x001f * 2);
                }
            }
            else{
                printf("%s.%s\t%dB\t<File>\t%d/%d/%d\t%02d:%02d:%02d\n",
                       (char*)(fcb_buff[i].filename),
                       (char*)(inode_ptr[fcb_buff[i].inode_index].exname),
                       inode_ptr[fcb_buff[i].inode_index].length,
                       (inode_ptr[fcb_buff[i].inode_index].date >> 9) + 2000,
                       (inode_ptr[fcb_buff[i].inode_index].date >> 5) & 0x000f,
                       (inode_ptr[fcb_buff[i].inode_index].date) & 0x001f,
                       (inode_ptr[fcb_buff[i].inode_index].time >> 11),
                       (inode_ptr[fcb_buff[i].inode_index].time >> 5) & 0x003f,
                       (inode_ptr[fcb_buff[i].inode_index].time) & 0x001f * 2);
            }
        }
    }
    memset((char*)buff,0,MAX_TEXT_SIZE);
}

int my_cd(char* dirname)
{
    if(dirname[strlen(dirname)-1]=='/'){
        fcb* fcb_buff=(fcb*) buff;
        inode* inode_ptr=INODE_PTR;

        if(strlen(dirname)-1==0){
            do_read(0,ROOT_BLOCK_INDEX,inode_ptr[((fcb *) getPtr_of_vDrive(((block0 *) my_vdrive)->root_block))->inode_index].length,(char*)fcb_buff);
            strcpy(cur_dir.filename,fcb_buff[0].filename);
            strcpy(cur_dir.exname,inode_ptr[fcb_buff[0].inode_index].exname);
            strcpy(cur_dir.dir,inode_ptr[fcb_buff[0].inode_index].dir);
            cur_dir.rw_ptr=0;
            cur_dir.fcbstate=1;
            cur_dir.topenfile=1;
            cur_dir.time=inode_ptr[fcb_buff[0].inode_index].time;
            cur_dir.date=inode_ptr[fcb_buff[0].inode_index].date;
            cur_dir.first_block=inode_ptr[fcb_buff[0].inode_index].first_block;
            cur_dir.length=inode_ptr[fcb_buff[0].inode_index].length;
            cur_dir.attribute=inode_ptr[fcb_buff[0].inode_index].attribute;

            memset((char*)buff,0,MAX_TEXT_SIZE);
            return 1;
        }else
        {
            printf("format error, cd fail!\n");
            return -1;
        }
    }
    if(strcmp(cur_dir.filename,"")==0){
        if(strcmp(dirname,"..")==0|| strcmp(dirname,".")==0||strcmp(dirname,"/")==0){
            return 1;
        }
    }
    fcb* fcb_buff=(fcb*) buff;
    inode* inode_ptr=INODE_PTR;
    int fcb_index= go_to_file(dirname,0,fcb_buff);

    if(fcb_index==-1||fcb_index==-2){ //没找到目录文件的fcb
        printf("no such file or directory, cd fail!\n");
        memset((char*)buff,0,MAX_TEXT_SIZE);
        return -1;
    }

    strcpy(cur_dir.filename,fcb_buff[fcb_index].filename);
    strcpy(cur_dir.exname,inode_ptr[fcb_buff[fcb_index].inode_index].exname);
    strcpy(cur_dir.dir,inode_ptr[fcb_buff[fcb_index].inode_index].dir);
    cur_dir.rw_ptr=0;
    cur_dir.fcbstate=1;
    cur_dir.topenfile=1;
    cur_dir.time=inode_ptr[fcb_buff[fcb_index].inode_index].time;
    cur_dir.date=inode_ptr[fcb_buff[fcb_index].inode_index].date;
    cur_dir.first_block=inode_ptr[fcb_buff[fcb_index].inode_index].first_block;
    cur_dir.length=inode_ptr[fcb_buff[fcb_index].inode_index].length;
    cur_dir.attribute=inode_ptr[fcb_buff[fcb_index].inode_index].attribute;

    memset((char*)buff,0,MAX_TEXT_SIZE);
    return 1;

}
int my_mkdir(char* dirname)
{
    if(dirname[strlen(dirname)-1]=='/'){
        printf("format error, mkdir fail!\n");
        return -1;
    }
    int free_block=get_free_block();
    if(free_block==-1){
        printf("disk is full, mkdir fail!\n");
        return -1;
    }
    int inode_index=get_free_inode();
    if(inode_index==-1){
        printf("inode is full, mkdir fail!\n");
        return -1;
    }

    inode* inode_ptr=INODE_PTR;
    fcb* fcb_buff=(fcb*)buff;

    int fcb_index= go_to_file(dirname,0,fcb_buff);


    if(fcb_index==-1)//是最后一层找不到,说明这个文件还没有被创建,则把他追加到fcb列表末尾
    {
        char* absolute_dir=(char*)  calloc(1,80);
        int length= strlen(dirname);
        int split_index=length;
        while(split_index > 0){
            if(dirname[split_index - 1] == '/'){
                break;
            }
            split_index--;
        }
        char* dir_filename=dirname+split_index;

        strcpy(absolute_dir,inode_ptr[fcb_buff[0].inode_index].dir);
        strcat(absolute_dir,"/");
        strcat(absolute_dir,dir_filename);

        fcb_index=inode_ptr[fcb_buff[0].inode_index].length/sizeof(fcb);

        fat* fat1=FAT1_PTR;
        fat* fat2=FAT2_PTR;

        fat1[free_block-1].index=END_OF_FILE;
        memcpy(fat2,fat1,BLOCK_SIZE*2);
//给新建的fcb赋值

        fcb_buff[fcb_index].inode_index=inode_index;
        strcpy(fcb_buff[fcb_index].filename, dir_filename);
        strcpy(inode_ptr[inode_index].exname, "dir");
        strcpy(inode_ptr[inode_index].dir,absolute_dir);
        time_t rawtime = time(NULL);
        struct tm* time = localtime(&rawtime);
        inode_ptr[inode_index].date = (time->tm_year - 100) * 512 + (time->tm_mon + 1) * 32 + (time->tm_mday);
        inode_ptr[inode_index].time = (time->tm_hour) * 2048 + (time->tm_min) * 32 + (time->tm_sec) / 2;
        inode_ptr[inode_index].first_block = free_block;
        inode_ptr[inode_index].free = 1;
        inode_ptr[inode_index].length = 2*sizeof (fcb);
        inode_ptr[inode_index].attribute = 0;
//修改父目录的inode
        inode_ptr[fcb_buff[0].inode_index].length+=sizeof (fcb);

        do_write(inode_ptr[fcb_buff[0].inode_index].first_block, 0, (char*)fcb_buff, inode_ptr[fcb_buff[0].inode_index].length);



//判断当前打开目录是否需要修改
        if(dirname[0]=='/'){
            if(cur_dir.first_block==ROOT_BLOCK_INDEX){
                cur_dir.length=inode_ptr[fcb_buff[0].inode_index].length;
            }
        }
        else{
            cur_dir.length=inode_ptr[fcb_buff[0].inode_index].length;
        }
//在新的目录文件中创建.和..两个特殊目录项
        fcb* new_fcb=(fcb*) calloc(2,sizeof (fcb));
        strcpy(new_fcb[0].filename,".");
        new_fcb[0].inode_index=inode_index;

        strcpy(new_fcb[1].filename,"..");
        new_fcb[1].inode_index=fcb_buff[0].inode_index;

        do_write(inode_ptr[inode_index].first_block,0,(char*)new_fcb,2*sizeof (fcb));
        free(new_fcb);
    }
    else if(fcb_index==-2)//不是最后一层，没找到目录，报错退出
    {
        printf("no such file or directory, mkdir fail!\n");
        memset((char*)buff,0,MAX_TEXT_SIZE);
        return -1;
    }
    else if(fcb_index>=0){
        printf("file already exists, mkdir fail!\n");
        memset((char*)buff,0,MAX_TEXT_SIZE);
        return -1;
    }
    memset((char*)buff,0,MAX_TEXT_SIZE);
    return 1;
}
int my_rmdir(char *dirname)
{
    if(dirname[strlen(dirname)-1]=='/'){
        printf("format error, mkdir fail!\n");
        return -1;
    }
    inode* inode_ptr=INODE_PTR;
    fcb* fcb_buff=(fcb*)buff;

    int fcb_index= go_to_file(dirname,0,fcb_buff);
    if(fcb_index==-2||fcb_index==-1){
        printf("no such file or directory, open fail!\n");
        memset((char*)buff,0,MAX_TEXT_SIZE);
        return -1;
    }

    if(strcmp(cur_dir.dir,inode_ptr[fcb_buff[fcb_index].inode_index].dir)==0){
        printf("can not remove current directory, rmdir fail!\n");
        memset((char*)buff,0,MAX_TEXT_SIZE);
        return -1;
    }
    if(inode_ptr[fcb_buff[fcb_index].inode_index].length>2*sizeof (fcb)){
        printf("only can remove empty directory, rmdir fail!\n");
        memset((char*)buff,0,MAX_TEXT_SIZE);
        return -1;
    }
//从fcb_buff中删除那个fcb，整合整个fcb_buff
    for(int i=fcb_index; i < inode_ptr[fcb_buff[0].inode_index].length; i++){
        memcpy((fcb*)(fcb_buff+i),(fcb*)(fcb_buff+i+1),sizeof (fcb));
    }
    time_t rawtime = time(NULL);
    struct tm* time = localtime(&rawtime);
    inode_ptr[fcb_buff[fcb_index].inode_index].date = (time->tm_year - 100) * 512 + (time->tm_mon + 1) * 32 + (time->tm_mday);
    inode_ptr[fcb_buff[fcb_index].inode_index].time = (time->tm_hour) * 2048 + (time->tm_min) * 32 + (time->tm_sec) / 2;
    inode_ptr[fcb_buff[fcb_index].inode_index].length-=sizeof (fcb);
    inode_ptr[fcb_buff[0].inode_index].length-=sizeof(fcb);
//判断删除的是否是当前打开目录的下的文件
    if(strcmp(cur_dir.dir,inode_ptr[fcb_buff[0].inode_index].dir)==0){
        cur_dir.length-=sizeof (fcb);
    }
//将fcb_buff写回磁盘，写完清空缓存
    do_write(inode_ptr[fcb_buff[0].inode_index].first_block,0,(char*)fcb_buff,(inode_ptr[fcb_buff[0].inode_index].length-1)*sizeof (fcb));

    memset((char*)buff,0,MAX_TEXT_SIZE);
    return 1;
}


int my_create(char* filedir){ //创建数据文件
//获取一个新盘块，没有则退出
    if(filedir[strlen(filedir)-1]=='/'){
        printf("format error, mkdir fail!\n");
        return -1;
    }
    int free_block=get_free_block();
    if(free_block==-1){
        printf("disk is full, create fail!\n");
        return -1;
    }
    int inode_index=get_free_inode();
    if(inode_index==-1){
        printf("inode is full. create fail!\n");
        return -1;
    }


//开始逐级读取目录fcb

    inode* inode_ptr=INODE_PTR;
    fcb* fcb_buff=(fcb*)buff;
    int fcb_index= go_to_file(filedir,1,fcb_buff);


    if(fcb_index==-1)//是最后一层找不到,说明这个文件还没有被创建,则把他追加到fcb列表末尾
    {
        char* absolute_dir=(char*)  calloc(1,80);

        int length= strlen(filedir);
        int split_index=length-1;
        while(split_index >= 0){
            if(filedir[split_index] == '/'){
                break;
            }
            split_index--;
        }
        char* comp_name=(char*) calloc(1,80);
        strcpy(comp_name,(char*)(filedir+split_index+1));

        int point_index= strlen(comp_name)-1;
        while(point_index >= 0){
            if(comp_name[point_index ] == '.'){
                break;
            }
            point_index--;
        }
        comp_name[point_index]='\0';
        char* filename=comp_name;
        char* exname=(char*)(comp_name+point_index+1);

        strcpy(absolute_dir,inode_ptr[fcb_buff[0].inode_index].dir);
        strcat(absolute_dir,"/");
        strcat(absolute_dir,filename);
        strcat(absolute_dir,".");
        strcat(absolute_dir,exname);
        fcb_index=inode_ptr[fcb_buff[0].inode_index].length/sizeof(fcb);

        fat* fat1=FAT1_PTR;
        fat* fat2=FAT2_PTR;

        fat1[free_block-1].index=END_OF_FILE;
        memcpy(fat2,fat1,BLOCK_SIZE*2);
//给新建的fcb赋值

        fcb_buff[fcb_index].inode_index=inode_index;
        strcpy(fcb_buff[fcb_index].filename, filename);
        strcpy(inode_ptr[inode_index].exname, exname);
        strcpy(inode_ptr[inode_index].dir,absolute_dir);
        time_t rawtime = time(NULL);
        struct tm* time = localtime(&rawtime);
        inode_ptr[inode_index].date = (time->tm_year - 100) * 512 + (time->tm_mon + 1) * 32 + (time->tm_mday);
        inode_ptr[inode_index].time = (time->tm_hour) * 2048 + (time->tm_min) * 32 + (time->tm_sec) / 2;
        inode_ptr[inode_index].first_block = free_block;
        inode_ptr[inode_index].free = 1;
        inode_ptr[inode_index].length = 0;
        inode_ptr[inode_index].attribute = 1;
//修改目录的inode
        inode_ptr[fcb_buff[0].inode_index].length+=sizeof (fcb);


        do_write(inode_ptr[fcb_buff[0].inode_index].first_block, 0, (char*)fcb_buff, inode_ptr[fcb_buff[0].inode_index].length);


        if(strcmp(inode_ptr[fcb_buff[0].inode_index].dir,cur_dir.dir)==0){
            cur_dir.length+=sizeof (fcb);
        }
    }
    else if(fcb_index==-2)//不是最后一层，没找到目录，报错退出
    {
        printf("no such file or directory, create fail!\n");
        memset((char*)buff,0,MAX_TEXT_SIZE);
        return -1;
    }
    else if(fcb_index>=0){
        printf("file already exists, create fail!\n");
        memset((char*)buff,0,MAX_TEXT_SIZE);
        return -1;
    }
    memset((char*)buff,0,MAX_TEXT_SIZE);
    return 1;
}
int my_rm(char* filedir){//只能删除数据文件

    inode* inode_ptr=INODE_PTR;
    fcb* fcb_buff=(fcb*)buff;

    int fcb_index= go_to_file(filedir,1,fcb_buff);
    if(fcb_index==-2||fcb_index==-1){
        printf("no such file or directory, open fail!\n");
        memset((char*)buff,0,MAX_TEXT_SIZE);
        return -1;
    }

//在已打开文件列表里面查找这个文件
    for(int i=0;i<MAX_OPEN_FILE;i++){
        if(open_file_list[i].topenfile==0){
            continue;
        }
        if(open_file_list[i].attribute==1&&strcmp(open_file_list[i].dir,inode_ptr[fcb_buff[fcb_index].inode_index].dir)==0){
            printf("can not rm a opened file, close it before rm!\n");
            printf("rm fail!\n");
            memset((char*)buff,0,MAX_TEXT_SIZE);
            return -1;
        }
    }

//从fcb_buff中删除那个fcb，整合整个fcb_buff
    for(int i=fcb_index; i < inode_ptr[fcb_buff[0].inode_index].length; i++){
        memcpy((fcb*)(fcb_buff+i),(fcb*)(fcb_buff+i+1),sizeof (fcb));
    }
    time_t rawtime = time(NULL);
    struct tm* time = localtime(&rawtime);
    inode_ptr[fcb_buff[fcb_index].inode_index].date = (time->tm_year - 100) * 512 + (time->tm_mon + 1) * 32 + (time->tm_mday);
    inode_ptr[fcb_buff[fcb_index].inode_index].time = (time->tm_hour) * 2048 + (time->tm_min) * 32 + (time->tm_sec) / 2;
    inode_ptr[fcb_buff[fcb_index].inode_index].length-=sizeof (fcb);

//判断删除的是否是当前打开目录的下的文件
    if(strcmp(cur_dir.dir,inode_ptr[fcb_buff[0].inode_index].dir)==0){
        cur_dir.length-=sizeof (fcb);
    }
//将fcb_buff写回磁盘，写完清空缓存
    do_write(inode_ptr[fcb_buff[0].inode_index].first_block,0,(char*)fcb_buff,(inode_ptr[fcb_buff[0].inode_index].length-1)*sizeof (fcb));


    memset((char*)buff,0,MAX_TEXT_SIZE);
    return 1;
}
int my_open(char* filedir)
{
//判断有没有从超过打开文件上限
    int new_fd=get_free_fd();
    if(new_fd==-1){
        printf("can not open, open file list is full!\n");
        return -1;
    }
    inode* inode_ptr=INODE_PTR;
    fcb* fcb_buff=(fcb*)buff;
    int fcb_index= go_to_file(filedir,1,fcb_buff);
    if(fcb_index==-2||fcb_index==-1){
        printf("no such file or directory, open fail!\n");
        memset((char*)buff,0,MAX_TEXT_SIZE);
        return -1;
    }
//判断文件是否已经打开？
    for(int i=0;i<MAX_OPEN_FILE;i++){
        if(open_file_list[i].topenfile==0){
            continue;
        }
        if(open_file_list[i].attribute==1&&strcmp(open_file_list[i].dir,inode_ptr[fcb_buff[fcb_index].inode_index].dir)==0){
            printf("file has been opened, open fail!\n");
            memset((char*)buff,0,MAX_TEXT_SIZE);
            return -1;
        }
    }
//分配一个新的open_file_list表项，并赋值
    strcpy(open_file_list[new_fd].filename,fcb_buff[fcb_index].filename);
    strcpy(open_file_list[new_fd].exname,inode_ptr[fcb_buff[fcb_index].inode_index].exname);
    open_file_list[new_fd].time=inode_ptr[fcb_buff[fcb_index].inode_index].time;
    open_file_list[new_fd].date=inode_ptr[fcb_buff[fcb_index].inode_index].date;
    open_file_list[new_fd].first_block=inode_ptr[fcb_buff[fcb_index].inode_index].first_block;
    open_file_list[new_fd].length=inode_ptr[fcb_buff[fcb_index].inode_index].length;
    strcpy(open_file_list[new_fd].dir,inode_ptr[fcb_buff[fcb_index].inode_index].dir);
    open_file_list[new_fd].attribute=1;
    open_file_list[new_fd].rw_ptr=0;
    open_file_list[new_fd].fcbstate=0;
    open_file_list[new_fd].topenfile=1;
//创建新的文件缓存
    open_file_list[new_fd].file_buff=(char*) calloc(1,MAX_FILE_BUFF_SIZE);

    memset((char*)buff,0,MAX_TEXT_SIZE);
}

int go_to_file(char* filedir,int attribute,fcb *fcb_buff) {  //attribute  0:目录 1:数据文件

    inode *inode_ptr = INODE_PTR;
    int fcb_count = -2;
    int start_block;
    char* go_to_filedir=(char*) calloc(1,80);
    strcpy(go_to_filedir,filedir);
    if (go_to_filedir[0] == '/') { //如果输入的是绝对路径
        //从根目录开始查找
        start_block = ROOT_BLOCK_INDEX;
        fcb_count = inode_ptr[((fcb *) getPtr_of_vDrive(((block0 *) my_vdrive)->root_block))->inode_index].length /sizeof(fcb);
    } else {
        //从当前目录开始找
        start_block = cur_dir.first_block;
        fcb_count = cur_dir.length / sizeof(fcb);
    }


    char directory[20][20];
    char *dir;
    dir = strtok(go_to_filedir, "/");
    if(dir==NULL){
        free(go_to_filedir);
        return -2;
    }
    strcpy(directory[0], dir);
    int fcb_index = -2;
    if (directory[0] == NULL) {
        printf("unexpected error!\n");
        free(go_to_filedir);
        memset((char *) buff, 0, MAX_TEXT_SIZE);
        return -2;
    }
//开始一层一层读
    int depth = 0;
    while (1) {
        dir = strtok(NULL, "/");
        if (dir == NULL) {
            break;
        } else {
            depth++;
            if (depth > 19) {
                printf("directory length out of range\n");
                free(go_to_filedir);
                return -2;
            }
            strcpy(directory[depth], dir);
        }
    }
    int pos = 0;
    char *dir_exname = (char *) malloc(8);
    char* filename=(char*) malloc(8);
    strcpy(dir_exname, "dir");
    while (pos <= depth) {
        strcpy(filename,directory[pos]);
        if (pos == depth) {
            if (attribute == 0) { //最后一个是目录
                //do nothing
            } else if (attribute == 1) {
                int length = strlen(directory[pos]);
                int point_index = length;
                while (point_index > 0) {
                    if (directory[pos][point_index - 1] == '.') {
                        break;
                    }
                    point_index--;
                }
                if (point_index == length || point_index == 0) {
                    free(dir_exname);
                    free(filename);
                    free(go_to_filedir);
                    return -2;
                }
                filename[point_index-1]='\0';
                strcpy(dir_exname, (char *) (directory[pos] + point_index));
                if (strcmp(dir_exname, "dir") == 0) {
                    free(dir_exname);
                    free(filename);
                    free(go_to_filedir);
                    return -2;
                }
            }
        }

        memset((char *) fcb_buff, 0, MAX_TEXT_SIZE);
        do_read(0, start_block, fcb_count * sizeof(fcb), (char *) fcb_buff);

        for (int i = 0; i < fcb_count; i++) { //在fcb列表中找到目录fcb
            if (strcmp(fcb_buff[i].filename, filename) == 0 &&strcmp(inode_ptr[fcb_buff[i].inode_index].exname, dir_exname) == 0) {
                fcb_index = i;
                break;
            }
        }

        if (fcb_index == -2) { //没找到目录文件的fcb
            //判断查找的是不是路径中最后一层
            if (pos != depth) {  //不是最后一层，没找到目录，返回-2
                free(dir_exname);
                free(filename);
                free(go_to_filedir);
                return fcb_index;
            } else { //是最后一层,返回-1
                fcb_index = -1;
                free(dir_exname);
                free(filename);
                free(go_to_filedir);
                return fcb_index;
            }
        } else { //找到目录fcb
            //判断是不是最后一层
            if (pos != depth) { //不是最后一层则再往下一层走
                fcb_count = inode_ptr[fcb_buff[fcb_index].inode_index].length / sizeof(fcb);
                start_block = inode_ptr[fcb_buff[fcb_index].inode_index].first_block;
                fcb_index = -2;
            } else {//是最后一层，而且找到了，返回位置
                free(dir_exname);
                free(filename);
                free(go_to_filedir);
                return fcb_index;
            }
        }
        pos++;
    }
    free(dir_exname);
    free(filename);
    free(go_to_filedir);
    return fcb_index;
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
//判断fd的范围是否合法
    if(fd<0||fd>MAX_OPEN_FILE){
        printf("fd error, close fail!\n");
        return -1;
    }
//判断打开文件的一些属性
    if(open_file_list[fd].topenfile==0){ //该项为空
        printf("file not opened ,close fail!\n");
        return -1;
    }
    if(open_file_list[fd].attribute==0){ //打开的是一个目录文件（正常永远为0，因为不能打开目录文件）
        printf("can not close a directory file, close fail!\n");
        return -1;
    }

    if(open_file_list[fd].fcbstate==0){ //文件有没有被修改过
        printf("closed %s\n",open_file_list[fd].dir);
        free(open_file_list[fd].file_buff);
        memset((char*)(&open_file_list[fd]),0,sizeof (useropen));

        return 1;
    }
    fat* fat1=FAT1_PTR;
//将文件的缓存写入到磁盘
    int offset=open_file_list[fd].rw_ptr;
    int write_block=open_file_list[fd].first_block;
    int write_length= strlen(open_file_list[fd].file_buff);
    switch (open_file_list[fd].fcbstate) {
        case 1:
            open_file_list[fd].length=0;
            open_file_list[fd].rw_ptr=0;
            do_write(write_block,0,open_file_list[fd].file_buff,write_length);
            open_file_list[fd].length=write_length;
            break;
        case 2:
            open_file_list[fd].length=open_file_list[fd].rw_ptr+write_length>open_file_list[fd].length?(open_file_list[fd].rw_ptr+write_length):open_file_list[fd].length;
            do_write(write_block,offset,open_file_list[fd].file_buff,write_length);
            break;
        case 3:
            while (fat1[write_block-1].index != END_OF_FILE){
                write_block=fat1[write_block].index;
            }
            offset=(open_file_list[fd].length)%BLOCK_SIZE;
            do_write(write_block,offset,open_file_list[fd].file_buff,write_length);
            open_file_list[fd].length+=write_length;
            break;
    }

//调用go_to_file找到文件父目录的fcb列表以及文件fcb的位置
    fcb* fcb_buff=(fcb*) buff;
    inode* inode_ptr=INODE_PTR;
    int fcb_index= go_to_file(open_file_list[fd].dir,1,fcb_buff);


//更新文件的fcb
    inode_ptr[fcb_buff[fcb_index].inode_index].length=open_file_list[fd].length;
    time_t rawtime = time(NULL);
    struct tm* time = localtime(&rawtime);
    inode_ptr[fcb_buff[fcb_index].inode_index].date = (time->tm_year - 100) * 512 + (time->tm_mon + 1) * 32 + (time->tm_mday);
    inode_ptr[fcb_buff[fcb_index].inode_index].time = (time->tm_hour) * 2048 + (time->tm_min) * 32 + (time->tm_sec) / 2;

    do_write(inode_ptr[fcb_buff[0].inode_index].first_block,fcb_index*sizeof (fcb),(char*)(fcb_buff+fcb_index),sizeof (fcb));

    printf("closed %s\n",open_file_list[fd].dir);
//在open_file_list中清空改文件对应的表项，释放文件缓冲区内存
    memset(&open_file_list[fd],0,sizeof (useropen));
    free(open_file_list[fd].file_buff);

    memset((char*)buff,0,MAX_TEXT_SIZE);
    return 1;
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
    u32_t write_length=0;

    printf("input text:\n");

    int ch=EOF;
    ch=getchar();
    while (ch!=EOF){
        open_file_list[fd].file_buff[write_length++]=(char)ch;
        ch=getchar();
    }
   // fgets(open_file_list[fd].file_buff,MAX_FILE_BUFF_SIZE,stdin);

    write_length= strlen(open_file_list[fd].file_buff);
    (open_file_list[fd].file_buff)[write_length-1]='\0';
    write_length--;

    open_file_list[fd].fcbstate=write_method;

    return write_length;
}
int do_write(int start_block,int offset, char* text, int tot_len) {//传入的offset为文件内偏移
    fat* fat1=FAT1_PTR;

    int write_len=0;
    int textptr=0;

    while(tot_len > 0)
    {
        if(offset>=BLOCK_SIZE){
            offset-=BLOCK_SIZE;
            start_block=fat1[start_block-1].index;
            continue;
        }

        if(tot_len + offset > BLOCK_SIZE) {
            write_len = BLOCK_SIZE - offset;
            tot_len -= write_len;
            memcpy((char*)(getPtr_of_vDrive(start_block) + offset), (char*)(text + textptr), write_len);
            textptr+=write_len;
            offset=0;
            if (fat1[start_block-1].index == END_OF_FILE)
            {
                int new_block;
                new_block = get_free_block();
                if (!new_block) {
                    printf("no free block, drive is full,fail to write!\n");
                    return 0;
                }
                fat1[start_block-1].index=new_block;
                fat1[new_block-1].index=END_OF_FILE;
            }
            start_block=fat1[start_block-1].index;
        }
        else
        {
            write_len=tot_len;
            tot_len-=write_len;
            memcpy((char*)(getPtr_of_vDrive(start_block) + offset), (char*)(text + textptr), write_len);
        }
    }
    return write_len;
}
int do_read(int offset,int start_block, int tot_len, char* read_buff){//传入的offset为块内偏移
    fat* fat1=(fat*)FAT1_PTR;

    int rw_offset=0;

    int cur_block=start_block;
    int read_ptr=0;
    int copy_len;
    char* block_ptr;

    while(tot_len > 0){

        if(fat1[cur_block-1].index != END_OF_FILE){
            cur_block=fat1[cur_block-1].index;
            copy_len=BLOCK_SIZE-offset;
        }
        else
        {
            copy_len=tot_len;
        }
        block_ptr=(char*)getPtr_of_vDrive(cur_block);
        memcpy((char*)(read_buff + read_ptr), (char*)(block_ptr + offset), copy_len);

        read_ptr+=copy_len;
        tot_len-=copy_len;
        offset=0;
        rw_offset+=copy_len;
    }

    return rw_offset;
}
int my_read(int fd)
{
    if(fd >= MAX_OPEN_FILE || fd < 0){
        printf("file not found, read fail!\n");
        return -1;
    }
    if(open_file_list[fd].topenfile==0){
        printf("file not opened, read fail!\n");
        return -1;
    }
    open_file_list[fd].rw_ptr = 0;


    do_read(0,open_file_list[fd].first_block,open_file_list[fd].length,buff);

    printf("read: %s\n",buff);
    memset((char*)buff,0,MAX_TEXT_SIZE);
    return 1;
}
int get_free_block()
{
    fat* fat1=FAT1_PTR;
    int i;
    int block_index=-1;
    for(i=6;i<1000;i++){
        if(fat1[i].index==FREE_BLOCK){
            block_index=i+1;
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
int get_free_inode(){
    inode* inode_ptr=INODE_PTR;
    int inode_index=-1;
    for(int i=0;i<1000;i++){
        if(inode_ptr[i].free==0){
            inode_index=i;
            break;
        }
    }
    return inode_index;
}
