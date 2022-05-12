// c ring buffer of fixed size defines array, and helper functions enqueue_NAME dequeue_NAME isfull_NAME isempty_NAME
#define FAIL 0
#define SUCCESS 1
#define buffer_def(type,size,name) \
    type name[size] = {0}; \
    size_t name##_tail = 0; \
    size_t name##_head = 0; \
    size_t name##_size  = size; \
    \
    int  isempty_##name() { return name##_head == name##_tail; } \
    int  isfull_##name()  { return (name##_tail + 1) % name##_size == (name##_head + 1); } \
    int  enqueue_##name(type v) { \
        if (isfull_##name()) \
            return FAIL; \
        name[name##_tail] = v; \
        name##_tail = (name##_tail + 1) % name##_size; \
        return SUCCESS; \
    } \
    int  dequeue_##name(type* v) { \
        if (isempty_##name()) \
            return FAIL; \
        *v = name[name##_head]; \
        name##_head = (name##_head + 1) % name##_size; \
        return SUCCESS; \
    } \

// example: 
//     buffer_def(int, 10, queue);
//     enqueue_queue(10);
//     dequeue_queue(&i);
//
//     buffer_def(int, 10, queue);
//     int i;
//     for (int i = 0; i < 10; ++i)
//     {
//         enqueue_queue(i);
//     }
//     for (int i = 0; i < 10; ++i)
//     {
//         dequeue_queue(&i);
//         printf("%d\n", i);
//     }
//
