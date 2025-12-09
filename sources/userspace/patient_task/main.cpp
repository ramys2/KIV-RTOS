#include <stdstring.h>
#include <stdfile.h>
#include <stdmutex.h>

#include <drivers/bridges/uart_defs.h>

int main()
{
    uint32_t uart_file = open("DEV:uart/0", NFile_Open_Mode::Write_Only);

    TUART_IOCtl_Params params;
    params.baud_rate = NUART_Baud_Rate::BR_115200;
    params.char_length = NUART_Char_Length::Char_8;
    ioctl(uart_file, NIOCtl_Operation::Set_Params, &params);

    uint32_t led = open("DEV:gpio/20", NFile_Open_Mode::Write_Only);
    uint32_t sh_pipe = pipe("sh_pipe", 4);
    semaphore_t data_ready = sem_create("data_ready#1");

    char *last_led_val = "0";
    while (true)
    {
        uint32_t num = 20;
        *last_led_val = *last_led_val == '0' ? '1' : '0';
        write(led, last_led_val, 1);
        sleep(10000);
        write(sh_pipe, reinterpret_cast<char *>(&num), sizeof(num));
        sem_release(data_ready, 1);
    }

    close(led);
    close(sh_pipe);
    sem_destroy(data_ready);

    return 0;
}
