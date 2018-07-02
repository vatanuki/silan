/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  (C) Copyright 2004, Li Chunqiang (chunqiang_li@c-sky.com)
 *  (C) Copyright 2009, Hu Junshan (junshan_hu@c-sky.com)
 *  (C) Copyright 2011, Dou Shaobin (shaobin_dou@c-sky.com)
 *  (C) Copyright 2011, C-SKY Microsystems Co., Ltd. (www.c-sky.com)
 *
 */

#ifndef __CSKY_UACCESS_H
#define __CSKY_UACCESS_H

/*
 * User space memory access functions
 */
#include <linux/compiler.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/string.h>
#include <asm/segment.h>

#define VERIFY_READ	0
#define VERIFY_WRITE	1

/* We let the MMU do all checking */
static inline int access_ok(int type, const void * addr, unsigned long size)
{
    return (((unsigned long)addr < current_thread_info()->addr_limit.seg) &&
              ((unsigned long)(addr + size) < current_thread_info()->addr_limit.seg));
}
/*
static inline int access_ok(int type, const void * addr, unsigned long size)
{
   return 1;
}
*/

static inline int verify_area(int type, const void * addr, unsigned long size)
{
    return access_ok(type, addr, size) ? 0 : -EFAULT;
}

#define __addr_ok(addr) (access_ok(VERIFY_READ, addr,0))

extern int __put_user_bad(void);

/*
 * Tell gcc we read from memory instead of writing: this is because
 * we do not write to any memory gcc knows about, so there are no
 * aliasing issues.
 */

/*
 * These are the main single-value transfer routines.  They automatically
 * use the right size if we just have the right pointer type.
 *
 * This gets kind of ugly. We want to return _two_ values in "get_user()"
 * and yet we don't want to do any pointers, because that is too much
 * of a performance impact. Thus we have a few rather ugly macros here,
 * and hide all the ugliness from the user.
 *
 * The "__xxx" versions of the user access functions are versions that
 * do not verify the address space, that must have been done previously
 * with a separate "access_ok()" call (this is used when we do multiple
 * accesses to the same area of user memory).
 *
 * As we use the same address space for kernel and user data on
 * Ckcore, we can just do these as direct assignments.  (Of course, the
 * exception handling means that it's no longer "just"...)
 */

#define put_user(x,ptr) \
  __put_user_check((x), (ptr), sizeof(*(ptr)))

#define __put_user(x,ptr) \
  __put_user_nocheck((x), (ptr), sizeof(*(ptr)))

#define __ptr(x) ((unsigned long *)(x))

#define get_user(x,ptr) \
  __get_user_check((x), (ptr), sizeof(*(ptr)))

#define __get_user(x,ptr) \
  __get_user_nocheck((x), (ptr), sizeof(*(ptr)))

#define __put_user_nocheck(x, ptr, size)                                \
({                                                                      \
	long __pu_err=0;                                                \
	typeof(*(ptr)) *__pu_addr = (ptr);                              \
	typeof(*(ptr)) __pu_val = (typeof(*(ptr)))(x);                  \
	if(__pu_addr){                                                  \
		__put_user_size(__pu_val, (__pu_addr), (size), __pu_err);   \
	}                                                               \
	__pu_err;                                                       \
})

#define __put_user_check(x,ptr,size)                                    \
({                                                                      \
	long __pu_err = -EFAULT;                                        \
	typeof(*(ptr)) *__pu_addr = (ptr);                              \
	typeof(*(ptr)) __pu_val = (typeof(*(ptr)))(x);                  \
	if (access_ok(VERIFY_WRITE, __pu_addr, size) && __pu_addr)      \
		__put_user_size(__pu_val, __pu_addr, (size), __pu_err); \
	__pu_err;                                                       \
})

#define __put_user_size(x,ptr,size,retval)                              \
do {                                                                    \
	retval = 0;                                                     \
	switch (size) {                                                 \
		case 1: __put_user_asm_b(x, ptr, retval); break;        \
		case 2: __put_user_asm_h(x, ptr, retval); break;        \
		case 4: __put_user_asm_w(x, ptr, retval); break;        \
		case 8: __put_user_asm_64(x, ptr, retval); break;       \
		default: __put_user_bad();                              \
	}	                                                        \
} while (0)

/*
 * We don't tell gcc that we are accessing memory, but this is OK
 * because we do not write to any memory gcc knows about, so there
 * are no aliasing issues.
 *
 * Note that PC at a fault is the address *after* the faulting
 * instruction.
 */
#define __put_user_asm_b(x, ptr, err)                           \
do{                                                             \
	int errcode;                                            \
	 __asm__ __volatile__(                                  \
	         "1:     stb   %1, (%2,0)        \n"            \
	         "       br    3f                \n"            \
	         "2:     mov   %0, %3            \n"            \
	         "       br    3f                \n"            \
	         ".section __ex_table,\"a\"      \n"            \
	         ".align   2                     \n"            \
	         ".long    1b,2b                 \n"            \
	         ".previous                      \n"            \
	          "3:                            \n"            \
	         :"=r"(err), "=r"(x), "=r"(ptr), "=r"(errcode)  \
	         :"0"(err), "1"(x), "2"(ptr), "3"(-EFAULT)      \
	         : "memory");                                   \
}while(0)

#define __put_user_asm_h(x, ptr, err)                           \
do{                                                             \
	int errcode;                                            \
	 __asm__ __volatile__(                                  \
	         "1:     sth   %1, (%2,0)        \n"            \
	         "       br    3f                \n"            \
	         "2:     mov   %0, %3            \n"            \
	         "       br    3f                \n"            \
	         ".section __ex_table,\"a\"      \n"            \
	         ".align   2                     \n"            \
	         ".long    1b,2b                 \n"            \
	         ".previous                      \n"            \
	          "3:                            \n"            \
	         :"=r"(err), "=r"(x), "=r"(ptr), "=r"(errcode)  \
	         :"0"(err), "1"(x), "2"(ptr), "3"(-EFAULT)      \
	         : "memory");                                   \
}while(0)

#define __put_user_asm_w(x, ptr, err)                           \
do{                                                             \
	int errcode;                                            \
	 __asm__ __volatile__(                                  \
	         "1:     stw   %1, (%2,0)        \n"            \
	         "       br    3f                \n"            \
	         "2:     mov   %0, %3            \n"            \
	         "       br    3f                \n"            \
	         ".section __ex_table,\"a\"      \n"            \
	         ".align   2                     \n"            \
	         ".long    1b,2b                 \n"            \
	         ".previous                      \n"            \
	          "3:                            \n"            \
	         :"=r"(err), "=r"(x), "=r"(ptr), "=r"(errcode)  \
	         :"0"(err), "1"(x), "2"(ptr), "3"(-EFAULT)      \
	         : "memory");                                   \
}while(0)


#define __put_user_asm_64(x, ptr, err)                          \
do{                                                             \
	int tmp;                                                \
	int errcode;                                            \
	typeof(*(ptr)) src = ( typeof(*(ptr)))x;                \
	typeof(*(ptr)) *psrc = &src;                            \
	                                                        \
	__asm__ __volatile__(                                   \
	        "     ldw     %3, (%1, 0)     \n"               \
	        "1:   stw     %3, (%2, 0)     \n"               \
	        "     ldw     %3, (%1, 4)     \n"               \
	        "2:   stw     %3, (%2, 4)     \n"               \
	        "     br      4f              \n"               \
	        "3:   mov     %0, %4          \n"               \
	        "     br      4f              \n"               \
	        ".section __ex_table, \"a\"   \n"               \
	        ".align   2                   \n"               \
	        ".long    1b, 3b              \n"               \
	        ".long    2b, 3b              \n"               \
	        ".previous                    \n"               \
	        "4:                           \n"               \
	        :"=r"(err), "=r"(psrc), "=r"(ptr), "=r"(tmp), "=r"(errcode) \
	        : "0"(err), "1"(psrc), "2"(ptr), "3"(0), "4"(-EFAULT)       \
	        : "memory" );                                   \
}while (0)

#define __get_user_nocheck(x, ptr, size)                        \
({                                                              \
	long  __gu_err;	                                        \
	__get_user_size(x, (ptr), (size), __gu_err);            \
	__gu_err;                                               \
})

#define __get_user_check(x, ptr, size)                          \
({                                                              \
	int __gu_err = -EFAULT;	                                \
	const __typeof__(*(ptr)) __user * __gu_ptr = (ptr);     \
	if (access_ok(VERIFY_READ, __gu_ptr, size) && __gu_ptr) \
		__get_user_size(x, __gu_ptr, (size), __gu_err); \
	__gu_err;                                               \
})

#define __get_user_size(x, ptr, size, retval)                   \
do {                                                            \
	switch (size) {                                         \
		case 1: __get_user_asm_common((x),ptr,"ldb",retval); break; \
		case 2: __get_user_asm_common((x),ptr,"ldh",retval); break; \
		case 4: __get_user_asm_common((x),ptr,"ldw",retval); break; \
		default:                                        \
			x=0;                                    \
			(retval) = __get_user_bad();            \
	}                                                       \
} while (0)

#define __get_user_asm_common(x, ptr, ins, err)                 \
do{                                                             \
	int errcode;                                            \
	__asm__ __volatile__(                                   \
	        "1:   " ins "   %1, (%4,0)      \n"             \
	        "       br    3f                \n"             \
	        /* Fix up codes */                              \
	        "2:     mov   %0, %2            \n"             \
	        "       movi  %1, 0             \n"             \
	        "       br    3f                \n"             \
	        ".section __ex_table,\"a\"      \n"             \
	        ".align   2                     \n"             \
	        ".long    1b,2b                 \n"             \
	        ".previous                      \n"             \
	        "3:                            \n"              \
	        :"=r"(err), "=r"(x), "=r"(errcode)              \
	        :"0"(0), "r"(ptr), "2"(-EFAULT)                 \
	        : "memory");                                    \
}while(0)

extern int __get_user_bad(void);

#define __copy_user(to, from, n)                                \
do{                                                             \
	int w0, w1, w2, w3;                                     \
	__asm__ __volatile__(                                   \
		"0:     cmpnei  %1, 0           \n"             \
		"       bf      8f              \n"             \
		"       mov     %3, %1          \n"             \
		"       or      %3, %2          \n"             \
		"       andi    %3, 3           \n"             \
		"       cmpnei  %3, 0           \n"             \
		"       bf      1f              \n"             \
		"       br      5f              \n"             \
		"1:     cmplti  %0, 16          \n"   /* 4W */  \
		"       bt      3f              \n"             \
		"       ldw     %3, (%2, 0)     \n"             \
		"       ldw     %4, (%2, 4)     \n"             \
		"       ldw     %5, (%2, 8)     \n"             \
		"       ldw     %6, (%2, 12)    \n"             \
		"2:     stw     %3, (%1, 0)     \n"             \
		"       stw     %4, (%1, 4)     \n"             \
		"       stw     %5, (%1, 8)     \n"             \
		"       stw     %6, (%1, 12)    \n"             \
		"       addi    %2, 16          \n"             \
		"       addi    %1, 16          \n"             \
		"       subi    %0, 16          \n"             \
		"       br      1b              \n"             \
		"3:     cmplti  %0, 4           \n"  /* 1W */   \
		"       bt      5f              \n"             \
		"       ldw     %3, (%2, 0)     \n"             \
		"4:     stw     %3, (%1, 0)     \n"             \
		"       addi    %2, 4           \n"             \
		"       addi    %1, 4           \n"             \
		"       subi    %0, 4           \n"             \
		"       br      3b              \n"             \
		"5:     cmpnei  %0, 0           \n"  /* 1B */   \
		"       bf      8f              \n"             \
		"       ldb     %3, (%2, 0)     \n"             \
		"6:     stb     %3, (%1, 0)     \n"             \
		"       addi    %2,  1          \n"             \
		"       addi    %1,  1          \n"             \
		"       subi    %0,  1          \n"             \
		"       br      5b              \n"             \
		"7:     br      8f              \n"             \
		".section __ex_table, \"a\"     \n"             \
		".align   2                     \n"             \
		".long    2b, 7b                \n"             \
		".long    4b, 7b                \n"             \
		".long    6b, 7b                \n"             \
		".previous                      \n"             \
		"8:                             \n"             \
	        : "=r"(n), "=r"(to), "=r"(from), "=r"(w0), "=r"(w1), "=r"(w2), "=r"(w3)   \
		: "0"(n), "1"(to), "2"(from)                    \
		: "memory" );                                   \
} while (0)

#define __copy_user_zeroing(to, from, n) \
do{                                                             \
	int tmp;                                                \
	int nsave;                                              \
	__asm__ __volatile__(                                   \
		"0:     cmpnei  %1, 0           \n"             \
		"       bf      7f              \n"             \
		"       mov     %3, %1          \n"             \
		"       or      %3, %2          \n"             \
		"       andi    %3, 3           \n"             \
		"       cmpnei  %3, 0           \n"             \
		"       bf      1f              \n"             \
		"       br      5f              \n"             \
		"1:     cmplti  %0, 16          \n"   /* 4W */  \
		"       bt      3f              \n"             \
		"2:     ldw     %3, (%2, 0)     \n"             \
		"       ldw     %4, (%2, 4)     \n"             \
		"       stw     %3, (%1, 0)     \n"             \
		"       stw     %4, (%1, 4)     \n"             \
		"       ldw     %3, (%2, 8)     \n"             \
		"       ldw     %4, (%2, 12)    \n"             \
		"       stw     %3, (%1, 8)     \n"             \
		"       stw     %4, (%1, 12)    \n"             \
		"       addi    %2, 16          \n"             \
		"       addi    %1, 16          \n"             \
		"       subi    %0, 16          \n"             \
		"       br      1b              \n"             \
		"3:     cmplti  %0, 4           \n"  /* 1W */   \
		"       bt      5f              \n"             \
		"4:     ldw     %3, (%2, 0)     \n"             \
		"       stw     %3, (%1, 0)     \n"             \
		"       addi    %2, 4           \n"             \
		"       addi    %1, 4           \n"             \
		"       subi    %0, 4           \n"             \
		"       br      3b              \n"             \
		"5:     cmpnei  %0, 0           \n"  /* 1B */   \
		"       bf      7f              \n"             \
		"6:     ldb     %3, (%2, 0)     \n"             \
		"       stb     %3, (%1, 0)     \n"             \
		"       addi    %2,  1          \n"             \
		"       addi    %1,  1          \n"             \
		"       subi    %0,  1          \n"             \
		"       br      5b              \n"             \
		"8:     mov     %3, %0          \n" /* zero */  \
		"       movi    %4, 0           \n"             \
		"9:     stb     %4, (%1, 0)     \n"             \
		"       addi    %1, 1           \n"             \
		"       subi    %3, 1           \n"             \
		"       cmpnei  %3, 0           \n"             \
		"       bt      9b              \n"             \
		"       br      7f              \n"             \
		".section __ex_table, \"a\"     \n"             \
		".align   2                     \n"             \
		".long    2b, 8b                \n"             \
		".long    4b, 8b                \n"             \
		".long    6b, 8b                \n"             \
		".previous                      \n"             \
		"7:                             \n"             \
		: "=r"(n), "=r"(to), "=r"(from), "=r"(nsave), "=r"(tmp)   \
		: "0"(n), "1"(to), "2"(from)                    \
		: "memory" );                                   \
} while (0)

unsigned long __generic_copy_from_user(void *to, const void *from, unsigned long n);
unsigned long __generic_copy_to_user(void *to, const void *from, unsigned long n);

#ifdef CONFIG_MMU
#define copy_from_user(to, from, n) __generic_copy_from_user(to, from, n)
#define copy_to_user(to, from, n) __generic_copy_to_user(to, from, n)

#define __copy_from_user(to, from, n) copy_from_user(to, from, n)
#define __copy_to_user(to, from, n) copy_to_user(to, from, n)
unsigned long clear_user(void *to, unsigned long n);
unsigned long __clear_user(void __user *to, unsigned long n);

#else /*CONFIG_MMU*/
#define __copy_from_user(to,from,n)	(memcpy(to, (void __force *)from, n),0)
#define __copy_to_user(to,from,n)	(memcpy((void __force *)to, from, n), 0)
unsigned long clear_user(void *to, unsigned long n);
unsigned long __clear_user(void __user *to, unsigned long n);

static inline unsigned long __must_check copy_from_user(void *to, const void __user *from, unsigned long n)
{
        if (access_ok(VERIFY_READ, from, n))
                n = __copy_from_user(to, from, n);
        else /* security hole - plug it */
                memset(to, 0, n);
        return n;
}

static inline unsigned long __must_check copy_to_user(void __user *to, const void *from, unsigned long n)
{
        if (access_ok(VERIFY_WRITE, to, n))
                n = __copy_to_user(to, from, n);
        return n;
}

#endif/*CONFIG_MMU*/

#define __copy_to_user_inatomic         __copy_to_user
#define __copy_from_user_inatomic       __copy_from_user

#define copy_to_user_ret(to, from, n, retval) \
	({ if (copy_to_user(to, from, n)) return retval; })

#define copy_from_user_ret(to, from, n, retval) \
	({ if (copy_from_user(to, from, n)) return retval; })

long strncpy_from_user(char *dst, const char *src, long count);
long __strncpy_from_user(char *dst, const char *src, long count);

/*
 * Return the size of a string (including the ending 0)
 *
 * Return 0 on exception, a value greater than N if too long
 */
long strnlen_user(const char *src, long n);

#define strlen_user(str) strnlen_user(str, 32767)

/*
 * Zero Userspace
 */
struct exception_table_entry
{
	unsigned long insn;
	unsigned long nextinsn;
};

extern int fixup_exception(struct pt_regs *regs);

#endif /* _CSKY_UACCESS_H */