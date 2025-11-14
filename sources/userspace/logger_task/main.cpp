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

    uint32_t uart_file = open("DEV:uart/0", NFile_Open_Mode::Write_Only);

	TUART_IOCtl_Params params;
	params.baud_rate = NUART_Baud_Rate::BR_115200;
	params.char_length = NUART_Char_Length::Char_8;
	ioctl(uart_file, NIOCtl_Operation::Set_Params, &params);

	fputs(uart_file, "UART Task A starting!");

    uint32_t memfile = open("SYS:shm/my_shared_memory", NFile_Open_Mode::Read_Write);
    char *mem = mmap(0x100000, memfile);
    mem[10] = 'a';
    mutex_t mutex = mutex_create("shm_mem_mutex");

    while (true)
    {
        char c[2];
        c[1] = '\0';
        mutex_lock(mutex);
        c[0] = mem[10];
        if (c[0] == 'a')
        {
            fputs(uart_file, c);
            mem[10] = 'b';
        }
        mutex_unlock(mutex);
    }

    // // free(mem);
    close(memfile);
    mutex_destroy(mutex);

    return 0;
}
