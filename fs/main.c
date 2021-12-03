
#include "myfs.h"

int main() {

    startsys();

    my_create("123.txt");
    my_open("123.txt");
    my_write(curfd);
    my_read(curfd,10);
    my_close(curfd);
    my_ls();
    exitsys();
    return 0;
}
