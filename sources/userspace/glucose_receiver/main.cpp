#include <stdfile.h>
#include <stdstring.h>
#include <stdmutex.h>

constexpr uint32_t READ_VALUES = 5;

float calculate_avg(const float *values)
{
    float sum = 0;
    for (uint32_t i = 0; i < READ_VALUES; i++)
    {
        sum += values[i];
    }

    return sum / READ_VALUES;
}

int main()
{
    // Patient communication primitives
    uint32_t patient_pipe = pipe("patient_pipe", 64);
    semaphore_t patient_sem = sem_create("patient_sem#1");
    semaphore_t glucrec_sem = sem_create("glucrec_sem#1");

    // Display communication primitives
    uint32_t disp_pipe = pipe("disp_pipe", 64);
    semaphore_t disp_sem = sem_create("disp_sem#1");
    semaphore_t gluc_disp_sem = sem_create("gluc_disp_sem#1");

    sem_acquire(glucrec_sem, 1);
    sem_acquire(gluc_disp_sem, 1);

    float values[READ_VALUES];
    uint32_t val_idx = 0;
    while (true)
    {
        // Wait for handshake with Patient
        sem_release(patient_sem, 1);
        sem_acquire(glucrec_sem, 1);
    
        // ========= Receive data =========
        char data[4];
        uint32_t read_bytes = read(patient_pipe, data, sizeof(data));
        float rec_data = *reinterpret_cast<float*>(data);
        values[val_idx++] = rec_data;
        // ================================

        if (val_idx != READ_VALUES)
        {
            continue;
        }

        val_idx = 0;
        float avg = calculate_avg(values);

        // Wait for handshake with Display
        sem_release(disp_sem, 1);
        sem_acquire(gluc_disp_sem, 1);

        write(disp_pipe, reinterpret_cast<char*>(&avg), sizeof(float));

    }


    sem_destroy(glucrec_sem);
    sem_destroy(patient_sem);
    sem_destroy(gluc_disp_sem);
    sem_destroy(disp_sem);

    close(patient_pipe);
    close(disp_pipe);

    return 0;
}
