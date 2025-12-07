#include "patient.h"
#include <stdfile.h>
#include <stdstring.h>
#include <stdmutex.h>



int main()
{
    CVirtual_Patient patient;

    uint32_t pat_gluc_pipe = pipe("pat_gluc_pipe", 64);
    semaphore_t pat_gluc_sem = sem_create("pat_gluc_sem#1");
    semaphore_t gluc_pat_sem = sem_create("gluc_pat_sem#1");

    sem_acquire(pat_gluc_sem, 1);

    while (true)
    {
        // Wait for handshake
        sem_release(gluc_pat_sem, 1);
        sem_acquire(pat_gluc_sem, 1);

        // ========= Send data =========
        float data = patient.Get_Current_Glucose();
        write(pat_gluc_pipe, reinterpret_cast<char*>(&data), sizeof(float));
        // =============================
        sleep(1000);
        patient.Step();
    }
    


    sem_destroy(gluc_pat_sem);
    sem_destroy(pat_gluc_sem);
    close(pat_gluc_pipe);

    return 0;
}
