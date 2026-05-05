#include <linux/kernel.h>
#include <linux/syscalls.h>

#define STACK_SIZE (256 / sizeof(long))

static int top = 0;
static long stack[STACK_SIZE];

SYSCALL_DEFINE1(push_data, long, data) {
    if (top >= STACK_SIZE) {
        return -1;
    }
    stack[top++] = data;
    return 0;
}

SYSCALL_DEFINE0(pop_data) {
    if (top <= 0) {
        return -1;
    }
    return stack[--top];
}
