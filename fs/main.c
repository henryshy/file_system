
#include "myfs.h"

int main() {

    startsys();
    my_ls();
    my_open("123.txt");

    my_ls();

    exitsys();
    return 0;
}
