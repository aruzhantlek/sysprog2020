#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/time.h>
#include <linux/hrtimer.h>

#define TAG "Tlek Aruzhan Lab9"
//#define MS_TO_NS(x)     (x * 1E6L)
MODULE_LICENSE("GPL");

static struct timer_list mytimer;
static struct tm res;
static struct timeval now;
static int fibnum;

unsigned long timer_interval_ns = 1e3;
static struct hrtimer hr_timer;


void exampleTimerFunc(struct timer_list *t) {
	int ret;
	do_gettimeofday(&now);
	time_to_tm(now.tv_sec, 0, &res);
	printk(KERN_INFO "%02d:%02d:%02d", res.tm_hour, res.tm_min, res.tm_sec);

	ret = mod_timer(&mytimer, jiffies + msecs_to_jiffies(1000));
	if (ret) {
		printk("Error in mod_timer\n");
	}
}

int fib(int n) 
{ 
   if (n <= 1) 
      return n; 
   return fib(n-1) + fib(n-2); 
} 

enum hrtimer_restart timer_callback( struct hrtimer *timer )
{
    ktime_t currtime, kt;
    int n;
    
    unsigned long delay_in_ms = 1e9;
    n= fibnum;
    currtime  = ktime_get();
    kt = ktime_set( 0, delay_in_ms * (unsigned long) fib(n));
    
    
    hrtimer_forward(timer, timer->_softexpires, kt);
 
    fibnum = n + 1;
  printk( "Timer callback (%ld). With fibnum: %d \n", jiffies, fib(n));
    
  return HRTIMER_RESTART;
}

void exampleWithTimer(void) {
    ktime_t ktime;
 
    unsigned long delay_in_ms = 1L;

    printk("HR Timer module installing\n");

    ktime = ktime_set( 0, MS_TO_NS(delay_in_ms) );

    hrtimer_init( &hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL );
    
    hr_timer.function = &fibTF_callback;

    printk( "Starting timer to fire in %ldms (%ld)\n", 
    delay_in_ms, jiffies );

    hrtimer_start( &hr_timer, ktime, HRTIMER_MODE_REL );

    add_timer(&mytimer); 

}

int init_module(void) {
   printk(KERN_INFO "Starting\n", TAG);
   printk("Starting timer to fire in 1000ms (%ld)\n", jiffies);
   mytimer.expires = jiffies + msecs_to_jiffies(1000);
   timer_setup(&mytimer, exampleTimerFunc, 0);

   exampleWithTimer();
   return 0;
}

void cleanup_module(void) {
   printk(KERN_INFO "Cleanup\n");
   del_timer(&mytimer);
} 