1. 微控制器特性：
CPU:			STM32F407ZGT6
packagename:	LQFP144
GPIO:			114
clock:			168MHz Max
Flash:			1024k
				地址 0x08000000~0x08100000
RAM:			192k
				128k通用RAM，地址 0x20000000~0x2001FFFF
				64k CCRAM,地址 0x10000000~0x1000FFFF
Timer:			2 32bit,12 16bit,2 motor control timer
				16位高级定时器：TIM1,TIM8
				32位通用定时器：TIM2,TIM5
				16位通用定时器：TIM3,TIM4,TIM9-14
				16位基本定时器：TIM6,TIM7				
				Timer2,3,4,5,6,7,12,13,14挂载在APB1总线上，一般时钟频率 HCLK/2
				Timer1,8,9,10,11挂载在APB2总线上，一般时钟频率 HCLK
				
ADC:			3 16-bit ADC 16 channels
DAC:			2 12-bit DAC
SPI:			4
IIC:			3
USART:			4+4

开发板扩展存储：
外扩SDRAM：		IS62WV51216，512K*16位
外扩SPI FLASH：	W25Q128，16M 字节
外扩EEPROM：	24C02，256 字节


2. 接口：
	PCComm：		USART1,用于调试（PA9_Tx,PA10_Rx）,485通信，Modbus协议，串口485通信部分需要修改，已经没有方向控制信号
					USART3,
					USART6,
	
3. 定时器：
	Timer7：		用于系统时间基准和软定时器，使用CubeMX配置，定时时间1ms
	

	
	
STM32CubeMX的坑：
1. 项目管理中配置“Set all free pin as analog”会配置PA13,14,15等引脚，造成无法用ST-Link下载
2. 更改“Generate peripheral initialization as a pair of '.c/.h' files per peripheral”配置，初始化配置中的用户代码会丢失。
3. CubeMX配置定时器后并不会启动定时器，需要自行撰写启动代码,
	-- HAL_TIM_Base_Start_IT(&htim7);
	-- HAL_TIM_Base_Start(&TimHandle);
	-- HAL_TIM_PWM_Start( &htim2, TIM_CHANNEL_1 );
4. 默认全局变量 SystemCoreClock=160000000， 对SysTick的配置在时钟配置前，如果时钟配置后 SystemCoreClock 有修改，则SysTick定时会不准
5. 无论在 CubeMX 中如何配置中断优先级分组，生成的代码都会设为 NVIC_PRIORITYGROUP_4
6. SPI接口使用硬件NSS会通信失败,用软件NSS可以
7. FSMC,FMC+HAL库驱动TFT-LCD时需要在项目配置-->C/C++页将"Optimization"配置为Level0,默认是level3,这是代码优化级别
8. 使用 cfft 函数时需要在项目配置-->C/C++页勾选"C99 Mode",否则基2运算会很慢。