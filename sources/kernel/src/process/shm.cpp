#include <process/shm.h>

CShared_Memory::CShared_Memory()
    : IFile (NFile_Type_Major::Shared_Memory),
    mPhys_Addrs {0}
{

}

CShared_Memory::~CShared_Memory()
{
    //
}

void CShared_Memory::Reset()
{
    mPhys_Addrs = 0;
}

uint32_t CShared_Memory::Read(char* buffer, uint32_t num)
{
    return 0;
}
uint32_t Write(const char* buffer, uint32_t num)
{
    return 0;
}
bool Close()
{
    // TODO: Free shared memory
    return true;
}

bool Wait(uint32_t count)
{
    return true;
}

uint32_t Notify(uint32_t count)
{
    return 0;
}