#include <stdfile.h>
#include <stdstring.h>
#include <stdmutex.h>

#include <drivers/bridges/uart_defs.h> // DELETE after testing

int main()
{
    // Virtaul patients primitives
    uint32_t vp_pipe = pipe("vp_pipe", 32);
    uint32_t vp_cv = open("SYS:cv/vp", NFile_Open_Mode::Read_Write);

    return 0;
}
