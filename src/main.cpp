#include <cstdio>
#include <libpmemobj.h>


int main() {
    PMEMobjpool* pool = pmemobj_create("/pmem/first_pool", "layout", PMEMOBJ_MIN_POOL, 0666);
    if (pool == nullptr) {
        return 1;
    }
}
