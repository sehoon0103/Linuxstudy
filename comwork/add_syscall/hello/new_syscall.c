#include <linux/kernel.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE1(new_syscall, int, code)
{
	printk(KERN_INFO "Hello world %d\n", code);
	return code + 1;
}
