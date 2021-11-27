
#include "myfs.h"

int main() {
    startsys();

    my_open(".");
    my_write(1);
    return 0;
}
