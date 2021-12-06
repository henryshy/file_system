
#include "myfs.h"

int main() {

    startsys();
    my_ls();

    my_create("123.txt");
    my_open("123.txt");
    my_read(0,10);
    my_write(0);

    my_close(0);

    my_ls();

    exitsys();
    return 0;
}
