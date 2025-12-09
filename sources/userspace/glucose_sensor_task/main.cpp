#include <stdstring.h>
#include <stdfile.h>
#include <stdmutex.h>

int main()
{
    uint32_t led = open("DEV:gpio/21", NFile_Open_Mode::Write_Only);
    uint32_t sh_pipe = pipe("sh_pipe", 4);
    semaphore_t data_ready = sem_create("data_ready#1");

    sem_acquire(data_ready, 1);

    char *led_last_val = "0";
    while(true)
    {
        char data[4];
        sem_acquire(data_ready, 1);
        uint32_t read_bytes = read(sh_pipe, data, sizeof(data));
        *led_last_val = *led_last_val == '0' ? '1' : '0';
        write(led, led_last_val, 1);
    }

    close(led);
    close(sh_pipe);
    sem_destroy(data_ready);
}
