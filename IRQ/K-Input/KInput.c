#include <linux/init.h>     
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>  
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/sched.h>

#define KBD_IRQ             1       /* IRQ number for keyboard (i8042) */
#define KBD_DATA_REG        0x60    /* I/O port for keyboard data */
#define KBD_SCANCODE_MASK   0x7f
#define KBD_STATUS_MASK     0x80

#define DEVICE_NAME  "kinput"
#define DRIVER_NAME "K-Input"
#define CLASS_NAME "keyr"
#define AUTHOR  "Daniel Bazaco"
#define DESCRIPTION "Creates a char device that reads keyboard input for a process from anywhere, until the user presses Ente (Not working yet)r"
#define LICENSE  "GPL"
#define VERSION "0.1"

#define BUF_LEN 102

static int size_of_send_buffer = 0;

static char msg[BUF_LEN];


static char scancode;
static dev_t deviceNumber; 
static struct cdev charDev; 
static struct class *cl; 

char * strcat(char *dest, const char *src)
{
    size_t i,j;
    for (i = 0; dest[i] != '\0'; i++)
        ;
    for (j = 0; src[j] != '\0'; j++)
        dest[i+j] = src[j];
    dest[i+j] = '\0';
    return dest;
}

static irqreturn_t kbd_handler(int irq, void *dev_id)
{
    
    char tempChar[6];
    scancode = inb(KBD_DATA_REG);
    if ( !(scancode & KBD_STATUS_MASK) ){
        sprintf(tempChar,"%d",(int)(scancode& KBD_SCANCODE_MASK));
        if (strlen(msg)<BUF_LEN-8){
              strcat(msg,tempChar);
              strcat(msg, "\n") ;
      
        }
    }
    


    size_of_send_buffer = strlen(msg);

   
    pr_info("Scan Code %x %s\n",
            scancode & KBD_SCANCODE_MASK,
            scancode & KBD_STATUS_MASK ? "Released" : "Pressed");
    
    return IRQ_HANDLED;
}

static int my_open(struct inode *i, struct file *f){
  
	static int counter = 0;
	

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
      
    copy_to_user(buffer, msg, size_of_send_buffer);
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
 
static int __init kinput_init(void){
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

  sprintf(msg, "");
  request_irq(KBD_IRQ, kbd_handler, IRQF_SHARED, "kbd2", (void *)kbd_handler);
  return 0;
}
 
static void __exit kinput_exit(void) {
        free_irq(KBD_IRQ, (void *)kbd_handler);
  cdev_del(&charDev);
  device_destroy(cl, deviceNumber);
  class_destroy(cl);
  unregister_chrdev_region(deviceNumber, 1);
}
 
MODULE_AUTHOR(AUTHOR);
MODULE_DESCRIPTION(DESCRIPTION);
MODULE_LICENSE(LICENSE);
MODULE_VERSION(VERSION); 

module_init(kinput_init);
module_exit(kinput_exit);