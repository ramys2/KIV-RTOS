#include <hal/peripherals.h>
#include <memory/mmu.h>

#include "stdstring.h"
#include "process/process_manager.h"
#include "process/resource_manager.h"
#include "process/process.h"
#include "memory/pages.h"

extern volatile __attribute__((section(".initsys.data"))) uint32_t Page_Directory_Kernel[PT_Size];

uint32_t *const Page_Directory_Kernel_High = reinterpret_cast<uint32_t *>(_virt_data_start + reinterpret_cast<uint32_t>(&Page_Directory_Kernel));

extern "C" void __attribute__((section(".text"))) kernel_mode_start();

static inline uint32_t __attribute__((section(".text"))) PT_Entry(uint32_t addr)
{
    return addr >> 20;
}

extern "C" void __attribute__((section(".text"))) _init_system_memory_high()
{
    // nechame v tabulce stranek jen high mapping kernelu a dat
    unsigned int addr;
    for (addr = 0; addr < 0x20000000; addr += PT_Region_Size)
        Page_Directory_Kernel_High[PT_Entry(addr)] = 0;

    // po modifikaci nesmime zapomenout vymazat cache a TLB
    mmu_invalidate_cache();
    mmu_invalidate_tlb();

    // tady muzeme pak mapovat jeste dalsi veci, ale pro ted to nechme takto

    // odskocime "zpatky" do assembly, kde vykoname zbytek inicializace jadra, co uz musi probihat nad relokovanym kodem a daty
    asm volatile("mov lr, %[kernel_mode_start]" : : [kernel_mode_start] "r"((unsigned int)&kernel_mode_start));
    asm volatile("bx lr");
}

void copy_kernel_page_table_to(uint32_t *target)
{
    // kopirovani by se dalo vyhnout pouzitim TTBR1 a prislusneho nastaveni boundary

    for (unsigned int i = 0; i < PT_Size; i++)
        target[i] = Page_Directory_Kernel_High[i];
}

void map_memory(uint32_t *target_pt, uint32_t phys, uint32_t virt)
{
    // zatim nechme vychozi sadu priznaku
    // do budoucna by se urcite hodilo oddelit kodovou stranku (read-only) a datovou stranku (read-write, execute-never)

    target_pt[PT_Entry(virt)] = (phys & 0xFFF00000) | DL1_Flags::Access_Type_Section_Address | DL1_Flags::Bufferable | DL1_Flags::Cacheable | DL1_Flags::Domain_0 | DL1_Flags::Access_Full_RW | DL1_Flags::TEX_001 | DL1_Flags::Shareable;
}

shmmem::CShared_Memory_Manager::CShared_Memory_Manager()
{
    //
}

shmmem::CShared_Memory_Manager::~CShared_Memory_Manager()
{
    //
}

char *shmmem::CShared_Memory_Manager::Map_File(const uint32_t requested_size, const uint32_t fd)
{

    const IFile *file = this->Retrieve_File(fd);
    // nejdrive se podivame, jestli je file otevreny a potom zkontrolujeme zda jeho obsahem jsou znaky
    if (!file || file->Get_File_Type() != NFile_Type_Major::Character)
    {
        return nullptr;
    }

    CShared_Memory *memory = sProcess_Resource_Manager.Get_Shared_Memory(file);
    if (!memory)
    {
        return nullptr;
    }

    uint32_t phys_addrs = memory->Get_Phys_Addrs();
    if (phys_addrs == 0)
    {
        uint32_t virt_addrs = sPage_Manager.Alloc_Page();
        if (virt_addrs == 0)
        {
            return nullptr;
        }
        phys_addrs = virt_addrs - mem::MemoryVirtualBase;
        memory->Set_Phys_Addrs(phys_addrs);
    }

    return this->Map_To_Process_Page(phys_addrs);;
}

const IFile *shmmem::CShared_Memory_Manager::Retrieve_File(const uint32_t fd)
{
    TTask_Struct *task = sProcessMgr.Get_Current_Process();

    // pro jistotu, kdyby soucasny process byl null nebo fd prekracoval maximalni pocet otevrenych souboru
    if (!task || fd >= Max_Process_Opened_Files)
    {
        return nullptr;
    }

    return task->opened_files[fd];
}

char *shmmem::CShared_Memory_Manager::Map_To_Process_Page(const uint32_t phys_addrs)
{
    TTask_Struct *current = sProcessMgr.Get_Current_Process();
    if (!current)
    {
        return nullptr;
    }

    unsigned long pt_phys_addrs = current->cpu_context.ttbr0 & ~ 0x3F;
    uint32_t *pt_virt_addrs = reinterpret_cast<uint32_t *>(pt_phys_addrs + mem::MemoryVirtualBase);

    // Pokusime se najit volnou stranku a pridat odkaz na sdileny ramec
    for (uint32_t i = 0; i < PT_Size; i++)
    {
        if (((pt_virt_addrs[i] & 0b11U) | DL1_Flags::Access_Type_Translation_Fault) == 0)
        {
            pt_virt_addrs[i] = (phys_addrs & 0xFFF00000)
            | DL1_Flags::Access_Type_Section_Address
            | DL1_Flags::Bufferable
            | DL1_Flags::Cacheable
            | DL1_Flags::Shareable
            | DL1_Flags::Access_Full_RW; // TODO maskovat dalsi priznaky pro danou stranku
            return reinterpret_cast<char *>(pt_virt_addrs[i]);
        }
    }

    return nullptr;
}

shmmem::CShared_Memory_Manager sShared_Memory_Manager; // vytvoreni statickeho manazeru pro spravu sdilene pameti
