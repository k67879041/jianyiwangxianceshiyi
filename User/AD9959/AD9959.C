#include "AD9959.h"
#include "main.h"
#include <math.h>  // 提供log10()等数学函数
#include "TouchLCD.h"

uint8_t CSR_DATA0[1] = {0x10};      // 开 CH0
uint8_t CSR_DATA1[1] = {0x20};      // 开 CH1
uint8_t CSR_DATA2[1] = {0x40};      // 开 CH2
uint8_t CSR_DATA3[1] = {0x80};      // 开 CH3		
uint32_t SinFre[4] = {50, 50,50,50 };
uint32_t SinAmp[4] = {1023, 1023, 1023, 1023};
uint32_t SinPhr[4] = {0, 4095, 4095*3, 4095*2};																	

uint8_t FR2_DATA[2] = {0x00,0x00};//default Value = 0x0000
uint8_t CFR_DATA[3] = {0x00,0x03,0x02};//default Value = 0x000302	   
																	
uint8_t CPOW0_DATA[2] = {0x00,0x00};//default Value = 0x0000   @ = POW/2^14*360
																	
uint8_t LSRR_DATA[2] = {0x00,0x00};//default Value = 0x----
																	
uint8_t RDW_DATA[4] = {0x00,0x00,0x00,0x00};//default Value = 0x--------
																	
uint8_t FDW_DATA[4] = {0x00,0x00,0x00,0x00};//default Value = 0x--------




															
uint8_t FR1_DATA[3] = {0xD0,0x00,0x00};//VCO gain control[23]=1系统时钟高于255Mhz; PLL[22:18]=10100,20倍频,20*25M=500MHZ; Charge pump control = 75uA 


//uint8_t FR2_DATA[2] = {0x00,0x00};	// 双方向扫描，即从起始值扫到结束值后，又从结束值扫到起始值
//uint8_t FR2_DATA[2] = {0x80,0x00};// 单方向扫描，即从起始值扫到结束值后，又从起始值扫到结束值，以此类推

double ACC_FRE_FACTOR = 8.589934592;	//频率因子8.589934592=(2^32)/500000000 其中500M=25M*20(倍频数可编程)

//uint8_t CFR_DATA[3] = {0x00,0x03,0x02};//default Value = 0x000302	   //通道功能寄存器

																	
																
																	
/************************************************************
** 函数名称 ：void AD9959_Init(void)  
** 函数功能 ：初始化控制AD9959需要用到的IO口,及寄存器
** 入口参数 ：无
** 出口参数 ：无
** 函数说明 ：无
**************************************************************/
void AD9959_Init(void)  
{ 
//	GPIO_InitTypeDef  GPIO_InitStructure;       
	uint8_t FR1_DATA[3] = {0xD0,0x00,0x00};//20倍频 Charge pump control = 75uA FR1<23> -- VCO gain control =0时 system clock below 160 MHz;
	//		u8 FR1_DATA[1] = {0x40};
	//u8 ACR_DATA[3] = {0x00,0x00,0x00};//default Value = 0x--0000 Rest = 18.91/Iout 	 
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | 
//							RCC_AHB1Periph_GPIOB | 
//							RCC_AHB1Periph_GPIOD | 
//							RCC_AHB1Periph_GPIOG, ENABLE); //PA,PC端口时钟使能

//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_11| GPIO_Pin_12;//初始化管脚PA 0.1.2.3.4.5
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	//推挽输出
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为2MHz
//	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOC

//	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;//初始化管脚PA 0.1.2.3.6.7		
//	GPIO_Init(GPIOB, &GPIO_InitStructure);					 			//根据设定参数初始化GPIOA
//	
//	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3 | GPIO_Pin_6 | GPIO_Pin_7;//初始化管脚PA 0.1.2.3.6.7		
//	GPIO_Init(GPIOD, &GPIO_InitStructure);					 			//根据设定参数初始化GPIOA
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_9 | GPIO_Pin_11 | GPIO_Pin_15;    
//	GPIO_Init(GPIOG,&GPIO_InitStructure);

		
	Intserve();  //IO口电平状态初始化
	IntReset();  //AD9959复位  
	
	//初始化功能寄存器
	AD9959_WriteData(FR1_ADD,3,FR1_DATA);//写功能寄存器1
	AD9959_WriteData(FR2_ADD,2,FR2_DATA);//
} 

//延时
void delay1 (uint32_t length)
{
	length = length*12;
   while(length--);
}
/************************************************************
** 函数名称 ：void Intserve(void)		   
** 函数功能 ：IO口电平状态初始化
**************************************************************/
void Intserve(void)		   
{   
	AD9959_PWR=0;
    CS = 1;
    SCLK = 0;
    UPDATE = 0;
    PS0 = 0;
    PS1 = 0;
    PS2 = 0;
    PS3 = 0;
    SDIO0 = 0;
    SDIO1 = 0;
    SDIO2 = 0;
    SDIO3 = 0;
}

/************************************************************
** 函数名称 ：void IntReset(void)		   
** 函数功能 ：AD9959复位
**************************************************************/
void IntReset(void)	  
{
  Reset = 0;
	delay1(1);
	Reset = 1;
	delay1(30);
	Reset = 0;
}

/************************************************************
** 函数名称 void IO_Update(void)  
** 函数功能 ： AD9959更新数据
**************************************************************/
void IO_Update(void)  
{
	UPDATE = 0;
	delay1(2);
	UPDATE = 1;
	delay1(4);
	UPDATE = 0;
}

/************************************************************
** 函数名称 ：void AD9959_WriteData(u8 RegisterAddress, u8 NumberofRegisters, u8 *RegisterData)
** 函数功能 ：使用模拟SPI向AD9959写数据
** 入口参数 ：RegisterAddress: 寄存器地址
						NumberofRegisters: 要写入的字节数
						*RegisterData: 数据起始地址
** 出口参数 ：无
** 函数说明 ：无
**************************************************************/
void AD9959_WriteData(uint8_t RegisterAddress, uint8_t NumberofRegisters, uint8_t *RegisterData)
{
	uint8_t	ControlValue = 0;
	uint8_t	ValueToWrite = 0;
	uint8_t	RegisterIndex = 0;
	uint8_t	i = 0;

	ControlValue = RegisterAddress;
//写入地址
	SCLK = 0;
	CS = 0;	 
	for(i=0; i<8; i++)
	{
		SCLK = 0;
		if(0x80 == (ControlValue & 0x80))
			SDIO0= 1;	  
		else
			SDIO0= 0;	  
		SCLK = 1;
		ControlValue <<= 1;
	}
	SCLK = 0;
//写入数据
	for (RegisterIndex=0; RegisterIndex<NumberofRegisters; RegisterIndex++)
	{
		ValueToWrite = RegisterData[RegisterIndex];
		for (i=0; i<8; i++)
		{
			SCLK = 0;
			if(0x80 == (ValueToWrite & 0x80))
				SDIO0= 1;	  
			else
				SDIO0= 0;	  
			SCLK = 1;
			ValueToWrite <<= 1;
		}
		SCLK = 0;		
	}	
  CS = 1;
} 

/************************************************************
** 函数名称 ：void Write_CFTW0(uint32_t fre)
** 函数功能 ：写CFTW0通道频率转换字寄存器
** 入口参数 ： Freq:	写入频率，范围0~200 000 000 Hz
** 出口参数 ：无
** 函数说明 ：无
**************************************************************/
void Write_CFTW0(uint32_t fre)
{
		uint8_t CFTW0_DATA[4] ={0x00,0x00,0x00,0x00};	//中间变量
	  uint32_t Temp;            
	  Temp=(uint32_t)fre*ACC_FRE_FACTOR;	 
	  CFTW0_DATA[3]=(uint8_t)Temp;
	  CFTW0_DATA[2]=(uint8_t)(Temp>>8);
	  CFTW0_DATA[1]=(uint8_t)(Temp>>16);
	  CFTW0_DATA[0]=(uint8_t)(Temp>>24);		
		AD9959_WriteData(CFTW0_ADD,4,CFTW0_DATA);//CTW0 address 0x04
}

/************************************************************
** 函数名称 ：void Write_ACR(uint16_t Ampli)
** 函数功能 ：写ACR通道幅度转换字寄存器
** 入口参数 ：Ampli:    输出幅度,范围0~1023，控制值0~1023对应输出幅度0~500mVpp左右
** 出口参数 ：无
** 函数说明 ：无
**************************************************************/
void Write_ACR(uint16_t Ampli)
{ 
	uint32_t A_temp=0;
	uint8_t ACR_DATA[3] = {0x00,0x00,0x00};//default Value = 0x--0000 Rest = 18.91/Iout 
  A_temp=Ampli|0x1000;
	
	ACR_DATA[1] = (uint8_t)(A_temp>>8); //高位数据
	ACR_DATA[2] = (uint8_t)A_temp;  //低位数据
  AD9959_WriteData(ACR_ADD,3,ACR_DATA); //ACR address 0x06.CHn设定幅度
}

/************************************************************
** 函数名称 ：void Write_CPOW0(uint16_t Phase)
** 函数功能 ：写CPOW0通道相位转换字寄存器
** 入口参数 ：Phase:		输出相位,范围：0~16383(对应角度：0°~360°)
** 出口参数 ：无
** 函数说明 ：无
**************************************************************/
void Write_CPOW0(uint16_t Phase)
{
	uint8_t CPOW0_data[2] = {0x00,0x00};
	CPOW0_data[1]=(uint8_t)Phase;
	CPOW0_data[0]=(uint8_t)(Phase>>8);

	AD9959_WriteData(CPOW0_ADD,2,CPOW0_data);//CPOW0 address 0x05.CHn设定相位
}

/************************************************************
** 函数名称 ：void Write_LSRR(uint8_t rsrr,uint8_t fsrr)
** 函数功能 ：写LSRR线性扫描斜率寄存器
** 入口参数 ：	rsrr:	上升斜率,范围：0~255
							fsrr:	下降斜率,范围：0~255
** 出口参数 ：无
** 函数说明 ：无
**************************************************************/
void Write_LSRR(uint8_t rsrr,uint8_t fsrr)
{
	uint8_t LSRR_data[2]={0x00,0x00};

	LSRR_data[1]=rsrr;	
	LSRR_data[0]=fsrr;//高8位下降斜率

	AD9959_WriteData(LSRR_ADD,2,LSRR_data);
}

/************************************************************
** 函数名称 ：void Write_RDW(uint32_t r_delta)
** 函数功能 ：写RDW上升增量寄存器
** 入口参数 ：r_delta:上升增量,0-4294967295
** 出口参数 ：无
** 函数说明 ：无
**************************************************************/
void Write_RDW(uint32_t r_delta)
{
		uint8_t RDW_data[4] ={0x00,0x00,0x00,0x00};	//中间变量          
 
	  RDW_data[3]=(uint8_t)r_delta;
	  RDW_data[2]=(uint8_t)(r_delta>>8);
	  RDW_data[1]=(uint8_t)(r_delta>>16);
	  RDW_data[0]=(uint8_t)(r_delta>>24);		
		AD9959_WriteData(RDW_ADD,4,RDW_data);
}

/************************************************************
** 函数名称 ：void Write_FDW(uint32_t f_delta)
** 函数功能 ：写FDW下降增量寄存器
** 入口参数 ：f_delta:下降增量,0-4294967295
** 出口参数 ：无
** 函数说明 ：无
**************************************************************/
void Write_FDW(uint32_t f_delta)
{
		uint8_t FDW_data[4] ={0x00,0x00,0x00,0x00};	//中间变量          
 
	  FDW_data[3]=(uint8_t)f_delta;
	  FDW_data[2]=(uint8_t)(f_delta>>8);
	  FDW_data[1]=(uint8_t)(f_delta>>16);
	  FDW_data[0]=(uint8_t)(f_delta>>24);		
		AD9959_WriteData(FDW_ADD,4,FDW_data);
}

/************************************************************
** 函数名称 ：void Write_Profile_Fre(uint8_t profile,uint32_t data)
** 函数功能 ：写Profile寄存器
** 入口参数 ：profile:	profile号(0~14)
							data:	写入频率，范围0~200 000 000 Hz
** 出口参数 ：无
** 函数说明 ：无
**************************************************************/
void Write_Profile_Fre(uint8_t profile,uint32_t data)
{
		uint8_t profileAddr;
		uint8_t Profile_data[4] ={0x00,0x00,0x00,0x00};	//中间变量
	  uint32_t Temp;            
	  Temp=(uint32_t)data*ACC_FRE_FACTOR;	   //将输入频率因子分为四个字节  4.294967296=(2^32)/500000000
	  Profile_data[3]=(uint8_t)Temp;
	  Profile_data[2]=(uint8_t)(Temp>>8);
	  Profile_data[1]=(uint8_t)(Temp>>16);
	  Profile_data[0]=(uint8_t)(Temp>>24);
		profileAddr = PROFILE_ADDR_BASE + profile;
		
		AD9959_WriteData(profileAddr,4,Profile_data);
}
/************************************************************
** 函数名称 ：void Write_Profile_Ampli(uint8_t profile,uint16_t data)
** 函数功能 ：写Profile寄存器
** 入口参数 ：profile:	profile号(0~14)
							data:	 写入幅度,范围0~1023，
** 出口参数 ：无
** 函数说明 ：无
**************************************************************/
void Write_Profile_Ampli(uint8_t profile,uint16_t data)
{
		uint8_t profileAddr;
		uint8_t Profile_data[4] ={0x00,0x00,0x00,0x00};	//中间变量

		//为幅度调制时幅度数据为[31:22]位
	  Profile_data[1]=(uint8_t)(data<<6);//[23:22]
	  Profile_data[0]=(uint8_t)(data>>2);//[31:24]
		
		profileAddr = PROFILE_ADDR_BASE + profile;
		
		AD9959_WriteData(profileAddr,4,Profile_data);//写入32位数据
}
/************************************************************
** 函数名称 ：void Write_Profile_Phase(uint8_t profile,uint16_t data)
** 函数功能 ：写Profile寄存器
** 入口参数 ：profile:	profile号(0~14)
							data:	 写入相位,范围：0~16383
** 出口参数 ：无
** 函数说明 ：无
**************************************************************/
void Write_Profile_Phase(uint8_t profile,uint16_t data)
{
		uint8_t profileAddr;
		uint8_t Profile_data[4] ={0x00,0x00,0x00,0x00};	//中间变量

		//为相位调制时相位数据为[31:18]位
	  Profile_data[1]=(uint8_t)(data<<2);//[23:18]
	  Profile_data[0]=(uint8_t)(data>>6);//[31:24]
		
		profileAddr = PROFILE_ADDR_BASE + profile;
		
		AD9959_WriteData(profileAddr,4,Profile_data);//写入32位数据
}



/************************************************************
** 函数名称 ：void AD9959_Set_Fre(uint8_t Channel,uint32_t Freq)
** 函数功能 ：设置通道的输出频率
** 入口参数 ：Channel:  输出通道  CH0-CH3
						 Freq:     输出频率，范围0~200 000 000 Hz
** 出口参数 ：无
** 函数说明 ：无
**************************************************************/
void AD9959_Set_Fre(uint8_t Channel,uint32_t Freq)
{	 
		uint8_t CHANNEL[1] = {0x00};
		
		CHANNEL[0]=Channel;
		AD9959_WriteData(CSR_ADD,1,CHANNEL);//控制寄存器写入CHn通道，选择CHn
    Write_CFTW0(Freq);//输出CHn设定频率																																			 
} 

/************************************************************
** 函数名称 ：void AD9959_Set_Amp(uint8_t Channel, uint16_t Ampli)
** 函数功能 ：设置通道的输出幅度
** 入口参数 ：Channel:  输出通道 CH0-CH3
							Ampli:    输出幅度,范围0~1023，控制值0~1023对应输出幅度0~500mVpp左右
** 出口参数 ：无
** 函数说明 ：无
**************************************************************/
void AD9959_Set_Amp(uint8_t Channel, uint16_t Ampli)
{ 
	uint8_t CHANNEL[1] = {0x00};

	CHANNEL[0]=Channel;
	AD9959_WriteData(CSR_ADD,1,CHANNEL); //控制寄存器写入CHn通道，选择CHn
	Write_ACR(Ampli);							//	CHn设定幅度
}

/************************************************************
** 函数名称 ：void AD9959_Set_Phase(uint8_t Channel,uint16_t Phase)
** 函数功能 ：设置通道的输出相位
** 入口参数 ：Channel:  输出通道 CH0-CH3
							Phase:		输出相位,范围：0~16383(对应角度：0°~360°)
** 出口参数 ：无
** 函数说明 ：无
**************************************************************/
void AD9959_Set_Phase(uint8_t Channel,uint16_t Phase)
{
	uint8_t CHANNEL[1] = {0x00};
	CHANNEL[0]=Channel;

	AD9959_WriteData(CSR_ADD,1,CHANNEL); //控制寄存器写入CHn通道，选择CHn
	Write_CPOW0(Phase);//CHn设定相位
}

/************************************************************
** 函数名称 ：void AD9959_Modulation_Init(uint8_t Channel,uint8_t Modulation,uint8_t Sweep_en,uint8_t Nlevel)
** 函数功能 ：设置各个通道的调制模式。
** 入口参数 ： Channel:  	输出通道 CH0-CH3
							Modulation:	调制模式DISABLE_Mod，ASK，FSK，PSK
							Sweep_en:		线性扫描模式 SWEEP_ENABLE启用、SWEEP_DISABLE不启用；启用时Nlevel只能是LEVEL_MOD_2
							Nlevel：		调制电平选择 LEVEL_MOD_2、4、8、16
** 出口参数 ：无
** 函数说明 ：如将调制电平设置为2电平调制时，对应的P0-P3引脚上的高低电平分别控制CH0-CH3通道(如果对应通道开启的话)
							如将调制电平设置为4电平调制时，对应的P0，P1和P2,P3引脚上的高低电平分别控制CH0-CH1通道(如果对应通道开启的话)
							由于AD9959只有P0-P3,4个用于调制控制的引脚，因此输出在4电平调制时，只能有2个通道同时设置为调制输出；
							8电平和16电平调制时，只能有1个通道同时设置为调制输出。请适当设置几电平调制以满足应用需求。

**注意！！！：设置成4电平调制时，输出通道只能选择CH0-1
							设置成8,16电平调制时，输出通道只能选择CH0
							本函数未做任意通道兼容，具体方法请参考AD9959芯片手册22-23页，操作FR1[14:12]为对应值。
**************************************************************/
void AD9959_Modulation_Init(uint8_t Channel,uint8_t Modulation,uint8_t Sweep_en,uint8_t Nlevel)
{
	uint8_t i=0;
	uint8_t CHANNEL[1]={0x00};
	uint8_t FR1_data[3];
	uint8_t FR2_data[2];
	uint8_t CFR_data[3];
	for(i=0;i<3;i++)//设置默认值
	{
		FR1_data[i]=FR1_DATA[i];
		CFR_data[i]=CFR_DATA[i];
	}
	FR2_data[0]=FR2_DATA[0];
	FR2_data[1]=FR2_DATA[1];
		
	CHANNEL[0]=Channel;
	AD9959_WriteData(CSR_ADD,1,CHANNEL); //控制寄存器写入CHn通道，选择CHn；以下设置均针对CHn
	
	FR1_data[1]=Nlevel;
	CFR_data[0]=Modulation;
	CFR_data[1]|=Sweep_en;
	CFR_data[2]=0x00;

	if(Channel!=0)//有通道被使能
	{
		AD9959_WriteData(FR1_ADD,3,FR1_data);//写功能寄存器1
		AD9959_WriteData(FR2_ADD,2,FR2_data);//写功能寄存器1
		AD9959_WriteData(CFR_ADD,3,CFR_data);//写通道功能寄存器
	}
}


/************************************************************
** 函数名称 ：void AD9959_SetFSK(uint8_t Channel, uint32_t *data,uint16_t Phase)
** 函数功能 ：设置FSK调制的参数
** 入口参数 ：Channel:  输出通道 CH0-CH3
							*data:	调整频率数据的起始地址
							Phase:	输出相位,范围：0~16383(对应角度：0°~360°)
** 出口参数 ：无
** 函数说明 ：FSK时信号幅度默认为最大
**************************************************************/
void AD9959_SetFSK(uint8_t Channel, uint32_t *data,uint16_t Phase)
{
	uint8_t i=0;
	uint8_t CHANNEL[1]={0x00};	
	
	CHANNEL[0]=Channel;
	AD9959_WriteData(CSR_ADD,1,CHANNEL); //控制寄存器写入CHn通道，选择CHn；以下设置均针对CHn
	Write_CPOW0(Phase);//设置相位
	
	Write_CFTW0(data[0]);
	for(i=0;i<15;i++)
		Write_Profile_Fre(i, data[i+1]);	
}

/************************************************************
** 函数名称 ：void AD9959_SetASK(uint8_t Channel, uint32_t *data,uint32_t fre,uint16_t Phase)
** 函数功能 ：设置ASK调制的参数
** 入口参数 ：Channel:  输出通道 CH0-CH3
							*data: 调整幅度数据的起始地址
							Freq:		输出频率，范围0~200 000 000 Hz
							Phase:	输出相位,范围：0~16383(对应角度：0°~360°)
** 出口参数 ：无
** 函数说明 ：无
**************************************************************/
void AD9959_SetASK(uint8_t Channel, uint16_t *data,uint32_t fre,uint16_t Phase)
{
	uint8_t i=0;
	uint8_t CHANNEL[1]={0x00};	
	
	CHANNEL[0]=Channel;
	AD9959_WriteData(CSR_ADD,1,CHANNEL); //控制寄存器写入CHn通道，选择CHn；以下设置均针对CHn
	Write_CFTW0(fre);//设置频率
	Write_CPOW0(Phase);//设置相位
	
	Write_ACR(data[0]);
	for(i=0;i<15;i++)
		Write_Profile_Ampli(i, data[i+1]);	
}

/************************************************************
** 函数名称 ：void AD9959_SetPSK(uint8_t Channel, uint16_t *data,uint32_t fre,uint16_t Phase)
** 函数功能 ：设置PSK调制的参数
** 入口参数 ：Channel:  输出通道 CH0-CH3
							*data:	调整相位数据的起始地址
							Freq:		输出频率，范围0~200 000 000 Hz
** 出口参数 ：无
** 函数说明 ：无
**************************************************************/
void AD9959_SetPSK(uint8_t Channel, uint16_t *data,uint32_t Freq)
{
	uint8_t i=0;
	uint8_t CHANNEL[1]={0x00};	
	
	CHANNEL[0]=Channel;
	AD9959_WriteData(CSR_ADD,1,CHANNEL); //控制寄存器写入CHn通道，选择CHn；以下设置均针对CHn
	Write_CFTW0(Freq);
	
	Write_CPOW0(data[0]);
	for(i=0;i<15;i++)
		Write_Profile_Phase(i, data[i+1]);	
}

/************************************************************
** 函数名称 ：void AD9959_SetFre_Sweep(uint8_t Channel, uint32_t s_data,uint32_t e_data,uint8_t fsrr,uint8_t rsrr,uint32_t r_delta,uint32_t f_delta,uint16_t Phase)
** 函数功能 ：设置线性扫频的参数
** 入口参数 ：Channel:  输出通道 CH0-CH3
							s_data:	起始频率，范围0~200 000 000 Hz
							e_data:	结束频率，范围0~200 000 000 Hz
							r_delta:上升步长频率,0~200 000 000Hz
							f_delta:下降步长频率,0~200 000 000Hz

							rsrr:		上升斜率,范围：1~255，系统时钟为500Mhz时一个控制字约为8ns
							fsrr:		下降斜率,范围：1~255
							Ampli:	输出幅度,范围0~1023，控制值0~1023对应输出幅度0~500mVpp左右
							Phase:	输出相位,范围：0~16383(对应角度：0°~360°)
** 出口参数 ：无
** 函数说明 ：频点与频点间停留时间 dT = Xsrr*8 单位ns，扫描点数=(起始-结束)/步长
							扫频总时间=总扫描频点数*dT
**************************************************************/
void AD9959_SetFre_Sweep(uint8_t Channel, uint32_t s_data,uint32_t e_data,uint32_t r_delta,uint32_t f_delta,uint8_t rsrr,uint8_t fsrr,uint16_t Ampli,uint16_t Phase)
{
	uint8_t CHANNEL[1]={0x00};
	uint32_t Fer_data=0;            
	
	CHANNEL[0]=Channel;
	AD9959_WriteData(CSR_ADD,1,CHANNEL); //控制寄存器写入CHn通道，选择CHn；以下设置均针对CHn
	
	Write_CPOW0(Phase);//设置相位
	Write_ACR(Ampli); //幅度设置
	
	Write_LSRR(rsrr,fsrr);//斜率
	
	Fer_data=(uint32_t)r_delta*ACC_FRE_FACTOR;	 //频率转换成控制字
	Write_RDW(Fer_data);//上升步长
	
	Fer_data=(uint32_t)f_delta*ACC_FRE_FACTOR;
	Write_FDW(Fer_data);//下降步长
	
	Write_CFTW0(s_data);//起始频率
	Write_Profile_Fre(0, e_data);//结束频率
}

/************************************************************
** 函数名称 ：void AD9959_SetAmp_Sweep(uint8_t Channel, uint32_t s_Ampli,uint16_t e_Ampli,uint32_t r_delta,uint32_t f_delta,uint8_t rsrr,uint8_t fsrr,uint32_t fre,uint16_t Phase)
** 函数功能 ：设置线性扫幅的参数
** 入口参数 ：Channel:  输出通道 CH0-CH3
							s_Ampli:	起始幅度，控制值0~1023对应输出幅度0~500mVpp左右
							e_Ampli:	结束幅度，
							
							r_delta:	上升步长幅度,0~1023
							f_delta:	下降步长幅度,0~1023

							rsrr:			上升斜率,范围：1~255，系统时钟为500Mhz时一个控制字约为8ns
							fsrr:			下降斜率,范围：1~255

							fre:			输出频率，范围0~200 000 000 Hz
							Phase:		输出相位,范围：0~16383(对应角度：0°~360°)
** 出口参数 ：无
** 函数说明 ：幅点与幅点间停留时间 dT = Xsrr*8 单位ns；扫描点数=(起始-结束)/步长
							扫幅总时间=总扫描幅点数*dT
**************************************************************/
void AD9959_SetAmp_Sweep(uint8_t Channel, uint32_t s_Ampli,uint16_t e_Ampli,uint32_t r_delta,uint32_t f_delta,uint8_t rsrr,uint8_t fsrr,uint32_t fre,uint16_t Phase)
{
	uint8_t CHANNEL[1]={0x00};
	uint8_t ACR_data[3] = {0x00,0x00,0x00};
		
	CHANNEL[0]=Channel;
	AD9959_WriteData(CSR_ADD,1,CHANNEL); //控制寄存器写入CHn通道，选择CHn；以下设置均针对CHn
	
	Write_CFTW0(fre); //幅度频率
	Write_CPOW0(Phase);//设置相位
	
	Write_LSRR(rsrr,fsrr);//斜率
	
	Write_RDW(r_delta<<22);//上升步长
	
	Write_FDW(f_delta<<22);//下降步长

	ACR_data[1] = (uint8_t)(s_Ampli>>8); //高位数据
	ACR_data[2] = (uint8_t)s_Ampli;  //低位数据
  AD9959_WriteData(ACR_ADD,3,ACR_data); //ACR 设定起始幅度
					
	Write_Profile_Ampli(0, e_Ampli);//结束幅度
}


/************************************************************
** 函数名称 ：void AD9959_SetPhase_Sweep(uint8_t Channel, uint16_t s_data,uint16_t e_data,uint16_t r_delta,uint16_t f_delta,uint8_t rsrr,uint8_t fsrr,uint32_t fre,uint16_t Ampli)
** 函数功能 ：设置线性扫相的参数
** 入口参数 ：Channel:  输出通道 CH0-CH3
							s_data:	起始相位，范围：0~16383(对应角度：0°~360°)
							e_data:	结束相位，
							r_delta:上升步长,范围：0~16383(对应角度：0°~360°)
							f_delta:下降步长,

							rsrr:		上升斜率,范围：1~255，系统时钟为500Mhz时一个控制字约为8ns
							fsrr:		下降斜率,范围：1~255
							fre:		输出频率，范围0~200 000 000 Hz
							Ampli:	输出幅度,范围0~1023，控制值0~1023对应输出幅度0~500mVpp左右
** 出口参数 ：无
** 函数说明 ：频点与频点间停留时间 dT = Xsrr*8 单位ns；扫描点数=(起始-结束)/步长
							扫频总时间=总扫描频点数*dT
**************************************************************/
void AD9959_SetPhase_Sweep(uint8_t Channel, uint16_t s_data,uint16_t e_data,uint16_t r_delta,uint16_t f_delta,uint8_t rsrr,uint8_t fsrr,uint32_t fre,uint16_t Ampli)
{
	uint8_t CHANNEL[1]={0x00};
	
	CHANNEL[0]=Channel;
	AD9959_WriteData(CSR_ADD,1,CHANNEL); //控制寄存器写入CHn通道，选择CHn；以下设置均针对CHn
	
	Write_CFTW0(fre); //幅度频率
	Write_ACR(Ampli); //幅度设置
	
	Write_LSRR(rsrr,fsrr);//斜率
	
	Write_RDW(r_delta<<18);//上升步长
	
	Write_FDW(f_delta<<18);//下降步长
	
	Write_CPOW0(s_data);//起始相位
	Write_Profile_Phase(0, e_data);//结束相位
}



///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////













/*--------------------------------------------
函数功能：控制器通过SPI向AD9959写数据
RegisterAddress: 寄存器地址
NumberofRegisters: 所含字节数
*RegisterData: 数据起始地址
temp: 是否更新IO寄存器
----------------------------------------------*/
void WriteData_AD9959(u8 RegisterAddress, u8 NumberofRegisters, u8 *RegisterData)
{
	uint8_t	ControlValue = 0;
	uint8_t	ValueToWrite = 0;
	uint8_t	RegisterIndex = 0;
	uint8_t	i = 0;

	ControlValue = RegisterAddress;
//写入地址
	SCLK = 0;
	CS = 0;	 
	for(i=0; i<8; i++)
	{
		SCLK = 0;
		if(0x80 == (ControlValue & 0x80))
			SDIO0= 1;	  
		else
			SDIO0= 0;	  
		SCLK = 1;
		ControlValue <<= 1;
	}
	SCLK = 0;
//写入数据
	for (RegisterIndex=0; RegisterIndex<NumberofRegisters; RegisterIndex++)
	{
		ValueToWrite = RegisterData[RegisterIndex];
		for (i=0; i<8; i++)
		{
			SCLK = 0;
			if(0x80 == (ValueToWrite & 0x80))
				SDIO0= 1;	  
			else
				SDIO0= 0;	  
			SCLK = 1;
			ValueToWrite <<= 1;
		}
		SCLK = 0;		
	}	
	CS = 1;
} 








/*---------------------------------------
函数功能：设置通道输出频率
Channel:  输出通道
Freq:     输出频率
---------------------------------------*/

void Write_frequence(u8 Channel,u32 Freq)
{
	uint8_t CFTW0_DATA[4] ={0x00,0x00,0x00,0x00};	//中间变量
	uint32_t Temp;            
	Temp=(uint32_t)Freq*8.589934592;	   //将输入频率因子分为四个字节  4.294967296=(2^32)/500000000
	CFTW0_DATA[3]=(uint8_t)Temp;
	CFTW0_DATA[2]=(uint8_t)(Temp>>8);
	CFTW0_DATA[1]=(uint8_t)(Temp>>16);
	CFTW0_DATA[0]=(uint8_t)(Temp>>24);
	switch(Channel)
	{
		case 0:WriteData_AD9959(CSR_ADD,1,CSR_DATA0);//控制寄存器写入CH0通道
			   WriteData_AD9959(CFTW0_ADD,4,CFTW0_DATA);//CTW0 address 0x04.输出CH0设定频率
			   break;
		case 1:WriteData_AD9959(CSR_ADD,1,CSR_DATA1);//控制寄存器写入CH1通道
			   WriteData_AD9959(CFTW0_ADD,4,CFTW0_DATA);//CTW0 address 0x04.输出CH1设定频率
			   break;
		case 2:WriteData_AD9959(CSR_ADD,1,CSR_DATA2);//控制寄存器写入CH2通道
			   WriteData_AD9959(CFTW0_ADD,4,CFTW0_DATA);//CTW0 address 0x04.输出CH2设定频率
			   break;
		case 3:WriteData_AD9959(CSR_ADD,1,CSR_DATA3);//控制寄存器写入CH3通道
			   WriteData_AD9959(CFTW0_ADD,4,CFTW0_DATA);//CTW0 address 0x04.输出CH3设定频率
			   break;
	}
} 
/*---------------------------------------
函数功能：设置通道输出幅度
Channel:  输出通道
Ampli:    输出幅度
---------------------------------------*/
void Write_Amplitude(u8 Channel, u16 Ampli)
{ 
	uint16_t A_temp;//=0x23ff;
	uint8_t ACR_DATA[3] = {0x00,0x00,0x00};//default Value = 0x--0000 Rest = 18.91/Iout 

	A_temp=Ampli|0x1000;
	ACR_DATA[2] = (uint8_t)A_temp;      //低位数据
	ACR_DATA[1] = (uint8_t)(A_temp>>8); //高位数据
	switch(Channel)
	{
		case 0:WriteData_AD9959(CSR_ADD,1,CSR_DATA0);
			   WriteData_AD9959(ACR_ADD,3,ACR_DATA);
			   break;
		case 1:WriteData_AD9959(CSR_ADD,1,CSR_DATA1);
			   WriteData_AD9959(ACR_ADD,3,ACR_DATA);
			   break;
		case 2:WriteData_AD9959(CSR_ADD,1,CSR_DATA2);
			   WriteData_AD9959(ACR_ADD,3,ACR_DATA);
			   break;
		case 3:WriteData_AD9959(CSR_ADD,1,CSR_DATA3);
			   WriteData_AD9959(ACR_ADD,3,ACR_DATA);
			   break;
	}
}

/*---------------------------------------
函数功能：设置通道输出相位
Channel:  输出通道
Phase:    输出相位,范围：0~16383(对应角度：0°~360°)
---------------------------------------*/
void Write_Phase(u8 Channel,u16 Phase)
{
	uint16_t P_temp=0;
	P_temp=(uint16_t)Phase;
	CPOW0_DATA[1]=(uint8_t)P_temp;
	CPOW0_DATA[0]=(uint8_t)(P_temp>>8);
	
	switch(Channel)
	{
		case 0:WriteData_AD9959(CSR_ADD,1,CSR_DATA0);
			   WriteData_AD9959(CPOW0_ADD,2,CPOW0_DATA);
			   break;
		case 1:WriteData_AD9959(CSR_ADD,1,CSR_DATA1);
			   WriteData_AD9959(CPOW0_ADD,2,CPOW0_DATA);
			   break;
		case 2:WriteData_AD9959(CSR_ADD,1,CSR_DATA2);
			   WriteData_AD9959(CPOW0_ADD,2,CPOW0_DATA);
			   break;
		case 3:WriteData_AD9959(CSR_ADD,1,CSR_DATA3);
			   WriteData_AD9959(CPOW0_ADD,2,CPOW0_DATA);
			   break;
	}
}
/**
*@brief 设置DDS信号输出通道和幅值
*@param Channel：信号输出通道（ref 0~3）Freq：输出信号的频率 
*       Amp:输出信号的幅度幅度控制字范围为0~1023对应于0到500毫伏
*     （在小幅度下不是线性关系，大概70mv以上为线性，且不同频段下相同的控制字输出幅度不一样）
*/
void AD9959_SetFreq( u8 Channel, u32 Freq, u16 Amp )
{
	Write_frequence(Channel,Freq);	
	Write_Amplitude(Channel, Amp);
	Write_Phase(Channel, 0);
    IO_Update();
}

/**
*@brief 设置DDS信号输出通道和幅值和相位
*@param Channel：信号输出通道（ref 0~3）Freq：输出信号的频率 
*       Amp:输出信号的幅度幅度控制字范围为0~1023对应于0到500毫伏
*     （在小幅度下不是线性关系，大概70mv以上为线性，且不同频段下相同的控制字输出幅度不一样）
*/
void AD9959_SetFreq_all( u8 Channel, u32 Freq, u16 Amp,uint16_t Phase )
{
	Write_frequence(Channel,Freq);	
	Write_Amplitude(Channel, Amp);
	Write_Phase(Channel, Phase);
  IO_Update();
}
/**************扫频变量**************/
//每十倍频程点数
uint16_t pointsPerDecade=20;//LOg扫频
ad9959 CH[4];

/**
 * 固定步进扫频函数 - 适配0~1023幅度控制范围
 * @param channel 通道号 (0-3)
 * @param stepFreq 固定步进值 (Hz)
 * @param dwellTime_ms 停留时间 (ms)
 */
  // 10位幅度补偿表（0~1023）- 需根据实际测量校准
      uint16_t amplitudeCompensation[41] = {
        800,804,804,804,808,809,810,811,814,817,// 1MHz-10MHz
				824,830,834,838,844,850,856,858,866,872,// 11MHz-20MHz
				882,888,895,903,912,916,924,932,940,944,// 21MHz-30MHz
				952,960,968,974,984,990,1000,1008,1015,1023,//31MHz-40MHz
				1023
    };
			
		uint16_t amplitudeCompensation_0_150_go[152] = {
    // 0~10MHz（索引0~9）：对应0MHz、1MHz...9MHz
    190,190,192,192,192,192,192,192,192,192,
    // 10~20MHz（索引10~19）：对应10MHz、11MHz...19MHz
    192,192,192,192,192,192,192,192,192,191,191,
    // 20~30MHz（索引20~29）：对应20MHz、21MHz...29MHz
    191,191,191,191,191,192,192,192,192,192,
		// 30~40MHz（索引30~39）：对应30MHz、31MHz...39MHz
		192,193,193,194,194,195,196,196,197,199,
    // 40~50MHz（索引40~49）：对应40MHz、41MHz...49MHz
    200,203,205,206,208,208,208,208,208,209,
		// 50~60MHz（索引50~59）：对应50MHz、51MHz...59MHz
    212,212,214,216,218,218,220,222,223,225,
    // 60~70MHz（索引60~69）：对应60MHz、61MHz...69MHz
    226,227,228,229,233,233,234,239,240,241,
    // 70~80MHz（索引70~79）：对应70MHz、71MHz...79MHz
    242,246,246,248,251,253,256,260,262,264,
    // 80~90MHz（索引80~89）：对应80MHz、81MHz...89MHz
    266,268,270,272,275,278,280,283,286,292,
    // 90~100MHz（索引90~99）：对应90MHz、91MHz...99MHz
    295,296,299,301,304,308,311,314,319,323,
    // 100~110MHz（索引100~109）：对应100MHz、101MHz...109MHz
    326,331,333,335,337,340,342,349,356,358,
    // 110~120MHz（索引110~119）：对应110MHz、111MHz...119MHz
    365,369,372,372,373,376,379,383,390,392,
    // 120~130MHz（索引120~129）：对应120MHz、121MHz...129MHz
    394,397,400,403,406,409,414,416,420,424,
    // 130~140MHz（索引130~139）：对应130MHz、131MHz...139MHz
    433,436,440,443,445,448,452,457,461,464,
    // 140~150MHz（索引140~150）：对应140MHz、141MHz...150MHz（包含150MHz）
    469,477,483,486,490,493,498,502,505,510,
    518
    
};//经放大的2444mv~254mv
/**
 * 对数步进扫频函数 - 适配0~1023幅度控制范围
 * @param channel 通道号 (0-3)
 * @param pointsPerDecade 每十倍频程点数
 * @param dwellTime_ms 停留时间 (ms)
 */
// 10位幅度补偿表（对数分布）
     uint16_t amplitudeCompensation1[41] = {
        1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, // 1Hz-10Hz
        1023, 1023, 1023, 1023, 1023, 1022, 1020, 1017, 1013, 1007, // 10Hz-100Hz
         998,  985,  965,  935,  895,  840,  770,  690,  600,  505, // 100Hz-1MHz
         415,  340,  280,  235,  200,  180,  165,  155,  145,  135, // 1MHz-200MHz
         125
    };

void AD9959_FixedStepSweep_Init( uint8_t channel,uint32_t startFreq,uint32_t stopFreq,uint32_t step,uint32_t dwellTime_ms,uint8_t count)//固定扫频，count循环次数
{
	SoftTimer_StartAutoTimer(TMR_AD9959_FixedStepSweep_ID,dwellTime_ms);
	CH[channel].Scan_state=1;
	CH[channel].Scan_step=step;
	CH[channel].Scan_startFreq=startFreq;
	CH[channel].Scan_stopFreq=stopFreq;
	CH[channel].Scan_count=count;
	
	AD9959_SetFreq(channel,(uint32_t)CH[channel].Scan_startFreq,amplitudeCompensation[CH[channel].Scan_COMP_index]);
}
//void AD9959_LogStepSweep_Init( uint8_t channel,uint32_t startFreq,uint32_t stopFreq,uint32_t dwellTime_ms,uint8_t count)//对数扫频，count循环次数
//{

//	SoftTimer_StartAutoTimer(TMR_AD9959_LogStepSweep_ID,dwellTime_ms);
//	CH[channel].Scan_state=2;
//	CH[channel].Scan_startFreq=startFreq;
//	CH[channel].Freq=CH[channel].Scan_startFreq;
//	CH[channel].Scan_stopFreq=stopFreq;
//	CH[channel].Scan_count=count;
//	AD9959_SetFreq(channel,(uint32_t)CH[channel].Scan_startFreq,amplitudeCompensation1[CH[channel].Scan_COMP_index]);

//}
//void AD9959_LogStepSweep_Stop(uint8_t channel)//扫频间隔时间设计
//{
//	if(!(CH[0].Scan_state||CH[1].Scan_state||CH[2].Scan_state||CH[3].Scan_state))
//	SoftTimer_StopTimer(TMR_AD9959_LogStepSweep_ID);
//	CH[channel].Scan_state=0;
//	CH[channel].Scan_startFreq=0;
//	CH[channel].Freq=0;
//	CH[channel].Scan_stopFreq=0;
//	CH[channel].Scan_count=0;
//	CH[channel].Scan_COMP_index=0;
//	
//	AD9959_SetFreq(channel,(uint32_t)CH[channel].Scan_startFreq,amplitudeCompensation1[CH[channel].Scan_COMP_index]);
//}
void AD9959_FixedStepSweep_Stop(uint8_t channel)//扫频间隔时间设计
{
	if(!(CH[0].Scan_state||CH[1].Scan_state||CH[2].Scan_state||CH[3].Scan_state))
	SoftTimer_StopTimer(TMR_AD9959_FixedStepSweep_ID);
	CH[channel].Scan_state=0;
	CH[channel].Scan_step=0;
	CH[channel].Scan_startFreq=0;
	CH[channel].Freq=0;
	CH[channel].Scan_stopFreq=0;
	CH[channel].Scan_count=0;
	CH[channel].Scan_COMP_index=0;
	
	AD9959_SetFreq(channel,(uint32_t)CH[channel].Scan_startFreq,amplitudeCompensation1[CH[channel].Scan_COMP_index]);
}


uint8_t AD9959_FixedStepSweep_10bit(void) /*软定时器配合扫频*///返回值re_value，反0无操作，1字节：xx xx xx xx，3~0通道，xx:结束+步进，1：有，0：无
{
   uint8_t ch=0,re_value=0;
//	if( !SoftTimer_CheckTimer(TMR_AD9959_FixedStepSweep_ID)) return 0;
	// 扫频循环
	for(ch=0;ch<4;ch++)
	{
		if(!(CH[ch].Scan_state==1)) 	continue ;
			
			if(CH[ch].Freq <= CH[ch].Scan_stopFreq)
			{
				CH[ch].Freq += CH[ch].Scan_step;
				// 计算补偿索引
				CH[ch].Scan_COMP_index = (CH[ch].Freq / 1000000); // 每10MHz一个补偿区间
				if (CH[ch].Scan_COMP_index >= 151) CH[ch].Scan_COMP_index = 151;
				// 设置频率和补偿后的幅度
				AD9959_SetFreq(ch,(uint32_t)CH[ch].Freq,amplitudeCompensation_0_150_go[CH[ch].Scan_COMP_index]);
				re_value=(re_value|(0x01<<(ch*2)));
				DEBUGOUT("\r\nCH[%d].Freq=%d",ch,CH[ch].Freq  );
			}
			else
			{
				
				if(--CH[ch].Scan_count==0)
				{
					AD9959_FixedStepSweep_Stop(ch);
					re_value=(re_value|(0x02<<(ch*2)));
				}
				else
				{
					CH[ch].Freq=CH[ch].Scan_startFreq;
					CH[ch].Scan_temp=0;
					CH[ch].Scan_COMP_index=0;
				}
			}
	}
	return re_value;
}

//uint8_t  AD9959_LogStepSweep_10bit(void) //返回值re_value，反0无操作，1字节：xx xx xx xx，3~0通道，xx:结束+步进，1：有，0：无
//{

// double decades,logFreq,logMax;
// uint16_t totalPoints;
// uint8_t ch=0,re_value=0;
//	
//	if( !SoftTimer_CheckTimer(TMR_AD9959_LogStepSweep_ID) )	return 0;
//	
//	for(ch=0;ch<4;ch++)
//	{
//		if(!(CH[ch].Scan_state==2)) 	continue ;
//    // 计算总频程和点数
//    decades = log10((double)(CH[ch].Scan_stopFreq /CH[ch].Scan_startFreq));
//    totalPoints = (uint16_t)(decades * pointsPerDecade);
//    
//    // 对数扫频循环
//		if( CH[ch].Scan_temp <= totalPoints)
//		{
//			CH[ch].Scan_temp++;
//			CH[ch].Freq = CH[ch].Scan_startFreq * pow(10, (double)CH[ch].Scan_count / pointsPerDecade);
//			
//			// 计算对数补偿索引
//			logFreq = log10((double)CH[ch].Freq);
//			logMax = log10((double) CH[ch].Scan_stopFreq);
//			CH[ch].Scan_COMP_index = (uint8_t)((logFreq / logMax) * 40);
//			if (CH[ch].Scan_COMP_index > 40) CH[ch].Scan_COMP_index = 40;

//				// 设置频率和幅度
//			AD9959_SetFreq(ch,(uint32_t)CH[ch].Freq,amplitudeCompensation1[CH[ch].Scan_COMP_index]);
//			re_value=(re_value|(0x01<<(ch*2)));
//		}
//		else
//		{
//			if(--CH[ch].Scan_count==0)
//			{
//				AD9959_LogStepSweep_Stop(ch);
//				re_value=(re_value|(0x02<<(ch*2)));
//			}
//			else
//			{
//				CH[ch].Freq=CH[ch].Scan_startFreq;
//				CH[ch].Scan_temp=0;
//				CH[ch].Scan_COMP_index=0;
//			}
//		}
//	}
//	return re_value;
//}

void AD9959_Scan_Task(void)
{
	
}

