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
 *  The module is designed for Intel Architectures. It doesn't support concurrency :( 
 * 
 *  Special Keys:
 *  
 *    - Left Shift released: Value 99999
 *    - Right Shift released: Value 99998

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
#define DESCRIPTION "Creates a char device that reads keyboard input for a process from anywhere, until the user presses Enter or the buffer is filled"
#define LICENSE  "GPL"
#define VERSION "0.6"

// The key with max scancode is E1D1 (in Spanish Keyboard: http://www.kbdlayout.info/KBDSP/scancodes)
// The decimal value is 57629 (5 characters max + \n character)
#define MAX_LEN_CHAR 6 
#define BUF_MIN_CHARACTERS 10 //Minimum chars that the buffer can hold. Depending on the length of the scancodes it can hold more characters more.
#define EXTRA 20 +1 // +1 from \0

#define BUF_LEN BUF_MIN_CHARACTERS*MAX_LEN_CHAR  



static char msg[BUF_LEN+EXTRA];

static int end_read =0;
static int ready_to_read = 0;
static int l_shift_pressed = 0;
static int r_shift_pressed = 0;

static char keyInput;
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

static void process_char( unsigned long data ){


        char tempChar[6];
        int printRelease = 0;

        int key_status = keyInput & KBD_STATUS_MASK; // True = Key released. False = Key pressed
        int scancode = keyInput & KBD_SCANCODE_MASK;

        if ( !(key_status) ){
            // Key pressed

            if (scancode ==42){
                if (l_shift_pressed)
                    return;
                else
                    l_shift_pressed = 1;
            }

             if (scancode ==54){
                if (r_shift_pressed)
                    return;
                else
                    r_shift_pressed = 1;
            }
           
            if (strlen(msg)<BUF_LEN){
                  
                  sprintf(tempChar,"%d",(int)(scancode));
                  strcat(msg,tempChar);
                  strcat(msg, "\n") ;
            }
        }

        if (key_status){
            //Key released

            if (scancode ==42){ // Left shift
                printRelease = 99999;
                l_shift_pressed = 0;
            }
            if (scancode ==54){ //Right shift
                printRelease = 99998;
                r_shift_pressed = 0;
            }

            if (printRelease){
                sprintf(tempChar,"%d",printRelease);
                strcat(msg,tempChar);
                strcat(msg, "\n") ;
            }
          
            

        }

    pr_info("Scan Code %x %s : %d  Otros: %d\n",
        scancode,
        key_status ? "Released" : "Pressed",scancode, key_status);

     if (strlen(msg)>=BUF_LEN || (  (scancode==28) && !key_status  )){
         
        printk("Buffer Full or Enter pressed");
        ready_to_read = 1;
        wake_up_interruptible(&wait_queue); /* awake any reading process */
      
        }
    
  
}

DECLARE_TASKLET( char_tasklet, process_char, 
         NULL );
 
static irqreturn_t kbd_handler(int irq, void *dev_id)
{
   
    keyInput = inb(KBD_DATA_REG);
    tasklet_schedule(&char_tasklet);
      
    return IRQ_HANDLED;
}


  static ssize_t my_read(struct file *f, char __user *buffer, size_t
  len, loff_t *off){

    int bytes_read = 0;

    if (end_read){
        end_read =0;
        return 0;
    }

    request_irq(KBD_IRQ, kbd_handler, IRQF_SHARED, "kbd2", (void *)kbd_handler);
  
    while (!ready_to_read) {
         wait_event_interruptible(wait_queue,ready_to_read);
         printk("Process woken, ready_to_read %d",ready_to_read);
    }
    
    free_irq(KBD_IRQ, (void *)kbd_handler);
    tasklet_kill(&char_tasklet);

    bytes_read = strlen(msg); 
    copy_to_user(buffer, msg, bytes_read); // Assumes that the buffer len is big enough


    ready_to_read = 0;
    end_read=1;
    msg[0]='\0'; // "Empties" the buffer

   
   
    return bytes_read;

}
  
  static struct file_operations pugs_fops =
{
  .owner = THIS_MODULE,
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
   tasklet_kill(&char_tasklet);
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
