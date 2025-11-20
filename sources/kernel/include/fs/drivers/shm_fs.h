#pragma once

#include <hal/peripherals.h>
#include <fs/filesystem.h>
#include <stdstring.h>
#include <process/pipe.h>
#include <process/resource_manager.h>

class CShm_FS_Driver : public IFilesystem_Driver
{
public:
    virtual void On_Register() override
    {
        //
    }

    virtual IFile *Open_File(const char *path, NFile_Open_Mode mode)
    {
        char memname[Max_Shared_Memory_Name_Length];
        memset(memname, '\0', Max_Shared_Memory_Name_Length);
        uint32_t memname_idx = 0;

        int path_len = strlen(path);
        bool loaded_slash_flag = false;

        for (int i = 0; i < path_len; i++)
        {
            if (path[i] == '/')
            {
                loaded_slash_flag = true;
                continue;
            }

            if (!loaded_slash_flag)
            {
                continue;
            }

            memname[memname_idx] = path[i];
            memname_idx++;
        }

        return sProcess_Resource_Manager.Alloc_Shared_Memory(memname);
    }
};

CShm_FS_Driver fsShm_FS_Driver;
