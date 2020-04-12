#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/kthread.h>  
#include <linux/moduleparam.h>
#include <linux/time.h>

static unsigned int param = 2000;
module_param(param, int, 0660);

static struct task_struct *thread;
static struct kmem_cache *my_cache;
struct my_struct {
	unsigned int arr[8];
};

int thread_function(void *data) {	
	struct timespec before;
	getnstimeofday(&before);
	
	int i;
	for(i = 0; i < param; i++) {
		struct my_struct *ptr = kmem_cache_alloc(my_cache, GFP_KERNEL);
		if (!ptr) {
			printk(KERN_INFO "error!");
		} else {
			kmem_cache_free(my_cache, ptr);
		}
	}
	
	struct timespec after;
	getnstimeofday(&after);
	printk(KERN_INFO "time spent=%ld\n", after.tv_nsec - before.tv_nsec);
	return 0;
}

int init_module(void) {
	printk(KERN_INFO "Hello, %u\n", param);
	my_cache = kmem_cache_create("my cache", sizeof(struct my_struct), ARCH_MIN_TASKALIGN, 0, NULL);
	if (my_cache)
		printk(KERN_INFO "Cache is created!");
	thread = kthread_run(thread_function, NULL, "myThread");

	if (thread) 
		printk(KERN_INFO "Thread is created!");
	else
		printk(KERN_INFO "Thread is Not created!");
	return 0;
}

void cleanup_module(void)
{
	kmem_cache_destroy(my_cache);
	printk(KERN_INFO "Goodbye :(\n");   
}

MODULE_LICENSE("GPL");