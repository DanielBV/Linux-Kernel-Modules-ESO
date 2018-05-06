#include <linux/init.h>     
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>  
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/sched.h>
#include <linux/sched/sysctl.h>
#include <linux/types.h>
#include <uapi/linux/sched/types.h>


#define DEVICE_NAME  "quantumBreak"
#define DRIVER_NAME "qb"
#define CLASS_NAME "qb"
#define AUTHOR  "Daniel Bazaco"
#define DESCRIPTION "Changes the scheduling policy of every process that opens the device to FIFO."


/**
 *  Quantum Break kernel Module
 *  
 *  QuantumBreak creates a device in /dev/quantumBreak. It works just as charEcho module, but when the device is opened it changes the process' scheduling
 *  policy to FIFO. This allows the process up to 95 % of CPU use () (http://linuxrealtime.org/index.php/Basic_Linux_from_a_Real-Time_Perspective#Real-Time_Throttling)
 * 
 *  In order to get 100% CPU use, the scheduler throttling can ve avoid by doing:
 *                 sudo su
 *                 echo -1 > /proc/sys/kernel/sched_rt_runtime_us
 */

 
#define LICENSE  "GPL"
#define VERSION "0.00000001"


#define BUF_LEN 102
static int size_of_send_buffer = 0;

static char msg[BUF_LEN];
static char *msg_Ptr;


static dev_t deviceNumber; 
static struct cdev charDev; 
static struct class *cl; 

struct sched_param param = {
				.sched_priority = 99
};
static int my_open(struct inode *i, struct file *f){
  

  printk("%d",sched_setscheduler(current,SCHED_FIFO,&param));

	msg_Ptr = msg;
	size_of_send_buffer  = strlen(msg_Ptr);

	return 0;
}
  static int my_close(struct inode *i, struct file *f){
    return 0;
}
  static ssize_t my_read(struct file *f, char __user *buffer, size_t
  len, loff_t *off){
    int bytes_read = 0;
  
    if (size_of_send_buffer==0)
    /** Este método se ejecuta en bucle infinito hasta que devuelva 0, porque 
     * el 0 marca el final el stream de datos, mientras que el return normal
     * es el número de datos que se han enviado*/
      return 0;
      
    copy_to_user(buffer, msg_Ptr, size_of_send_buffer);
    bytes_read = size_of_send_buffer;
    size_of_send_buffer=0;
    return bytes_read;

}
 
  static struct file_operations pugs_fops =
{
  .owner = THIS_MODULE,
  .open = my_open,
  .release = my_close,
  .read = my_read,
 
};
 
static int __init quantum_break_init(void){

  if (alloc_chrdev_region(&deviceNumber, 0, 1, DRIVER_NAME) < 0)
      return -1;
  
    if ((cl = class_create(THIS_MODULE,CLASS_NAME)) == NULL){
        unregister_chrdev_region( deviceNumber, 1);
        return -1;

    }

    if (device_create(cl, NULL, deviceNumber, NULL, DEVICE_NAME) == NULL) {
          class_destroy(cl);
          unregister_chrdev_region(deviceNumber, 1);
          return -1;
    }

    cdev_init(&charDev, &pugs_fops);

    if (cdev_add(&charDev, deviceNumber, 1) == -1){
        device_destroy(cl, deviceNumber);
        class_destroy(cl);
        unregister_chrdev_region(deviceNumber, 1);
        return -1;
  }

  sprintf(msg, "Quantum Broken \n");

  return 0;
}
 
static void __exit quantum_break_exit(void) {
  cdev_del(&charDev);
  device_destroy(cl, deviceNumber);
  class_destroy(cl);
  unregister_chrdev_region(deviceNumber, 1);
}
 
MODULE_AUTHOR(AUTHOR);
MODULE_DESCRIPTION(DESCRIPTION);
MODULE_LICENSE(LICENSE);
MODULE_VERSION(VERSION); 

module_init(quantum_break_init);
module_exit(quantum_break_exit);