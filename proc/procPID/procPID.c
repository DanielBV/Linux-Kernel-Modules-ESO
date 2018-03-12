#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>		
#include <linux/proc_fs.h>	
#include <linux/seq_file.h>	
#include <linux/sched.h>

#define DEVICE_NAME  "PROCPID"
#define PROC_NAME "procpid"
#define AUTHOR  "Daniel Bazaco"
#define DESCRIPTION "Módulo muy complejo que crea un /proc que devuelve el PID del proceso en ejecución."
#define LICENSE  "GPL"
#define VERSION "0.00000001"
/**
 *  Simple proc file that prints the pid of the caller process.
 * */
 static struct proc_dir_entry* proc_file;

 static int pid_show(struct seq_file *m, void *v)
 {
     seq_printf(m, "pid: %d\n",current->pid);
     return 0;
 }

 static int proc_open(struct inode *inode, struct file *file)
 {
     return single_open(file, pid_show, NULL);
 }

 static const struct file_operations procpid_fops = {
     .owner	= THIS_MODULE,
     .open	= proc_open,
     .read	= seq_read,
     .llseek	= seq_lseek,
     .release	= single_release,
 };

 static int __init procpid_init(void)
 {
     proc_file = proc_create(PROC_NAME, 0, NULL, &procpid_fops);

     if (!proc_file) {
         return -ENOMEM;
     }

     return 0;
 }

 static void __exit procpid_exit(void)
 {
     remove_proc_entry(PROC_NAME, NULL);
 }

 module_init(procpid_init);
 module_exit(procpid_exit);

 MODULE_AUTHOR(AUTHOR);
 MODULE_DESCRIPTION(DESCRIPTION);
 MODULE_LICENSE(LICENSE);
 MODULE_VERSION(VERSION); 