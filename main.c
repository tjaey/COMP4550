#include "stm32f4xx.h"// Device header
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"

int LED_ON;

//Detect Long Press
int TIM2_Value;
int BUTTON_PRESSED;

void initTimer()
{
	TIM_TimeBaseInitTypeDef timerSettings;
	/* Enable timer 2, using the Reset and Clock Control register */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	timerSettings.TIM_Prescaler = 8399;
	timerSettings.TIM_CounterMode = TIM_CounterMode_Up; //set counter to count up
	timerSettings.TIM_Period = 10000; 
	timerSettings.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM2, &timerSettings);
	TIM_Cmd(TIM2, ENABLE); /* start counting by enabling CEN in CR1 */
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); //Enable timer interrupts
}

void initLEDs()
{
	GPIO_InitTypeDef GPIO_Initstructure; // initiate the structure that contains the configuration information for the specified GPIO peripheral.
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); // activate RCC_AHB1 clock for LED, located at GPIOD
// The configuration of the locked GPIO pins can no longer be modified until the next reset.
	// LED Configuration
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;  // I/O PD12, PD13, PD14, and PD15
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_OUT; // output mode
	GPIO_Initstructure.GPIO_OType = GPIO_OType_PP;  // push pull output type
	GPIO_Initstructure.GPIO_PuPd = GPIO_PuPd_NOPULL; // no pull
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_100MHz; // maximum frequency of the GPIO
	GPIO_Init(GPIOD, &GPIO_Initstructure);
}

void TIM2_IRQHandler()
{
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	if(LED_ON == 0)
	{
		GPIO_SetBits(GPIOD, GPIO_Pin_15);
		LED_ON = 1;
	}
	else
	{
		GPIO_ResetBits(GPIOD, GPIO_Pin_15);
		LED_ON = 0;
	}
	if(BUTTON_PRESSED)
	{
		TIM2_Value = TIM2_Value + 1;
	}
	else
	{
		TIM2_Value = 0;
	}
		
}

void EXTI0_IRQHandler()
{
    // Checks whether the interrupt from EXTI0 or not
    if(EXTI_GetITStatus(EXTI_Line0) != RESET)
    {   
			if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0))
				GPIO_ResetBits(GPIOD, GPIO_Pin_13);
			else
				GPIO_SetBits(GPIOD, GPIO_Pin_13);

			// Clears the EXTI line pending bit
      EXTI_ClearITPendingBit(EXTI_Line0);
    }
}

void InitEXTI()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    // Selects the GPIOA pin 0 used as external interrupt source
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
    
		EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_Init(&EXTI_InitStructure);
}


void EnableTimerInterrupt()
{
    NVIC_InitTypeDef nvicStructure;
    nvicStructure.NVIC_IRQChannel = TIM2_IRQn;
    nvicStructure.NVIC_IRQChannelPreemptionPriority = 0;
    nvicStructure.NVIC_IRQChannelSubPriority = 1;
    nvicStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvicStructure);
}

void EnableEXTIInterrupt()
{
		NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void InitButton() // initialize user button
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 
	
	GPIO_InitTypeDef GPIO_Initstructure; // initiate the structure that contains the configuration information for the specified GPIO peripheral.
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Initstructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Initstructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA, &GPIO_Initstructure);
}


int main()
{
	TIM2_Value = 0;
	BUTTON_PRESSED = 0;
	
	
	initLEDs();	
	EnableTimerInterrupt();
	initTimer();
	EnableEXTIInterrupt();
	InitEXTI();
	InitButton();

	while(1){
		//button_pin = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);
		//if (button_pin){
		//	GPIO_SetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
		//}
		//else{
		//	GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
		//}
		BUTTON_PRESSED = 0;

	}
}
