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
    uint32_t pat_gluc_pipe = pipe("pat_gluc_pipe", 64);
    semaphore_t pat_gluc_sem = sem_create("pat_gluc_sem#1");
    semaphore_t gluc_pat_sem = sem_create("gluc_pat_sem#1");

    // Display communication primitives
    uint32_t gluc_disp_pipe = pipe("gluc_disp_pipe", 64);
    semaphore_t gluc_disp_sem = sem_create("gluc_disp_sem#1");
    semaphore_t disp_gluc_sem = sem_create("disp_gluc_sem#1");

    // Dose calculator communication primitives
    uint32_t dose_gluc_pipe = pipe("dose_gluc_pipe", 64);
    semaphore_t dose_gluc_sem = sem_create("dose_gluc_sem#1");
    semaphore_t gluc_dose_sem = sem_create("gluc_dose_sem#1");

    sem_acquire(gluc_pat_sem, 1);
    sem_acquire(gluc_disp_sem, 1);
    sem_acquire(gluc_dose_sem, 1);

    float values[READ_VALUES];
    uint32_t val_idx = 0;
    while (true)
    {
        // Wait for handshake with Patient
        sem_release(pat_gluc_sem, 1);
        sem_acquire(gluc_pat_sem, 1);
    
        // ========= Receive data =========
        char data[4];
        uint32_t read_bytes = read(pat_gluc_pipe, data, sizeof(data));
        // ================================
        if (read_bytes == sizeof(data))
        {
            float rec_data = *reinterpret_cast<float*>(data);
            values[val_idx++] = rec_data;
    
            if (val_idx != READ_VALUES)
            {
                continue;
            }

            val_idx = 0;
            float avg = calculate_avg(values);

            sem_release(dose_gluc_sem, 1);
            sem_acquire(gluc_dose_sem, 1);
    
            write(dose_gluc_pipe, reinterpret_cast<char*>(&avg), sizeof(float));

            // Wait for handshake with Display
            sem_release(disp_gluc_sem, 1);
            sem_acquire(gluc_disp_sem, 1);
    
            write(gluc_disp_pipe, reinterpret_cast<char*>(&avg), sizeof(float));

            // Wait for handshake with Dose calculator
        }
    }


    sem_destroy(gluc_pat_sem);
    sem_destroy(pat_gluc_sem);
    sem_destroy(gluc_disp_sem);
    sem_destroy(disp_gluc_sem);
    sem_destroy(dose_gluc_sem);
    sem_destroy(gluc_dose_sem);

    close(pat_gluc_pipe);
    close(gluc_disp_pipe);
    close(dose_gluc_pipe);

    return 0;
}
