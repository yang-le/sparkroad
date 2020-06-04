#define GPIO_A_ODR (*(volatile char*)0x10000000)
#define GPIO_RGB(i) ((i) << 5)

#define UART_ODR (*(volatile char*)0x10000010)
#define UART_IDR (*(volatile char*)0x10000014)
#define UART_BSRR (*(volatile unsigned int*)0x10000018)
#define UART_SR (*(volatile char*)0x1000001C)

void putc(char c)
{
	UART_ODR = c;
	while(UART_SR & 0x01);
}

char getc()
{
	while(!(UART_SR & 0x02)); //Wait for recieve
	UART_SR = 0; //Clear
	return UART_IDR;
}

void puts(const char *s)
{
	while (*s)
		putc(*s++);
}

void printhex(char i)
{
	const char table[]="0123456789ABCDEF";

	putc(table[i >> 4]);
	putc(table[i & 0x0f]);
}

/*void* memset(void *ptr, int value, size_t num)
{
	char *p = (char*)ptr;
	while(num--)
		*p++ = value;

	return ptr;	
}*/

void printdec(unsigned long long l)
{
	int i = 0;
	char dec[19] = {0};

	while(l) {
		dec[i++] = (l % 10) + '0';
		l /= 10;
	}

	while(i)
		putc(dec[--i]);
}

void printhword(unsigned short s)
{
	printhex(s >> 8);
	printhex(s);
}

void printword(unsigned int i)
{
	printhword(i >> 16);
	printhword(i);
}

/*void printdword(unsigned long long l)
{
	printword(l >> 32);
	printword(l);
}*/

void printcrlf()
{
	putc('\r');
	putc('\n');
}

void *memcpy(void *dest, const void *src, int n)
{
	while (n--)
		((char*)dest)[n] = ((char*)src)[n];

	return dest;
}

char memcmp(const void *s1, const void *s2, int n)
{
	for(int i = 0; i < n; i++)
		if(((char*)s1)[i] != ((char*)s2)[i])
			return 0;

	return 1;
}

int strlen(const char *s)
{
	int i = 0;
	while(*s++) ++i;
	return i;
}

char* strcpy(char *dest, const char *src)
{
	while (*src)
		*dest++ = *src++;

	return dest;
}

char strcmp(const char *s1, const char *s2)
{
	while(*s1 || *s2)
		if (*s1++ != *s2++)
			return 0;

	return 1;
}

void print_version()
{
	puts("\r\nPicoRV32 (RV32IMC ISA) running on SparkRoad(EG4S20NG88)\r\n");
}

void dump_memory(int address, int size)
{
	char *dat = (char *)address;
	int disp_address;
	puts("Dump memory from 0x");
	printword(disp_address);
	puts(" size: 0x");
	printword(size);
	printcrlf();
	for(int i = 0; i < size / 16; i++)
	{
		disp_address = address + i * 16;
		printcrlf();
		printword(disp_address);
		putc(' ');
		for (int j = 0; j < 16; ++j) {
			printhex(dat[i * 16 + j]);
			putc(' ');
		}
		for (int j = 0; j < 16; ++j) {
			char c = dat[i * 16 + j];
			if ((0x20 <= c) && (c <= 0x7E))
				putc(c);
			else
				putc('.');
		}
	}
}

void main()
{
	char input_entry[32] = {0};
	unsigned int num_cycles, num_instr;
	int i = 0; //输入指针
	char a = 0;

	UART_BSRR = 69; //24M晶振，波特率115200

	print_version();
	//while(1)
	//{
	//	GPIO_A_ODR = a;
	//	a++;
	//	for(i = 0; i < 100000; i++);
	//}
	while(1)
	{
		i = 0;
		puts("console> ");
		while((a = getc()) != '\r')
		{ /* 处理输入 */
			putc(a);

			if ((a == '\b') || (a == 0x7F)) {
				--i;
				if (i < 0) i = 0;
				input_entry[i] = 0;
				continue;
			}

			input_entry[i++] = a;

			if(i >= 32)
			{
				i = 0;
				puts("\r\nYou 've entered too many characters!\r\nconsole> ");
			}
		}

		puts("\r\n");
		input_entry[i] = 0;

		if(input_entry[0] == 0)
			continue;

		if(strcmp(input_entry, "help") || strcmp(input_entry, "?"))
		{
			print_version();
			puts("help - display this message\r\n");
			puts("dump - dump memory from 0 to 8192\r\n");
			puts("sd - detect spi flash, dump model\r\n");
			puts("uptime - display system clock ticks\r\n");
			puts("exit - executes EBREAK to end the loop\r\n");
			puts("gpio <color> - test gpio, where <color> = 0 ~ 7\r\n");
			continue;
		}
		if(memcmp(input_entry, "gpio ", 5))
		{
			GPIO_A_ODR = GPIO_RGB(input_entry[5] - '0');
			continue;
		}
		if(strcmp(input_entry, "dump"))
		{
			dump_memory(0x00000000, 8192);
			printcrlf();
			continue;
		}
		if(strcmp(input_entry, "uptime"))
		{
			unsigned int lo, hi, tmp;

			__asm__ __volatile__ (
			"1:\n"
			"rdcycleh %0\n"
			"rdcycle %1\n"
			"rdcycleh %2\n"
			"bne %0, %2, 1b"
			: "=&r" (hi), "=&r" (lo), "=&r" (tmp));

			puts("System cycle counter: 0x");
			printword(hi);
			printword(lo);

			__asm__ __volatile__ (
			"1:\n"
			"rdtimeh %0\n"
			"rdtime %1\n"
			"rdtimeh %2\n"
			"bne %0, %2, 1b"
			: "=&r" (hi), "=&r" (lo), "=&r" (tmp));

			unsigned long long time = (((unsigned long long)hi << 32) | lo) * 4167 / 100000000;
			puts("\r\nSystem up time: ");
			printdec(time);
			puts(" ms");

			__asm__ __volatile__ (
			"1:\n"
			"rdinstreth %0\n"
			"rdinstret %1\n"
			"rdinstreth %2\n"
			"bne %0, %2, 1b"
			: "=&r" (hi), "=&r" (lo), "=&r" (tmp));

			puts("\r\nSystem instruction counter: 0x");	;
			printword(hi);
			printword(lo);
			printcrlf();
			continue;
		}
		if(strcmp(input_entry, "exit"))
		{
			puts("exit the main loop\r\n");
			break;
		}
		puts("error: ");
		puts(input_entry);
		puts(" is not a specific command!\r\n");
	}
}