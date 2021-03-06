/*****************************************************
文件名：I2C.c
文件功能：模拟I2C的底层的C子程序
编写者：张阳
编写日期：2020-08-10

*****************************************************/




/*头文件的包含*/
#include <absacc.h> 
#include <intrins.h>
#include "I2C.h"



/*************************全局变量*************************/
bit ack;	         /*应答标志位*/



/*************************函数实现*************************/
/**********************************
启动总线函数
函数原形：void Start_I2c(void)
函数功能：启动I2C总线,即发送I2C起始条件
入口参数：无
出口参数：无
**********************************/
void Start_I2c(void)
{
		SDA=1;   		  //发送起始条件的数据信号
		_Nop();
		SCL=1;
		_Nop();    		//起始条件建立时间大于4.7us,延时
		_Nop();
		_Nop();
		_Nop();
		_Nop();        
		SDA=0;   		  //发送起始信号
		_Nop();    		//起始条件锁定时间大于4us
		_Nop();
		_Nop();
		_Nop();
		_Nop();       
		SCL=0;   		  //钳住I2C总线，准备发送或接收数据
		_Nop();
		_Nop();
}




/**********************************
结束总线函数
函数原形：void Stop_I2c(void)
函数功能：结束I2C总线,即发送I2C结束条件
入口参数：无
出口参数：无
**********************************/
void Stop_I2c(void)
{
		SDA=0;  		  //发送结束条件的数据信号
		_Nop();   		//发送结束条件的时钟信号
		SCL=1;  		  //结束条件建立时间大于4us
		_Nop();
		_Nop();
		_Nop();
		_Nop();
		_Nop();
		SDA=1;  		  //发送I2C总线结束信号
		_Nop();
		_Nop();
		_Nop();
		_Nop();
}




/**********************************
字节数据发送函数
函数原形：void SendByte(unsigned char c)
函数功能：将数据c发送出去,可以是地址,也可以是数据,
		      发完后等待应答,并对此状态位进行操作.
		      (不应答或非应答都使ack=0 假)     
     	    发送数据正常，ack=1;ack=0表示被控器无应答或损坏。
入口参数：unsigned char c
出口参数：无
**********************************/
void SendByte(unsigned char c)
{
		unsigned char BitCnt;
		for(BitCnt=0;BitCnt<8;BitCnt++)  	//要传送的数据长度为8位
		{
				if((c<<BitCnt)&0x80)
				{
						SDA=1;   					            //判断发送位
				}
				else
				{
						SDA=0;
				}  
												
				_Nop();
				SCL=1;               			     //置时钟线为高，通知被控器开始接收数据位
				_Nop(); 
				_Nop();               			   //保证时钟高电平周期大于4us
				_Nop();
				_Nop();
				_Nop();         
				SCL=0; 
		}
			
		_Nop();
		_Nop();
		SDA=1;               				     //8位发送完后释放数据线，准备接收应答位
		_Nop();
		_Nop();   
		SCL=1;
		_Nop();
		_Nop();
		_Nop();
		if(SDA==1)
		{
				ack=0;
		}
		else
		{
				ack=1;        					       //判断是否接收到应答信号
		}     
				 
		SCL=0;
		_Nop();
		_Nop();
}




/**********************************
字节数据接收函数
函数原形：unsigned char RcvByte(void)
函数功能：用来接收从器件传来的数据,并判断总线错误(不发应答信号)，
     	    发完后请用应答函数。
入口参数：无
出口参数：unsigned char型，返回接收到的数据
**********************************/
unsigned char RcvByte(void)
{
		unsigned char retc;
		unsigned char BitCnt;
		
		retc=0; 
		SDA=1;             					//置数据线为输入方式
		for(BitCnt=0;BitCnt<8;BitCnt++)
		{
				_Nop();           
				SCL=0;       					    //置时钟线为低，准备接收数据位
				_Nop();
				_Nop();         				  //时钟低电平周期大于4.7us
				_Nop();
				_Nop();
				_Nop();
				SCL=1;       					    //置时钟线为高使数据线上数据有效
				_Nop();
				_Nop();
				retc=retc<<1;
				if(SDA==1)retc=retc+1; 		//读数据位,接收的数据位放入retc中
				_Nop();
				_Nop(); 
		}
		SCL=0;    
		_Nop();
		_Nop();

		return(retc);
}




/**********************************
应答子函数
函数原形：void Ack_I2c(bit a)
函数功能：主控器进行应答信号,(可以是应答或非应答信号)
入口参数：bit a
出口参数：无
**********************************/
void Ack_I2c(bit a)
{
  
		if(a==0)
		{
				SDA=0;
		}     								//在此发出应答或非应答信号
		else 
		{
				SDA=1;
		}
		_Nop();
		_Nop();
		_Nop();      
		SCL=1;
		_Nop();
		_Nop();              	//时钟低电平周期大于4us
		_Nop();
		_Nop();
		_Nop();  
		SCL=0;                //清时钟线，钳住I2C总线以便继续接收
		_Nop();
		_Nop();    
}




/**********************************
向无子地址器件发送字节数据函数
函数原形：bit ISendByte(unsigned char sla, unsigned char c)
函数功能：从启动总线到发送地址，数据，结束总线的全过程,从器件地址sla.
          如果返回1表示操作成功，否则操作有误。
入口参数：unsigned char sla, 
		      unsigned char c
出口参数：返回是否成功标志位
注意：    使用前必须已结束总线。
**********************************/
bit ISendByte(unsigned char sla, unsigned char c)
{
		Start_I2c();               			//启动总线
		SendByte(sla);            			//发送器件地址
		if(ack==0)
		{
				return(0);
		}
		SendByte(c);               			//发送数据
		if(ack==0)
		{	
				return(0);
		}
		Stop_I2c();                 	//结束总线 

		return(1);
}




/**********************************
向有子地址器件发送多字节数据函数
函数原形：ISendStr(uchar sla,uchar suba,uchar *s,uchar no)
函数功能：从启动总线到发送地址，子地址,数据，结束总线的全过程,
		      从器件地址sla，子地址suba，发送内容是s指向的内容，
		      发送no个字节。如果返回1表示操作成功，否则操作有误。
入口参数：unsigned char sla, 
		      unsigned char suba, 
		      unsigned char *s, 
		      unsigned char no
出口参数：返回是否成功标志位
注意：    使用前必须已结束总线。
**********************************/
bit ISendStr(unsigned char sla, unsigned char suba, unsigned char *s, unsigned char no)
{
		unsigned char i;

		Start_I2c();               			//启动总线
		SendByte(sla);            			//发送器件地址
		if(ack==0)return(0);
		SendByte(suba);            			//发送器件子地址
		if(ack==0)return(0);

		for(i=0;i<no;i++)
		{   
				SendByte(*s);               	//发送数据
				if(ack==0)return(0);
				s++;
		} 
		Stop_I2c();                 		//结束总线 

		return(1);
}







/**********************************
向有子地址器件读取多字节数据函数
函数原形：bit IRcvStr(unsigned char sla, unsigned char suba, unsigned char *s, unsigned char no)
函数功能：从启动总线到发送地址，子地址，读数据，结束总线的全过程,
		      从器件地址sla，子地址suba，读出的内容放入s指向的存储区，读no个字节。
          如果返回1表示操作成功，否则操作有误。
入口参数：unsigned char sla, 
		      unsigned char suba, 
		      unsigned char *s, 
		      unsigned char no
出口参数：返回是否成功标志位
注意：    使用前必须已结束总线。
**********************************/
bit IRcvStr(unsigned char sla, unsigned char suba, unsigned char *s, unsigned char no)
{
		unsigned char i;

		Start_I2c();               //启动总线
		SendByte(sla);             //发送器件地址
		if(ack==0)return(0);
		SendByte(suba);            //发送器件子地址
		if(ack==0)return(0);

		Start_I2c();
		SendByte(sla+1);
		if(ack==0)return(0);

		for(i=0;i<no-1;i++)
		{   
				*s=RcvByte();             //发送数据
				Ack_I2c(0);               //发送应答位  
				s++;
		} 
		*s=RcvByte();
		Ack_I2c(1);                 //发送非应位
		Stop_I2c();                 //结束总线 
		return(1);
} 




/**********************************
向无子地址器件读多字节数据函数
函数原形：bit mRcvByte(unsigned char sla, unsigned char *s,unsigned char no)
函数功能：从启动总线到发送地址，子地址，读数据，结束总线的全过程,
		      从器件地址sla,读出的内容放入s指向的存储区，读no个字节。
          如果返回1表示操作成功，否则操作有误。
入口参数：unsigned char sla, 
		      unsigned char *c
          
出口参数：返回是否成功标志位
注意：    使用前必须已结束总线。
**********************************/
bit mRcvByte(unsigned char sla, unsigned char *s,unsigned char no)
{
		unsigned char i;

		Start_I2c();               //启动总线
		SendByte(sla+1);           //发送器件地址
		if(ack==0)return(0);
		for(i=0;i<no-1;i++)
		{   
				*s=RcvByte();            //发送数据
				Ack_I2c(0);              //发送应答位  
				s++;
		} 
		*s=RcvByte();
		Ack_I2c(1);                //发送非应位
		Stop_I2c();                //结束总线 
		return(1);
} 




/**********************************
向无子地址器件发送多字节数据函数
函数原形：bit SendStr(unsigned char sla, unsigned char *s, unsigned char no)
函数功能：从启动总线到发送地址，子地址,数据，结束总线的全过程,
		      从器件地址sla，子地址suba，发送内容是s指向的内容，
		      发送no个字节。如果返回1表示操作成功，否则操作有误。
入口参数：unsigned char sla, 
		      unsigned char suba, 
		      unsigned char *s, 
		      unsigned char no
出口参数：返回是否成功标志位
注意：    使用前必须已结束总线。
**********************************/
bit mSendStr(unsigned char sla, unsigned char *s, unsigned char no)
{
		unsigned char i;

		Start_I2c();               			//启动总线
		SendByte(sla);            			//发送器件地址
		if(ack==0)return(0);

		for(i=0;i<no;i++)
		{   
				SendByte(*s);               	//发送数据
				if(ack==0)return(0);
				s++;
		} 
		Stop_I2c();                 		//结束总线 
		return(1);
}

