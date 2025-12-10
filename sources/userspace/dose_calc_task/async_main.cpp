#include "dose_calculator.h"
#include <stdfile.h>
#include <stdstring.h>
#include <stdmutex.h>

int main()
{
    CDose_Calculator calc;

    uint32_t gluc_dose_fd = open("SYS:shm/gluc_dose_mem", NFile_Open_Mode::Read_Write);
    char *gluc_dose_mem = mmap(0x100000, gluc_dose_fd);
    uint32_t dose_pat_fd = open("SYS:shm/dose_pat_mem", NFile_Open_Mode::Read_Write);
    char *dose_pat_mem = mmap(0x100000, dose_pat_fd);
    uint32_t dose_disp_fd = open("SYS:shm/dose_disp_mem", NFile_Open_Mode::Read_Write);
    char *dose_disp_mem = mmap(0x100000, dose_disp_fd);

    semaphore_t dose_gluc_sem = sem_create("dose_gluc_sem#1");
    semaphore_t pat_dose_sem = sem_create("pat_dose_sem#1");
    semaphore_t disp_dose_sem = sem_create("disp_dose_sem#1");

    sem_acquire(dose_gluc_sem, 1);

    while(true)
    {
        sem_acquire(dose_gluc_sem, 1);
        float glucose = *reinterpret_cast<float *>(gluc_dose_mem);
        float dose = calc.Calculate_Dose(glucose);

        memcpy(&dose, dose_pat_mem, sizeof(dose));
        sem_release(pat_dose_sem, 1);

        memcpy(&dose, dose_disp_mem, sizeof(dose));
        sem_release(disp_dose_sem, 1);
    }

    return 0;
}
