/*
 *	serial.c
 *
 * This module implements the rs232 io functions
 *	void rs_write(struct tty_struct * queue);
 *	void rs_init(void);
 * and all interrupts pertaining to serial IO.
 */

#include <linux/tty.h>
#include <linux/sched.h>
#include <asm/system.h>
#include <asm/io.h>

#define WAKEUP_CHARS (TTY_BUF_SIZE/4)

extern void rs1_interrupt(void);
extern void rs2_interrupt(void);

static void init(int port)
{
	outb_p(0x80,port+3);	/* set DLAB of line control reg */
	outb_p(0x30,port);	/* LS of divisor (48 -> 2400 bps */
	outb_p(0x00,port+1);	/* MS of divisor */
	outb_p(0x03,port+3);	/* reset DLAB */
	outb_p(0x0b,port+4);	/* set DTR,RTS, OUT_2 */
	outb_p(0x0d,port+1);	/* enable all intrs but writes */
	(void)inb(port);	/* read data port to reset things (?) */
}

void rs_init(void)
{
	// idt[0x24] type=14, dpl=0, addr=rs1_interrupt
	set_intr_gate(0x24,rs1_interrupt);

	// idt[0x23] type=14, dpl=0, addr=rs2_interrupt
	set_intr_gate(0x23,rs2_interrupt);
	
	// tty_table[1].read_q.data == 0x3f8
	// outb_p(0x80,0x3f8+3);	/* set DLAB of line control reg */
	// outb_p(0x30,0x3f8);	/* LS of divisor (48 -> 2400 bps */
	// outb_p(0x00,0x3f8+1);	/* MS of divisor */
	// outb_p(0x03,0x3f8+3);	/* reset DLAB */
	// outb_p(0x0b,0x3f8+4);	/* set DTR,RTS, OUT_2 */
	// outb_p(0x0d,0x3f8+1);	/* enable all intrs but writes */
	// (void)inb(0x3f8);	/* read data port to reset things (?) */
	init(tty_table[1].read_q.data);
	
	// tty_table[2].read_q.data == 0x2f8
	// outb_p(0x80,0x2f8+3);	/* set DLAB of line control reg */
	// outb_p(0x30,0x2f8);	/* LS of divisor (48 -> 2400 bps */
	// outb_p(0x00,0x2f8+1);	/* MS of divisor */
	// outb_p(0x03,0x2f8+3);	/* reset DLAB */
	// outb_p(0x0b,0x2f8+4);	/* set DTR,RTS, OUT_2 */
	// outb_p(0x0d,0x2f8+1);	/* enable all intrs but writes */
	// (void)inb(0x2f8);	/* read data port to reset things (?) */
	init(tty_table[2].read_q.data);

	// read one byte from 0x21 port to variable x
	// then write (x & 0xE7) to 0x21 port
	outb(inb_p(0x21)&0xE7,0x21);
}

/*
 * This routine gets called when tty_write has put something into
 * the write_queue. It must check wheter the queue is empty, and
 * set the interrupt register accordingly
 *
 *	void _rs_write(struct tty_struct * tty);
 */
void rs_write(struct tty_struct * tty)
{
	cli();
	if (!EMPTY(tty->write_q))
		outb(inb_p(tty->write_q.data+1)|0x02,tty->write_q.data+1);
	sti();
}
