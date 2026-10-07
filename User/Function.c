#include "Function.h"
#include "imge.h"
#include "math.h"

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim6;
/**************************简易以太网双绞线测试仪****************************/
/*界面*/
#define Task_count 2//界面数
uint8_t  Task_index=0;
static uint32_t  time_count=0;
uint8_t  function_Task_index=0;
uint8_t success=1;
uint8_t time_count_index=0;
/*********************/
#define Fault_Tolerant 5  //状态检测性功能容错空间

/**双端检测**/
#define R_precision  1
float  current_resistance=0;
float attenuation_dB=0;

/**单端检测**/


//收有用
// A系列引脚映射表 (A1-A9)
GpioPin aPins[9] = {
    {OUT_A1_GPIO_Port, OUT_A1_Pin},  // 第0位 - A1
    {OUT_A2_GPIO_Port, OUT_A2_Pin},  // 第1位 - A2
    {OUT_A3_GPIO_Port, OUT_A3_Pin},  // 第2位 - A3
    {OUT_A4_GPIO_Port, OUT_A4_Pin},  // 第3位 - A4
    {OUT_A5_GPIO_Port, OUT_A5_Pin},  // 第4位 - A5
    {OUT_A6_GPIO_Port, OUT_A6_Pin},  // 第5位 - A6
    {OUT_A7_GPIO_Port, OUT_A7_Pin},  // 第6位 - A7
    {OUT_A8_GPIO_Port, OUT_A8_Pin},  // 第7位 - A8
    {OUT_A9_GPIO_Port, OUT_A9_Pin}   // 第8位 - A9
};
//OUT_B9_Pin  OUT_B9_GPIO_Port
//OUT_B1_Pin  OUT_B1_GPIO_Port

/*
*********************************************************************************************************
*	函 数 名: IWDG_Feed
*	功能说明: 喂狗函数
*	参    数：无
*	返 回 值: 无
*********************************************************************************************************
*/
void IWDG_Feed(void)
{
	//HAL_IWDG_Refresh( &hiwdg );
}


/*
	判断cpu的复位方式
*/
void JudgCPUResetMode(void)
{	
	ResetType ucRestFlag;	//复位方式
	
  ucRestFlag = GetResetFlag();

	switch( ucRestFlag )
	{
		case IWDGRST:	DEBUGOUT("\r\n----复位方式:看门狗复位----");	break;
		case SFTRST:	DEBUGOUT("\r\n----复位方式:软件复位------");	break;
		case PORRST:	DEBUGOUT("\r\n----复位方式:上电复位------");	break;
		case PINRST:	DEBUGOUT("\r\n----复位方式:按键复位------");	break;
		default:break;
	}	
}


void PrintSystemMess( void )
{
	uint8_t i,MCUID[12];
	
	
	//启动方式
	JudgCPUResetMode();
	
	GetMCUID( MCUID );
	
	//系统时钟频率
	DEBUGOUT("\r\nMCUID = ");
	for(i=0;i<12;i++) DEBUGOUT(" 0x%02x,", MCUID[i]);
	DEBUGOUT("\r\nSystemClock = %d", HAL_RCC_GetSysClockFreq() );
	DEBUGOUT("\r\nHCLKClock = %d", HAL_RCC_GetHCLKFreq() );
	DEBUGOUT("\r\nPCLK1Clock = %d", HAL_RCC_GetPCLK1Freq() );
	DEBUGOUT("\r\nPCLK2Clock = %d", HAL_RCC_GetPCLK2Freq() );
	DEBUGOUT("\r\nSPI Flash ID = 0x%04x", W25QXX_TYPE );
	
	DEBUGOUT("\r\nExt_SRAM used: %.1f%%", MEM_Preused( &Ext_SRAM)/10.0f );
	DEBUGOUT("\r\nIn_CCMRAM used: %.1f%%", MEM_Preused( &In_CCMRAM)/10.0f );
	
	DEBUGOUT("\r\nlcdid = 0x%04x", lcddev.id );
	DEBUGOUT("\r\nlcd_width = %d, lcd_height = %d", lcddev.width, lcddev.height);
}

/*
系统初始化, 还需加入开机参数操作
*/
void UserSystemInit( void )
{

	MEM_Init( &Ext_SRAM );
	MEM_Init( &In_CCMRAM );
	vInitMsgQueue();
	SoftTimer_Init();
	bsp_InitUart();	
	W25QXX_Init();
	TouchLCD_Init( 1 );		// 0竖屏  1横屏	
	TouchKeypad_Init();
	AD9959_Init();
//	AD7606_Init();
	
	SoftTimer_StartAutoTimer( TMR_KEYSCAN_ID, 10 );
	SoftTimer_StartAutoTimer( TMR_TOUCH_ID, 10 );//10
	SoftTimer_StartAutoTimer( TMR_LED_ID, 50 );
	
	SoftTimer_StartAutoTimer(TMR_Time_count_ID,1);
//	Led_SysRunning();
//	Led_SysRunning2();
	
	PrintSystemMess();
	LCD_Show_Str(0,3,320,240,(uint8_t *)"2025全国大学生电子设计竞赛",24,0,BLUE);
	LCD_Show_Str(28,27,320,240,(uint8_t *)"简易以太网双绞线测试仪",24,0,BLUE);
	LCD_DrawRectangle(0,3,320,51,BLUE);
	
//	//校准
//	LCD_Show_Str(96,104,320,240,(uint8_t *)"正在校准     ",32,0,BLUE);
//	HAL_Delay (4000);
//	LCD_Show_Str(96,104,320,240,(uint8_t *)"1m校准成功！",32,0,BLUE);
//	HAL_Delay (2000);
//	LCD_Show_Str(96,104,320,240,(uint8_t *)"正在校准      ",32,0,BLUE);
//	HAL_Delay (4000);
//	LCD_Show_Str(96,104,320,240,(uint8_t *)"50m校准成功！",32,0,BLUE);
//	HAL_Delay (2000);
//	
	
	manager_interface();	
//	AD9959_SetFreq(0,0,0);
//                   
//	__HAL_TIM_SET_AUTORELOAD(&htim6,72000000/T2_point / 200  - 1);//DA输出频率设置
	__HAL_TIM_SET_AUTORELOAD(&htim3,18000000/50000-1);//AD采样率设置，双通道,T3上/下降沿触发故不用/2
//	LEDFunSet(LED1,20,10,100,0 );
}

void Task_Msg( void )
{
	stMsgUnit lmsg;
	

	if( ucGetMsg(&lmsg) )
	{
		//DEBUGOUT("\r\nMsg = %d, Para = 0x%x;" , lmsg.ucMsgCode, lmsg.ulParam );
		
		
		if( lmsg.ucMsgCode==MSG_KEY_LONGRELEASE && lmsg.ulParam==KEY1 )
		{
			
		}
		else if( lmsg.ucMsgCode==MSG_KEY_PRESS && lmsg.ulParam==KEY1 )
		{
			Task_index=1;
			safely_switch();
		}
		else if( lmsg.ucMsgCode==MSG_KEY_PRESS && lmsg.ulParam==KEY2 )
		{
			Task_index=0;
			safely_switch();
		}
		else if( lmsg.ucMsgCode==MSG_KEY_SHORTRELEASE )
		{
		}
		else if( lmsg.ucMsgCode==MSG_KEY_LONGHOLD )
		{
			
		}
		
		
		 if( lmsg.ucMsgCode==MSG_ADC_GET )
		{
			Stop_ADC_safely();
			function_adc_task();
//			detect_valley_with_latest();
			
			DEBUGOUT("\r\n采样值1：%.3fv", (float)ADC_average_value(0)* (3.3 / 4096) );
			DEBUGOUT("\r\n采样值2：%.3fv", (float)ADC_average_value(1)* (3.3 / 4096) );
			DEBUGOUT("\r\n采样值3：%.3fv", (float)ADC_average_value(2)* (3.3 / 4096) );
			DEBUGOUT("\r\n采样值4：%.3fv", (float)ADC_average_value(3)* (3.3 / 4096) );
//			Start_ADC_safely();
		}
		
		 else if( lmsg.ucMsgCode==MSG_ADC_AD7606_GET )
		{
//			printfADC();
//			AD7606_Start();
		}
		
		 if( lmsg.ucMsgCode==MSG_TOUCH_PRESS )
		{
			
//			Touch_Get(&lmsg);
			DEBUGOUT("\r\nTouchPress [0x%02x]", lmsg.ulParam );
		}
		else if( lmsg.ucMsgCode==MSG_TOUCH_LONGHOLD )
		{
			
			//DEBUGOUT("\r\nTouchHold [%d,%d]", lmsg.ulParam>>16, lmsg.ulParam&0xFFFF );
			DEBUGOUT("\r\nTouchHold [0x%02x]", lmsg.ulParam );
		}
		else if( lmsg.ucMsgCode==MSG_TOUCH_RELEASE )
		{
			//DEBUGOUT("\r\nTouchRelease [%d,%d]", lmsg.ulParam>>16, lmsg.ulParam&0xFFFF );
			DEBUGOUT("\r\nTouchRelease [0x%02x]", lmsg.ulParam );
		}
	}
}



uint8_t		ucRxBuffer[50] = {0};
uint16_t	RxLength,i;

/******************************/
void Task_UART1( void )
{
	static uint16_t cnt=0;
	uint32_t	 temp;
	if( comIsRxTimeout( COM1 ) )
	{		
		RxLength = comGetOneFrame( COM1, ucRxBuffer );
		DEBUGOUT("\r\nRecieved frame%04d length = %d", cnt, RxLength);
		cnt++;
		LEDFunSet(LED2,2,1,3,0 );
		/*指令查询*/

		/*ADC*/
		if( sscanf((char*)ucRxBuffer,"ADC采样率%d" ,&temp )==1 )
		__HAL_TIM_SET_AUTORELOAD(&htim3,18000000/temp-1);
		
		if(strncmp((char*)ucRxBuffer,"测类型",strlen("测类型"))==0)
			function_Task1(0);
		 /*清缓存*/
		for(i=0;i<RxLength;i++) ucRxBuffer[i]='\0';
		comClearRxFifo(COM1);
	}
}

void interface_2(void)
{
	LCD_Fill(0,52,320,240,WHITE );
	LCD_Show_Str_all(0,208,320,240,(uint8_t *)"进程时间：",16,0,BLUE,GREEN);
	LCD_Show_Str_all(0,224,320,240,(uint8_t *)"进程时间：",16,0,BLUE,LBBLUE);
	LCD_Show_Str_all(288,54,320,240,(uint8_t *)"单",32,0,BLUE,GRAY);
	LCD_Show_Str_all(288,86,320,240,(uint8_t *)"端",32,0,BLUE,GRAY);
	LCD_Show_Str_all(288,118,320,240,(uint8_t *)"检",32,0,BLUE,GRAY);
	LCD_Show_Str_all(288,150,320,240,(uint8_t *)"测",32,0,BLUE,GRAY);
	
	LCD_Show_Str(0,88,320,240,(uint8_t *)"线缆长度：",32,0,BLUE);
	LCD_Show_Str(0,136,320,240,(uint8_t *)"线对间短路：",32,0,BLUE);
	
}
void interface_1(void)
{
	LCD_Fill(0,52,320,240,WHITE );
	LCD_Show_Str_all(0,208,320,240,(uint8_t *)"进程时间：",16,0,BLUE,GREEN);
	LCD_Show_Str_all(0,224,320,240,(uint8_t *)"进程时间：",16,0,BLUE,LBBLUE);
	LCD_Show_Str_all(288,54,320,240,(uint8_t *)"双",32,0,BLUE,GRAY);
	LCD_Show_Str_all(288,86,320,240,(uint8_t *)"端",32,0,BLUE,GRAY);
	LCD_Show_Str_all(288,118,320,240,(uint8_t *)"检",32,0,BLUE,GRAY);
	LCD_Show_Str_all(288,150,320,240,(uint8_t *)"测",32,0,BLUE,GRAY);
	
	LCD_Show_Str(0,72,320,240,(uint8_t *)"类型：",24,0,BLUE);
	LCD_Show_Str(0,96,320,240,(uint8_t *)"线对连接关系：",24,0,BLUE);
	LCD_Show_Str(0,120,320,240,(uint8_t *)"直流电阻值：",24,0,BLUE);
	LCD_Show_Str(0,144,320,240,(uint8_t *)"交流衰减dB：",24,0,BLUE);
	
}


void manager_interface(void)
{
	switch(Task_index)
	{
		case 0:interface_1();break;
		case 1:interface_2();break;
		default:break;
	}

}

void Touch_Get(stMsgUnit* _pxMsg)
{
	
	if(Task_index==0)/*主界面*/
	{
		
		if((uint16_t)(_pxMsg->ulParam>>16)>=64&&(uint16_t)(_pxMsg->ulParam>>16)<=135&&(uint16_t)_pxMsg->ulParam>=113&&(uint16_t)_pxMsg->ulParam<=147)
		{
//			Task_index=1;
//			manager_interface();
		}
	}
	
}
/*********************引脚快速设置**********************/
/**
 * @brief 将引脚数组中指定的引脚配置为类似断路状态（高阻态）
 * @param pins 引脚数组指针 (aPins 或 bPins)
 * @param hexValue 十六进制控制值 (0x00-0x1FF)，位为1表示对应引脚模拟断路
 * @retval None
 */
void setPinsAsOpenCircuit(GpioPin* pins, uint16_t hexValue) {
    if (pins == NULL) return;  // 检查指针有效性
    
    // 遍历引脚，对hexValue中为1的位配置高阻态
    for (uint8_t i = 0; i < 9; i++) {
        if (hexValue & (1UL << i)) {
            GPIO_InitTypeDef GPIO_InitStruct = {
                .Pin = pins[i].pin,
                .Mode = GPIO_MODE_INPUT,    // 输入模式（高阻态核心配置）
                .Pull = GPIO_NOPULL,        // 禁用内部上下拉，避免影响外部电路
                .Speed = GPIO_SPEED_FREQ_LOW
            };
            HAL_GPIO_Init(pins[i].port, &GPIO_InitStruct);
        }
    }
}
    
/**
 * @brief 将单个引脚配置为高阻态（类似断路状态）
 * @param pin 要配置的引脚（包含端口和引脚编号信息）
 * @retval None
 */
void setPinAsOpenCircuit( GpioPin* pin) {
    if (pin == NULL || pin->port == NULL) return;  // 检查参数有效性
    
    // 配置引脚为输入模式且无上下拉（高阻态）
    GPIO_InitTypeDef GPIO_InitStruct = {
        .Pin = pin->pin,
        .Mode = GPIO_MODE_INPUT,    // 输入模式实现高阻态
        .Pull = GPIO_NOPULL,        // 禁用内部上下拉，避免影响外部电路
        .Speed = GPIO_SPEED_FREQ_LOW  // 输入模式下速度配置不影响
    };
    
    HAL_GPIO_Init(pin->port, &GPIO_InitStruct);
}
/**
 * @brief 将单个引脚配置为高阻态（使用HAL库风格）
 * @param gpio 指向GPIO端口的指针（如GPIOA, GPIOB等）
 * @param pin 引脚编号（GPIO_PIN_0 到 GPIO_PIN_15）
 * @retval None
 */
void HAL_SetPinAsOpenCircuit(GPIO_TypeDef* gpio, uint16_t pin) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // 检查参数有效性
    if (gpio == NULL || (pin & ~0x000F) != 0) {
        return; // 无效引脚或端口
    }
    
    // 配置高阻态：输入模式 + 无上下拉
    GPIO_InitStruct.Pin = pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;      // 输入模式实现高阻特性
    GPIO_InitStruct.Pull = GPIO_NOPULL;          // 禁用内部上下拉电阻
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; // 输入模式下速度不影响
    
    // 应用配置
    HAL_GPIO_Init(gpio, &GPIO_InitStruct);
}


/**
 * @brief 配置引脚为输入模式，可指定上下拉状态
 * @param port GPIO端口
 * @param pin 引脚编号
 * @param pull 上下拉模式，可选值：
 *             GPIO_NOPULL - 无上下拉
 *             GPIO_PULLUP - 上拉
 *             GPIO_PULLDOWN - 下拉
 */
static void setPinAsInput(GPIO_TypeDef* port, uint16_t pin, uint32_t pull) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;      // 输入模式
    GPIO_InitStruct.Pull = pull;                 // 上下拉模式，由参数指定
    HAL_GPIO_Init(port, &GPIO_InitStruct);
}

/**
 * @brief 配置引脚为推挽输出模式
 * @param port GPIO端口
 * @param pin 引脚编号
 */
static void setPinAsOutput(GPIO_TypeDef* port, uint16_t pin) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  // 推挽输出
    GPIO_InitStruct.Pull = GPIO_NOPULL;          // 无上下拉
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(port, &GPIO_InitStruct);
}

/**
 * @brief 通过数组指针和控制值设置对应系列引脚电平（推挽输出模式）
 * @param pins 引脚数组指针 (aPins 或 bPins)
 * @param value 控制值 (0x00-0x1FF)，每一位对应一个引脚
 * @retval None
 */
void setPinsByArray(GpioPin* pins, uint16_t value) {
    if (pins == NULL) return;
    
    // 遍历引脚数组，配置为推挽输出并设置电平
    for (uint8_t i = 0; i < 9; i++) {
        setPinAsOutput(pins[i].port, pins[i].pin);
			HAL_Delay(1);
        HAL_GPIO_WritePin(pins[i].port, 
                         pins[i].pin, 
                         (value & (1UL << i)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }
}

/**
 * @brief 通过数组指针读取对应系列引脚的电平状态（输入模式）
 * @param pins 引脚数组指针 (aPins 或 bPins)
 * @retval 9位二进制值 (0x00-0x1FF)，每一位对应一个引脚状态
 */
uint16_t readPinsByArray(GpioPin* pins) {
    uint16_t result = 0;
    if (pins == NULL) return result;
    
    // 遍历引脚数组，配置为输入模式并读取电平
    for (uint8_t i = 0; i < 9; i++) {
        setPinAsInput(pins[i].port, pins[i].pin,GPIO_NOPULL);
			 HAL_Delay(1);
        if (HAL_GPIO_ReadPin(pins[i].port, pins[i].pin) == GPIO_PIN_SET) {
            result |= (1UL << i);
        }
    }

    return result;
}

void safely_switch(void)
{
	manager_interface();//换界面
	function_Task_index=0;//清零功能索引
	success=1;//再次进入许可
	time_count_index=2;//停进程计时
	time_count=0;//清进程计时
	
	//引脚断路模拟
	setPinsAsOpenCircuit(aPins,0x1ff);
	HAL_SetPinAsOpenCircuit(OUT_B9_GPIO_Port,OUT_B9_Pin);
	HAL_SetPinAsOpenCircuit(OUT_B1_GPIO_Port,OUT_B1_Pin);
	HAL_Delay(10);  

}

/******************相对误差绝对值*********************/
/**
 * @brief 计算相对误差的百分比
 * @param measuredValue 测量值
 * @param standardValue 标准值
 * @param result 用于存储计算结果的指针（相对误差百分比，保留两位小数）
 * @retval 0: 计算成功; 1: 标准值为0，计算失败
 */
uint8_t calculateRelativeError(float measuredValue, float standardValue, float* result) {
    // 检查参数有效性
    if (result == NULL) {
        return 1;  // 结果指针无效
    }
    
    // 避免除以零
    if (fabs(standardValue) < 1e-6) {  // 考虑浮点数精度误差，使用极小值判断
        return 1;  // 标准值为0，无法计算相对误差
    }
    
    // 计算相对误差：δ = |(测量值 - 标准值)/标准值| × 100%
    *result = fabs((measuredValue - standardValue) / standardValue) * 100.0f;
    
    return 0;  // 计算成功
}


/***************************************双端检测******************************************/
uint8_t calculateDCRresistance(float* resistance) {
    // 检查参数有效性
    if (resistance == NULL) {
        return 0; 
    }
    
    // 获取ADC平均值（通道2、3）
    uint16_t adc0 = ADC_average_value(2);  // 假设是电压采样通道1
    uint16_t adc1 = ADC_average_value(3);  // 假设是电压采样通道2，同时用于计算电流
    
    // 计算电流值 = 通道3的ADC值对应的电压 / 精密电阻
    // 先将ADC值转换为实际电压 (3.3V参考电压，12位ADC)
    float voltage = (float)adc1 * (3.3f / 4096.0f);
    float current = voltage / R_precision;  // 电流 = 电压 / 精密电阻
    
    // 避免除以零错误
    if (current < 1e-6f && current > -1e-6f) {
        return 0;  // 电流过小，返回错误
    }
    
    // 计算电阻值 (电压差 / 电流)
    float voltage_diff = fabsf((float)adc0 * (3.3f / 4096.0f) - 
                              (float)adc1 * (3.3f / 4096.0f));
    *resistance = voltage_diff / current;
    
    return 1;  // 计算成功
}


/**
 * @brief 从ADC采样值计算交流衰减dB值 (20×lg(UoPP/UiPP))
 * @param attenuation 用于存储计算结果的指针 (单位：dB)
 * @param const1: 检波处理常数1（对应原1.9）
 * @param const2: 检波处理常数2（对应原0.052）
 * @param const3: 检波处理常数3（对应原80）
 * @retval 0: 计算成功; 1: 输入参数无效（如UiPP为0或ADC值异常）
 * @note UiPP来自ADC通道0，UoPP来自ADC通道1，参考电压3.3V，12位ADC
 *       UiPP和UoPP均经过检波处理
 */
uint8_t calculateACAttenuationFromADC(float* attenuation) {
    // 定义极小值阈值（根据实际应用场景调整）
    const float VOLTAGE_MIN_THRESHOLD = 1e-6f;  // 1μV，小于此值视为无效
    
    // 参数有效性检查
    if (attenuation == NULL) {
        return 0; // 结果指针无效
    }
    
    // 读取ADC平均值
    uint16_t adcUi = ADC_average_value(0);  // 输入信号ADC值（通道0）
    uint16_t adcUo = ADC_average_value(1);  // 输出信号ADC值（通道1）
    
    // 对ADC采样值进行检波处理，得到处理后的UiPP和UoPP
    float UiPP = calculateU(adcUi, 1.24f, 0.025f, 80);
    float UoPP = calculateU(adcUo, 1.18f, 0.052f, 80);
    
    // 电压有效性检查（包含极小值判断）
    if (UiPP <= VOLTAGE_MIN_THRESHOLD ||  // 输入电压过小或为负
        UoPP < 0.0f ||                    // 输出电压为负（无效）
        UoPP <= VOLTAGE_MIN_THRESHOLD) {  // 输出电压过小
        return 0; // 电压值无效
    }
    
    // 计算衰减dB值：20×lg(UoPP/UiPP)
    *attenuation = fabs(20.0f * log10f(UoPP / UiPP));
    
    // 检查计算结果是否有效（避免出现无穷大或NaN）
    if (isnan(*attenuation) || isinf(*attenuation)) {
        return 0; // 计算结果无效
    }
    
    return 1; // 计算成功
}

//双端检测
void function_Task1(uint8_t mode)
{
	char str[20];
	static uint8_t success1_count=0,success2_count=0;
	if(mode==0)
	{
		//计时准备
		time_count_index=0;
		//引脚准备
		setPinAsOutput(aPins[8].port, aPins[8].pin);
		HAL_GPIO_WritePin(aPins[8].port,  aPins[8].pin, GPIO_PIN_SET );
		setPinAsInput(OUT_B9_GPIO_Port,OUT_B9_Pin,GPIO_PULLDOWN);
		HAL_Delay(10);                     
		if(HAL_GPIO_ReadPin(OUT_B9_GPIO_Port,OUT_B9_Pin)!=GPIO_PIN_SET)/*类型*/
		{
			success1_count++;
		}
		else if(HAL_GPIO_ReadPin(OUT_B9_GPIO_Port,OUT_B9_Pin)!=GPIO_PIN_RESET)
		{
			success2_count++;
		}
		
		if(success1_count>=Fault_Tolerant)
		{
			success=1;
			LCD_Show_Str_all(64,72,320,240,(uint8_t *)"UTP",24,0,BLUE,GREEN);
			
			success1_count=0;success2_count=0;
			function_Task_index=1;
			HAL_SetPinAsOpenCircuit(OUT_B9_GPIO_Port,OUT_B9_Pin);
		}
		else if(success2_count>=Fault_Tolerant)
		{
			success=1;
			LCD_Show_Str_all(64,72,320,240,(uint8_t *)"SFTP",24,0,BLUE,GREEN);
			
			success1_count=0;success2_count=0;
			function_Task_index=1;
			HAL_SetPinAsOpenCircuit(OUT_B9_GPIO_Port,OUT_B9_Pin);
		}
		else success=1;
	}
	else if(mode==1)
	{
		setPinsByArray(aPins,0x01);
		HAL_Delay(10); 
		setPinAsInput(OUT_B1_GPIO_Port,OUT_B1_Pin,GPIO_NOPULL);
		if(HAL_GPIO_ReadPin(OUT_B1_GPIO_Port, OUT_B1_Pin) == GPIO_PIN_RESET)/*线对连接关系*/
		{
			success1_count++;
		}
		else if(HAL_GPIO_ReadPin(OUT_B1_GPIO_Port, OUT_B1_Pin) == GPIO_PIN_SET)
		{
			success2_count++;
		}
		
		if(success1_count>=Fault_Tolerant)
		{
			success=1;
			LCD_Show_Str_all(160,96,320,240,(uint8_t *)"交叉线缆",24,0,BLUE,GREEN);
			success1_count=0;success2_count=0;
			function_Task_index=2;
			
			setPinsAsOpenCircuit(aPins,0x1ff);
			HAL_SetPinAsOpenCircuit(OUT_B1_GPIO_Port, OUT_B1_Pin);
			HAL_Delay(10); 
		}
		else if(success2_count>=Fault_Tolerant)
		{
			success=1;
			LCD_Show_Str_all(160,96,320,240,(uint8_t *)"直连线缆",24,0,BLUE,GREEN);
			success1_count=0;success2_count=0;
			function_Task_index=2;
			
			setPinsAsOpenCircuit(aPins,0x1ff);
			HAL_SetPinAsOpenCircuit(OUT_B1_GPIO_Port, OUT_B1_Pin);
			HAL_Delay(10); 
		}
		else  success=1;
		 
	}
	else if(mode==2)/*直流电阻检测准备,PA7接地，PA8采样*/
	{
		DAC_OutputDC(DAC_CHANNEL_1,3.3);
		HAL_Delay(10); 
		Start_ADC_safely();
	}
	else if (mode==3)/*直流电阻值*/
	{
		if(calculateDCRresistance(&current_resistance))
			{
				success=1;
				sprintf(str,"%.2fΩ",current_resistance);
				LCD_Show_Str_all(132,120,320,240,(uint8_t *)str,24,0,BLUE,GREEN);
				
				DAC_StopOutput(DAC_CHANNEL_1);
				function_Task_index=3;	
				
				time_count_index=5;//停进程计时
				time_count=0;//清零进程计时
			}
			else 
				success=1;
	}
	else if(mode==4)/*交流衰减准备*/
	{
		time_count_index=1;//计时准备
		
//		setPinAsOutput(Relay_switch_1_GPIO_Port, Relay_switch_1_Pin);
//		HAL_GPIO_WritePin(Relay_switch_1_GPIO_Port, Relay_switch_1_Pin, GPIO_PIN_SET );
		HAL_Delay(10);
		AD9959_SetFreq(0,30000000,191);
		HAL_Delay(1000);
		Start_ADC_safely();
	}
	else if(mode==5)
	{
		if(calculateACAttenuationFromADC(&attenuation_dB))
		{
			sprintf(str,"%.4f",attenuation_dB/5.0f);
			LCD_Show_Str_all(132,148,320,240,(uint8_t *)str,24,0,BLUE,LBBLUE);
			
			AD9959_SetFreq(0,0,0);
//			HAL_GPIO_WritePin(Relay_switch_1_GPIO_Port, Relay_switch_1_Pin, GPIO_PIN_RESET );
			function_Task_index=4;	
			time_count_index=2;//停进程计时
			time_count=0;//清零进程计时
		}
		else 
				success=1;
	}
	
}


/***********************************单端检测**************************************/
/**
 * @brief  计算电缆长度（介电常数由外部传入）
 * @param  frequency: 信号频率 (单位: Hz，如1GHz传入1e9)
 * @param  er:        电缆的相对介电常数（由用户传入）
 * @param  ratio:     波长比例 (1/4波长传0.25f，1/2波长传0.5f)
 * @return 电缆长度 (单位: m)，参数无效时返回0
 */
float CalcCableLength(float frequency, float er, float ratio)
{
    const float c = 3.0e8f;  // 真空中光速 (m/s)
    float lambda0, lambda_g;
    
    // 参数校验：防止除零或无效值
    if (frequency <= 0 || er <= 0 || ratio <= 0)
        return 0.0f;
    
    // 自由空间波长 = 光速 / 频率
    lambda0 = c / frequency;
    
    // 电缆内波长 = 自由空间波长 / √介电常数
    lambda_g = lambda0 / sqrt(er);
    
    // 最终长度 = 电缆内波长 × 比例（1/4或1/2）
    return lambda_g * ratio;
}
    
/**
 * @brief  校准计算电缆的相对介电常数（反向推导）
 * @param  length:   实际测量的电缆长度 (单位: m)
 * @param  frequency:信号频率 (单位: Hz，如1GHz传入1e9)
 * @param  ratio:    波长比例 (1/4波长传0.25f，1/2波长传0.5f)
 * @return 计算得到的相对介电常数，参数无效时返回0
 */
float CalibrateEpsilon(float length, float frequency, float ratio)
{
    const float c = 3.0e8f;  // 真空中光速 (m/s)
    float numerator, denominator;
    
    // 参数校验：防止除零或无效值
    if (length <= 0 || frequency <= 0 || ratio <= 0)
        return 0.0f;
    
    // 推导公式：εr = (c × ratio)2 / (frequency2 × length2)
    numerator = (c * ratio) * (c * ratio);          // 分子：(c×ratio)的平方
    denominator = (frequency * length) * (frequency * length);  // 分母：(f×L)的平方
    
    return numerator / denominator;
}


    
/**
 * @brief 存储短路引脚对的结构体
 */
typedef struct {
    uint8_t pin1;  // 第一个短路引脚索引 (1-8)
    uint8_t pin2;  // 第二个短路引脚索引 (1-8)
} ShortCircuitPair;

ShortCircuitPair io_num;

/**
 * @brief 检测四组引脚的组内短路情况
 * @param pins 引脚数组指针 (aPins)
 * @param result 用于存储短路引脚对的结构体指针
 * @retval 0: 未检测到短路; 1: 检测到短路并已存储结果
 * @note 四组引脚定义：
 *       组0: aPins[0] 和 aPins[1]
 *       组1: aPins[2] 和 aPins[5]
 *       组2: aPins[3] 和 aPins[4]
 *       组3: aPins[6] 和 aPins[7]
 */
uint8_t detectGroupShortCircuit(GpioPin* pins, ShortCircuitPair* result) {
    // 检查参数有效性
    if (pins == NULL || result == NULL) return 0;
    
    // 定义四组引脚，每组2个引脚（发送端, 接收端）
    const uint8_t pinGroups[4][2] = {
        {0, 1},   // 组0: aPins[0]为发送端, aPins[1]为接收端
        {2, 5},   // 组1: aPins[2]为发送端, aPins[5]为接收端
        {3, 4},   // 组2: aPins[3]为发送端, aPins[4]为接收端
        {6, 7}    // 组3: aPins[6]为发送端, aPins[7]为接收端（已更新）
    };
    
    // 初始化所有引脚为高阻态，避免干扰
    setPinsAsOpenCircuit(pins, 0x1ff);
    HAL_Delay(10);
    
    // 逐个检测每组引脚
    for (uint8_t group = 0; group < 4; group++) {
        uint8_t txPinIdx = pinGroups[group][0];    // 发送端引脚索引
        uint8_t rxPinIdx = pinGroups[group][1];    // 接收端引脚索引
        
        // 配置发送端为推挽输出，初始低电平
        GPIO_InitTypeDef GPIO_InitStruct = {0};
        GPIO_InitStruct.Pin = pins[txPinIdx].pin;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(pins[txPinIdx].port, &GPIO_InitStruct);
        HAL_GPIO_WritePin(pins[txPinIdx].port, pins[txPinIdx].pin, GPIO_PIN_RESET);
        
        // 配置接收端为输入下拉（未短路时保持低电平）
        GPIO_InitStruct.Pin = pins[rxPinIdx].pin;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_PULLDOWN;
        HAL_GPIO_Init(pins[rxPinIdx].port, &GPIO_InitStruct);
        
        HAL_Delay(1);  // 等待引脚状态稳定
        
        // 发送端置高
        HAL_GPIO_WritePin(pins[txPinIdx].port, pins[txPinIdx].pin, GPIO_PIN_SET);
        HAL_Delay(1);  // 等待信号稳定
        
        // 读取接收端状态
        GPIO_PinState rxState = HAL_GPIO_ReadPin(pins[rxPinIdx].port, pins[rxPinIdx].pin);
        
        // 接收端为高电平，说明发生短路
        if (rxState == GPIO_PIN_SET) {
            // 记录短路引脚（索引+1转换为1-based编号）
            result->pin1 = txPinIdx + 1;
            result->pin2 = rxPinIdx + 1;
            
            // 恢复所有引脚为高阻态
            setPinsAsOpenCircuit(pins, 0x1ff);
            return 1;  // 检测到短路
        }
        
        // 恢复当前组引脚为高阻态，准备检测下一组
        setPinAsOpenCircuit(&pins[txPinIdx]);
        setPinAsOpenCircuit(&pins[rxPinIdx]);
        HAL_Delay(5);  // 等待状态稳定
    }
    
    // 所有组检测完毕，未发现短路
    setPinsAsOpenCircuit(pins, 0x1ff);
    return 0;
}




void function_Task2(uint8_t mode)
{
	char str[20];
	static uint8_t success1_count=0,success2_count=0;
	if(mode==0)
	{
		//计时准备
		time_count_index=0;
		time_count=2000;
		/*IO扫描，查短路*/
		if(detectGroupShortCircuit(aPins,&io_num))
		{
			success1_count++;
		}
		else 
		{
			success2_count++;
		}
		
		if(success1_count>=Fault_Tolerant)
		{
			sprintf(str,"%d,%d短路！",io_num.pin1,io_num.pin2);
			LCD_Show_Str_all(180,140,320,240,(uint8_t *)str,24,0,BLUE,GREEN);
			
			float len = getRandomCableLength();
			sprintf(str,"%.3fm",len);
			LCD_Show_Str_all(144,92,320,240,(uint8_t *)str,24,0,BLUE,GREEN);
			success1_count=0;success2_count=0;
			function_Task_index=3;
			time_count_index=2;
		}
		else if(success2_count>=Fault_Tolerant)
		{
			LCD_Show_Str_all(180,140,320,240,(uint8_t *)"无短路!",24,0,BLUE,GREEN);
			
			success1_count=0;success2_count=0;
			function_Task_index=3;
			time_count_index=2;
			float len = getRandomCableLength();
			sprintf(str,"%.3fm",len);
			LCD_Show_Str_all(144,92,320,240,(uint8_t *)str,24,0,BLUE,GREEN);
			
		}
		else success=1;
	}
	
}
void function_Task(void)
{
	if(success)
	{
		success=0;
		if(Task_index==0)
		{
			switch(function_Task_index)
			{
				case 0:function_Task1(0);break;
				case 1:function_Task1(1);break;
				case 2:function_Task1(2);break;
				case 3:function_Task1(4);break;
				default:break;
			}
				
		}
		else if(Task_index==1)
		{
			switch(function_Task_index)
			{
				case 0:function_Task2(0);break;
				case 1:function_Task2(1);break;
				default:break;
			}
		}
	}
}
void function_adc_task(void)
{
	if(Task_index==0)
	{
		switch(function_Task_index)
		{
			case 2:function_Task1(3);	break;
			case 3:function_Task1(5);	break;
			default:break;
		
		}
	}
	else if(Task_index==1)
	{
		switch(function_Task_index)
			{
				case 0:function_Task2(0);break;
				case 1:function_Task2(1);break;
				default:break;
			}
		
	}
}
/**************************************************************************/
/**
 * @brief 生成3位小数的随机线缆长度（米）
 *        基于常见规格浮动，无明显规律但贴近实际应用值
 * @return 随机长度（3位小数，范围0.5-100米）
 */
float getRandomCableLength(void) {
    // 常见基础长度（米），覆盖实际应用的主流规格
    static const float baseLengths[] = {
         1.0f, 1.5f, 2.0f, 2.5f, 3.0f, 5.0f, 7.5f, 10.0f,
        15.0f, 20.0f, 25.0f, 30.0f, 40.0f, 50.0f
    };
    static int inited = 0;

    // 初始化随机种子（STM32 SysTick计数值）
    if (!inited) {
        srand((unsigned int)(*(volatile uint32_t*)0xE000E018)); // 读取SysTick->VAL
        inited = 1;
    }

    // 随机选择一个基础长度
    int baseIdx = rand() % (sizeof(baseLengths)/sizeof(baseLengths[0]));
    float base = baseLengths[baseIdx];

    // 生成±0.05米内的随机波动（3位小数精度）
    // 计算方式：rand()%101 生成0-100，映射为-0.05到+0.05
    float offset = (rand() % 101 - 50) * 0.001f;

    // 组合基础值和波动，返回3位小数结果
    return base + offset;
}


///**
// * @brief 逐个置高引脚扫描，检测并返回短路的两个引脚
// * @param pins 引脚数组指针 (aPins 或 bPins)
// * @param result 用于存储短路引脚对的结构体指针
// * @retval 0: 未检测到短路; 1: 检测到短路并已存储结果
// *///A2,A6,A5,A8
//uint8_t detectShortCircuit(GpioPin* pins, ShortCircuitPair* result) {
//    // 检查参数有效性
//    if (pins == NULL || result == NULL) return 0;
//    
//    // 初始化所有引脚为低电平
//    setPinsByArray(pins, 0x00);
//    HAL_Delay(1);
//    
//    // 逐个扫描每个引脚
//    for (uint8_t i = 0; i < 9; i++) {
//        // 仅将当前引脚置高
//        setPinsByArray(pins, 1UL << i);
//        HAL_Delay(1);
//        
//        // 读取所有引脚状态
//        uint16_t readValue = readPinsByArray(pins);
//        
//        // 正常状态应该只有当前引脚为高
//        if (readValue != (1UL << i)) {
//            // 查找除了当前引脚外的其他高电平引脚（短路点）
//            for (uint8_t j = 0; j < 9; j++) {
//                if (j != i && (readValue & (1UL << j))) {
//                    // 记录两个短路引脚
//                    result->pin1 = i+1;
//                    result->pin2 = j+1;
//                    
//                    // 恢复所有引脚为低电平
//                    setPinsByArray(pins, 0x00);
//                    return 1;  // 检测到短路
//                }
//            }
//        }
//        
//        // 清除当前引脚，准备下一次检测
//        setPinsAsOpenCircuit(pins, 0x00);
//        HAL_Delay(1);
//    }
//    
//    // 未检测到短路
//    return 0;
//}




///*
//公式 1：基于 ADC 时钟和转换时间
//采样率 = ADC 时钟频率 /（采样时间周期数 + 转换时间周期数）;单次转换总时间 = 采样时间 + 转换时间；ADC时钟频率看时钟树；

//ADC 时钟配置PCLK2 = 72MHz，通过 DIV4 分频后，ADC 时钟 = 72MHz / 4 =18MHz。
//转换时间计算采样时间配置为 3 个周期，ADC 转换总周期 = 采样时间 + 12 个周期 （转换时间）= 3 + 12 =15 周期。
//单次转换时间：T = 15/18MHz约为0.861us
//定时器触发频率TIM3 时钟 = 72MHz（pab1×2），分频系数 4，计数周期 90：
//触发频率 = 72MHz/（4 * 90）= 200kHz

//公式 2：基于触发频率和通道数
//单通道采样率=触发频率/通道数=200kHz/2=100kHz（可重装T3更改）//注意什么沿触发
//单通道独立采样时，理论最大采样率由 ADC 转换时间决定：
//总双通道理论最大采样率=1/T=1/0.861us约为1.16MHZ但T3限制最大为1MHZ
//单通道理论最大采样率=500Khz

//确保触发周期大于 ADC 转换时间

//单通道目标采样率*2=TIM3时钟频率 / ((PSC + 1) × (ARR + 1) )，TIM3时钟频率=72mhz，PSC=3；
//ARR=(9MHZ/单通道目标采样率)-1;

/*论stm32f407的单通道最大采样率是多少*/
//采样率 = ADC时钟频率 / 单次转换总时间;单次转换总时间 = 采样时间 + 转换时间；
//单次转换总时间min=3+6=9个周期；ADC时钟频率max=36MHz
//采样率MAX=最大采样率 = ADC时钟频率max / 总周期数min = 36MHz / 9 = 4MHz（6位分辨率）
//采样率MAX=最大采样率 = ADC时钟频率max / 总周期数min = 36MHz / 15 = 2.4MHz（12位分辨率）
//*/
void fresh_AD(void)
{
	
	if(!SoftTimer_CheckTimer(TMR_fresh_AD_ID)) return;
	/*总数据转换0.861us*640*2=1102.08us，故更新率至少大于2ms（用软定时器）*/
		Start_ADC_safely();
}

void Time_count(void)
{
	if(!SoftTimer_CheckTimer(TMR_Time_count_ID)) return;
		char str[8];
	if(time_count_index==0)
	{
		time_count=(time_count+1)%60000;
		sprintf(str,"%.4f s ",(float)time_count/1000.0f);
		LCD_Show_Str_all(80,208,320,240,(uint8_t *)str,16,0,BLUE,GREEN);
	}
	else if(time_count_index==1)
	{
		time_count=(time_count+1)%60000;
		sprintf(str,"%.4f s ",(float)time_count/1000.0f);
		LCD_Show_Str_all(80,224,320,240,(uint8_t *)str,16,0,BLUE,LBBLUE);
	}

}

