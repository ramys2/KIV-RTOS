#pragma once

#include <fs/filesystem.h>

class CShared_Memory : public IFile
{
    private:
        uint32_t mPhys_addrs;

    public:
        CShared_Memory();
        ~CShared_Memory() = default;

        uint32_t Get_Phys_Addrs() const
        {
            return mPhys_addrs;
        }

        void Set_Phys_Addrs(uint32_t phys_addrs)
        {
            mPhys_addrs = phys_addrs;
        }

        virtual uint32_t Read(char* buffer, uint32_t num) override;
        virtual uint32_t Write(const char* buffer, uint32_t num) override;
        virtual bool Close() override;
        virtual bool Wait(uint32_t count) override;
        virtual uint32_t Notify(uint32_t count) override;
};
