/*
* 用户态使用fs寄存器引用线程的glibc TLS和线程在用户态的stack canary; 
* 用户态的glibc不适用gs寄存器; 应用可以自行决定是否使用该寄存器
*
* 内核态使用gs寄存器引用percpu变量和进程在内核态的stack canary;
* 内核态不使用fs寄存器
*/
extern inline unsigned char get_fs_byte(const char * addr)
{
	unsigned register char _v;

	__asm__ ("movb %%fs:%1,%0":"=r" (_v):"m" (*addr));
	return _v;

	/*
	*movb %fs:($addr), %al
	*movl %eax, ($&_v)
	*movl ($&_v), %eax
	*/
}

extern inline unsigned short get_fs_word(const unsigned short *addr)
{
	unsigned short _v;

	__asm__ ("movw %%fs:%1,%0":"=r" (_v):"m" (*addr));
	return _v;
}

extern inline unsigned long get_fs_long(const unsigned long *addr)
{
	unsigned long _v;

	__asm__ ("movl %%fs:%1,%0":"=r" (_v):"m" (*addr)); \
	return _v;
}

extern inline void put_fs_byte(char val,char *addr)
{
__asm__ ("movb %0,%%fs:%1"::"r" (val),"m" (*addr));
}

extern inline void put_fs_word(short val,short * addr)
{
__asm__ ("movw %0,%%fs:%1"::"r" (val),"m" (*addr));
}

extern inline void put_fs_long(unsigned long val,unsigned long * addr)
{
__asm__ ("movl %0,%%fs:%1"::"r" (val),"m" (*addr));
}
