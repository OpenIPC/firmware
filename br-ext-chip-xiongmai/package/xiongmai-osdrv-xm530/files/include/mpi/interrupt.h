#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__
/*
 * COPYRIGHT (C) 2013-2014, Shanghai Real-Thread Technology Co., Ltd
 *
 *  All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

void aos_hw_interrupt_clear(int vector);

/*
 * Interrupt handler definition
 */
#define MAX_HANDLERS               IRQ_XM510_END
#define INTERRUPT_NAME_MAX	16
typedef void (*rt_isr_handler_t)(int vector, void *param);

struct rt_irq_desc
{
    rt_isr_handler_t handler;
    void            *param;
    char             name[INTERRUPT_NAME_MAX];
    uint32_t      counter;
};

/*
 * Interrupt interfaces
 */
 #if 0
void rt_hw_interrupt_init(void);
void rt_hw_interrupt_mask(int vector);
void rt_hw_interrupt_umask(int vector);
rt_isr_handler_t rt_hw_interrupt_install(int              vector,
                                         rt_isr_handler_t handler,
                                         void            *param,
                                         char            *name);
#else
typedef void (*irq_hdlr_t)(int irq, void *param);
void os_hw_interrupt_enable(int irq);
irq_hdlr_t os_hw_interrupt_create(int irq, irq_hdlr_t irq_handler,void *param, char *name);
#endif


#endif /* end of include guard: __INTERRUPT_H__ */

