/* vim: tabstop=4 shiftwidth=4 noexpandtab
 */
#ifndef __SYSTEM_H
#define __SYSTEM_H
#include <types.h>
#include <fs.h>
#include <va_list.h>
#include <list.h>
#include <task.h>
#include <process.h>

#define STR(x) #x
#define STRSTR(x) STR(x)

#define asm __asm__
#define volatile __volatile__

extern unsigned int __irq_sem;

#define IRQ_OFF { asm volatile ("cli"); }
#define IRQ_RES { asm volatile ("sti"); }
#define PAUSE   { asm volatile ("hlt"); }

#define STOP while (1) { PAUSE; }

#define SYSCALL_VECTOR 0x7F
#define SIGNAL_RETURN 0xFFFFDEAF
#define THREAD_RETURN 0xFFFFB00F

extern void * code;
extern void * end;

extern char * boot_arg; /* Argument to pass to init */
extern char * boot_arg_extra; /* Extra data to pass to init */

extern void *sbrk(uintptr_t increment);

extern void spin_lock(uint8_t volatile * lock);
extern void spin_unlock(uint8_t volatile * lock);

extern void return_to_userspace(void);

/* Kernel Main */
extern int max(int,int);
extern int min(int,int);
extern int abs(int);
extern void swap(int *, int *);
extern void *memcpy(void *restrict dest, const void *restrict src, size_t count);
extern void *memmove(void *restrict dest, const void *restrict src, size_t count);
extern void *memset(void *dest, int val, size_t count);
extern unsigned short *memsetw(unsigned short *dest, unsigned short val, int count);
extern uint32_t strlen(const char *str);
extern char * strdup(const char *str);
extern char * strcpy(char * dest, const char * src);
extern int atoi(const char *str);
extern unsigned char inportb(unsigned short _port);
extern void outportb(unsigned short _port, unsigned char _data);
extern unsigned short inports(unsigned short _port);
extern void outports(unsigned short _port, unsigned short _data);
extern unsigned int inportl(unsigned short _port);
extern void outportl(unsigned short _port, unsigned int _data);
extern void outportsm(unsigned short port, unsigned char * data, unsigned long size);
extern void inportsm(unsigned short port, unsigned char * data, unsigned long size);
extern int strcmp(const char *a, const char *b);
extern char * strtok_r(char * str, const char * delim, char ** saveptr);
extern size_t lfind(const char * str, const char accept);
extern size_t rfind(const char * str, const char accept);
extern size_t strspn(const char * str, const char * accept);
extern char * strpbrk(const char * str, const char * accept);
extern uint32_t krand(void);
extern char * strstr(const char * haystack, const char * needle);
extern uint8_t startswith(const char * str, const char * accept);

/* GDT */
extern void gdt_install(void);
extern void gdt_set_gate(size_t num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);
extern void set_kernel_stack(uintptr_t stack);

/* IDT */
extern void idt_install(void);
extern void idt_set_gate(unsigned char num, void (*base)(void), unsigned short sel, unsigned char flags);

/* Registers */
struct regs {
	unsigned int gs, fs, es, ds;
	unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
	unsigned int int_no, err_code;
	unsigned int eip, cs, eflags, useresp, ss;
};

typedef struct regs regs_t;

typedef void (*irq_handler_t) (struct regs *);

/* Panic */
#define HALT_AND_CATCH_FIRE(mesg, regs) halt_and_catch_fire(mesg, __FILE__, __LINE__, regs)
#define assert(statement) ((statement) ? (void)0 : assert_failed(__FILE__, __LINE__, #statement))
void halt_and_catch_fire(char *error_message, const char *file, int line, struct regs * regs);
void assert_failed(const char *file, uint32_t line, const char *desc);

/* ISRS */
extern void isrs_install(void);
extern void isrs_install_handler(size_t isrs, irq_handler_t);
extern void isrs_uninstall_handler(size_t isrs);

/* Interrupt Handlers */
extern void irq_install(void);
extern void irq_install_handler(size_t irq, irq_handler_t);
extern void irq_uninstall_handler(size_t irq);
extern void irq_gates(void);
extern void irq_ack(size_t);

/* Timer */
extern void timer_install(void);
extern unsigned long timer_ticks;
extern unsigned char timer_subticks;
extern void relative_time(unsigned long seconds, unsigned long subseconds, unsigned long * out_seconds, unsigned long * out_subseconds);

/* Memory Management */
extern uintptr_t placement_pointer;
extern void kmalloc_startat(uintptr_t address);
extern uintptr_t kmalloc_real(size_t size, int align, uintptr_t * phys);
extern uintptr_t kmalloc(size_t size);
extern uintptr_t kvmalloc(size_t size);
extern uintptr_t kmalloc_p(size_t size, uintptr_t * phys);
extern uintptr_t kvmalloc_p(size_t size, uintptr_t * phys);

// Page types moved to task.h

extern page_directory_t *kernel_directory;
extern page_directory_t *current_directory;

extern void paging_install(uint32_t memsize);
extern void paging_prestart(void);
extern void paging_finalize(void);
extern void paging_mark_system(uint64_t addr);
extern void switch_page_directory(page_directory_t * new);
extern void invalidate_page_tables(void);
extern void invalidate_tables_at(uintptr_t addr);
extern page_t *get_page(uintptr_t address, int make, page_directory_t * dir);
extern void page_fault(struct regs *r);
extern void dma_frame(page_t * page, int, int, uintptr_t);
extern void debug_print_directory(page_directory_t *);

int debug_shell_start(void);

void heap_install(void);

void alloc_frame(page_t *page, int is_kernel, int is_writeable);
void free_frame(page_t *page);
uintptr_t memory_use(void);
uintptr_t memory_total(void);

/* klmalloc */
void * __attribute__ ((malloc)) malloc(size_t size);
void * __attribute__ ((malloc)) realloc(void *ptr, size_t size);
void * __attribute__ ((malloc)) calloc(size_t nmemb, size_t size);
void * __attribute__ ((malloc)) valloc(size_t size);
void free(void *ptr);

/* Tasks */
extern uintptr_t read_eip(void);
extern void copy_page_physical(uint32_t, uint32_t);
extern page_directory_t * clone_directory(page_directory_t * src);
extern page_table_t * clone_table(page_table_t * src, uintptr_t * physAddr);
extern void move_stack(void *new_stack_start, size_t size);
extern void kexit(int retval);
extern void task_exit(int retval);
extern uint32_t next_pid;

extern void tasking_install(void);
extern void switch_task(uint8_t reschedule);
extern void switch_next(void);
extern uint32_t fork(void);
extern uint32_t clone(uintptr_t new_stack, uintptr_t thread_func, uintptr_t arg);
extern uint32_t getpid(void);
extern void enter_user_jmp(uintptr_t location, int argc, char ** argv, uintptr_t stack);

extern uintptr_t initial_esp;

/* Kernel Argument Parser */
extern void parse_args(char * argv);

/* CMOS */
extern void get_time(uint16_t * hours, uint16_t * minutes, uint16_t * seconds);
extern void get_date(uint16_t * month, uint16_t * day);

struct timeval {
	uint32_t tv_sec;
	uint32_t tv_usec;
};

extern int gettimeofday(struct timeval * t, void * z);
extern uint32_t now(void);


/* Floating Point Unit */
extern void switch_fpu(void);
extern void fpu_install(void);

/* ELF */
extern int exec( char *, int, char **, char **);
extern int system( char *, int, char **);

/* Sytem Calls */
extern void syscalls_install(void);

/* wakeup queue */
extern int wakeup_queue(list_t * queue);
extern int wakeup_queue_interrupted(list_t * queue);
extern int sleep_on(list_t * queue);

typedef struct {
	uint32_t  signum;
	uintptr_t handler;
	regs_t registers_before;
} signal_t;

extern void handle_signal(process_t *, signal_t *);

extern int send_signal(pid_t process, uint32_t signal);

#define USER_STACK_BOTTOM 0xAFF00000
#define USER_STACK_TOP    0xB0000000
#define SHM_START         0xB0000000

extern void validate(void * ptr);
extern int validate_safe(void * ptr);

#include <errno_defs.h>

#endif
