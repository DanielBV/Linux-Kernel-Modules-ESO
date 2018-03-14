#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>		
#include <linux/proc_fs.h>	
#include <linux/seq_file.h>	
#include <linux/sched.h>
#include<linux/slab.h>


#define DEVICE_NAME  "Sequence Counter"
#define PROC_NAME "seqCount"
#define AUTHOR  "Daniel Bazaco"
#define DESCRIPTION "Generates a proc sequence file that counts from 0 to 99"
#define LICENSE  "GPL"
#define VERSION "0.00000001"
#define ITERABLE_ARRAY_SIZE 100

 static struct proc_dir_entry* proc_file;
 static int iterable_array[ITERABLE_ARRAY_SIZE];

  void * myseq_start (struct seq_file *m, loff_t *pos){
    
      if (*pos>=ITERABLE_ARRAY_SIZE || *pos<0)
        return NULL;

      return &iterable_array[*pos];
     
  }

  static void *myseq_next(struct seq_file *s, void *v, loff_t *pos)
{
    int* array = (int * ) v;
    array+=1;
    *pos+=1;

    if ((*pos) >= ITERABLE_ARRAY_SIZE|| (*pos)<0)     
          return NULL;
  
    return array;
}

static void myseq_stop(struct seq_file *s, void *v)
{
    printk("Sequence file stopped");
    
}
static int myseq_show(struct seq_file *s, void *v)
{
    int* array = (int * ) v;
    int test =(int)array[0];
    seq_printf(s,"%d\n",test);
    return 0;
}

static struct seq_operations myseq_op = {
        .start =        myseq_start,
        .next =         myseq_next,
        .stop =         myseq_stop,
        .show =         myseq_show,
};


 static int proc_open(struct inode *inode, struct file *file)
 {
        return seq_open(file, &myseq_op);
 }

 static const struct file_operations seqCount_fops = {
     .owner	= THIS_MODULE,
     .open	= proc_open,
     .read	= seq_read,
     .llseek	= seq_lseek,
     .release	= seq_release,
 };

 static int __init seqCount_init(void)
 {
     int i=0;
     for (i=0;i<ITERABLE_ARRAY_SIZE;i++){
         iterable_array[i]=i;
     }
     proc_file = proc_create(PROC_NAME, 0, NULL, &seqCount_fops);
     

     if (!proc_file) {
         return -ENOMEM;
     }

     return 0;
 }

 static void __exit seqCount_exit(void)
 {
     remove_proc_entry(PROC_NAME, NULL);
 }

 module_init(seqCount_init);
 module_exit(seqCount_exit);

 MODULE_AUTHOR(AUTHOR);
 MODULE_DESCRIPTION(DESCRIPTION);
 MODULE_LICENSE(LICENSE);
 MODULE_VERSION(VERSION); 