#include "patient.h"
#include <stdfile.h>
#include <stdstring.h>
#include <stdmutex.h>

constexpr uint32_t SEND_VALUES = 5;

int main()
{
    CVirtual_Patient patient;

    uint32_t switch_fd = open("DEV:gpio/17", NFile_Open_Mode::Read_Only);
    uint32_t led = open("DEV:gpio/18", NFile_Open_Mode::Write_Only);
    uint32_t pat_gluc_pipe = pipe("pat_gluc_pipe", sizeof(float));
    uint32_t dose_pat_pipe = pipe("dose_pat_pipe", sizeof(float));

    bool first_ite = true;
    uint32_t sent_vals = 0;
    char switch_val[4];
    while (true)
    {
        memset(switch_val, '\0', sizeof(switch_val));
        read(switch_fd, switch_val, 1);

        uint32_t sleep_time = switch_val[0] == '1' ? 1000 : 500;

        float curr_glucose = patient.Get_Current_Glucose();
        write(pat_gluc_pipe, reinterpret_cast<char*>(&curr_glucose), sizeof(curr_glucose));

        if (sent_vals == SEND_VALUES)
        {
            char data[sizeof(float)];
            uint32_t read_bytes = read(dose_pat_pipe, data, sizeof(data));
            write(led, "1", 1);

            float dose = *reinterpret_cast<float *>(data);
            patient.Dose_Insulin(dose);
            sent_vals = 0;
            sleep(sleep_time);
            patient.Step();
            continue;
        }

        sent_vals++;
        sleep(sleep_time);
        patient.Step();
    }

    close(led);
    close(pat_gluc_pipe);
    close(dose_pat_pipe);

    return 0;
}
