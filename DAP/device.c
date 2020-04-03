#include "cmsis_compiler.h"
#include "cmsis_os2.h"
#include "rl_usb.h"
#include "device.h"
void DEVICE_Init(void)
{
	//借以实现时间戳更新
	 CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  /**
    * On Cortex-M7 core there is a LAR register in DWT domain.
    * Any time we need to setup DWT registers, we MUST write
    * 0xC5ACCE55 into LAR first. LAR means Lock Access Register.
    */
  DWT->CYCCNT = 0;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}
static volatile int osFlags;  /* Use "volatile" to prevent GCC optimizing the code. */

void osKernelInitialize(void)
{
  osFlags = 0;
  return;
}
osThreadId_t osThreadNew(void (*func)(void *), void * n, void * ctx)
{
  (void)n;

  (*func)(ctx);
  return 0;
}
int osThreadFlagsWait(int mask, int b, int c)
{
  (void)b;
	(void)c;

  int ret;

  while((osFlags&mask) == 0)
  {
    ;
  }
  ret = osFlags; osFlags &= ~mask;
  return ret;
}
void osThreadFlagsSet(int tid, int f)
{
  (void)tid;

  osFlags |= f;
  return;
}

/**
  * Replace USB Middleware of Keil MDK
  */
extern USBD_HandleTypeDef hUsbDeviceFS;

/**
  * @brief  I use USB Middleware from STMicro. The func "MX_USB_DEVICE_Init"
  *         is used to initialize the USB device peripheral.
  * @param  n: Unused
  */
void USBD_Initialize(int n)
{
  (void)n;

  /**
    * I inserted the call of USBD_HID0_Initialize/USBD_HID0_Uninitialize
    * into usbd_custom_hid_if.c. Please refer:
    *
    * line 179  @ usbd_custom_hid_if.c
    * line 191  @ usbd_custom_hid_if.c
    */
  /* USBD_HID0_Initialize(); */
  return;
}
void USBD_Connect(int n)
{
  (void)n;

  return;
}
int USBD_Configured(int n)
{
  (void)n;

  return (hUsbDeviceFS.dev_state == USBD_STATE_CONFIGURED ? 1:0);
}

/**
  * @brief  This routine is used to send a REPORT to the host.
  * @param  a: Unused
  * @param  b: Unused
  * @param  report: Data sent to host
  * @param  len: Data length, usually to be "DAP_PACKET_SIZE"
  */
void USBD_HID_GetReportTrigger(int a, int b, void * report, int len)
{
  (void)a;
  (void)b;

  if (USBD_OK != USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, report, len))
  {
    ;
  }
  return;
}

bool USBD_HID0_SetReport (uint8_t rtype, uint8_t req, uint8_t rid, const uint8_t *buf, int32_t len);
/**
  * @brief  This is a bridge routine to connect the USB Middleware and the DAP module.
  *
  *         When a OUTPUT REPORT from the host is got by USB peripheral, a interface func
  *         "CUSTOM_HID_OutEvent_FS" will be invoked by USB Middleware. I did a tiny
  *         modification for the interface func to call USBD_OutEvent. Please refer:
  *
  *         line 99   @ usbd_customhid.h
  *         line 160  @ usbd_custom_hid_if.c
  *         line 206  @ usbd_custom_hid_if.c
  */
void USBD_OutEvent(void)
{
  USBD_CUSTOM_HID_HandleTypeDef *hhid = (USBD_CUSTOM_HID_HandleTypeDef *)hUsbDeviceFS.pClassData;
  USBD_HID0_SetReport(HID_REPORT_OUTPUT, 0, 0, hhid->Report_buf, USBD_CUSTOMHID_OUTREPORT_BUF_SIZE);
}

int32_t USBD_HID0_GetReport (uint8_t rtype, uint8_t req, uint8_t rid, uint8_t *buf);
/**
  * @brief  This is a bredge routine to connect the USB Middleware and the DAP module.
  *
  *         After we use "USBD_HID_GetReportTrigger" to send a INPUT REPORT to the host,
  *         an interrupt will be generated by USB peripheral when the host responses an ACK.
  *         The func "USBD_CUSTOM_HID_DataIn" of usbd_customhid.c will be invoked. I added a
  *         new interface func into the structure "USBD_CUSTOM_HID_ItfTypeDef" and inserted
  *         a func call into "USBD_CUSTOM_HID_DataIn". Then this bridge func "USBD_InEvent"
  *         could be invoked by the USB Middleware. Please refer:
  *
  *         line 101  @ usbd_customhid.h
  *         line 218  @ usbd_custom_hid_if.c
  *         line 634  @ usbd_customhid.c
  */
void USBD_InEvent(void)
{
  int32_t len;

  USBD_CUSTOM_HID_HandleTypeDef *hhid = (USBD_CUSTOM_HID_HandleTypeDef *)hUsbDeviceFS.pClassData;
  if ((len=USBD_HID0_GetReport(HID_REPORT_INPUT, USBD_HID_REQ_EP_INT, 0, hhid->Report_buf)) > 0)
  {
    USBD_HID_GetReportTrigger(0, 0, hhid->Report_buf, len);
  }
}