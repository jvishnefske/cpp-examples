typedef enum {
    SUCCESS,
    FAILURE
} Status;

/**
 *  this defines function to manipulate a fifo of templatable containing type.
 * only use DeclareFifo once for each type, then call the generated functions from code.
 */
#define DeclareFifo(T, N) \
typedef struct { \
    int head; \
    int tail; \
    T data[N]; \
} T##Fifo; \
void T##Fifo_init(T##Fifo *fifo){ \
    fifo->head = 0; \
    fifo->tail = 0; \
} \
int T##Fifo_isEmpty(T##Fifo *fifo){ \
    return fifo->head == fifo->tail;\
} \
int T##Fifo_size(T##Fifo *fifo){ \
    return (((fifo->head - fifo->tail) % (N)) + (N)) % (N);\
}\
Status T##Fifo_push(T##Fifo *fifo, T data){ \
    if((N) <= T##Fifo_size(fifo) + 1  ){return FAILURE;}\
    fifo->data[fifo->head] = data; \
    fifo->head = (fifo->head + 1) % (N); \
    return SUCCESS; \
} \
Status T##Fifo_pop(T##Fifo *fifo, T* output){ \
    if(T##Fifo_isEmpty(fifo)){return FAILURE;}\
    *(output) = fifo->data[fifo->tail];\
    fifo->tail = (fifo->tail + 1) % (N);\
    return SUCCESS;\
}
/**
 * static_assert will fail if the condition is false.
 * this uses a switch statement to check if the condition is true.
 */
#define STATIC_ASSERT(condition) \
    do{ switch(0){case 0:case (condition):default:;}} while (0)

#define STATIC_ASSERT_OFFSETOF(structure, member, expected) \
    STATIC_ASSERT(offsetof(structure, member) == (expected))

#define STATIC_ASSERT_EXACT_BYTESIZE(type, size)     \
    STATIC_ASSERT(sizeof(type) == (size))

#define STATIC_ASSERT_EXACT_BITSIZE(type, size)     \
    STATIC_ASSERT(sizeof(type) * CHAR_BIT == (size))

/*byteswap using bitwise operations*/
#define byteswap_16(x) \
    ((((x) & 0xff00) >> 8) | \
    (((x) & 0x00ff) << 8))
#define byteswap_32(x) \
    ((((x) & 0xff000000) >> 24) | \
    (((x) & 0x00ff0000) >>  8) | \
    (((x) & 0x0000ff00) <<  8) | \
    (((x) & 0x000000ff) << 24))

#if 0
//void my_test() {
//    // verify binary float is same length IEEE 754
//    STATIC_ASSERT(sizeof(float) == 4);
//    STATIC_ASSERT(1 == 1);
//    STATIC_ASSERT(byteswap_32(0x12345678) == 0x78563412);
//}
#endif
#if TEST_MACROS_MAIN
typedef int size;
typedef int value;
LOGGER(int, "%d .")
LOGGER(size, "size(%d)")
LOGGER(value, "value(%d)")
DeclareFifo(int, 3)
int test_fifo(){
    intFifo tx;
    intFifo_init(&tx);
    info_int(intFifo_push(&tx, 100));
    int value;
    info_size(intFifo_size(&tx));
    info_int(intFifo_pop(&tx, &value));
    info_value(value);
    info_size(intFifo_size(&tx));
    info_int(intFifo_push(&tx, 100));
    info_int(intFifo_push(&tx, 100));
    info_size(intFifo_size(&tx));
    info_int(intFifo_push(&tx, 100));
    info_size(intFifo_size(&tx));
    info_int(intFifo_pop(&tx, &value));
    info_int(intFifo_pop(&tx, &value));
    info_int(intFifo_push(&tx, 100));
    info_int(intFifo_push(&tx, 100));
    info_size(intFifo_size(&tx));
}
#endif
