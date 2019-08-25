#include <cstdio>
#include <libpmemobj.h>

struct RootObject {
    size_t data_size;
    char data[20];
};

int main() {
    PMEMobjpool* pool = pmemobj_create("/pmem/first_pool", "layout", PMEMOBJ_MIN_POOL, 0666);
    if (pool == nullptr) {
        pool = pmemobj_open("/pmem/first_pool", "layout");
    }

    RootObject* root = (RootObject*)pmemobj_direct(pmemobj_root(pool, sizeof(RootObject)));

    if (strnlen(root->data, 12) == root->data_size) {
        printf("%s\n", root->data);
    }

    scanf("%10s", root->data);
    root->data_size = strlen(root->data);

    pmemobj_persist(pool, root, sizeof(RootObject));

    pmemobj_close(pool);
}
