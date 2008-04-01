#include "sys.h"
#include "usb.h"
extern volatile u_int download_addr;
extern volatile u_char *downPt;
extern volatile u_int download_len;
extern volatile u_int totalDmaCount;
extern volatile u_short checkSum;

static void RdPktEp3_CheckSum(u_char *buf,int num);

// ===================================================================
// All following commands will operate in case 
// - out_csr3 is valid.
// ===================================================================

#define CLR_EP3_OUT_PKT_READY() rOUT_CSR1_REG= ( out_csr3 &(~ EPO_WR_BITS)&(~EPO_OUT_PKT_READY) ) 
#define SET_EP3_SEND_STALL()	rOUT_CSR1_REG= ( out_csr3 & (~EPO_WR_BITS)| EPO_SEND_STALL) )
#define CLR_EP3_SENT_STALL()	rOUT_CSR1_REG= ( out_csr3 & (~EPO_WR_BITS)&(~EPO_SENT_STALL) )
#define FLUSH_EP3_FIFO() 		rOUT_CSR1_REG= ( out_csr3 & (~EPO_WR_BITS)|EPO_FIFO_FLUSH) )

// ***************************
// *** VERY IMPORTANT NOTE ***
// ***************************
// Prepare for the packit size constraint!!!

// EP3 = OUT end point. 

extern u_int UsbFunction;
extern u_int UsbState;
extern u_int UsbInLength;
extern u_char *UsbTxAddr;

u_char ep3Buf[EP3_PKT_SIZE];

void Ep3Handler(void)
{
    u_char out_csr3;
    int fifoCnt; 
 
    rINDEX_REG = 3;
    out_csr3 = rOUT_CSR1_REG;
    if(out_csr3 & EPO_OUT_PKT_READY)
    {   
		fifoCnt = rOUT_FIFO_CNT1_REG;
		
		if(download_len==0)
		{
			RdPktEp3((u_char *)downPt, 8);
			
			download_addr = *(u_int *)downPt;
			download_len  = *(u_int *)(downPt+4);
//			checkSum=0;
			downPt=(u_char *)download_addr;
			
			RdPktEp3_CheckSum((u_char *)downPt, fifoCnt-8); //The first 8-bytes are deleted.	    
  	    	downPt+=fifoCnt-8;  
  	    
	  	#if USBDMA
    	 	    //CLR_EP3_OUT_PKT_READY() is not executed. 
				//So, USBD may generate NAK until DMA2 is configured for USB_EP3;
     	    //DisableIrq(BIT_USBD);	
      	    return;	
	  	#endif	
		}
		else
		{
		#if USBDMA    	
			//DbgOut("<ERROR>");
		#endif    
		    RdPktEp3_CheckSum((u_char *)downPt,fifoCnt);
	    	downPt+=fifoCnt;  //fifoCnt=64
		}
	   	CLR_EP3_OUT_PKT_READY();
		return;
    }
   
    //I think that EPO_SENT_STALL will not be set to 1.
    if(out_csr3 & EPO_SENT_STALL)
    {   
		//DbgOut("[STALL]");
	   	CLR_EP3_SENT_STALL();
   		return;
    }	
}

void RdPktEp3_CheckSum(u_char *buf,int num)
{
    int i;
    	
    for(i=0;i<num;i++)
    {
        buf[i]=(u_char)rEP3_FIFO;
        checkSum+=buf[i];
    }
}

void ClearEp3OutPktReady(void)
{
    u_char out_csr3;
    rINDEX_REG=3;
    out_csr3=rOUT_CSR1_REG;
    CLR_EP3_OUT_PKT_READY();
}