#include <process/shm.h>


CShared_Memory::CShared_Memory() : IFile{NFile_Type_Major::Shm_File}, mPhys_addrs {0}
{
    //
}

uint32_t CShared_Memory::Read(char* buffer, uint32_t num)
{
    return 0;
}
uint32_t CShared_Memory::Write(const char* buffer, uint32_t num)
{
    return 0;
}
bool CShared_Memory::Close()
{
    return false;
}
bool CShared_Memory::Wait(uint32_t count)
{
    return false;
}
uint32_t CShared_Memory::Notify(uint32_t count)
{
    return 0;
}
