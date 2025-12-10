#include <stdfile.h>
#include <stdstring.h>
#include <stdmutex.h>

#include <drivers/bridges/uart_defs.h>

constexpr char GLUCOSE_PREFIX[] = "GLU:";
constexpr char INSULIN_PREFIX[] = "INS:";

int main()
{
    uint32_t uart_file = open("DEV:uart/0", NFile_Open_Mode::Write_Only);

    TUART_IOCtl_Params params;
    params.baud_rate = NUART_Baud_Rate::BR_115200;
    params.char_length = NUART_Char_Length::Char_8;
    ioctl(uart_file, NIOCtl_Operation::Set_Params, &params);

    uint32_t gluc_disp_fd = open("SYS:shm/gluc_disp_mem", NFile_Open_Mode::Read_Write);
    char *gluc_disp_mem = mmap(0x100000, gluc_disp_fd);
    uint32_t dose_disp_fd = open("SYS:shm/dose_disp_mem", NFile_Open_Mode::Read_Write);
    char *dose_disp_mem = mmap(0x100000, dose_disp_fd);

    semaphore_t disp_gluc_sem = sem_create("disp_gluc_sem#1");
    semaphore_t disp_dose_sem = sem_create("disp_dose_sem#1");

    sem_acquire(disp_gluc_sem, 1);
    sem_acquire(disp_dose_sem, 1);

    char str[128];
    char float_str[64];
    while(true)
    {
        memset(str, '\0', sizeof(str));
        memset(float_str, '\0', sizeof(float_str));

        sem_acquire(disp_gluc_sem, 1);
        float glucose = *reinterpret_cast<float *>(gluc_disp_mem);
        ftoa(glucose, float_str, 2);

        strncpy(str, GLUCOSE_PREFIX, sizeof(GLUCOSE_PREFIX));
        strncpy(str + strlen(str), float_str, strlen(float_str));
        strncpy(str + strlen(str), "\n", 1);

        write(uart_file, str, strlen(str));

        // Reset str and float str
        memset(str, '\0', sizeof(str));
        memset(float_str, '\0', sizeof(float_str));

        sem_acquire(disp_dose_sem, 1);
        float insulin = *reinterpret_cast<float *>(dose_disp_mem);
        ftoa(insulin, float_str, 2);

        strncpy(str, INSULIN_PREFIX, sizeof(INSULIN_PREFIX));
        strncpy(str + strlen(str), float_str, strlen(float_str));
        strncpy(str + strlen(str), "\n", 1);

        write(uart_file, str, strlen(str));
    }

    return 0;
}
