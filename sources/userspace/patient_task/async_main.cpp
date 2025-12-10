#include "patient.h"
#include <stdfile.h>
#include <stdstring.h>
#include <stdmutex.h>

constexpr uint32_t SEND_VALUES = 5;

int main()
{
    CVirtual_Patient patient;

    uint32_t pat_gluc_fd = open("SYS:shm/pat_gluc_mem", NFile_Open_Mode::Read_Write);
    char *pat_gluc_mem = mmap(0x100000, pat_gluc_fd);
    uint32_t dose_pat_fd = open("SYS:shm/dose_pat_mem", NFile_Open_Mode::Read_Write);
    char *dose_pat_mem = mmap(0x100000, dose_pat_fd);

    semaphore_t gluc_pat_sem = sem_create("gluc_pat_sem#1");
    semaphore_t pat_dose_sem = sem_create("pat_dose_sem#1");

    sem_acquire(pat_dose_sem, 1);

    bool first_ite = true;
    uint32_t sent_vals = 0;
    while (true)
    {
        float curr_glucose = patient.Get_Current_Glucose();
        memcpy(&curr_glucose, pat_gluc_mem, sizeof(curr_glucose));
        sem_release(gluc_pat_sem, 1);

        if (sent_vals == SEND_VALUES)
        {
            sem_acquire(pat_dose_sem, 1);
            float dose = *reinterpret_cast<float *>(dose_pat_mem);
            patient.Dose_Insulin(dose);
            sent_vals = 0;
            continue;
        }


        sent_vals++;
        sleep(1000);
        patient.Step();
    }

    close(pat_gluc_fd);
    sem_destroy(gluc_pat_sem);

    return 0;
}
