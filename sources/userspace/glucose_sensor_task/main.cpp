#include <stdfile.h>
#include <stdstring.h>
#include <stdmutex.h>

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
    uint32_t led = open("DEV:gpio/19", NFile_Open_Mode::Write_Only);
    uint32_t pat_gluc_pipe = pipe("pat_gluc_pipe", sizeof(float));
    uint32_t gluc_dose_pipe = pipe("gluc_dose_pipe", sizeof(float));
    uint32_t gluc_disp_pipe = pipe("gluc_disp_pipe", sizeof(float));

    float last_vals[LAST_VALS_SIZE];
    uint32_t vals_idx = 0;
    while(true)
    {
        char data[sizeof(float)];
        uint32_t read_bytes = read(pat_gluc_pipe, data, sizeof(data));
        write(led, "1", 1);

        float glucose = *reinterpret_cast<float *>(data);
        last_vals[vals_idx++] = glucose;

        if (vals_idx != LAST_VALS_SIZE)
        {
            continue;
        }
        vals_idx = 0;

        float glucose_avg = calculate_avg(last_vals);

        write(gluc_dose_pipe, reinterpret_cast<char *>(&glucose_avg), sizeof(glucose_avg));
        write(gluc_disp_pipe, reinterpret_cast<char *>(&glucose_avg), sizeof(glucose_avg));
    }

    close(pat_gluc_pipe);
    close(gluc_dose_pipe);
    close(gluc_disp_pipe);

    return 0;
}
