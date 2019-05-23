#include <cstdio>
#include <libpmemobj.h>


int main() {
    PMEMobjpool* pool = pmemobj_create("/dev/pmem0", "layout", PMEMOBJ_MIN_POOL, 0666);
    //PMEMobjpool* pool = pmemobj_open("/dev/pmem0", "layout");
    if (pool == nullptr) {
        return 1;
    }
}
