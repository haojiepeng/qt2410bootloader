#include <stdio.h>
#include "isr.h"
#include "sys.h"

extern void mmu_tlb_init(void);
extern void CleanMMUTable(void);
extern void StartMMU(unsigned int mmu_base);
extern void PortInit(void);
extern void InitIRQDevices(void);
extern void setupIRQEnv(void);
extern void Test_Adc(void);

int main(void)
{
 	char ch;
 	
 	
 	PortInit();
 	CleanMMUTable();
 	mmu_tlb_init();
 	StartMMU(MMU_TABLE_BASE);
 	setupIRQEnv();
 	InitIRQDevices();	
 	printf("Joey Software-Reset bootloader Test\n\r");
 	//Test_Adc();
 	//Test_DMA();
 	//NAND FLASH TEST
 	//NandFlashInit();	
	//NANDFlashTest();
	//IICInit();
	//TestIIC();
	//printf("Watchdog test....System will reboot after 5 seconds\n\r");
	//WDInit();
	//ethernet test
 	//InitEthernet();
 	//TestTransmitPacket();
 	//usb test
 	//usbInit();
 	//usbTest();
 	//tftp test
 	eth_init();
 	StartTFTPServer(0,0);
 	
 	while (1)
  	{
 		scanf("%c",&ch);
 		printf("%c",ch);
 		
		//Delay(1000);
  	}
}