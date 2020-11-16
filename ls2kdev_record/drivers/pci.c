#include <stdio.h>
#include <rtthread.h>

struct pci_header
{
    uint16_t VendorID;
    uint16_t DeviceID;
    uint16_t Command;
    uint16_t Status;
    uint32_t RevisionID : 8;
    uint32_t ClassCode : 24;
    uint8_t CachelineSize;
    uint8_t LatencyTimer;
    uint8_t HeaderType;
    uint8_t BIST;
    uint32_t BaseAddressRegister[6];
    uint32_t CardbusCISPointer;
    uint16_t SubsystemVendorID;
    uint16_t SubsystemID;
    uint32_t ExpansionROMBaseAddress;
    uint32_t CapabilitiesPointer : 8;
    uint32_t resv1 : 24;
    uint32_t resv2;
    uint8_t InterruptLine;
    uint8_t InterruptPin;
    uint8_t Min_Gnt;
    uint8_t Max_Lat;
};

void print_pci_header(struct pci_header *pci)
{
    rt_kprintf("VendorID:%4x\n", pci->VendorID);
    rt_kprintf("DeviceID:%4x\n", pci->DeviceID);
    rt_kprintf("Command:%4x\n", pci->Command);
    rt_kprintf("Status:%4x\n", pci->Status);
    rt_kprintf("RevisionID:%8x\n", pci->RevisionID);
    rt_kprintf("ClassCode:%8x\n", pci->ClassCode);
    rt_kprintf("CachelineSize:%2x\n", pci->CachelineSize);
    rt_kprintf("LatencyTimer:%2x\n", pci->LatencyTimer);
    rt_kprintf("HeaderType:%2x\n", pci->HeaderType);
    rt_kprintf("BIST:%2x\n", pci->BIST);


    rt_kprintf("BaseAddressRegister[6]:\n");
    for(int i=0;i<6;i++)
    rt_kprintf("[%d]:%8x\n",i,pci->BaseAddressRegister[i]);
    
    rt_kprintf("CardbusCISPointer:%8x\n", pci->CardbusCISPointer);
    rt_kprintf("SubsystemVendorID:%4x\n", pci->SubsystemVendorID);
    rt_kprintf("SubsystemID:%4x\n", pci->SubsystemID);
    rt_kprintf("ExpansionROMBaseAddress:%8x\n", pci->ExpansionROMBaseAddress);
    rt_kprintf("CapabilitiesPointer:%8x\n", pci->CapabilitiesPointer);
    rt_kprintf("resv1:%8x\n", pci->resv1);
    rt_kprintf("resv2:%8x\n", pci->resv2);
    rt_kprintf("InterruptLine:%2x\n", pci->InterruptLine);
    rt_kprintf("InterruptPin:%2x\n", pci->InterruptPin);
    rt_kprintf("Min_Gnt:%2x\n", pci->Min_Gnt);
    rt_kprintf("Max_Lat:%2x\n", pci->Max_Lat);
}

int pci_get_reg(int ac, unsigned char *av[])
{

    //0xFE00001800 GMAC1
    //0xFE00003800 HDA (High Definition Audio Specification Revision 1.0a)
    if (ac == 2)
    {
        uint64_t pci_base = strtoul(av[1], NULL, 0);
        struct pci_header *p = (struct pci_header *)(0x9000000000000000 | pci_base);
        uint64_t base_addr = (0x9000000000000000 | ((p->BaseAddressRegister[0]) & 0xffffff00));

        print_pci_header(p);

        rt_kprintf("\nBASE_ADDR:%16x\n", base_addr);
    }
    else
    {
        rt_kprintf("usage: pci_get_reg CONF_HEADER_CONF");
    }
}
MSH_CMD_EXPORT(pci_get_reg, test);