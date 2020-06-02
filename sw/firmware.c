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
	volatile int j;
	while (*s)
	{
		putc(*s++);
	}
}

void *memcpy(void *dest, const void *src, int n)
{
	while (n)
	{
		n--;
		((char*)dest)[n] = ((char*)src)[n];
	}
	return dest;
}

char memcmp(void *s1, void *s2, int n)
{
	int i;
	for(i = 0; i < n; i++)
	{
		if(((char*)s1)[i] != ((char*)s2)[i])
			return 0;
	}
	return 1;
}

void print_version()
{
	puts("\r\nPicoRV32 (RV32I ISA) running on SparkRoad(EG4S20NG88)\r\n");
}

void printhex(char i)
{
	char table[]="0123456789ABCDEF";

	putc(table[i >> 4]);
	putc(table[i & 0x0f]);
}

void printcrlf()
{
	putc('\r');
	putc('\n');
}

void dump_memory(int address, int size)
{
	int i, j;
	char *dat = (char *)address;
	int disp_address;
	puts("Dump memory from 0x");
	printhex(disp_address >> 24);
	printhex(disp_address >> 16);
	printhex(disp_address >> 8);
	printhex(disp_address & 0xff);
	puts(" size: 0x");
	printhex(size >> 8);
	printhex(size & 0xff);
	printcrlf();
	for(i = 0; i < size / 16; i++)
	{
		disp_address = address + i * 16;
		printcrlf();
		printhex(disp_address >> 24);
		printhex(disp_address >> 16);
		printhex(disp_address >> 8);
		printhex(disp_address & 0xff);
		putc(' ');
		for (j = 0; j < 16; ++j) {
			printhex(dat[i * 16 + j]);
			putc(' ');
		}
		for (j = 0; j < 16; ++j) {
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

		if(memcmp(input_entry, "help", 4) || input_entry[0] == '?')
		{
			print_version();
			puts("help - display this message\r\n");
			puts("d - dump memory from 0 to 4096\r\n");
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
		if(memcmp(input_entry, "d", 1))
		{
			dump_memory(0x00000000, 4096);
			printcrlf();
			continue;
		}
		if(memcmp(input_entry, "uptime", 6))
		{
			__asm__("rdcycle %0; rdinstret %1;" : "=r"(num_cycles), "=r"(num_instr));
			puts("System cycle counter: 0x");
			printhex(num_cycles >> 24);
			printhex(num_cycles >> 16);
			printhex(num_cycles >> 8);
			printhex(num_cycles & 0xff);
			puts("\r\nSystem instruction counter: 0x");	;
			printhex(num_instr >> 24);
			printhex(num_instr >> 16);
			printhex(num_instr >> 8);
			printhex(num_instr & 0xff);
			printcrlf();
			continue;
		}
		if(memcmp(input_entry, "exit", 4))
		{
			puts("exit the main loop\r\n");
			break;
		}
		puts("error: ");
		puts(input_entry);
		puts(" is not a specific command!\r\n");
	}
}