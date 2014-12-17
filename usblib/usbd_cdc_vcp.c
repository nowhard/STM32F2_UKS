/**
  ******************************************************************************
  * @file    usbd_cdc_vcp.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    22-July-2011
  * @brief   Generic media access Layer.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#pragma     data_alignment = 4
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */

/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc_vcp.h"
//#include "stm32f4xx_usart.h"
//#include "stm32f4_discovery.h"


//USART_InitTypeDef USART_InitStructure;

/* These are external variables imported from CDC core to be used for IN
   transfer management. */
extern uint8_t  APP_Rx_Buffer []; /* Write CDC received data in this buffer.
                                     These data will be sent over USB IN endpoint
                                     in the CDC core functions. */
extern uint32_t APP_Rx_ptr_in;    /* Increment this pointer or roll it back to
                                     start address when writing received data
                                     in the buffer APP_Rx_Buffer. */

/* Private function prototypes -----------------------------------------------*/
static uint16_t VCP_Init     (void);
static uint16_t VCP_DeInit   (void);
static uint16_t VCP_Ctrl     (uint32_t Cmd, uint8_t* Buf, uint32_t Len);
//static uint16_t VCP_DataRx   (uint8_t* Buf, uint32_t Len);


CDC_IF_Prop_TypeDef VCP_fops =
{
  VCP_Init,
  VCP_DeInit,
  VCP_Ctrl,
  VCP_DataTx,
  VCP_DataRx
};

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  VCP_Init
  *         Initializes the Media on the STM32
  * @param  None
  * @retval Result of the opeartion (USBD_OK in all cases)
  */
static uint16_t VCP_Init(void)
{
  return USBD_OK;
}

/**
  * @brief  VCP_DeInit
  *         DeInitializes the Media on the STM32
  * @param  None
  * @retval Result of the opeartion (USBD_OK in all cases)
  */
static uint16_t VCP_DeInit( void )
{
    return USBD_OK;
}


/**
  * @brief  VCP_Ctrl
  *         Manage the CDC class requests
  * @param  Cmd: Command code
  * @param  Buf: Buffer containing command data (request parameters)
  * @param  Len: Number of data to be sent (in bytes)
  * @retval Result of the opeartion (USBD_OK in all cases)
  */
static uint16_t VCP_Ctrl (uint32_t Cmd, uint8_t* Buf, uint32_t Len)
{
  switch (Cmd)
  {
  case SEND_ENCAPSULATED_COMMAND:
    /* Not  needed for this driver */
    break;

  case GET_ENCAPSULATED_RESPONSE:
    /* Not  needed for this driver */
    break;

  case SET_COMM_FEATURE:
    /* Not  needed for this driver */
    break;

  case GET_COMM_FEATURE:
    /* Not  needed for this driver */
    break;

  case CLEAR_COMM_FEATURE:
    /* Not  needed for this driver */
    break;

  case SET_LINE_CODING:
    break;

  case GET_LINE_CODING:
    break;

  case SET_CONTROL_LINE_STATE:
    /* Not  needed for this driver */
    break;

  case SEND_BREAK:
    /* Not  needed for this driver */
    break;

  default:
    break;
  }

  return USBD_OK;
}

/**
  * @brief  VCP_DataTx
  *         CDC received data to be send over USB IN endpoint are managed in
  *         this function.
  * @param  Buf: Buffer of data to be sent
  * @param  Len: Number of data to be sent (in bytes)
  * @retval Result of the opeartion: USBD_OK if all operations are OK else VCP_FAIL
  */
uint16_t VCP_DataTx (uint8_t* Buf, uint32_t Len)
{
    uint32_t i;

    if ( Len < 0 )
        return USBD_FAIL;

    for ( i = 0; i < Len; i++ )
    {
        APP_Rx_Buffer[ APP_Rx_ptr_in++ ] = Buf[i];

        /* To avoid buffer overflow */
        if ( APP_Rx_ptr_in == APP_RX_DATA_SIZE )
        {
            APP_Rx_ptr_in = 0;
        }
    }
    return USBD_OK;
}

/**
  * @brief  VCP_DataRx
  *         Data received over USB OUT endpoint are sent over CDC interface
  *         through this function.
  *
  *         @note
  *         This function will block any OUT packet reception on USB endpoint
  *         untill exiting this function. If you exit this function before transfer
  *         is complete on CDC interface (ie. using DMA controller) it will result
  *         in receiving more data while previous ones are still not sent.
  *
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the opeartion: USBD_OK if all operations are OK else VCP_FAIL
  */

uint32_t counter=0, counter2=0;

//uint16_t VCP_DataRx( uint8_t* Buf, uint32_t Len )
//{
//    counter=0;
//	while(counter<Len)
//	{
//		APP_Rx_Buffer[APP_Rx_ptr_in] = Buf[counter2];
//		APP_Rx_ptr_in++;
//		counter++;
//		counter2++;
//
//		/* To avoid buffer overflow */
//		if(APP_Rx_ptr_in >= APP_RX_DATA_SIZE)
//		{
//			APP_Rx_ptr_in = 0;
//		}
//	}
//
//	if(counter2>64)
//	{
//		counter2=0;
//	}
//}
uint32_t APP_Rx_ptr_tail  = 0;

uint16_t VCP_DataRx (uint8_t* Buf, uint32_t Len)
{
  uint32_t i;

  for (i = 0; i < Len; i++)
  {
	  APP_Rx_Buffer[APP_Rx_ptr_in] = *(Buf + i);
	  APP_Rx_ptr_in++;
	  if(APP_Rx_ptr_in == APP_RX_DATA_SIZE)
		  APP_Rx_ptr_in = 0;

	  if(APP_Rx_ptr_in == APP_Rx_ptr_tail)
		 return USBD_FAIL;
  }

  return USBD_OK;
}

uint8_t VCP_get_char(uint8_t *buf)
{
 if(APP_Rx_ptr_in == APP_Rx_ptr_tail)
 	return 0;

 *buf = APP_Rx_Buffer[APP_Rx_ptr_tail];
 APP_Rx_ptr_tail++;
 if(APP_Rx_ptr_tail == APP_RX_DATA_SIZE)
  APP_Rx_ptr_tail = 0;

 return 1;
}

//uint8_t usb_cdc_kbhit(void){
//	if(APP_tx_ptr_head == APP_tx_ptr_tail){
//		return 0; //N�o h� caracter novo
//	}
//	else{
//		return 1; //H� caracter novo
//	}
//}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
