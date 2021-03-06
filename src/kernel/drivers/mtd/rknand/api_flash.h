/********************************************************************************
*********************************************************************************
			COPYRIGHT (c)   2004 BY ROCK-CHIP FUZHOU
				--  ALL RIGHTS RESERVED  --

File Name:  api_flash.h
Author:     XUESHAN LIN
Created:    1st Dec 2008
Modified:
Revision:   1.00
Modify log:
            1.01    ����FtlClose�ӿں���    2009.10.15  lxs
********************************************************************************
********************************************************************************/
#ifndef _API_FLASH_H
#define _API_FLASH_H

//1����ԭ������
/***************************************************************************
��������:FTL��FLASH��ʼ��
��ڲ���:��
���ڲ���:
        0=�������� 
        1=ӳ������, ��ǿ�Ƶ͸�
        2=flash��ʼ��ʧ��,������Ӳ�������Ҫ���й̼�����
        ����ֵ:�ڲ�����, ����������ʹ��
˵    ��:�ϵ�ֻ�����һ�γ�ʼ������
***************************************************************************/
extern int FtlInit(unsigned int nandcBaseAddr,unsigned char pageRemapEn);
extern int FTLInit_WithoutPageRemap(void);
extern int FTLInit(void);


/***************************************************************************
��������:��ȡFLASH��������
��ڲ���:
        DISK_NAND_CODE:�̼���
        DISK_NAND_DATA:ϵͳ������
        DISK_NAND_USER:�û���
���ڲ���:������������
˵    ��:
***************************************************************************/
extern unsigned int FtlGetCapacity(unsigned char LUN);

/***************************************************************************
��������:�������ӿ�
��ڲ���:LUN=�߼�������, Index=��ʼ������ַ, buf=���ݻ�����, nSec=������
���ڲ���:0=��ȡ��ȷ; ��0=��ȡ����, ���ݲ�����
���ú���:
˵    ��:
***************************************************************************/
extern int FtlRead(unsigned char LUN, unsigned int Index, unsigned int nSec, void *buf);

/***************************************************************************
��������:д�����ӿ�
��ڲ���:LUN=�߼�������, Index=��ʼ������ַ, buf=���ݻ�����, nSec=������
���ڲ���:0=��ȷд��; ��0=д����ʧ��, ����û�б���ȷд��
���ú���:
˵    ��:
***************************************************************************/
extern int FtlWrite(unsigned char LUN, unsigned int Index, unsigned int nSec, void *buf);

/***************************************************************************
��������:MLC FLASH����ˢ��
��ڲ���:
���ڲ���:
���ú���:
˵    ��:��ϵͳIDLE/�ڱȽϼ��ж�����ʱ(��A-B���š�Ƶ������Դ��)���øú���, 
				 �ܼ�ʱ������Ƶ�����ж���������,����ִ��ʱ����ܽϳ�(����ms������s)
***************************************************************************/
extern	void    FlashRefreshHook(void);

/***************************************************************************
��������:AHB��Ƶ����ʱ��Ҫ���ô˽ӿ�������FLASHʱ��
��ڲ���:AHBnKHz=��Ƶ(AHB)
���ڲ���:
���ú���:
***************************************************************************/
extern	void    FlashTimingCfg(unsigned int AHBnKHz);

/***************************************************************************
��������:FTL �رգ��ػ�ʱ����
��ڲ���:��
���ڲ���:��
˵    ��:�ػ�ʱ��������д�ļ��������������
***************************************************************************/
extern	void  FtlClose(void);

/***************************************************************************
��������:FTL ��ʱ�ص�������ϵͳ1S���ҵ���һ��
��ڲ���:��
���ڲ���:��
˵    ��:FTL��һЩCACHE�ڶ�ʱ�л�дflash
***************************************************************************/
extern	void  FtlTimeHook(void);

/***************************************************************************
��������:��ȡ flash page ��С���Ա����濪����,
��ڲ���:��
���ڲ���:page��С��sector��λ
���ú���:
˵��: ��Ҫ��FTLInit��ſ���
***************************************************************************/
extern	int   FlashGetPageSize(void);

/***************************************************************************
��������:AHB��Ƶ����ʱ��Ҫ���ô˽ӿ�������FLASHʱ��
��ڲ���:AHBnMHz=��Ƶ(AHB)
���ڲ���:
���ú���:
***************************************************************************/
extern	void    FlashTimingCfg(unsigned int AHBnKHz);

/***************************************************************************
��������:ϵͳ��д������
��ڲ���:
���ڲ���:
���ú���:
***************************************************************************/
extern void FtlFlashSysProtSetOn(void);

/***************************************************************************
��������:ϵͳ��д����
��ڲ���:
���ڲ���:
���ú���:
***************************************************************************/
extern void FtlFlashSysProtSetOff(void);

/***************************************************************************
��������:����ϵͳ�̣�����ʱʹ��
��ڲ���:
���ڲ���:
���ú���:
***************************************************************************/
extern void FtlLowFormatSysDisk(void);

extern int FtlWriteImage(unsigned int Index, unsigned int nSec, void *buf);
/***************************************************************************
��������:�����л����cache д��cache ��
��ڲ���:
���ڲ���:
���ú���:
***************************************************************************/
extern void	FtlCacheDelayWriteALL(void);

/***************************************************************************
��������:���ô���д������ַ��
��ڲ���:LBA��ַ��С��LBA�ĵ�ַд����
���ڲ���:
���ú���:
***************************************************************************/
void FtlSetSysProtAddr(int LBA);


/***************************************************************************
��������:��ȡFLASH��������
��ڲ���:�̷�
���ڲ���:����,������Ϊ��λ
���ú���:
***************************************************************************/
int FtlGetPageZoneCapacity(void);

/***************************************************************************
��������:��������
��ڲ���:Index=������, nSec=������
���ڲ���:�����������ڻ�����buf��
���ú���:
***************************************************************************/
int FtlPageWrite(int Index, int nSec, void *buf);


/***************************************************************************
��������:��������
��ڲ���:Index=������, nSec=������
���ڲ���:�����������ڻ�����buf��
���ú���:
***************************************************************************/
int FtlPageRead(int Index, int nSec, void *buf);

/***************************************************************************
��������:��ȡ SN sector��Ϣ
��ڲ���:pbuf
���ڲ���:
���ú���:
ע����Ϣ����Ҫ��flash �������غ���ܵ��ã�pbuf��С��Ҫ���ڵ���512 bytes��������ϢΪ512
          bytes��
***************************************************************************/
char GetSNSectorInfo(char * pbuf);

/**************************************************************************
 *
 */
char GetSNSectorInfoBeforeNandInit(char * pbuf);

/**************************************************************************
 *
 */
char GetVendor0InfoBeforeNandInit(char * pbuf);

/**************************************************************************
 *
 */
char GetChipSectorInfo(char * pbuf);

/**************************************************************************
 *
 */
int GetParamterInfo(char * pbuf , int len);

/**************************************************************************
 *
 */
int GetflashDataByLba(int lba,char * pbuf , int len);

/**************************************************************************
 *
 */
extern int NandInit(void);
extern int NandRead(unsigned int Index, unsigned int nSec, void * buf) ;
extern int NandWriteImage(unsigned int Index, unsigned int nSec, void * buf);
extern int NandWrite(unsigned int Index, unsigned int nSec, void * buf) ;
extern int NandDeInit(void) ;
extern int NandForceDeInit(void);
extern unsigned int NandGetCapacity(void); 
extern void NandSetSysProtAddr(unsigned int SysImageWriteEndAdd);
extern void FtlDelayWriteCacheEn(unsigned int en);
#endif
