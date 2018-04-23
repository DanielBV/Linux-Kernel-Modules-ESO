#include <linux/init.h>     
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>  
#include <linux/device.h>
#include <linux/cdev.h>

#define DEVICE_NAME  "CharEcho"
#define DRIVER_NAME "char"
#define CLASS_NAME "char"
#define AUTHOR  "Daniel Bazaco"
#define DESCRIPTION "Char device that when read, returns the last string written on it. Max buffer = 100"
#define LICENSE  "GPL"
#define VERSION "0.00000001"

#define BUF_LEN 102



static char msg[BUF_LEN];



static dev_t deviceNumber; 
static struct cdev charDev; 
static struct class *cl; 
static int my_open(struct inode *i, struct file *f){
  
	return 0;
}
  static int my_close(struct inode *i, struct file *f){
    return 0;
}
  static ssize_t my_read(struct file *f, char __user *buffer, size_t
  len, loff_t *off){

    int size_of_send_buffer  = strlen(msg);
    copy_to_user(buffer, msg, size_of_send_buffer);
    msg[0]='\0';
    return size_of_send_buffer; 
 
}
  static ssize_t my_write(struct file *f, const char __user *buffer,
  size_t len, loff_t *off)
{
  int min = len;
  if (min>100)
      min = 100;
    
  
  

  copy_from_user(msg,buffer,min);
  msg[min] = '\n';
  msg [min+1] ='\0';

  //Lies to the process, telling it that has read all the message.
  //But if the message is bigger than the buffer, only copies the buffer size
  return len;
}
  static struct file_operations pugs_fops =
{
  .owner = THIS_MODULE,
  .open = my_open,
  .release = my_close,
  .read = my_read,
  .write = my_write
};
 
static int __init chartest_init(void){

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

  sprintf(msg, "Default Message: Change it by writing in the file (Buffer = 100 chars) \n");

  return 0;
}
 
static void __exit chartest_exit(void) {
  cdev_del(&charDev);
  device_destroy(cl, deviceNumber);
  class_destroy(cl);
  unregister_chrdev_region(deviceNumber, 1);
}
 
MODULE_AUTHOR(AUTHOR);
MODULE_DESCRIPTION(DESCRIPTION);
MODULE_LICENSE(LICENSE);
MODULE_VERSION(VERSION); 

module_init(chartest_init);
module_exit(chartest_exit);