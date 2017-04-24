#include "stm32f4xx_conf.h"
#include <stm32f4xx.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_tim.h>
#include <stm32f4xx_dma.h>
#include <stm32f4xx_usart.h>
#include "stm32f4xx_exti.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_syscfg.h"
#include <misc.h>
#include "delay.h"
#include "ws2812.h"
#include "math.h"
#include "stm32f4xx_usart.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <sekwencje.h>
#include <colours.h>
#include <clear.h>
#include <time.h>

#define false 0
#define true 1

int j=0;
int x=0;
bool skanuje,s,k,m;

void rcc()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE );
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
}

void gpio()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	// konfiguracja linii Tx
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_USART3);
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	// konfiguracja linii Rx
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART3);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void usart()
{
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;

	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl =
	USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	// konfiguracja
	USART_Init(USART3, &USART_InitStructure);

	// wlaczenie ukladu USART
	USART_Cmd(USART3, ENABLE );

	NVIC_InitTypeDef NVIC_InitStructure;
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE );
	NVIC_InitStructure. NVIC_IRQChannel = USART3_IRQn ;
	NVIC_InitStructure. NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure. NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure. NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_EnableIRQ( USART3_IRQn );
}

uint8_t brightness;
uint8_t *pixels;

void setBrightness(uint8_t x) {
  uint8_t newBrightness = x + 1;
  if(newBrightness != brightness)
  {
    uint8_t  c,
            *ptr           = pixels,
             oldBrightness = brightness - 1;
    uint16_t scale;
    if(oldBrightness == 0) scale = 0;
    else if(x == 255) scale = 65535 / oldBrightness;
    else scale = (((uint16_t)newBrightness << 8) - 1) / oldBrightness;
    for(uint16_t i=0; i<3; i++) {
      c      = *ptr;
      *ptr++ = (c * scale) >> 8;
    }
   brightness = newBrightness;
   if(brightness==41)
   {GPIO_SetBits(GPIOD,GPIO_Pin_14);}
  }
}

uint8_t getBrightness(void) {
  return brightness - 1;
}

int main(void)
{
	SystemInit();
	rcc();
	gpio();
	usart();
	init_systick();	//delay
	ws2812_init();	//LED + PWM

	while(1)
	{
	}
}

int numerdiody;
int red_p;
int green_p;
int blue_p;
int c;

int x;
bool x1;

int red_br;
int green_br;
int blue_br;
int red_br1;
int blue_br1;
int green_br1;
void USART3_IRQHandler(void)
{
		if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
		{
			//ODBIERANIE JASNO�CI
			uint8_t c = USART3->DR;
			//uint8_t c = USART3->DR;
			//>>>>>>>>>>>>>>>>>>>>>>>
			if(USART3->DR=='L')
			{
				x=0;
				x1=1;
			}
			if(x1==1)
			{
				if(c==0)
				x=0;
				if (c >= 1 && c <= 100&&c!=82&&c!=71&&c!=66&&c!=97&&c!=98&&c!=99&&c!=100)
				x = c*10;
				else
				x1=0;
				setBrightness(x);
			}

			if(USART3->DR == 'R')
			{
				red_p = 0;
				s = 1;
			}
			if (s==1)
			{
				if(c==0)
				red_p=0;
				if (c >= 1 && c <= 255&&c!=76&&c!=71&&c!=66)
				{
					red_p = c*4-1;
					red_br=red_p;
				}
				else
				s=0;
			}

			if(USART3->DR == 'G')
			{
				green_p = 0;
				k = 1;
			}
			if (k==1)
			{
				if(c<1)
				green_p=0;
				if (c >= 1 && c <= 255&&c!=76&&c!=82&&c!=66)
				{
					green_p = c*4-1;
					green_br=green_p;
				}
				else
				k=0;
			}

			if(USART3->DR == 'B')
			{
				blue_p = 0;
				m = 1;
			}
			if (m==1)
			{
				if(c<1)
				blue_p=0;

				if (c >= 1 && c <= 255&&c!=76&&c!=82&&c!=71)
				{
					blue_p = c*4-1;
					blue_br=blue_p;
				}
				else
				m=0;
			}
			rgb(red_p, green_p,blue_p);
			if(brightness)
			{
				if(red_br==327)
				{
					red_br=0;
				}
				if(green_br==283)
				{
					green_br=0;
				}
				if(blue_br==263)
				{
					blue_br=0;
				}
				int i=0;

				for(i=0;i<8;i++)
				{
					diode[i].red=(red_br* brightness) >> 8;
					diode[i].green=(green_br* brightness) >> 8;
					diode[i].blue=(blue_br* brightness) >> 8;
				}
			}
			//do kolorow
			if(USART3->DR == 'g')
			{
				brightness=100;
				red_br=135;
				green_br=31;
				blue_br=0;
			}
			if(USART3->DR == 'h')
			{
				brightness=101;
				red_br=195;
				green_br=111;
				blue_br=0;
			}
			if(USART3->DR == 'i')
			{
				brightness=101;
				red_br1=24;
				green_br1=100;
				blue_br1=0;
			}
			if(USART3->DR == 'j')
			{
				brightness=101;
				red_br1=0;
				green_br1=243;
				blue_br1=75;
			}
			if(USART3->DR == 'k')
			{
				brightness=101;
				red_br1=255;
				green_br1=0;
				blue_br1=51;
			}
			if(USART3->DR == 'l')
			{
				brightness=101;
				red_br1=255;
				green_br1=0;
				blue_br1=255;
			}
			if(USART3->DR == 'm')
			{
				brightness=101;
				red_br1=19;
				green_br1=0;
				blue_br1=207;
			}

			//do sekwencji
			if(USART3->DR == 'a')
			{
				red_p=0;
				green_p=0;
				blue_p=0;
				s1();
			}
			if(USART3->DR == 'b')
			{
				red_p=0;
				green_p=0;
				blue_p=0;
				s2();
			}
			if(USART3->DR == 'c')
			{
				red_p=0;
				green_p=0;
				blue_p=0;
				clear();
				s3();
			}
			if(USART3->DR == 'd')
			{
				red_p=0;
				green_p=0;
				blue_p=0;
				s4();
			}
			if(USART3->DR == 'e')
			{
				red_p=0;
				green_p=0;
				blue_p=0;
				s5();
			}
			if(USART3->DR == 'f')
			{
				red_p=0;
				green_p=0;
				blue_p=0;
				s6();
			}
			//turn off
			if(USART3->DR == 'O')
			{
				clear_app();
				red_br=0;
				green_br=0;
				blue_br=0;
				red_p=0;
				green_p=0;
				blue_p=0;
			}
		}
}