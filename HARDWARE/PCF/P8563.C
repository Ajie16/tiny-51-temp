//******************************************
/*------------------------------------------
               ����ͷ�ļ�
------------------------------------------*/
//******************************************
#include "main.h"   
#include "I2C.h"


/*
#define	YEAR_REG	0x08
#define MONTH_REG	0x07
#define WEEK_REG	0x06
#define DATA_REG	0x05
#define HOUR_REG	0x04
#define MIN_REG		0x03
#define SEC_REG		0x02*/

#define P8563_SLA	0xA2


/*********************************ȫ�ֺ�������******************************/
//��P8563��ָ���Ĵ���д��ֵ
bit P8563_SendStr(unsigned char sub_addr, unsigned char *send_buf, unsigned char data_no);
//��P8563��ָ���Ĵ�������ֵ
bit P8563_RcvStr(unsigned char sub_addr, unsigned char *rec_buf, unsigned char data_no);




/**********************************
��P8563��ָ���Ĵ���д��ֵ
����ԭ�Σ�P8563_SendStr(unsigned char sub_addr, unsigned char *send_buf, unsigned char data_no)
�������ܣ����������ߵ����͵�ַ���ӵ�ַ,���ݣ��������ߵ�ȫ����,
		  �ӵ�ַsuba������������sָ������ݣ�
		  ����no���ֽڡ��������1��ʾ�����ɹ��������������
��ڲ�����unsigned char sub_addr, 
		  unsigned char *send_buf, 
		  unsigned char data_no
���ڲ����������Ƿ�ɹ���־λ
ע�⣺    ʹ��ǰ�����ѽ������ߡ�
**********************************/
bit P8563_SendStr(unsigned char sub_addr, unsigned char *send_buf, unsigned char data_no)
{
		bit	rec_result;

		rec_result = ISendStr(P8563_SLA, sub_addr, send_buf, data_no);
		
		return(rec_result);
}





/**********************************
//��P8563��ָ���Ĵ�������ֵ
����ԭ�Σ�P8563_RcvStr(unsigned char sub_addr, unsigned char *rec_buf, unsigned char data_no)
�������ܣ����������ߵ����͵�ַ���ӵ�ַ,���ݣ��������ߵ�ȫ����,
		  �ӵ�ַsuba������������sָ������ݣ�
		  ����no���ֽڡ��������1��ʾ�����ɹ��������������
��ڲ�����unsigned char sub_addr, 
		  unsigned char *rec_buf, 
		  unsigned char data_no
���ڲ����������Ƿ�ɹ���־λ
ע�⣺    ʹ��ǰ�����ѽ������ߡ�
**********************************/
bit P8563_RcvStr(unsigned char sub_addr, unsigned char *rec_buf, unsigned char data_no)
{
		bit	rec_result;

		rec_result = IRcvStr(P8563_SLA, sub_addr, rec_buf, data_no);

		rec_buf[0] &= 0x7F;
		rec_buf[1] &= 0x7F;
		rec_buf[2] &= 0x3F;
		rec_buf[3] &= 0x3F;
		rec_buf[4] &= 0x07;
		rec_buf[5] &= 0x1F;
		
		return(rec_result);
}
 
 
 
 	