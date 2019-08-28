#include <cstdio>
#include <libpmemobj.h>

struct RootObject {
    size_t data_size;
    char data[20];
};

POBJ_LAYOUT_BEGIN(store);
    POBJ_LAYOUT_ROOT(store, RootObject);
POBJ_LAYOUT_END(store);

int main() {
    PMEMobjpool* pool = pmemobj_create("/pmem/first_pool", POBJ_LAYOUT_NAME(store), PMEMOBJ_MIN_POOL, 0666);
    if (pool == nullptr) {
        pool = pmemobj_open("/pmem/first_pool", POBJ_LAYOUT_NAME(store));
    }

    TOID(RootObject) root = POBJ_ROOT(pool, RootObject);

    if (strnlen(D_RW(root)->data, 12) == D_RW(root)->data_size) {
        printf("%s\n", D_RW(root)->data);
    }

    TX_BEGIN(pool)
        TX_ADD(root);
        if (scanf("%10s", D_RW(root)->data) != 1) {
            pmemobj_tx_abort(1);
        }
        D_RW(root)->data_size = strlen(D_RW(root)->data);
    TX_ONABORT
        printf("onabort");
    TX_END

    //pmemobj_persist(pool, D_RW(root), sizeof(RootObject));

    pmemobj_close(pool);
}
