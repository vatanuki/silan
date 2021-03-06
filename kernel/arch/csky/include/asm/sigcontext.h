/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  (C) Copyright 2004, Li Chunqiang (chunqiang_li@c-sky.com)
 *  (C) Copyright 2009, Hu Junshan (junshan_hu@c-sky.com)
 *  (C) Copyright 2009, C-SKY Microsystems Co., Ltd. (www.c-sky.com)
 *  
 */ 
 
#ifndef _ASM_CSKY_SIGCONTEXT_H
#define _ASM_CSKY_SIGCONTEXT_H

struct sigcontext {
	unsigned long   sc_mask;     /* old sigmask */
        unsigned long   sc_usp;      /* old user stack pointer */
        unsigned long   sc_a0;
        unsigned long   sc_a1;
        unsigned long   sc_a2;
        unsigned long   sc_a3;
	// ABIV2: r4 ~ r13; ABIV1: r6 ~ r14, r1.
        unsigned long   sc_regs[10];
        unsigned long   sc_r15;
#if (__CSKY__ == 2)                     /* config CPU=cskyv2(ck800) */ 
	// r16 ~ r31;
        unsigned long   sc_exregs[16];
        unsigned long   sc_rhi;
        unsigned long   sc_rlo;
#endif
        unsigned long   sc_sr;
        unsigned long   sc_pc;
	/* fpu */
        unsigned long   sc_fcr;
        unsigned long   sc_fsr;		/* Nothing in CPU_CSKYV2 */
        unsigned long   sc_fesr;
        unsigned long   sc_feinst1;	/* Nothing in CPU_CSKYV2 */
        unsigned long   sc_feinst2;	/* Nothing in CPU_CSKYV2 */
        unsigned long   sc_fpregs[32];
};

#endif /* _ASM_CSKY_SIGCONTEXT_H */
