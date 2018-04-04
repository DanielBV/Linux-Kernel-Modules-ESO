
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>		
#include <linux/proc_fs.h>	
#include <linux/seq_file.h>	
#include <linux/sched.h>
#include<linux/slab.h>
#include <linux/init_task.h>

#define DEVICE_NAME  "Process Getter Proc"
#define PROC_NAME "procProcess"
#define AUTHOR  "Daniel Bazaco"
#define DESCRIPTION "Generates a proc file that prints the name, pid and scheduling policy of every process"
#define LICENSE  "GPL"
#define VERSION "0.00000001"


 static struct proc_dir_entry* proc_file;
 
  void * myseq_start (struct seq_file *m, loff_t *pos){
      printk("Sequence started offset: %lld",*pos);
      int i;
      struct task_struct *init;

      init = &init_task;

      if (*pos<0)
        return NULL;

      for (i=0;i<*pos;i++){
          init = next_task(init);
          if (init == &init_task){
               printk("End of sequence in myseq_start, offset: %lld",*pos);
               return NULL;

          }
      }


      return init;
     
  }

  static void *myseq_next(struct seq_file *s, void *v, loff_t *pos)
{
    struct task_struct *task =(struct task_struct*) v;
    struct task_struct *nextTask = next_task(task);
     *pos+=1;
    if (nextTask == &init_task)
        return NULL;
   
  
    return nextTask;
}

static void myseq_stop(struct seq_file *s, void *v)
{
    printk("Sequence stopped");
    
}
static int myseq_show(struct seq_file *s, void *v)
{
    struct task_struct *task =(struct task_struct*) v;

    seq_printf(s,"%s:  pid=[%d] policy = [%d] \n", task->comm, task->pid,task->policy);
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

 static const struct file_operations procProcess_fops = {
     .owner	= THIS_MODULE,
     .open	= proc_open,
     .read	= seq_read,
     .llseek	= seq_lseek,
     .release	= seq_release,
 };

 static int __init procProcess_init(void)
 {
   
     proc_file = proc_create(PROC_NAME, 0, NULL, &procProcess_fops);
    
     if (!proc_file) {
         return -ENOMEM;
     }

     return 0;
 }

 static void __exit procProcess_exit(void)
 {
     remove_proc_entry(PROC_NAME, NULL);
 }

 module_init(procProcess_init);
 module_exit(procProcess_exit);

 MODULE_AUTHOR(AUTHOR);
 MODULE_DESCRIPTION(DESCRIPTION);
 MODULE_LICENSE(LICENSE);
 MODULE_VERSION(VERSION); 