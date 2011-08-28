/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/

#include "diskio.h"
#include "SdCard.h"
#include "Rtc.h"		//karlno add
#include "Debug.h"
/*-----------------------------------------------------------------------*/
/* Correspondence between physical drive number and physical drive.      */

#define BlockSize            512U /* Block Size in Bytes */
#define SD_Mode				0		//0 dma,1 interrupt
SD_CardInfo SDCardInfo;
/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */

DSTATUS disk_initialize (
	BYTE drv				/* Physical drive nmuber (0..) */
)
{
	SD_Error Status = SD_OK;

	if(drv==0)
	{
    	Status = SD_Init();
    	if(Status != SD_OK)
    	{
        	return STA_NOINIT; //�������󣺳�ʼ��ʧ��
    	}
    	else
    	{
	  		Status = SD_GetCardInfo(&SDCardInfo); //��sd����Ϣ
	   		if (Status != SD_OK)
	    	{
	     		return  STA_NOINIT;//RES_NOTRDY;  //��NOT READY����
	        }
	       
            Status = SD_SelectDeselect((uint32_t) (SDCardInfo.RCA << 16));
	     	if (Status != SD_OK)
	     	{
	        	//return  STA_NOINIT;//RES_NOTRDY;  //��NOT READY����
	     	}
	     	
		    switch(SD_Mode)
		    {
		    	case 0:
		    		Status = SD_EnableWideBusOperation(SDIO_BusWide_4b);
		       		if (Status != SD_OK)
		        	{  
		         		return RES_NOTRDY;  //��NOT READY����
		        	}

		       		Status = SD_SetDeviceMode(SD_DMA_MODE);
		      		if (Status != SD_OK)
		      		{
		         		return RES_NOTRDY;  //��NOT READY����
		      		}
		      		break;

	    		case 1:  //�жϷ�ʽ
		       		Status = SD_EnableWideBusOperation(SDIO_BusWide_1b);
		       		if (Status != SD_OK)
		        	{  
		         		return RES_NOTRDY;  //��NOT READY����
		        	}
		        	
		       		Status = SD_SetDeviceMode(SD_INTERRUPT_MODE);  
		      		if (Status != SD_OK)
		      		{
		         		return RES_NOTRDY;  //��NOT READY����
		      		}
		       		break;
	       		
	     		default :
	     			return RES_NOTRDY;
	   		}
	 	}
    }
    else//��֧�ִ���0�Ĳ���
    {
		return STA_NOINIT;  
    }

    return 0;         
}

/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */

DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0..) */
)
{
	 if(drv==0)
	 {
	     return 0;               //��ʼ���ɹ�
	 }
	 else    //��֧�ִ���0�Ĳ���
	 {
		return STA_NOINIT;  
	 }

} 

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
DRESULT disk_read (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..255) */
)
{
	SD_Error Status=SD_OK; 

	if (!count) return RES_PARERR;  //count���ܵ���0�����򷵻ز�������

	if(drv==0)
	{		
 		switch(SD_Mode)
	  	{
	   		case 0:  //dma��ʽ
	      		if(count==1)// 1��sector�Ķ�����      
	      		{      
	    			Status = SD_ReadBlock((u32 *)(&buff[0]),sector << 9,BlockSize);                               
	      		}                                                
	      		else                    //���sector�Ķ�����     
	      		{    
	       			Status = SD_ReadMultiBlocks((u8 *)(&buff[0]),sector << 9,BlockSize,count);                                      
	      		}
	     		break;
	     		
	  		case 1:  //�жϷ�ʽ
	      		if(count==1)            // 1��sector�Ķ�����      
	      		{      
	    			Status = SD_ReadBlock((u32 *)(&buff[0]),sector<<9,BlockSize);                                              
	      		}                                                
	      		else                    //���sector�Ķ�����     
	      		{    
	       			Status = SD_ReadMultiBlocks((u8 *)(&buff[0]),sector<<9 ,BlockSize,count);                                     
	      		}  	  
	      		break;
	      		
	    	default:
	    		Status=SD_ERROR;
		}
		
	    //��������ֵ����sdcard.c�ķ���ֵת��ff.c�ķ���ֵ
	    if(Status == SD_OK)
	        return RES_OK;
	    else
	        return RES_ERROR;
	}
	else//��֧�ִ���0�Ĳ���
    {
		return RES_ERROR;  
    }
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
#if _READONLY == 0
DRESULT disk_write (
	BYTE drv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	BYTE count			/* Number of sectors to write (1..255) */
)
{
   SD_Error Status=SD_OK;
  
    if (!count)  return RES_PARERR;  //count���ܵ���0�����򷵻ز�������

	if(drv==0)
	{
 		switch(SD_Mode)
  		{
   			case 0:  //dma��ʽ
	      		if(count==1)            // 1��sector��д����      
	      		{      
	          		Status = SD_WriteBlock((u32 *)(&buff[0]),sector << 9,BlockSize);                                      
	      		}                                                
	     	 	else                    //���sector��д����     
	      		{    
	          		Status = SD_WriteMultiBlocks((u8 *)(&buff[0]),sector << 9,BlockSize,count);                                          
	      		}  	  
				break;

   			case 1:  //�жϷ�ʽ
				if(count==1)           // 1��sector��д����      
      			{      
    				Status = SD_WriteBlock((u32 *)(&buff[0]),sector << 9 ,BlockSize);                                            
      			}                                                
      			else                    //���sector��д����     
      			{    
       				Status = SD_WriteMultiBlocks((u8 *)(&buff[0]),sector << 9 ,BlockSize,count);                                     
      			}  
				break;

			default :
				Status=SD_ERROR;
		}
                                        
	    //��������ֵ����sdcard.c�ķ���ֵת��ff.c�ķ���ֵ
	    if(Status == SD_OK)
	        return RES_OK;
	    else
	        return RES_ERROR;
	}
	else//��֧�ִ���0�Ĳ���
    {
		return RES_ERROR;  
    }
}
#endif /* _READONLY */

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	u32 x, y, z;
    DRESULT res=RES_OK;
	
    if (drv==0)
    {
    	//FATFSĿǰ�汾���账��CTRL_SYNC��GET_SECTOR_COUNT��GET_BLOCK_SIZ��������
    	switch(ctrl)
    	{
     		case CTRL_SYNC:
				if(SD_GetTransferState()==SD_TRANSFER_OK)
         		{
             		res = RES_OK;
         		}
         		else
         		{
             		res = RES_ERROR;
         		}
         		break;
        
     		case GET_BLOCK_SIZE:
				*(WORD*)buff = BlockSize;
         		res = RES_OK;
         		break;

     		case GET_SECTOR_COUNT:     //��������
		        ////formula of the capacity///////////////
		        //
		        //  memory capacity = BLOCKNR * BLOCK_LEN
		        // 
		        // BLOCKNR = (C_SIZE + 1)* MULT
		        //
		        //           C_SIZE_MULT+2
		        // MULT = 2
		        //
		        //               READ_BL_LEN
		        // BLOCK_LEN = 2
		    	//////////////////////////////////////////
     			if (SD_GetCardInfo(&SDCardInfo)==SD_OK)//��sd����Ϣ
         		{
          			x=SDCardInfo.SD_csd.DeviceSize+1; //C_SIZE + 1
    				y=SDCardInfo.SD_csd.DeviceSizeMul+2; //C_SIZE_MULT+2
    				z=SDCardInfo.SD_csd.RdBlockLen+y;
       				*(DWORD*)buff =x<<z; 
					res = RES_OK;
				}
				else
				{
					res = RES_ERROR ;
				}
				break;

     		default:
				res = RES_PARERR;
    	}

    	return res;
    }
    else	//��֧�ִ���0�Ĳ���
    {
		return RES_ERROR;  
    }
}


/*-----------------------------------------------------------------------*/
/* User defined to give a current time to fatfs module          */
/* 31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31)     */                                                                                                                                                                                                                                          
/* 15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2)           */ 
/*-----------------------------------------------------------------------*/                                                                                                                                                                                                                                               
DWORD get_fattime (void)
{
#if 0
    RTC_TIME t;
    DWORD date;
    
    RTC_GetTime(&t);
    t.year-=1980;
    
    date = 0;
    date = (t.year<< 25) | (t.mon<<21) | (t.day<<16)|\
            (t.hour<<11) | (t.min<<5) | (t.sec>>1);

    return date;
#else
	return 0;
#endif
}

