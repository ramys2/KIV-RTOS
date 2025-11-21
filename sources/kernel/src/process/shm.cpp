#include <process/shm.h>

CShared_Memory::CShared_Memory()
    : IFile (NFile_Type_Major::Character),
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

uint32_t CShared_Memory::Get_Phys_Addrs() const
{
    return mPhys_Addrs;
}

void CShared_Memory::Set_Phys_Addrs(uint32_t phys_addrs)
{
    mPhys_Addrs = phys_addrs;
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
    // TODO: Free shared memory
    return true;
}

bool CShared_Memory::Wait(uint32_t count)
{
    return true;
}

uint32_t CShared_Memory::Notify(uint32_t count)
{
    return 0;
}