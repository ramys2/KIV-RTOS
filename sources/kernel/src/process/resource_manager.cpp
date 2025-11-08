#include <process/resource_manager.h>
#include <stdstring.h>

CProcess_Resource_Manager sProcess_Resource_Manager;

CProcess_Resource_Manager::CProcess_Resource_Manager()
{
    for (uint32_t i = 0; i < Mutex_Count; i++)
    {
        mMutexes[i].name[0] = '\0';
        mMutexes[i].alloc_count = 0;
    }
}

CProcess_Resource_Manager::~CProcess_Resource_Manager()
{
    //
}

CMutex* CProcess_Resource_Manager::Alloc_Mutex(const char* name)
{
    for (uint32_t i = 0; i < Mutex_Count; i++)
    {
        if (mMutexes[i].alloc_count > 0)
        {
            if (strncmp(mMutexes[i].name, name, Max_Mutex_Name_Length) == 0)
            {
                mMutexes[i].alloc_count++;
                return &mMutexes[i].mtx;
            }
        }
    }

    for (uint32_t i = 0; i < Mutex_Count; i++)
    {
        if (mMutexes[i].alloc_count == 0)
        {
            strncpy(mMutexes[i].name, name, Max_Mutex_Name_Length);
            mMutexes[i].alloc_count++;
            return &mMutexes[i].mtx;
        }
    }

    return nullptr;
}

void CProcess_Resource_Manager::Free_Mutex(CMutex* mtx)
{
    for (uint32_t i = 0; i < Mutex_Count; i++)
    {
        if (&mMutexes[i].mtx == mtx && mMutexes[i].alloc_count > 0)
        {
            mMutexes[i].alloc_count--;
            return;
        }
    }
}

CSemaphore* CProcess_Resource_Manager::Alloc_Semaphore(const char* name, uint32_t initial_res_count)
{
    for (uint32_t i = 0; i < Semaphore_Count; i++)
    {
        if (mSemaphores[i].alloc_count > 0)
        {
            if (strncmp(mSemaphores[i].name, name, Max_Semaphore_Name_Length) == 0)
            {
                // pokud oteviraci proces udal pocet zdroju, a nesouhlasi se skutecnosti, nepovedlo se
                if (initial_res_count != Semaphore_Initial_Res_Count_Unknown && mSemaphores[i].semaphore.Get_Max_Count() != initial_res_count)
                    return nullptr;

                mSemaphores[i].alloc_count++;
                return &mSemaphores[i].semaphore;
            }
        }
    }

    // pokud zadatel neudal pocet zdroju, musel semafor uz existovat - nelze pokracovat
    if (initial_res_count == Semaphore_Initial_Res_Count_Unknown)
        return nullptr;

    for (uint32_t i = 0; i < Semaphore_Count; i++)
    {
        if (mSemaphores[i].alloc_count == 0)
        {
            strncpy(mSemaphores[i].name, name, Max_Semaphore_Name_Length);
            mSemaphores[i].semaphore.Reset(initial_res_count, initial_res_count);
            mSemaphores[i].alloc_count++;
            return &mSemaphores[i].semaphore;
        }
    }

    return nullptr;
}

void CProcess_Resource_Manager::Free_Semaphore(CSemaphore* sem)
{
    for (uint32_t i = 0; i < Semaphore_Count; i++)
    {
        if (&mSemaphores[i].semaphore == sem && mSemaphores[i].alloc_count > 0)
        {
            if ((--mSemaphores[i].alloc_count) == 0)
                mSemaphores[i].semaphore.Reset();
            return;
        }
    }
}

CCondition_Variable* CProcess_Resource_Manager::Alloc_Condition_Variable(const char* name)
{
    for (uint32_t i = 0; i < Cond_Var_Count; i++)
    {
        if (mCondVars[i].alloc_count > 0)
        {
            if (strncmp(mCondVars[i].name, name, Max_Cond_Var_Name_Length) == 0)
            {
                mCondVars[i].alloc_count++;
                return &mCondVars[i].cv;
            }
        }
    }

    for (uint32_t i = 0; i < Cond_Var_Count; i++)
    {
        if (mCondVars[i].alloc_count == 0)
        {
            CMutex* cvmtx = Alloc_Mutex(name);

            strncpy(mCondVars[i].name, name, Max_Cond_Var_Name_Length);
            mCondVars[i].alloc_count++;
            return &mCondVars[i].cv;
        }
    }

    return nullptr;
}

void CProcess_Resource_Manager::Free_Condition_Variable(CCondition_Variable* cv)
{
    for (uint32_t i = 0; i < Cond_Var_Count; i++)
    {
        if (&mCondVars[i].cv == cv && mCondVars[i].alloc_count > 0)
        {
            if ((--mCondVars[i].alloc_count) == 0)
                mCondVars[i].cv.Reset(nullptr);
            return;
        }
    }
}

CPipe* CProcess_Resource_Manager::Alloc_Pipe(const char* name, uint32_t pipe_size)
{
    for (uint32_t i = 0; i < Pipe_Count; i++)
    {
        if (mPipes[i].alloc_count > 0)
        {
            if (strncmp(mPipes[i].name, name, Max_Pipe_Name_Length) == 0)
            {
                mPipes[i].alloc_count++;
                return &mPipes[i].pipe;
            }
        }
    }

    if (pipe_size == Pipe_Byte_Count_Unknown)
        return nullptr;

    for (uint32_t i = 0; i < Pipe_Count; i++)
    {
        if (mPipes[i].alloc_count == 0)
        {
            mPipes[i].pipe.Reset(pipe_size);

            strncpy(mPipes[i].name, name, Max_Pipe_Name_Length);
            mPipes[i].alloc_count++;
            return &mPipes[i].pipe;
        }
    }

    return nullptr;
}

void CProcess_Resource_Manager::Free_Pipe(CPipe* pipe)
{
    for (uint32_t i = 0; i < Pipe_Count; i++)
    {
        if (&mPipes[i].pipe == pipe && mPipes[i].alloc_count > 0)
        {
            if ((--mPipes[i].alloc_count) == 0)
                mPipes[i].pipe.Reset(0);
            return;
        }
    }
}

bool CProcess_Resource_Manager::Register_New_File(const char *filepath, const uint32_t pid, const uint32_t fd)
{
    TOpened_File_Record *new_record = new TOpened_File_Record();
    if (!filepath || !new_record)
    {
        return false;
    }

    new_record->pid = pid;
    new_record->file_descriptor = fd;


    int filename_len = strlen(filepath);
    new_record->filepath = new char[filename_len + 1];
    if (!new_record->filepath)
    {
        delete new_record;
        return false;
    }
    strncpy(new_record->filepath, filepath, filename_len + 1);

    new_record->next = first_record;
    first_record = new_record;

    return true;
}
