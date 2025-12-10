#include <stdfile.h>
#include <stdstring.h>
#include <stdmutex.h>

#include <drivers/bridges/uart_defs.h>

constexpr uint32_t LAST_VALS_SIZE = 5;

float calculate_avg(const float *values)
{
    float sum = 0;
    for (uint32_t i = 0; i < LAST_VALS_SIZE; i++)
    {
        sum += values[i];
    }

    return sum / LAST_VALS_SIZE;
}

int main()
{
    uint32_t pat_gluc_fd = open("SYS:shm/pat_gluc_mem", NFile_Open_Mode::Read_Write);
    char *pat_gluc_mem = mmap(0x100000, pat_gluc_fd);
    uint32_t gluc_dose_fd = open("SYS:shm/gluc_dose_mem", NFile_Open_Mode::Read_Write);
    char *gluc_dose_mem = mmap(0x100000, gluc_dose_fd);
    uint32_t gluc_disp_fd = open("SYS:shm/gluc_disp_mem", NFile_Open_Mode::Read_Write);
    char *gluc_disp_mem = mmap(0x100000, gluc_disp_fd);

    semaphore_t gluc_pat_sem = sem_create("gluc_pat_sem#1");
    semaphore_t dose_gluc_sem = sem_create("dose_gluc_sem#1");
    semaphore_t disp_gluc_sem = sem_create("disp_gluc_sem#1");

    sem_acquire(gluc_pat_sem, 1);

    float last_vals[LAST_VALS_SIZE];
    uint32_t vals_idx = 0;
    while(true)
    {
        sem_acquire(gluc_pat_sem, 1);
        float glucose = *reinterpret_cast<float *>(pat_gluc_mem);

        last_vals[vals_idx++] = glucose;

        if (vals_idx != LAST_VALS_SIZE)
        {
            continue;
        }
        vals_idx = 0;

        float glucose_avg = calculate_avg(last_vals);

        memcpy(&glucose_avg, gluc_dose_mem, sizeof(glucose_avg));
        sem_release(dose_gluc_sem, 1);

        memcpy(&glucose_avg, gluc_disp_mem, sizeof(glucose_avg));
        sem_release(disp_gluc_sem, 1);
    }

    close(pat_gluc_fd);
    close(gluc_dose_fd);
    close(gluc_disp_fd);
    sem_destroy(gluc_pat_sem);
}
