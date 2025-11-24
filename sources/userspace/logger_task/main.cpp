#include <stdstring.h>
#include <stdfile.h>
#include <stdmutex.h>

#include <drivers/bridges/uart_defs.h>

/**
 * Logger task
 * 
 * Prijima vsechny udalosti od ostatnich tasku a oznamuje je skrz UART hostiteli
 **/

static void fputs(uint32_t file, const char* string)
{
	write(file, string, strlen(string));
}

int main(int argc, char** argv)
{
	uint32_t uart_file = open("DEV:uart/0", NFile_Open_Mode::Write_Only);

	TUART_IOCtl_Params params;
	params.baud_rate = NUART_Baud_Rate::BR_115200;
	params.char_length = NUART_Char_Length::Char_8;
	ioctl(uart_file, NIOCtl_Operation::Set_Params, &params);

	fputs(uart_file, "UART task starting!");

    uint32_t memfile = open("SYS:shm/mem", NFile_Open_Mode::Read_Write);
    char out[32];
    memset(out, '\0', 32);
    itoa(memfile, out, 10);
    fputs(uart_file, out);

    char *mem = mmap(0x100000, memfile);
    memset(out, '\0', 32);
    itoa(reinterpret_cast<uint32_t>(mem), out, 16);
    fputs(uart_file, out);

    mem[10] = 'a';

    while(true)
    {
        if (mem[10] == 'a')
        {
            char c[2];
            c[1] = '\0';
            c[0] = mem[10];
            fputs(uart_file, c);
            mem[10] = 'b';
        }
    }

	// char buf[16];
	// char tickbuf[16];
	// bzero(buf, 16);
	// bzero(tickbuf, 16);

	// uint32_t last_tick = 0;

	// uint32_t logpipe = pipe("log", 32);

	// while (true)
	// {
	// 	wait(logpipe, 1, 0x1000);

	// 	uint32_t v = read(logpipe, buf, 15);
	// 	if (v > 0)
	// 	{
	// 		buf[v] = '\0';
	// 		fputs(uart_file, "\r\n[ ");
	// 		uint32_t tick = get_tick_count();
	// 		itoa(tick, tickbuf, 16);
	// 		fputs(uart_file, tickbuf);
	// 		fputs(uart_file, "]: ");
	// 		fputs(uart_file, buf);
	// 	}
	// }

    return 0;
}
