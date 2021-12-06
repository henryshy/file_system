//
// Created by 12719 on 2021/11/24.
//

#ifndef FS_MYFS_H
#define FS_MYFS_H
#include <stdio.h>
#include "stdlib.h"
#include "string.h"
#include "time.h"

#define FILENAME "123.txt"
#define BLOCK_SIZE 1024     //1MB
#define vDRIVE_SIZE 1024000 //1000MB
#define END_OF_FILE 65535
#define FREE_BLOCK 0
#define MAX_TEXT_SIZE 20480  //20MB
#define MAX_FILE_BUFF_SIZE 20480 //20MB
#define MAX_OPEN_FILE 10


#define getPtr_of_vDrive(n) (my_vdrive+(n-1)*BLOCK_SIZE)
#define FAT1_PTR (fat*)getPtr_of_vDrive(2)
#define FAT2_PTR (fat*)getPtr_of_vDrive(4)
#define ROOT_BLOCK_INDEX 6


typedef unsigned char u8_t;
typedef unsigned short u16_t;
typedef unsigned int u32_t;
typedef unsigned long u64_t;

typedef struct FAT{ //2B
    u16_t index;
}fat;
typedef struct FCB{   //32B
    u8_t filename[8]; // 8B
    u8_t exname[3]; // 3B
    u16_t time; //2B
    u16_t date;// 2B
    u16_t first_block; //2B
    u32_t length;  //4B
    u8_t attribute;// 1B
    u8_t free;  //标记整个是否为空 0空 1不空
    u8_t reserve[5]; //保留8B 凑32B
}fcb;



typedef struct USEROPEN{
    u8_t filename[8];
    u8_t exname[3];
    u16_t time;
    u16_t date;
    u16_t first_block;
    u64_t length;

    u8_t attribute;  // 0:目录 1:数据文件
    u8_t dir[80]; //当前打开文件的绝对路径
    u32_t rw_ptr;
    u8_t fcbstate;
    u8_t topenfile;
    char* file_buff;
}useropen;

typedef struct BLOCK0{
    u8_t id[8];
    u8_t information[200];
    u8_t root_dir_name[20];
    u16_t root_block;

}block0;

extern char* my_vdrive;
extern useropen open_file_list[MAX_OPEN_FILE];
extern useropen cur_dir;
extern char* buff;

void parse_command();
void startsys();
int do_write(int start_block,int offset, char* text, int tot_len);
int do_read(int offset,int start_block, int tot_len, char* read_buff);
void my_format();
int get_free_block();
int get_free_fd();
int my_open(char* filedir);
int my_write(int fd);
int name_split(char* filedir,char* dir_and_filename,char* exname,char* filename);
int check_name(char* name,int length);
int go_to_dir(char* dir_and_filename,char* filename,fcb* fcb_buff);
int my_rm(char* filedir); //只能删除数据文件
int my_create(char* filedir);
int my_close(int fd);
void my_ls();
void exitsys();
int my_read(int fd);
int my_mkdir(char* dirname);
void my_rmdir(char *dirname);
int my_cd(char* dirname);
void error(char *command);
void print_opended();
void show_help();

#endif //FS_MYFS_H
