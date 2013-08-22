#ifndef LLCC_LIST_HEADER
#define LLCC_LIST_HEADER

struct list {
    void **arr;
    size_t capacity;
    size_t size;
};

#endif
