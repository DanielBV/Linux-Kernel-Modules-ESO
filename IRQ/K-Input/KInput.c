#include <linux/init.h>     
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>  
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>

/**
 *  K-Input Kernel Module  
 *  
 *  K-Input creates a char device in /dev/KInput. When the file is read, the process is set to sleep until the user presses Enter or the buffer is full.
 *  The buffer stores the scancode of the keyboard keys pressed by the user, separated by an end of line (\n).
 *  When the process if awoken, it will read the buffer. 
 * 
 *  The objective of the module is to allow processes to get keyboard input from anywhere, like input() in python but not bound to the shell.
 * 
 *  The module is designed for Intel Architectures. Also it doesn't support concurrent process :(. 
 * 
 */


DECLARE_WAIT_QUEUE_HEAD(wait_queue);
#define KBD_IRQ             1       /* IRQ number for keyboard (i8042) */
#define KBD_DATA_REG        0x60    /* I/O port for keyboard data */
#define KBD_SCANCODE_MASK   0x7f
#define KBD_STATUS_MASK     0x80

#define DEVICE_NAME  "Kinput"
#define DRIVER_NAME "K-Input"
#define CLASS_NAME "key"
#define AUTHOR  "Daniel Bazaco"
#define DESCRIPTION "Creates a char device that reads keyboard input for a process from anywhere, until the user presses Ente (Not working yet)r"
#define LICENSE  "GPL"
#define VERSION "0.1"

// The key with max scancode is E1D1 (in Spanish Keyboard: http://www.kbdlayout.info/KBDSP/scancodes)
// The decimal value is 57629 (5 characters max + \n character)
#define MAX_LEN_CHAR 6 
#define BUF_MIN_CHARACTERS 10 //Minimum chars that the buffer can hold. Depending on the length of the scancodes can be higher.
#define EXTRA 20 +1 // +1 from \0

#define BUF_LEN BUF_MIN_CHARACTERS*MAX_LEN_CHAR  



static char msg[BUF_LEN+EXTRA];

static int end_read =0;
static int file_opened = 0;
static int ready_to_read = 0;

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
    if (file_opened){

        char tempChar[6];
        scancode = inb(KBD_DATA_REG);

        if ( !(scancode & KBD_STATUS_MASK) ){

           
            if (strlen(msg)<BUF_LEN){
                  sprintf(tempChar,"%d",(int)(scancode& KBD_SCANCODE_MASK));
                  strcat(msg,tempChar);
                  strcat(msg, "\n") ;
            }
        }

        pr_info("Scan Code %x %s : %d\n",
            scancode & KBD_SCANCODE_MASK,
            scancode & KBD_STATUS_MASK ? "Released" : "Pressed",scancode & KBD_SCANCODE_MASK);

     if (strlen(msg)>=BUF_LEN || ( ( (scancode & KBD_SCANCODE_MASK)) ==28 &&   !(scancode & KBD_STATUS_MASK))){

        printk("Buffer Full or Enter pressed");
        ready_to_read = 1;
        wake_up_interruptible(&wait_queue); /* awake any reading process */

        }
    }
    return IRQ_HANDLED;
}

static int my_open(struct inode *i, struct file *f){

	return 0;
}
  static int my_close(struct inode *i, struct file *f){
   
  
    return 0;
}
  static ssize_t my_read(struct file *f, char __user *buffer, size_t
  len, loff_t *off){
    
    int bytes_read = 0;
 
    if (end_read){
        end_read =0;
        return 0;
    }

    file_opened = 1;
     
    DEFINE_WAIT(wait);
    while (!ready_to_read) {
      
        prepare_to_wait(&wait_queue, &wait, TASK_INTERRUPTIBLE);
        if (!ready_to_read)
           schedule();
           
          finish_wait(&wait_queue, &wait);
    }

  
    bytes_read = strlen(msg); 
    copy_to_user(buffer, msg, bytes_read); // Assumes that the buffer len is big enough


    file_opened = 0;
    ready_to_read = 0;
    end_read=1;
    msg[0]='\0'; // "Empties" the buffer
   
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

    msg[0]='\0';
    request_irq(KBD_IRQ, kbd_handler, IRQF_SHARED, "kbd2", (void *)kbd_handler);

    cdev_init(&charDev, &pugs_fops);

    if (cdev_add(&charDev, deviceNumber, 1) == -1){
        device_destroy(cl, deviceNumber);
        class_destroy(cl);
        unregister_chrdev_region(deviceNumber, 1);
        return -1;
  }

 
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