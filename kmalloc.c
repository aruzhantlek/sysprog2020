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

struct my_struct {
	unsigned int arr[8];
};

int thread_function(void *data) {	
	struct timespec before;
	getnstimeofday(&before);
	
	int i;
	for(i = 0; i < param; i++) {
		struct my_struct *ptr = kmalloc(sizeof(struct my_struct), GFP_KERNEL);
		if(!ptr) {
			printk(KERN_INFO "error!");
		} else {
			kfree(ptr);
		}
	}
	
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