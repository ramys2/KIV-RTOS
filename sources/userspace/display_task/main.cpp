#include <stdfile.h>
#include <stdstring.h>
#include <stdmutex.h>

#include <drivers/bridges/uart_defs.h>

constexpr char GLUCOSE_PREFIX[] = "GLU: ";

int main()
{
    uint32_t uart_file = open("DEV:uart/0", NFile_Open_Mode::Write_Only);

    TUART_IOCtl_Params params;
    params.baud_rate = NUART_Baud_Rate::BR_115200;
    params.char_length = NUART_Char_Length::Char_8;
    ioctl(uart_file, NIOCtl_Operation::Set_Params, &params);

    // Glucose receiver communications primivites
    uint32_t gluc_disp_pipe = pipe("gluc_disp_pipe", 64);
    semaphore_t gluc_disp_sem = sem_create("gluc_disp_sem#1");
    semaphore_t disp_gluc_sem = sem_create("disp_gluc_sem#1");

    // Dose communications primitives

    sem_acquire(disp_gluc_sem, 1);

    while (true)
    {
        // Get data from glucose receiver
        sem_release(gluc_disp_sem, 1);
        sem_acquire(disp_gluc_sem, 1);

        char data[4];
        uint32_t read_bytes = read(gluc_disp_pipe, data, sizeof(data));
        if (read_bytes == sizeof(data))
        {
            float glucose_val;
            memcpy(data, &glucose_val, sizeof(float));

            char gluc_str[32];
            ftoa(glucose_val, gluc_str, 2);

            char glucose_lvl[128];
            memset(glucose_lvl, '\0', 128);
            strncpy(glucose_lvl, GLUCOSE_PREFIX, sizeof(GLUCOSE_PREFIX));
            strncpy(glucose_lvl + strlen(glucose_lvl), gluc_str, strlen(gluc_str));
            strncpy(glucose_lvl + strlen(glucose_lvl), "\n", 1);

            write(uart_file, glucose_lvl, strlen(glucose_lvl));
        }

        // Get data from insuline calcurator
    }

    return 0;
}
