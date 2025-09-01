/**
 * @file
 *
 * Functions related to object pools.
 *
 * An object pool stores elements in a similar way to vectors, except the elements are not packed;
 * their indices do not change.
 *
 * This is particularly useful to keep track of multiple objects which need to be randomly accessed
 * by an identifier (in this implementation identifiers are indices in the pool's underlying array.
 */
#ifndef OBJECT_POOL_H
#define OBJECT_POOL_H

#include <stdbool.h>
#include <stddef.h>

/**
 * Structure representing a pool of objects.
 */
typedef struct PoolAllocator {
    void* mem;
    struct obj_node* head;
    struct obj_node* tail;
    size_t capacity;             /**< The total capacity of this vector. */
    size_t size;                 /**< The number of elements stored inside this vector. */
    size_t stride;               /**< The size in bytes of elements. */
} PoolAllocator;

/**
 * Initializes a new object pool.
 *
 * @param[out] pool A pointer to an object pool structure to initialize.
 * @param[in] arena The arena to use to allocate the pool's memory blocks.
 * @param[in] capacity The maximum number of elements the pool can hold.
 * @param[in] stride The size in bytes of the elements.
 */
void pool_init(PoolAllocator* pool, size_t capacity, size_t stride);

/**
 * Removes all elements from an object pool.
 *
 * @param[inout] pool The pool to empty.
 */
void pool_clear(PoolAllocator* pool);

void pool_destroy(PoolAllocator* pool);

/**
 * Adds an element to an object pool.
 *
 * @param[inout] pool The pool to add an element into.
 * @param[out] out_index A pointer to an integer variable written with the index at which the
 * element was added in the pool.
 * @return A pointer to the object's slot inside the pool.
 */
void* pool_alloc(PoolAllocator* pool, long* out_index);

bool pool_free(PoolAllocator* pool, void* ptr);

/**
 * Removes an element from an object pool.
 *
 * @param[inout] pool The pool to remove an element from.
 * @param[in] idx The index of the element to remove, as returned when adding the element with @ref
 * pool_add.
 * @return @ref TRUE if the element exists and has been removed, @ref FALSE otherwise.
 */
bool pool_free_idx(PoolAllocator* pool, long idx);


/**
 * Retrieves an element from an object pool.
 *
 * @param[in] pool The object pool to query.
 * @param[in] index The index of the element to retrieve.
 * @return A non-null pointer pointing to the element if it exists, `NULL` otherwise.
 */
void* pool_get(PoolAllocator* pool, long index);

/**
 * Performs an action on all the elements of an object pool.
 *
 * @param[in] pool The object pool to iterate.
 * @param[in] action The action to perform.
 * @param[in] user_data Data passed to each call of @p action.
 */
void pool_foreach(const PoolAllocator* pool, void (*action)(void*, long, void*), void* user_data);

#define pool_size(pool) ((pool)->size)
#define pool_cap(pool) ((pool)->capacity)
#define pool_stride(pool) ((pool)->stride)
#define pool_is_dynamic(pool) ((pool)->dynamic)

#endif /* ! OBJECT_POOL_H */
