#include <stdio.h>
#include "my_foo.h"
#include "foo/call_lib_foo.h"

int main(int argc, const char * argv[]) {
    foo();
    call_lib_foo();
    return 0;
}
