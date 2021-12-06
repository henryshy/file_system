#include "myfs.h"

char* my_vdrive; //虚拟磁盘起始地址
useropen open_file_list[MAX_OPEN_FILE];
useropen cur_dir;
char* buff;
void parse_command() {
    char cmd[15][10] = {"mkdir", "rmdir", "ls", "cd", "create", "rm", "open", "close", "write", "read", "exit", "help","printopen"};
    char command[50];
    while (1) {
        printf("%s > ", cur_dir.dir);
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
                    return;
                    break;
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
    fcb* root_fcb=(fcb*) getPtr_of_vDrive(ROOT_BLOCK_INDEX);

    strcpy(cur_dir.filename,boot_block->root_dir_name);
    strcpy(cur_dir.exname,"dir");
    cur_dir.time=root_fcb->time;
    cur_dir.date=root_fcb->date;
    cur_dir.first_block=boot_block->root_block;
    cur_dir.length=root_fcb->length;

    strcpy(cur_dir.dir,"/");
    strcat(cur_dir.dir,boot_block->root_dir_name);
    cur_dir.rw_ptr=0;
    cur_dir.fcbstate = 0;
    cur_dir.topenfile = 1;
    cur_dir.attribute=0;



}
void  exitsys()
{
    for(int i=0;i<MAX_OPEN_FILE;i++){
        if(open_file_list[i].topenfile!=0&&open_file_list[i].attribute==1){
            my_close(i);
        }
    }
    FILE *fp = fopen(FILENAME, "w+");
    fwrite((char*)my_vdrive, vDRIVE_SIZE, 1, fp);
    fclose(fp);

    free(buff);
    free(my_vdrive);
}
void  my_format()
{
    block0* boot_block=(block0*)my_vdrive;

    strcpy(boot_block->id,"myownsys");
    strcpy(boot_block->information,"123123123");
    boot_block->root_block=ROOT_BLOCK_INDEX;
    strcpy(boot_block->root_dir_name, "");

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
    root1->free=1;

    //root2 指向根目录区的第二个fcb,即特殊目录项..,因为根目录区没有上级目录,所以指向自己
    fcb* root2 = root1 + 1;
    memcpy(root2, root1, sizeof(fcb));
    strcpy(root2->filename, "..");

    FILE *fp = fopen(FILENAME, "w+");
    fwrite(my_vdrive, vDRIVE_SIZE, 1, fp);
    fclose(fp);

}

int my_cd(char* dirname)
{
    if(strcmp(cur_dir.filename,"")==0){
        if(strcmp(dirname,"..")==0|| strcmp(dirname,".")==0||strcmp(dirname,"/")==0){
            return 1;
        }
    }
    char* absolute_dir=(char*)  calloc(1,80); //文件的实际绝对路径
    char* dir_and_filename=(char*)  calloc(1,80); //去除.和后缀名
    char* exname=(char*)  calloc(1,80);//后缀名
    char* filename=(char*)  calloc(1,80);//文件名

    int ret= name_split(dirname,dir_and_filename,exname,filename);
    if(strcmp(filename,"")==0){
        printf("invalid directory name, cd fail!\n");
        return -1;
    }
    if(strcmp(exname,"")!=0){
        printf("invalid directory name, cd fail!\n");
        return -1;
    }

    if(ret==-1){
        printf("cd fail!\n");
        return -1;
    }

    fcb* fcb_buff=(fcb*) buff;

    int fcb_count;
    int fcb_index=-1;
    ret=go_to_dir(dir_and_filename,filename,fcb_buff);
    if(ret==-1){
        printf("cd fail!\n");
        return -1;
    }
    else
    {
        fcb_count=ret/sizeof (fcb);
    }
    for(int i=0;i<fcb_count;i++){
        if(fcb_buff[i].attribute==0&& strcmp(fcb_buff[i].filename,filename)==0){
            fcb_index=i;
            break;
        }
    }
    if(fcb_index==-1){
        printf("no such directory, cd fail!\n");
        return -1;
    }

    if(strcmp(fcb_buff[fcb_index].filename)
    if(dir_and_filename[0]=='/') { //如果是绝对路径
        strcpy(absolute_dir,dir_and_filename);
    }
    else{
        if(strcmp(cur_dir.filename,"")==0){
            strcat(absolute_dir,"/");
            strncat(absolute_dir,dirname,strlen(dirname));
        }
        else{
            strcpy(absolute_dir,cur_dir.dir);
            strcat(absolute_dir,"/");
            strncat(absolute_dir,dirname,strlen(dirname));
        }
    }

    strcpy(cur_dir.filename,fcb_buff[fcb_index].filename);
    strcpy(cur_dir.exname,fcb_buff[fcb_index].exname);
    strcpy(cur_dir.dir,absolute_dir);
    cur_dir.rw_ptr=0;
    cur_dir.fcbstate=1;
    cur_dir.topenfile=1;
    cur_dir.time=fcb_buff[fcb_index].time;
    cur_dir.date=fcb_buff[fcb_index].date;
    cur_dir.first_block=fcb_buff[fcb_index].first_block;
    cur_dir.length=fcb_buff[fcb_index].length;
    cur_dir.attribute=fcb_buff[fcb_index].attribute;
    memset((char*)buff,0,sizeof (buff));
}
int my_mkdir(char* dirname)
{
    char* absolute_dir=(char*)  calloc(1,80); //文件的实际绝对路径
    char* dir_and_filename=(char*)  calloc(1,80); //去除.和后缀名
    char* exname=(char*)  calloc(1,80);//后缀名
    char* filename=(char*)  calloc(1,80);//文件名

    int ret=name_split(dirname, dir_and_filename, exname, filename);
    if(ret==-1){
        printf("mkdir fail!\n");
        return -1;
    }
    if(strcmp(filename,"..")==0|| strcmp(filename,"")==0){
        printf("invalid filename, mkdir fail!\n");
        return -1;
    }

    if(strcmp(exname,"")!=0){
        printf("invalid name format, mkdir fail!\n");
        return -1;
    }
    if(dir_and_filename[0]=='/') { //如果是绝对路径
        strncat(absolute_dir,dir_and_filename,strlen(dir_and_filename));
    }
    else{
        if(strcmp(cur_dir.filename,"")==0){
            strcat(absolute_dir,"/");
            strncat(absolute_dir,dirname,strlen(dirname));
        }
        else{
            strcpy(absolute_dir,cur_dir.dir);
            strcat(absolute_dir,"/");
            strncat(absolute_dir,dirname,strlen(dirname));
        }

    }
}
void my_rmdir(char *dirname)
{

}
void my_ls()
{

    fcb* fcb_buff=(fcb*) buff;
    do_read(0,cur_dir.first_block,cur_dir.length,(char*)fcb_buff);
    int fcb_count=(int)(cur_dir.length/sizeof (fcb));
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
    memset((char*)buff,0,MAX_TEXT_SIZE);
}
int my_create(char* filedir) //创建数据文件
{
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

    fcb* fcb_buff=(fcb*) buff;

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

    fat1[free_block-1].index=END_OF_FILE;
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

    if(filedir[0]=='/'){
        if(strcmp(cur_dir.filename,"root")==0){
            cur_dir.length=fcb_buff[0].length;
        }
    }
    else{
        cur_dir.length=fcb_buff[0].length;
    }

    free(dir_and_filename);
    free(exname);
    free(filename);
    memset((char*)buff,0,MAX_TEXT_SIZE);
}
int my_rm(char* filedir) //只能删除数据文件
{


    char* absolute_dir=(char*)  calloc(1,80);
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
    if(dir_and_filename[0]=='/') { //如果是绝对路径
        strncat(absolute_dir,dir_and_filename,strlen(dir_and_filename));
    }
    else{
        if(strcmp(cur_dir.filename,"")==0){
            strcat(absolute_dir,"/");
            strncat(absolute_dir,filedir,strlen(filedir));
        }
        else{
            strcpy(absolute_dir,cur_dir.dir);
            strcat(absolute_dir,"/");
            strncat(absolute_dir,filedir,strlen(filedir));
        }

    }
    for(int i=0;i<MAX_OPEN_FILE;i++){
        if(open_file_list[i].topenfile==0){
            continue;
        }
        if(open_file_list[i].attribute==1&&strcmp(open_file_list[i].dir,absolute_dir)==0){
            printf("can not rm a opened file, close it before rm!\n");
            printf("close fail!\n");
            return -1;
        }
    }
    fcb* fcb_buff=(fcb*) buff;
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

    for(int i=fcb_index; i < fcb_count; i++){
        memcpy((fcb*)(fcb_buff+i),(fcb*)(fcb_buff+i+1),sizeof (fcb));
    }

    do_write(fcb_buff[0].first_block,0,(char*)fcb_buff,(fcb_count-1)*sizeof (fcb));
    memset((char*)buff,0,MAX_TEXT_SIZE);


    free(absolute_dir);
    free(filename);
    free(dir_and_filename);
    free(exname);
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

    if(strcmp(filedir,".")==0){
        strcpy(dir_and_filename,".");
        strcpy(filename,".");
        strcpy(exname,"");
    }
    else {
        int point_index = -1;
        int i = strlen(filedir);
        int ex_length = 0;
        int point_mark = 0;
        int split_mark = 0;
        while (i >= 0) {
            if (point_mark == 0 && split_mark == 0) {
                if (filedir[i] == '/') {
                    point_index = strlen(filedir);
                    break;
                } else if (filedir[i] == '.') {
                    point_mark++;
                    point_index = i;
                }
            } else if (point_mark > 0) {
                if (filedir[i] == '/') {
                    break;
                } else if (filedir[i] == '.') {
                    if (point_index - 1 == i) {
                        point_index = strlen(filedir);
                    } else {
                        break;
                    }
                    break;
                }
            }
            i--;
        }
        ex_length= strlen(filedir)-point_index> strlen(filedir)? strlen(filedir): strlen(filedir)-point_index;
        memcpy(dir_and_filename,filedir,point_index>0?point_index:0);
        memcpy(exname,filedir+point_index+1,ex_length);
        int big_length = strlen(dir_and_filename);
        int name_index = big_length - 1;
        int name_length = 0;
        while (name_index >= 0) {

            if (dir_and_filename[name_index] != '/') {
                name_index--;
                name_length++;
            } else {
                break;
            }
        }
        memcpy(filename, dir_and_filename + name_index + 1, name_length);
    }
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
        start_block=cur_dir.first_block;
        fcb_count=cur_dir.length/sizeof (fcb);
    }

    char* dir= strtok(dir_and_filename,"/");
    int fcb_index=-1;
    if(strcmp(dir,filename)==0){
        memset(fcb_buff,0,MAX_TEXT_SIZE);
        do_read(0, start_block, fcb_count*sizeof (fcb), (char*)fcb_buff);
    }
    while(strcmp(dir,filename)!=0){  //定位到文件目录
        memset(fcb_buff,0,MAX_TEXT_SIZE);
        do_read(0, start_block, fcb_count, (char*)fcb_buff);

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
    int new_fd=get_free_fd();
    if(new_fd==-1){
        printf("can not open, open file list is full!\n");
        return -1;
    }
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
    for(int i=0;i<MAX_OPEN_FILE;i++){
        if(open_file_list[i].topenfile==0){
            continue;
        }
        if(open_file_list[i].attribute==1&&strcmp(open_file_list[i].dir,absolute_dir)==0){
            printf("file has been opened, open fail!\n");
            return -1;
        }
    }

    if(dir_and_filename[0]=='/') { //如果是绝对路径
        strncat(absolute_dir,dir_and_filename,strlen(dir_and_filename));
    }
    else{
        if(strcmp(cur_dir.filename,"")==0){
            strcat(absolute_dir,"/");
            strncat(absolute_dir,filedir,strlen(filedir));
        }
        else{
            strcpy(absolute_dir,cur_dir.dir);
            strcat(absolute_dir,"/");
            strncat(absolute_dir,filedir,strlen(filedir));
        }

    }


    fcb* fcb_buff=(fcb*) buff;

    if(strcmp(exname,"dir")==0){
        printf("can not open file with extend name \".dir\", open fail!\n");
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

    strcpy(open_file_list[new_fd].filename,fcb_buff[fcb_index].filename);
    strcpy(open_file_list[new_fd].exname,fcb_buff[fcb_index].exname);
    open_file_list[new_fd].time=fcb_buff[fcb_index].time;
    open_file_list[new_fd].date=fcb_buff[fcb_index].date;
    open_file_list[new_fd].first_block=fcb_buff[fcb_index].first_block;
    open_file_list[new_fd].length=fcb_buff[fcb_index].length;
    strcpy(open_file_list[new_fd].dir,absolute_dir);
    open_file_list[new_fd].attribute=1;
    open_file_list[new_fd].rw_ptr=0;
    open_file_list[new_fd].fcbstate=0;
    open_file_list[new_fd].topenfile=1;

    open_file_list[new_fd].file_buff=(char*) calloc(1,MAX_FILE_BUFF_SIZE);


    free(absolute_dir);
    free(exname);
    free(filename);
    free(dir_and_filename);
    memset((char*)buff,0,MAX_TEXT_SIZE);
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
    if(fd<0||fd>MAX_OPEN_FILE){
        printf("fd error, close fail!\n");
        return -1;
    }
    if(open_file_list[fd].topenfile==0){
        printf("file not opened ,close fail!\n");
        return -1;
    }
    if(open_file_list[fd].attribute==0){
        printf("can not close a directory file, close fail!\n");
        return -1;
    }

    if(open_file_list[fd].fcbstate==0){
        printf("closed %s\n",open_file_list[fd].dir);
        free(open_file_list[fd].file_buff);
        memset(&open_file_list[fd],0,sizeof (useropen));

        return 1;
    }
    fat* fat1=FAT1_PTR;

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
    char* dir_and_filename=(char*)  calloc(1,80); //去除.和后缀名
    char* exname=(char*)  calloc(1,80);//后缀名
    char* filename=(char*)  calloc(1,80);//文件名

    int fcb_index=-1;
    int fcb_count=-1;
    int ret;

    ret= name_split(open_file_list[fd].dir,dir_and_filename,exname,filename);
    if(ret==-1){
        printf("close fail!\n");
        return -1;
    }

    fcb* fcb_buff=(fcb*) buff;

    ret=go_to_dir(dir_and_filename,filename,fcb_buff);

    if(ret==-1){
        printf("close fail!\n");
    }
    else
    {
        fcb_count=ret/sizeof (fcb);
    }

    for(int i=0;i<fcb_count;i++){
        if(strcmp(fcb_buff[i].filename,open_file_list[fd].filename)==0&& strcmp(fcb_buff[i].exname,open_file_list[fd].exname)==0){
            fcb_index=i;
            break;
        }
    }
    fcb_buff[fcb_index].length=open_file_list[fd].length;

    do_write(fcb_buff[0].first_block,fcb_index*sizeof (fcb),(char*)(fcb_buff+fcb_index),sizeof (fcb));

    printf("closed %s\n",open_file_list[fd].dir);
    memset(&open_file_list[fd],0,sizeof (useropen));
    free(open_file_list[fd].file_buff);



    free(exname);
    free(filename);
    free(dir_and_filename);

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

//    int ch=EOF;
//    ch=getchar();
//    while (ch!=EOF){
//        write_txt[write_length++]=(char)ch;
//        ch=getchar();
//    }
    fgets(open_file_list[fd].file_buff,MAX_FILE_BUFF_SIZE,stdin);

    write_length= strlen(open_file_list[fd].file_buff);
    (open_file_list[fd].file_buff)[write_length-1]='\0';
    write_length--;

    open_file_list[fd].fcbstate=write_method;

    return write_length;
}
int do_write(int start_block,int offset, char* text, int tot_len) //传入的offset为文件内偏移
{
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

int do_read(int offset,int start_block, int tot_len, char* read_buff)//传入的offset为块内偏移
{
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
    memset(buff,0,MAX_TEXT_SIZE);
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

