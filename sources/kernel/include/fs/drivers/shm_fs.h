#pragma once

#include <fs/filesystem.h>
#include <process/resource_manager.h>

class CShm_FS_Driver : public IFilesystem_Driver
{
    public:
        virtual void On_Register() override
        {
            //
        }

        virtual IFile *Open_File(const char* path, NFile_Open_Mode mode) override
        {
            return sProcess_Resource_Manager.Alloc_Memory_File(path);
        }
};

CShm_FS_Driver fsShm_FS_Driver;
