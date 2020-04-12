#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/kthread.h>  
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/gfp.h>
#include <linux/mm_types.h>
#include <linux/time.h>

#define MY_PAGE_SIZE 4096
#define MAX_NUM 128

static unsigned int param = 2000;
module_param(param, int, 0660);

static struct task_struct *thread;
static struct page *my_page;
static unsigned long page;

struct my_struct {
	unsigned int arr[8];
};

unsigned int getPower(int num) {
	unsigned int pow = -1;
	while(num > 0) {
		num >>= 1;
		++pow;
	}
	return pow;
}

int thread_function(void *data) {
	ssize_t size = sizeof(struct my_struct);
	int num = param / MAX_NUM + 1;
	if(num % 2 != 0) num += 1;
	unsigned int pow = getPower(num);
	if (printk_ratelimit()) {
		printk(KERN_INFO "param value=%u\n", param);
		printk(KERN_INFO "my struct size=%zd\n", size);
		printk(KERN_INFO "num of pages=%d\n", num);
		printk(KERN_INFO "order=%d\n", pow);
	}
	struct timespec before;
	getnstimeofday(&before);

	my_page = alloc_pages(GFP_KERNEL, pow);
	__free_pages(my_page, pow);

	struct timespec after;
	getnstimeofday(&after);
	printk(KERN_INFO "time spent=%ld\n", after.tv_nsec - before.tv_nsec);
	return 0;
}

int init_module(void) {
	printk(KERN_INFO "Hello, %u\n", param);

	thread = kthread_run(thread_function, NULL, "myThread");

	if (thread) 
		printk(KERN_INFO "Thread is created!");
	else
		printk(KERN_INFO "Thread is Not created!");
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "Goodbye :(\n");   
}

MODULE_LICENSE("GPL");