#include <stdstring.h>
#include <stdfile.h>
#include <stdmutex.h>

#include <oled.h>

#include <drivers/bridges/uart_defs.h>
#include <drivers/gpio.h>

#include <process/process_manager.h>

/**
 * Displejovy task
 * 
 * Zobrazuje hlasky na OLED displeji, a pokud prijde udalost od jinych tasku, zobrazi neco relevantniho k nim
 **/

// const char* messages[] = {
// 	"I blink, therefore I am.",
// 	"I see dead pixels.",
// 	"One CPU rules them all.",
// 	"My favourite sport is ARM wrestling",
// 	"Old MacDonald had a farm, EIGRP",
// };

int main(int argc, char** argv)
{
    // uint32_t uart_file = open("DEV:uart/0", NFile_Open_Mode::Write_Only);

	// TUART_IOCtl_Params params;
	// params.baud_rate = NUART_Baud_Rate::BR_115200;
	// params.char_length = NUART_Char_Length::Char_8;
	// ioctl(uart_file, NIOCtl_Operation::Set_Params, &params);

	// fputs(uart_file, "UART Task B starting!");
	// COLED_Display disp("DEV:oled");
	// disp.Clear(false);
	// disp.Put_String(10, 10, "KIV-RTOS init...");
	// disp.Flip();

	// uint32_t trng_file = open("DEV:trng", NFile_Open_Mode::Read_Only);
	// uint32_t num = 0;

	// sleep(0x800, 0x800);

	// while (true)
	// {
	// 	// ziskame si nahodne cislo a vybereme podle toho zpravu
	// 	read(trng_file, reinterpret_cast<char*>(&num), sizeof(num));
	// 	const char* msg = messages[num % (sizeof(messages) / sizeof(const char*))];

	// 	disp.Clear(false);
	// 	disp.Put_String(0, 0, msg);
	// 	disp.Flip();

	// 	sleep(0x4000, 0x800); // TODO: z tohohle bude casem cekani na podminkove promenne (na eventu) s timeoutem
	// }

    uint32_t memfile = open("SYS:shm/my_shared_memory", NFile_Open_Mode::Read_Write);
    char *mem = mmap(0x100000, memfile);
    mutex_t mutex = mutex_create("shm_mem_mutex");

    while (true)
    {
        char c[2];
        c[1] = '\0';
        mutex_lock(mutex);
        c[0] = mem[10];
        if (c[0] == 'b')
        {
            mem[10] = 'a';
        }
        mutex_unlock(mutex);
    }

    // free(mem);
    close(memfile);
    mutex_destroy(mutex);

    return 0;
}
