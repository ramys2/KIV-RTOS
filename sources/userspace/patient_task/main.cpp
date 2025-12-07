#include "patient.h"
#include <stdfile.h>
#include <stdstring.h>
#include <stdmutex.h>



int main()
{
    CVirtual_Patient patient;
    uint32_t patient_pipe = pipe("patient_pipe", 64);
    semaphore_t patient_sem = sem_create("patient_sem#1");
    semaphore_t glucrec_sem = sem_create("glucrec_sem#1");

    sem_acquire(patient_sem, 1);

    while (true)
    {
        // Wait for handshake
        sem_release(glucrec_sem, 1);
        sem_acquire(patient_sem, 1);

        // ========= Send data =========
        float data = patient.Get_Current_Glucose();
        write(patient_pipe, reinterpret_cast<char*>(&data), sizeof(float));
        // =============================
        sleep(1000);
        patient.Step();
    }
    


    sem_destroy(glucrec_sem);
    sem_destroy(patient_sem);
    close(patient_pipe);

    return 0;
}
