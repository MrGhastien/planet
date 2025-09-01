#include "pool.h"

#include <stdlib.h>

#define NODE_HEADER_SIZE (sizeof(struct obj_node) - sizeof(struct obj_node*))

struct obj_node {
    bool allocated;
    struct obj_node* next;
};

void* offset(const void* ptr, long offset) {
    unsigned long a = (unsigned long) ptr;
    a += offset;
    return (void*) a;
}

void* offsetu(const void* ptr, unsigned long offset) {
    unsigned long a = (unsigned long) ptr;
    a += offset;
    return (void*) a;
}

static struct obj_node* get_node_from_idx(PoolAllocator* pool, long index) {
    long total_stride = pool->stride + NODE_HEADER_SIZE;

    if (index < (long) pool->capacity) {
        return offset(pool->mem, index * total_stride);
    }
    return NULL;
}

static void add_node_to_free_list(PoolAllocator* pool, struct obj_node* node) {
    node->next = NULL;
    node->allocated = false;
    if (!pool->head)
        pool->head = node;
    if (pool->tail)
        pool->tail->next = node;
    pool->tail = node;
    pool->size--;
}

static void prepare_block(PoolAllocator* pool, void* start, size_t length) {
    size_t total_stride = pool->stride + NODE_HEADER_SIZE;
    for (size_t i = 0; i < length; i++) {
        struct obj_node* node = offset(start, i * total_stride);
        add_node_to_free_list(pool, node);
    }
}

static void init_free_list(PoolAllocator* pool) {
    prepare_block(pool, pool->mem, pool->capacity);
    pool->size = 0;
}

void pool_init(PoolAllocator* pool, size_t capacity, size_t stride) {
    pool->capacity = capacity;
    pool->stride = stride;
    if (pool->stride < sizeof(struct obj_node*))
        pool->stride = sizeof(struct obj_node*);
    pool->mem = calloc(capacity, (pool->stride + NODE_HEADER_SIZE));
    pool->head = NULL;
    pool->tail = NULL;
    pool->size = 0;

    init_free_list(pool);
}

void pool_destroy(PoolAllocator* pool) {
    free(pool->mem);
    *pool = (PoolAllocator) {
        .mem = NULL,
        .head = NULL,
        .tail = NULL,
    };
}

static bool ensure_capacity(PoolAllocator* pool, size_t size) {
    if (size <= pool->capacity)
        return true;

    size_t byte_cap = pool->capacity * (pool->stride + NODE_HEADER_SIZE);

    void* new_blk = realloc(pool->mem, byte_cap << 1);
    if (new_blk == NULL)
        abort();

    prepare_block(pool, offset(new_blk, byte_cap), pool->capacity);

    pool->capacity <<= 1;
    return true;
}

void pool_clear(PoolAllocator* pool) {
    size_t size = pool_size(pool);
    size_t total_stride = pool->stride + NODE_HEADER_SIZE;
    size_t count = 0;
    for (size_t i = 0; i < pool->capacity && count < size; i++) {
        struct obj_node* node = offset(pool->mem, i * total_stride);
        if (node->allocated) {
            add_node_to_free_list(pool, node);
            count++;
        }
    }
    pool->size = 0;
}

void* pool_alloc(PoolAllocator* pool, long* out_index) {
    if (!ensure_capacity(pool, pool->size + 1))
        abort();

    struct obj_node* node = pool->head;
    void* ptr = offset(node, NODE_HEADER_SIZE);
    pool->head = pool->head->next;

    if (out_index)
        *out_index = ((size_t) ptr - (size_t)pool->mem) / (pool->stride + NODE_HEADER_SIZE);
    pool->size++;
    node->allocated = true;
    node->next = NULL;
    return ptr;
}

static bool free_node(PoolAllocator* pool, struct obj_node* node) {
    if (!node->allocated)
        return false;

    add_node_to_free_list(pool, node);
    return true;
}

bool pool_free(PoolAllocator* pool, void* ptr) {
    // TODO: Check if address is valid

    return free_node(pool, offset(ptr, -NODE_HEADER_SIZE));
}

bool pool_free_idx(PoolAllocator* pool, long idx) {
    if (idx < 0 || (size_t) idx >= pool->capacity)
        return false;

    struct obj_node* node = get_node_from_idx(pool, idx);
    if (!node->allocated)
        return false;

    return pool_free(pool, offset(node, NODE_HEADER_SIZE));
}

void* pool_get(PoolAllocator* pool, long index) {
    if (index < 0 || (size_t) index >= pool->capacity)
        return NULL;

    struct obj_node* node = get_node_from_idx(pool, index);
    if (!node->allocated)
        return NULL;
    return offset(node, NODE_HEADER_SIZE);
}

void pool_foreach(const PoolAllocator* pool, void (*action)(void*, long, void*), void* user_data) {
    size_t size = pool_size(pool);
    size_t total_stride = pool->stride + NODE_HEADER_SIZE;

    size_t idx = 0;
    size_t count = 0;

    for (size_t i = 0; i < pool->capacity && count < size; i++) {
        struct obj_node* node = offset(pool->mem, i * total_stride);
        if (node->allocated) {
            action(offset(node, NODE_HEADER_SIZE), idx, user_data);
            count++;
        }
        idx++;
    }
}
