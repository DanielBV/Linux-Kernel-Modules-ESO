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

static int size_of_send_buffer = 0;

static char msg[BUF_LEN];
static char *msg_Ptr;


static dev_t deviceNumber; 
static struct cdev charDev; 
static struct class *cl; 
static int my_open(struct inode *i, struct file *f){
  
	static int counter = 0;
	
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
  static ssize_t my_write(struct file *f, const char __user *buffer,
  size_t len, loff_t *off)
{
  int min = len;
  if (min>100)
      min = 100;
      msg[101] = '\n';
      

  copy_from_user(msg,buffer,min);
  msg [min] ='\0';

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