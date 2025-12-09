#include <stdfile.h>
#include <stdstring.h>
#include <stdmutex.h>

#include <drivers/bridges/uart_defs.h>

constexpr char GLUCOSE_PREFIX[] = "GLU:";
constexpr char INSULIN_PREFIX[] = "INS:";

int main()
{
    uint32_t uart_file = open("DEV:uart/0", NFile_Open_Mode::Write_Only);

    TUART_IOCtl_Params params;
    params.baud_rate = NUART_Baud_Rate::BR_115200;
    params.char_length = NUART_Char_Length::Char_8;
    ioctl(uart_file, NIOCtl_Operation::Set_Params, &params);

    uint32_t led = open("DEV:gpio/21", NFile_Open_Mode::Write_Only);
    uint32_t gluc_disp_pipe = pipe("gluc_disp_pipe", sizeof(float));
    uint32_t dose_disp_pipe = pipe("dose_disp_pipe", sizeof(float));

    char data[sizeof(float)];
    char str[128];
    char float_str[64];
    while(true)
    {
        memset(str, '\0', sizeof(str));
        memset(float_str, '\0', sizeof(float_str));

        uint32_t read_bytes = read(gluc_disp_pipe, data, sizeof(data));
        write(led, "1", 1);
        float glucose = *reinterpret_cast<float *>(data);
        ftoa(glucose, float_str, 2);

        strncpy(str, GLUCOSE_PREFIX, sizeof(GLUCOSE_PREFIX));
        strncpy(str + strlen(str), float_str, strlen(float_str));
        strncpy(str + strlen(str), "\n", 1);

        write(uart_file, str, strlen(str));

        // Reset str and float str
        memset(str, '\0', sizeof(str));
        memset(float_str, '\0', sizeof(float_str));

        read_bytes = read(dose_disp_pipe, data, sizeof(data));
        float insulin = *reinterpret_cast<float *>(data);
        ftoa(insulin, float_str, 2);

        strncpy(str, INSULIN_PREFIX, sizeof(INSULIN_PREFIX));
        strncpy(str + strlen(str), float_str, strlen(float_str));
        strncpy(str + strlen(str), "\n", 1);

        write(uart_file, str, strlen(str));
    }

    close(gluc_disp_pipe);
    close(dose_disp_pipe);

    return 0;
}
