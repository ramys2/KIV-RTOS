#include <stdstring.h>
#include <stdfile.h>
#include <stdmutex.h>

#include <drivers/gpio.h>
#include <process/process_manager.h>

#include <drivers/bridges/uart_defs.h>

/**
 * Tilt task
 * 
 * Ceka na vstup ze senzoru naklonu, a prehraje neco na buzzeru (PWM) dle naklonu
 **/

static void fputs(uint32_t file, const char* string)
{
	write(file, string, strlen(string));
}

int main(int argc, char** argv)
{
	// char state = '0';
	// char oldstate = '0';

	// uint32_t tiltsensor_file = open("DEV:gpio/23", NFile_Open_Mode::Read_Only);
	// // TODO: otevrit PWM

	// NGPIO_Interrupt_Type irtype;
	
	// //irtype = NGPIO_Interrupt_Type::Rising_Edge;
	// //ioctl(tiltsensor_file, NIOCtl_Operation::Enable_Event_Detection, &irtype);

	// irtype = NGPIO_Interrupt_Type::Falling_Edge;
	// ioctl(tiltsensor_file, NIOCtl_Operation::Enable_Event_Detection, &irtype);

	// uint32_t logpipe = pipe("log", 32);

	// while (true)
	// {
	// 	wait(tiltsensor_file, 0x800);

	// 	// "debounce" - tilt senzor bude chvili flappovat mezi vysokou a nizkou urovni
	// 	//sleep(0x100, Deadline_Unchanged);

	// 	read(tiltsensor_file, &state, 1);

	// 	//if (state != oldstate)
	// 	{
	// 		if (state == '0')
	// 		{
	// 			write(logpipe, "Tilt UP", 7);
	// 		}
	// 		else
	// 		{
	// 			write(logpipe, "Tilt DOWN", 10);
	// 		}
	// 		oldstate = state;
	// 	}

	// 	sleep(0x1000, Indefinite/*0x100*/);
	// }

	// // TODO zavrit PWM
	// close(tiltsensor_file);

    uint32_t uart_file = open("DEV:uart/0", NFile_Open_Mode::Write_Only);

	TUART_IOCtl_Params params;
	params.baud_rate = NUART_Baud_Rate::BR_115200;
	params.char_length = NUART_Char_Length::Char_8;
	ioctl(uart_file, NIOCtl_Operation::Set_Params, &params);

    uint32_t pipe_cv = open("SYS:cv/pipe", NFile_Open_Mode::Read_Write);
    mutex_t pipe_mutex = mutex_create("pipe");
    uint32_t pipe_p = pipe("com_pipe", 10);

    char buf[16];
    char read_b[16];
    memset(buf, '\0', 16);

    mutex_lock(pipe_mutex);
    uint32_t read_bytes;
    while ((read_bytes = read(pipe_p, buf, 5)) < 5)
    {
        fputs(uart_file, "Waiting for something to appear in pipe!\n");
        NSWI_Result_Code code =  wait(pipe_cv);
    }
    mutex_unlock(pipe_mutex);

    itoa(read_bytes, read_b, 10);
    fputs(uart_file, "Read bytes: ");
    fputs(uart_file, read_b);
    fputs(uart_file, "\n");
    fputs(uart_file, buf);
    fputs(uart_file, "\n");

    close(uart_file);
    close(pipe_p);
    close(pipe_cv);
    mutex_destroy(pipe_mutex);

    return 0;
}
