#include <stdfile.h>
#include <stdstring.h>
#include <stdmutex.h>

#include <drivers/bridges/uart_defs.h> // DELETE after testing

constexpr uint32_t READ_VALUES = 5;

int main()
{
    uint32_t uart_file = open("DEV:uart/0", NFile_Open_Mode::Write_Only);

	TUART_IOCtl_Params params;
	params.baud_rate = NUART_Baud_Rate::BR_115200;
	params.char_length = NUART_Char_Length::Char_8;
	ioctl(uart_file, NIOCtl_Operation::Set_Params, &params);

    uint32_t patient_pipe = pipe("patient_pipe", 64);
    semaphore_t patient_sem = sem_create("patient_sem#1");
    semaphore_t glucrec_sem = sem_create("glucrec_sem#1");

    sem_acquire(glucrec_sem, 1);
    float values[READ_VALUES];
    uint32_t val_idx = 0;

    while (true)
    {
        // Wait for handshake
        sem_release(patient_sem, 1);
        sem_acquire(glucrec_sem, 1);
    
        // ========= Receive data =========
        char data[4];
        uint32_t read_bytes = read(patient_pipe, data, 4);
        float rec_data = *reinterpret_cast<float*>(data);
        values[val_idx++] = rec_data;
        // ================================
        if (val_idx == READ_VALUES)
        {
            val_idx = 0;
            float sum = 0;
            for (uint32_t i = 0; i < READ_VALUES; i++)
            {
                sum += values[i];
            }

            float avg = sum / READ_VALUES;
            char data_buf[64];
            memset(data_buf, '\0', 64);
            ftoa(avg, data_buf, 2);
            write(uart_file, data_buf, strlen(data_buf));
            write(uart_file, "\n", 1);
        }
    }


    sem_destroy(glucrec_sem);
    sem_destroy(patient_sem);
    close(uart_file);
    close(patient_pipe);

    return 0;
}
