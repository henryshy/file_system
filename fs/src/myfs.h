//
// Created by 12719 on 2021/11/24.
//

#ifndef FS_MYFS_H
#define FS_MYFS_H
#include <stdio.h>
#include "stdlib.h"
#include "string.h"
#include "time.h"
#define FILENAME "myfs.txt"
#define BLOCK_SIZE 1024
#define vDRIVE_SIZE 10240000
#define FULL_BLOCK 65535
#define FREE_BLOCK 0
#define MAX_TEXT_SIZE 10000
#define MAX_OPEN_FILE 10


#define getPtr_of_vDrive(n) (my_vdrive+(n-1)*BLOCK_SIZE)
#define FAT1_PTR getPtr_of_vDrive(2)
#define FAT2_PTR getPtr_of_vDrive(4)


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
    u8_t empty;// 1B
    u8_t reserve[10]; //保留10B 凑32B
}fcb;



typedef struct USEROPEN{
    u8_t filename[8];
    u8_t exname[3];
    u16_t time;
    u16_t date;
    u16_t first_block;
    u64_t length;

    u8_t dir[80];
    u32_t rw_ptr;
    u8_t fcbstate;
    u8_t topenfile;
}useropen;

typedef struct BLOCK0{  //5B
    u8_t id[8];
    u8_t information[200];
    u16_t root;
    u8_t* startblock_ptr;
}block0;

extern u8_t* my_vdrive;
extern useropen open_file_list[MAX_OPEN_FILE];
extern int curdir_fd;
extern u8_t current_dir[80];
extern u8_t* data_start_ptr;
extern u8_t buff[vDRIVE_SIZE];  //文件系统缓冲区

void parse_command();
void startsys();
int do_write(int fd, char* text, int tot_len, int write_method);
int do_read(int fd, int tot_len, char* text);
void my_format();
int get_free_block();


#endif //FS_MYFS_H
