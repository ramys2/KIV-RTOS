#include <stdfile.h>
#include <stdmutex.h>

int main(int argv, char **argc)
{
    uint32_t display_file = open("DEV:segd", NFile_Open_Mode::Write_Only);
    uint32_t memfile = open("SYS:shm/my_shared_memory", NFile_Open_Mode::Read_Write);
    char *mem = mmap(0x100000, memfile);
    mutex_t mutex = mutex_create("shm_mem_mutex");

    while (true)
    {
        char *c;
        mutex_lock(mutex);
        c = &mem[10];
        if (!c && *c == 'a')
        {
            write(display_file, c, 1);
        }
        mem[10] = 'b';
        mutex_unlock(mutex);
    }

    // free(mem);
    close(memfile);
    close(display_file);
    mutex_destroy(mutex);
    return 0;
}