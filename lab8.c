#include </usr/include/linux/kernel.h>
#include </usr/include/linux/init.h>
#include <linux/module.h>
#include <linux/random.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/spinlock.h>
#include <linux/completion.h>
#include <linux/mm.h>
#include <asm/current.h>
#include <linux/mm_types.h>
#include <linux/slab.h>
#define TAG "lab8.1"
#define MAX_NUM 100
#define DELAY 100

static struct task_struct *task1;
static struct task_struct *task2;
static struct task_struct *task3;
static struct task_struct *task4;

DEFINE_SPINLOCK(lock);
DEFINE_SPINLOCK(clock);

static int cnt = 0;
static int tmp;

static int a[MAX_NUM]={0}; //set array size 
static int part = 0;

int gen_int(void){
    int n;
    get_random_bytes(&n, sizeof(n));
    n = n<0 ? n*(-1):n;
    n = n%MAX_NUM;
    return n;
}

static int threadMethod(void *unused){
    int i =0;
int local_cnt; 
int n;
    spin_lock(&lock);
    local_cnt = cnt; //read shared data

    for (i=0;i<5;i++){
        n=gen_int();
         if (printk_ratelimit())
         {
             printk(KERN_INFO "It's %s, random num is: %d, in counter: %d", current->comm, n,local_cnt);
             a[local_cnt]=n;
         }
         udelay[DELAY];
         local_cnt++; //update cnt
         
    }
    counter = local_cnt; //store 
    spin_unlock(&lock);
    return 0;

} 
void myswap(int *xp, int *yp){
    int temp=*xp;
    *xp= *yp;
    *yp=temp;
}

void printArray(void){
    int i; 
    for(i=0;i<MAX_NUM;i+=1)
    {
        printk(KERN_INFO "Sorted by %s, in counter: %d, randomnum is: %d", current->comm, i, a[i]);
    }
}

//merge function 
void merge(int low, int mid, int high)
{
    int k, i, j;
    int* left=kmalloc(sizeof(int)*(mid-low+1), GFP_KERNEL);
    int* right=kmalloc(sizeof(int)*(high-mid), GFP_KERNEL);
    int n1=mid-low+1; //size of left part
    int n2=high-mid; //size of right part 

    for(i =0; i<n1;i++){
        left[i]=a[i+low];
    }
    for(i=0;i<n2;i++){
        right[i]=a[i+mid+1];
    }

    k=low;
    i=j=0;
 
    while(i<n1 && j<n2){
        if (left[i] <= right[j]){
            a[k++]= left[i++];
        }
        else 
            a[k++]= right[j++];
    }

    while (i<n1)
    {
       a[k++]= left[i++];
    }
    
    while (j<n2)
    {
        a[k++]= right[j++];
    }
     
    void merge_sort_function(int low, int high){
        int mid= low+ (high -low) /2;
        if(low<high){
            merge_sort_function(low, mid);
            merge_sort_function(mid+1, high)
            merge(low, mid, high);
        }
    }

    void* merge_sort(void* arg){
        int thread_part;
        int low, high, mid;
        thread_part=part++;

        low=thread_part + (MAX_NUM / 4);
        high=(thread_part+1)*(MAX_NUM/4) -1;

    
        mid=low+ (high -low)/2;
        if(low<high){
            merge_sort_function(low, mid);
            merge_sort_function(mid+1, high);
            merge(low, mid, high);
        }

    }


    static int threadSort(void *arg){
        int thread_part;
        int low, high, mid;
        int i;
        spin_lock(&lock);
        thread_part = part;
        part=thread_part+1; //store 

        //calculate low and high 
         low=thread_part + (MAX_NUM / 4);
        high=(thread_part+1)*(MAX_NUM/4) -1;

        // evaluate mid point 
        mid=low+ (high -low)/2;
        if(low<high){
            merge_sort_function(low, mid);
            merge_sort_function(mid+1, high);
            merge(low, mid, high);
        }
        printk( KERN_INFO "Parts of array: \n low %d \n high %d \n part %d \n", low, high, thread_part);
        for(i=low; i<=high; i++){
            printk(KERN_INFO "Sorting in %s, in counter %d, random num is: %d", current->comm, i, a[i]);
        }
        spin_lock(&lock);
        return 0;

    }


    void createThreads(void){
            spin_lock(&complock);
        task1=kthread_run(threadSort, NULL, "first.th");
        task2=kthread_run(threadSort, NULL, "second.th");
        task3=kthread_run(threadSort, NULL, "third.th");
        task4=kthread_run(threadSort, NULL, "fourth.th");

        spin_lock(&lock);
        merge(0, (MAX_NUM/2 -1)/2, MAX_NUM/2 -1);
        merge(MAX_NUM/2, MAX_NUM/2+(MAX_NUM-1-MAX_NUM/2)/2, MAX_NUM-1);
        merge(0, (MAX_NUM-1)/2, MAX_NUM-1);
        spin_unlock(&lock);
        spin_lock(&lock);
    }

    int init_module(void){
        int n;
        int i;
          printk(KERN_INFO "Start %s\n", TAG);

        for(i=0; i<MAX_NUM;i++){
            n=gen_int();
            a[i]=n;
            if(printk_ratelimit()){
                printk(KERN_INFO "It's %s, random num is :%d, in counter: %d", current->comm, a[i],i);

            }
        }

        createThreads();
    udelay(DELAY);
    spin_lock(&lock);

    printArray();

    spin_unlock(&lock);
    return 0;
    }
    void cleanup_module(void){
        printk(KERN_INFO "End %s\n", TAG);
    }
}
/////