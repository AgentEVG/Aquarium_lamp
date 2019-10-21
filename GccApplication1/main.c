
/*
---------------------------------------------------------------------------
2019, Evgeny Bykov <evg-bikov@yandex.ru>
Atmega328P

Аквариумный RGBW светильник c часами:
- Отображение времени на дисплее SSD1306 (128x64 pix)
- Включение и выключение 4х каналов света (Red, Green, Blue, White) по заданному времени
- - Настройка яркости каждого канала света 
- - Возможность включения режима рассвет/закат для каждого канала света
- Включение и выключение подачи CO2 в аквариум по заданному времени
- Включение и выключение охлаждения светоизлучающих элементов происходит автоматически при включении/выключении света
- - Настройка скорости вращения вентилятора охлаждения
- Измерение температуры воды в аквариуме

 * Settings:
 *  F_CPU=1600000
 --------------------------------------------------------------------------
 */

#define F_CPU 16000000L
#define XTAL 16000000L
#define BOOTLOADER_ADDRESS = 0x7000;
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <avr/eeprom.h>
	
#include "lib/main_init.h"
#include "lib/ssd1306.h"
#include "lib/symbols_charset.h"
#include "lib/ds1307.h"
#include "lib/ds18b20.h"

#define White_light_level OCR1A		//PB1
#define Blue_light_level OCR1B		//PB2
#define Red_light_level OCR2A		//PB3
#define Green_light_level OCR0A		//PD6
#define Fan_speed OCR0B				//PD5
									//PD4 Датчик температуры
#define CO2_open PORTB|=1			//Открыть клапан CO2 PB0
#define CO2_close PORTB&=~1			//Закрыть клапан CO2 PB0
signed char progressbar_pos=-6;
char clock_dot_flag=0; //флаг разделителя часов и минут
char global_hour, global_minute, global_temperature; //глобальные переменные часов/минут

//Белый свет
uint8_t EEMEM white_bright;
uint8_t EEMEM white_time_h_start;
uint8_t EEMEM white_time_m_start;
uint8_t EEMEM white_time_h_end;
uint8_t EEMEM white_time_m_end;
uint16_t EEMEM white_rising_time;

// Красный свет
uint8_t EEMEM red_bright;
uint8_t EEMEM red_time_h_start;
uint8_t EEMEM red_time_m_start;
uint8_t EEMEM red_time_h_end;
uint8_t EEMEM red_time_m_end;
uint16_t EEMEM red_rising_time;

//Зелёный свет
uint8_t EEMEM green_bright;
uint8_t EEMEM green_time_h_start;
uint8_t EEMEM green_time_m_start;
uint8_t EEMEM green_time_h_end;
uint8_t EEMEM green_time_m_end;
uint16_t EEMEM green_rising_time;

//Синий свет
uint8_t EEMEM blue_bright;
uint8_t EEMEM blue_time_h_start;
uint8_t EEMEM blue_time_m_start;
uint8_t EEMEM blue_time_h_end;
uint8_t EEMEM blue_time_m_end;
uint16_t EEMEM blue_rising_time;

//Вентилятор
uint8_t EEMEM fan_max_speed;
uint8_t EEMEM fan_min_speed;

//Клапан CO2
uint8_t EEMEM CO2_time_h_start;
uint8_t EEMEM CO2_time_m_start;
uint8_t EEMEM CO2_time_h_end;
uint8_t EEMEM CO2_time_m_end;


char check_button_menu()
{
	if ((PINC&(1<<PC3))==0)
	{
		while ((PINC&(1<<PC3))==0)
		{
			_delay_ms(100);
		}
		return 1;
	}
	else
	{
		return 0;
	}
}

char check_button_up()
{
char autocounter;
	if ((PINC&(1<<PC2))==0)
	{
		autocounter=0;
		while (((PINC&(1<<PC2))==0))
		{
			_delay_ms(50);
			autocounter ++;
			if (autocounter == 10)
			{
				autocounter = 0;
				return 1;
			}
		}
		return 1;
	}
	else
	{
		return 0;
	}
}

char check_button_down()
{
char autocounter;
	if ((PINC&(1<<PC1))==0)
	{
		autocounter=0;
		while (((PINC&(1<<PC1))==0))
		{
			_delay_ms(50);
			autocounter ++;
			if (autocounter == 10)
			{
				autocounter = 0;
				return 1;
			}
		}
		return 1;
	}
	else
	{
		return 0;
	}
}

void progressbar_progress(unsigned char vol,char y)
{
	BYTE i;
	if (vol>100){vol=100;}
	vol=vol / 5;
	ssd1306_goto_xy(0, y);
	if (vol<1)
	{
		ssd1306_putchar(0x83);	
	} 
	else
	{
		ssd1306_putchar(0x84);
	}
	
	for (i=2;i<20;i++)
	{	
		if (vol>=i)
		{
			ssd1306_putchar(0x88);
		} 
		else
		{
			ssd1306_putchar(0x87);
		}
	}
	
	if (vol>=20)
	{
		ssd1306_putchar(0x86);
	}
	else
	{
		ssd1306_putchar(0x85);
	}	
}

void progressbar_waiting()
{
		switch (abs(progressbar_pos))
		{
			case 6:
			ssd1306_goto_xy(1, 1);
			ssd1306_putchar(0x84);
			ssd1306_putchar(0x87);
			ssd1306_putchar(0x87);
			ssd1306_putchar(0x87);
			ssd1306_putchar(0x87);
			ssd1306_putchar(0x87);
			ssd1306_putchar(0x85);	
			break;
			
			case 5:
			ssd1306_goto_xy(1, 1);
			ssd1306_putchar(0x83);
			ssd1306_putchar(0x88);
			ssd1306_putchar(0x87);
			ssd1306_putchar(0x87);
			ssd1306_putchar(0x87);
			ssd1306_putchar(0x87);
			ssd1306_putchar(0x85);
			break;

			case 4:
			ssd1306_goto_xy(1, 1);
			ssd1306_putchar(0x83);
			ssd1306_putchar(0x87);
			ssd1306_putchar(0x88);
			ssd1306_putchar(0x87);
			ssd1306_putchar(0x87);
			ssd1306_putchar(0x87);
			ssd1306_putchar(0x85);
			break;
			
			case 3:
			ssd1306_goto_xy(1, 1);
			ssd1306_putchar(0x83);
			ssd1306_putchar(0x87);
			ssd1306_putchar(0x87);
			ssd1306_putchar(0x88);
			ssd1306_putchar(0x87);
			ssd1306_putchar(0x87);
			ssd1306_putchar(0x85);
			break;
			
			case 2:
			ssd1306_goto_xy(1, 1);
			ssd1306_putchar(0x83);
			ssd1306_putchar(0x87);
			ssd1306_putchar(0x87);
			ssd1306_putchar(0x87);
			ssd1306_putchar(0x88);
			ssd1306_putchar(0x87);
			ssd1306_putchar(0x85);
			break;
			
			case 1:
			ssd1306_goto_xy(1, 1);
			ssd1306_putchar(0x83);
			ssd1306_putchar(0x87);
			ssd1306_putchar(0x87);
			ssd1306_putchar(0x87);
			ssd1306_putchar(0x87);
			ssd1306_putchar(0x88);
			ssd1306_putchar(0x85);
			break;
			
			case 0:
			ssd1306_goto_xy(1, 1);
			ssd1306_putchar(0x83);
			ssd1306_putchar(0x87);
			ssd1306_putchar(0x87);
			ssd1306_putchar(0x87);
			ssd1306_putchar(0x87);
			ssd1306_putchar(0x87);
			ssd1306_putchar(0x86);
			break;
		}
		progressbar_pos++;
		if (progressbar_pos==6){progressbar_pos=-6;}
}

void show_digit(char digit, BYTE offset)
{
	//Clear digit place
	ssd1306_goto_xy(offset, 0);
	ssd1306_putchar(0xA0);
	ssd1306_putchar(0xA0);
	ssd1306_putchar(0xA0);
	ssd1306_goto_xy(offset, 1);
	ssd1306_putchar(0xA0);
	ssd1306_putchar(0xA0);
	ssd1306_putchar(0xA0);
	ssd1306_goto_xy(offset, 2);
	ssd1306_putchar(0xA0);
	ssd1306_putchar(0xA0);
	ssd1306_putchar(0xA0);
	ssd1306_goto_xy(offset, 3);
	ssd1306_putchar(0xA0);
	ssd1306_putchar(0xA0);
	ssd1306_putchar(0xA0);
	
	switch (digit)
	{
	   case '-':
	   ssd1306_goto_xy(offset, 0);
	   ssd1306_putchar(0xA0);
	   ssd1306_putchar(0xA0);
	   ssd1306_putchar(0xA0);
	   ssd1306_goto_xy(offset, 1);
	   ssd1306_putchar(0xA0);
	   ssd1306_putchar(0xA0);
	   ssd1306_putchar(0xA0);
	   ssd1306_goto_xy(offset, 2);
	   ssd1306_putchar(0xA0);
	   ssd1306_putchar(0xA0);
	   ssd1306_putchar(0xA0);
	   ssd1306_goto_xy(offset, 3);
	   ssd1306_putchar(0xA0);
	   ssd1306_putchar(0xA0);
	   ssd1306_putchar(0xA0);
	   break;

	   case '0':
	   ssd1306_goto_xy(offset, 0);
	   ssd1306_putchar(0x91);
	   ssd1306_putchar(0x89);
	   ssd1306_putchar(0x94);
	   ssd1306_goto_xy(offset, 1);
	   ssd1306_putchar(0x8B);
	   ssd1306_putchar(0xA0);
	   ssd1306_putchar(0x8C);
	   ssd1306_goto_xy(offset, 2);
	   ssd1306_putchar(0x8B);
	   ssd1306_putchar(0xA0);
	   ssd1306_putchar(0x8C);
	   ssd1306_goto_xy(offset, 3);
	   ssd1306_putchar(0x92);
	   ssd1306_putchar(0x8A);
	   ssd1306_putchar(0x93);
	   break;
	   
	   case '1':
	   ssd1306_goto_xy(offset, 0);
	   ssd1306_putchar(0xA0);
	   ssd1306_putchar(0xA0);
	   ssd1306_putchar(0x8C);
	   ssd1306_goto_xy(offset, 1);
	   ssd1306_putchar(0xA0);
	   ssd1306_putchar(0xA0);
	   ssd1306_putchar(0x8C);
	   ssd1306_goto_xy(offset, 2);
	   ssd1306_putchar(0xA0);
	   ssd1306_putchar(0xA0);
	   ssd1306_putchar(0x8C);
	   ssd1306_goto_xy(offset, 3);
	   ssd1306_putchar(0xA0);
	   ssd1306_putchar(0xA0);
	   ssd1306_putchar(0x8C);
	   break;
	   
	   case '2':
	   ssd1306_goto_xy(offset, 0);
	   ssd1306_putchar(0x89);
	   ssd1306_putchar(0x89);
	   ssd1306_putchar(0x94);
	   ssd1306_goto_xy(offset, 1);
	   ssd1306_putchar(0x9A);
	   ssd1306_putchar(0x9A);
	   ssd1306_putchar(0x8F);
	   ssd1306_goto_xy(offset, 2);
	   ssd1306_putchar(0x8D);
	   ssd1306_putchar(0x99);
	   ssd1306_putchar(0x99);
	   ssd1306_goto_xy(offset, 3);
	   ssd1306_putchar(0x92);
	   ssd1306_putchar(0x8A);
	   ssd1306_putchar(0x8A);
	   break;
	   
	   case '3':
	   ssd1306_goto_xy(offset, 0);
	   ssd1306_putchar(0x89);
	   ssd1306_putchar(0x89);
	   ssd1306_putchar(0x94);
	   ssd1306_goto_xy(offset, 1);
	   ssd1306_putchar(0x9A);
	   ssd1306_putchar(0x9A);
	   ssd1306_putchar(0x8F);
	   ssd1306_goto_xy(offset, 2);
	   ssd1306_putchar(0x99);
	   ssd1306_putchar(0x99);
	   ssd1306_putchar(0x90);
	   ssd1306_goto_xy(offset, 3);
	   ssd1306_putchar(0x8A);
	   ssd1306_putchar(0x8A);
	   ssd1306_putchar(0x93);
	   break;
	   
	   case '4':
	   ssd1306_goto_xy(offset, 0);
	   ssd1306_putchar(0x8B);
	   ssd1306_putchar(0xA0);
	   ssd1306_putchar(0x8C);
	   ssd1306_goto_xy(offset, 1);
	   ssd1306_putchar(0x8E);
	   ssd1306_putchar(0x9A);
	   ssd1306_putchar(0x8F);
	   ssd1306_goto_xy(offset, 2);
	   ssd1306_putchar(0x99);
	   ssd1306_putchar(0x99);
	   ssd1306_putchar(0x90);
	   ssd1306_goto_xy(offset, 3);
	   ssd1306_putchar(0x20);
	   ssd1306_putchar(0x20);
	   ssd1306_putchar(0x8C);
	   break;
	   
	   case '5':
	   ssd1306_goto_xy(offset, 0);
	   ssd1306_putchar(0x91);
	   ssd1306_putchar(0x89);
	   ssd1306_putchar(0x89);
	   ssd1306_goto_xy(offset, 1);
	   ssd1306_putchar(0x8E);
	   ssd1306_putchar(0x9A);
	   ssd1306_putchar(0x9A);
	   ssd1306_goto_xy(offset, 2);
	   ssd1306_putchar(0x99);
	   ssd1306_putchar(0x99);
	   ssd1306_putchar(0x90);
	   ssd1306_goto_xy(offset, 3);
	   ssd1306_putchar(0x8A);
	   ssd1306_putchar(0x8A);
	   ssd1306_putchar(0x93);
	   break;
	   
	   case '6':
	   ssd1306_goto_xy(offset, 0);
	   ssd1306_putchar(0x91);
	   ssd1306_putchar(0x89);
	   ssd1306_putchar(0x89);
	   ssd1306_goto_xy(offset, 1);
	   ssd1306_putchar(0x8E);
	   ssd1306_putchar(0x9A);
	   ssd1306_putchar(0x9A);
	   ssd1306_goto_xy(offset, 2);
	   ssd1306_putchar(0x8D);
	   ssd1306_putchar(0x99);
	   ssd1306_putchar(0x90);
	   ssd1306_goto_xy(offset, 3);
	   ssd1306_putchar(0x92);
	   ssd1306_putchar(0x8A);
	   ssd1306_putchar(0x93);
	   break;
	   
	   case '7':
	   ssd1306_goto_xy(offset, 0);
	   ssd1306_putchar(0x89);
	   ssd1306_putchar(0x89);
	   ssd1306_putchar(0x94);
	   ssd1306_goto_xy(offset, 1);
	   ssd1306_putchar(0xA0);
	   ssd1306_putchar(0xA0);
	   ssd1306_putchar(0x8C);
	   ssd1306_goto_xy(offset, 2);
	   ssd1306_putchar(0xA0);
	   ssd1306_putchar(0xA0);
	   ssd1306_putchar(0x8C);
	   ssd1306_goto_xy(offset, 3);
	   ssd1306_putchar(0xA0);
	   ssd1306_putchar(0xA0);
	   ssd1306_putchar(0x8C);
	   break;
	   
	   case '8':
	   ssd1306_goto_xy(offset, 0);
	   ssd1306_putchar(0x91);
	   ssd1306_putchar(0x89);
	   ssd1306_putchar(0x94);
	   ssd1306_goto_xy(offset, 1);
	   ssd1306_putchar(0x8E);
	   ssd1306_putchar(0x9A);
	   ssd1306_putchar(0x8F);
	   ssd1306_goto_xy(offset, 2);
	   ssd1306_putchar(0x8D);
	   ssd1306_putchar(0x99);
	   ssd1306_putchar(0x90);
	   ssd1306_goto_xy(offset, 3);
	   ssd1306_putchar(0x92);
	   ssd1306_putchar(0x8A);
	   ssd1306_putchar(0x93);
	   break;
	   
	   case '9':
	   ssd1306_goto_xy(offset, 0);
	   ssd1306_putchar(0x91);
	   ssd1306_putchar(0x89);
	   ssd1306_putchar(0x94);
	   ssd1306_goto_xy(offset, 1);
	   ssd1306_putchar(0x8E);
	   ssd1306_putchar(0x9A);
	   ssd1306_putchar(0x8F);
	   ssd1306_goto_xy(offset, 2);
	   ssd1306_putchar(0x99);
	   ssd1306_putchar(0x99);
	   ssd1306_putchar(0x90);
	   ssd1306_goto_xy(offset, 3);
	   ssd1306_putchar(0x8A);
	   ssd1306_putchar(0x8A);
	   ssd1306_putchar(0x93);
	   break;
	   
	   default:
	   ssd1306_goto_xy(offset, 0);
	   ssd1306_putchar(0x91);
	   ssd1306_putchar(0x89);
	   ssd1306_putchar(0x94);
	   ssd1306_goto_xy(offset, 1);
	   ssd1306_putchar(0x8B);
	   ssd1306_putchar(0xA0);
	   ssd1306_putchar(0x8C);
	   ssd1306_goto_xy(offset, 2);
	   ssd1306_putchar(0x8B);
	   ssd1306_putchar(0xA0);
	   ssd1306_putchar(0x8C);
	   ssd1306_goto_xy(offset, 3);
	   ssd1306_putchar(0x92);
	   ssd1306_putchar(0x8A);
	   ssd1306_putchar(0x93);
	   break;
	}
}

void clock_dot_flash(void)
{
	if (clock_dot_flag == 0)
	{
		ssd1306_goto_xy(8,0);
		ssd1306_putchar(0x20);
		ssd1306_goto_xy(8,1);
		ssd1306_putchar(0x95);
		ssd1306_goto_xy(8,2);
		ssd1306_putchar(0x95);
		ssd1306_goto_xy(8,3);
		ssd1306_putchar(0x20);
		clock_dot_flag=1;	
	}
	else
	{
		ssd1306_goto_xy(8,0);
		ssd1306_putchar(0x20);
		ssd1306_goto_xy(8,1);
		ssd1306_putchar(0x20);
		ssd1306_goto_xy(8,2);
		ssd1306_putchar(0x20);
		ssd1306_goto_xy(8,3);
		ssd1306_putchar(0x20);
		clock_dot_flag=0;	
	}
}

void display_time(char h, char m)
{
	char hour[3],minute[3];
		if (h!=global_hour)
		{
			global_hour=h;
			itoa(h,hour,10);
			if (h<10)
			{
				show_digit('-',0);
				show_digit(hour[0],4);
			} 
			else
			{
				show_digit(hour[0],0);
				show_digit(hour[1],4);
			}
		} 
		
		if (m!=global_minute)
		{
			global_minute=m;
			itoa(m,minute,10);
			if (m<10)
			{
				show_digit('0',10);
				show_digit(minute[0],14);
			} 
			else
			{
				show_digit(minute[0],10);
				show_digit(minute[1],14);
			}
		}
}

unsigned char set_light_max_bright(unsigned char value, char feature[20], char svet[20], char item)
{
	//Яркость света
	ssd1306_clear();
	ssd1306_goto_xy(0, 0);
	send_string(svet);
	ssd1306_goto_xy(0, 2);
	send_string(feature);
	progressbar_progress((value*100)/255,3);
	set_PWM_level(item,value);
	while (!check_button_menu())
	{
		if (check_button_up())
		{
			if (value<255){value++;}
			progressbar_progress((value*100)/255,3);
			set_PWM_level(item,value);
		}
		
		if (check_button_down())
		{
			if (value>0){value--;}
			progressbar_progress((value*100)/255,3);
			set_PWM_level(item,value);
		}
		_delay_ms(50);
	}
	return value;
}

char set_time_h(char h, char m, char svet[20], char second_string[20])
{
//Установка часов
		char hour[2],minute[2];
		
		ssd1306_clear();
		ssd1306_goto_xy(0, 0);
		send_string(svet); //цвет света
		ssd1306_goto_xy(0, 1);
		send_string(second_string); //Время включения
		
		ssd1306_goto_xy(0, 2);
		send_string("                     ");
		ssd1306_goto_xy(0, 2);
		if (h<10){send_string("0");}
		itoa(h,hour,10);
		send_string(hour);
		send_string(":");
		if (m<10){send_string("0");}
		itoa(m,minute,10);
		send_string(minute);
		
		//Подчеркивание цифр
		ssd1306_goto_xy(0, 3);
		ssd1306_putchar(0x99);
		ssd1306_putchar(0x99);
				
		while (!check_button_menu())
		{
			if (check_button_up())
			{
				h++;
				if (h>23){h=0;}
				ssd1306_goto_xy(0, 2);
				if (h<10){send_string("0");}
				itoa(h,hour,10);
				send_string(hour);
			}
			
			if (check_button_down())
			{
				if (h==0){h=24;}
				h--;
				ssd1306_goto_xy(0, 2);
				if (h<10){send_string("0");}
				itoa(h,hour,10);
				send_string(hour);
			}
			_delay_ms(50);
		}
		return h;
}

char set_time_m(char h, char m, char svet[20], char second_string[20])
{
//Установка минут
		char hour[2],minute[2];
	
		ssd1306_clear();
		ssd1306_goto_xy(0, 0);
		send_string(svet); //цвет света
		ssd1306_goto_xy(0, 1);
		send_string(second_string); //Время включения
		
		ssd1306_goto_xy(0, 2);
		send_string("                     ");
		ssd1306_goto_xy(0, 2);
		if (h<10){send_string("0");}
		itoa(h,hour,10);
		send_string(hour);
		send_string(":");
		if (m<10){send_string("0");}
		itoa(m,minute,10);
		send_string(minute);	

		//Подчеркивание цифр
		ssd1306_goto_xy(3, 3);
		ssd1306_putchar(0x99);
		ssd1306_putchar(0x99);

		while (!check_button_menu())
		{
			if (check_button_up())
			{
				m++;
				if (m>59){m=0;}
				
				ssd1306_goto_xy(3, 2);
				if (m<10){send_string("0");}
				itoa(m,minute,10);
				send_string(minute);
			}
			
			if (check_button_down())
			{
				
				if (m==0){m=60;}
				m--;
				ssd1306_goto_xy(3, 2);
				if (m<10){send_string("0");}
				itoa(m,minute,10);
				send_string(minute);
			}
			_delay_ms(50);
		}
		return m;
}

unsigned short int set_rising_time(unsigned short int rising_time, char svet[20], unsigned short int max_time)
{
//Установка продолжительности рассвета/заката в минутах
	char rising[5];
	
	ssd1306_clear();
	ssd1306_goto_xy(0, 0);
	send_string(svet);
	ssd1306_goto_xy(0, 1);
	send_string("Длительность расс/зак");
	ssd1306_goto_xy(0, 2);
	send_string("Минуты: ");
	ssd1306_goto_xy(8, 2);
	if (rising_time<100){send_string("0");}
	if (rising_time<10){send_string("0");}
	itoa(rising_time,rising,10);
	send_string(rising);

	//Подчеркивание цифр
	ssd1306_goto_xy(8, 3);
	ssd1306_putchar(0x99);
	ssd1306_putchar(0x99);
	ssd1306_putchar(0x99);
	
	
	while (!check_button_menu())
	{
		if (check_button_up())
		{
			rising_time++;
			if (rising_time>max_time){rising_time=0;}
			ssd1306_goto_xy(8, 2);
			if (rising_time<100){send_string("0");}
			if (rising_time<10){send_string("0");}
			itoa(rising_time,rising,10);
			send_string(rising);
		}
		
		if (check_button_down())
		{
			if (rising_time==0){rising_time=max_time+1;}
			rising_time--;
			ssd1306_goto_xy(8, 2);
			if (rising_time<100){send_string("0");}
			if (rising_time<10){send_string("0");}
			itoa(rising_time,rising,10);
			send_string(rising);
		}
		_delay_ms(50);
	}
	return rising_time;
}

void menu()
{
	unsigned char value;
	char h,m;
	unsigned short int rising_time, max_time, time_start, time_end;

// Выключение света, вентилятора и CO2
set_PWM_level('w',0);
set_PWM_level('r',0);
set_PWM_level('g',0);
set_PWM_level('b',0);
set_PWM_level('f',0);
CO2_close;

//================================================================
// Настройка часов
//================================================================
	i2c_restart();
	h=DS1307_get_hours();
	i2c_restart();
	m=DS1307_get_minutes();
	i2c_restart();
	h=set_time_h(h,m,"Системное время"," ");
	m=DS1307_get_minutes();
	i2c_restart();
	m=set_time_m(h,m,"Системное время"," ");
	DS1307_set_hours(h);
	i2c_restart();
	DS1307_set_minutes(m);
	i2c_restart();
	DS1307_set_seconds(0);
	i2c_restart();
//================================================================
// Настройка белого света
//================================================================	
	//Яркость белого света
	eeprom_busy_wait();
	value = eeprom_read_byte(&white_bright);
	value=set_light_max_bright(value,"Максимальная яркость","Белый",'w');
	set_PWM_level('w',0);
	eeprom_busy_wait();
	eeprom_write_byte (&white_bright, value);
	if (value>=0){
		//Время включения белого света
		eeprom_busy_wait();
		h = eeprom_read_byte(&white_time_h_start);
		eeprom_busy_wait();
		m = eeprom_read_byte(&white_time_m_start);
		if (h>23){h=0;}
		if (m>59){m=0;}
		h=set_time_h(h,m,"Белый свет","Время включения");
		eeprom_busy_wait();
		eeprom_write_byte(&white_time_h_start,h);
		m=set_time_m(h,m,"Белый свет","Время включения");
		eeprom_busy_wait();
		eeprom_write_byte(&white_time_m_start,m);
	
		//Время выключения белого света
		eeprom_busy_wait();
		h = eeprom_read_byte(&white_time_h_end);
		eeprom_busy_wait();
		m = eeprom_read_byte(&white_time_m_end);
		if (h>23){h=0;}
		if (m>59){m=0;}
		h=set_time_h(h,m,"Белый свет","Время выключения");
		eeprom_busy_wait();
		eeprom_write_byte(&white_time_h_end,h);
		m=set_time_m(h,m,"Белый свет","Время выключения");
		eeprom_busy_wait();
		eeprom_write_byte(&white_time_m_end,m);
	
		//Установка длительности заката/рассвета белого света
		eeprom_busy_wait();
		rising_time = eeprom_read_word(&white_rising_time);

		eeprom_busy_wait();
		time_start = (eeprom_read_byte(&white_time_h_start) * 60) + eeprom_read_byte(&white_time_m_start); 
		time_end = (eeprom_read_byte(&white_time_h_end) * 60) + eeprom_read_byte(&white_time_m_end);
		
		if (time_start <= time_end) {max_time = (time_end - time_start) / 2;} else {max_time = ((1440 - time_start) + time_end) / 2;}
		if ((rising_time<0)||(rising_time>720)){rising_time=0;}
		rising_time=set_rising_time(rising_time,"Белый свет",max_time);
		eeprom_busy_wait();
		eeprom_write_word(&white_rising_time, rising_time);
	}
	
//================================================================
// Настройка красного света
//================================================================
	//Яркость красного света
	eeprom_busy_wait();
	value = eeprom_read_byte(&red_bright);
	value=set_light_max_bright(value,"Максимальная яркость","Красный",'r');
	set_PWM_level('r',0);
	eeprom_busy_wait();
	eeprom_write_byte (&red_bright, value);
	if (value>=0){

		//Время включения красного света
		eeprom_busy_wait();
		h = eeprom_read_byte(&red_time_h_start);
		eeprom_busy_wait();
		m = eeprom_read_byte(&red_time_m_start);
		if (h>23){h=0;}
		if (m>59){m=0;}
		h=set_time_h(h,m,"Красный свет","Время включения");
		eeprom_busy_wait();
		eeprom_write_byte(&red_time_h_start,h);
		m=set_time_m(h,m,"Красный свет","Время включения");

		eeprom_busy_wait();
		eeprom_write_byte(&red_time_m_start,m);
		
		//Время выключения красного света
		eeprom_busy_wait();
		h = eeprom_read_byte(&red_time_h_end);
		eeprom_busy_wait();
		m = eeprom_read_byte(&red_time_m_end);
		if (h>23){h=0;}
		if (m>59){m=0;}
		h=set_time_h(h,m,"Красный свет","Время выключения");
		eeprom_busy_wait();
		eeprom_write_byte(&red_time_h_end,h);
		m=set_time_m(h,m,"Красный свет","Время выключения");
		eeprom_busy_wait();
		eeprom_write_byte(&red_time_m_end,m);
		
		//Установка длительности заката/рассвета красного света
		eeprom_busy_wait();
		rising_time = eeprom_read_word(&red_rising_time);
		if ((rising_time<0)||(rising_time>1440)){rising_time=0;}
		rising_time=set_rising_time(rising_time,"Красный свет",10);
		eeprom_busy_wait();
		eeprom_write_word(&red_rising_time, rising_time);
	}

//================================================================
// Настройка зелёного света
//================================================================
	//Яркость зелёного света
	eeprom_busy_wait();
	value = eeprom_read_byte(&green_bright);
	value=set_light_max_bright(value,"Максимальная яркость","Зелёный",'g');
	set_PWM_level('g',0);
	eeprom_busy_wait();
	eeprom_write_byte (&green_bright, value);
	if (value>=0){

		//Время включения зелёного света
		eeprom_busy_wait();
		h = eeprom_read_byte(&green_time_h_start);
		eeprom_busy_wait();
		m = eeprom_read_byte(&green_time_m_start);
		if (h>23){h=0;}
		if (m>59){m=0;}
		h=set_time_h(h,m,"Зелёный свет","Время включения");
		eeprom_busy_wait();
		eeprom_write_byte(&green_time_h_start,h);
		m=set_time_m(h,m,"Зелёный свет","Время включения");
		eeprom_busy_wait();
		eeprom_write_byte(&green_time_m_start,m);
		
		//Время выключения зелёного света
		eeprom_busy_wait();
		h = eeprom_read_byte(&green_time_h_end);
		eeprom_busy_wait();
		m = eeprom_read_byte(&green_time_m_end);
		if (h>23){h=0;}
		if (m>59){m=0;}
		h=set_time_h(h,m,"Зелёный свет","Время выключения");
		eeprom_busy_wait();
		eeprom_write_byte(&green_time_h_end,h);
		m=set_time_m(h,m,"Зелёный свет","Время выключения");
		eeprom_busy_wait();
		eeprom_write_byte(&green_time_m_end,m);
		
		//Установка длительности заката/рассвета зелёного света
		eeprom_busy_wait();
		rising_time = eeprom_read_word(&green_rising_time);
		if ((rising_time<0)||(rising_time>1440)){rising_time=0;}
		rising_time=set_rising_time(rising_time,"Зелёный свет",5);
		eeprom_busy_wait();
		eeprom_write_word(&green_rising_time, rising_time);
	}

//================================================================
// Настройка синего света
//================================================================
	//Яркость синего света
	eeprom_busy_wait();
	value = eeprom_read_byte(&blue_bright);
	value=set_light_max_bright(value,"Максимальная яркость","Синий",'b');
	set_PWM_level('b',0);
	eeprom_busy_wait();
	eeprom_write_byte (&blue_bright, value);
	if (value>=0){

		//Время включения синего света
		eeprom_busy_wait();
		h = eeprom_read_byte(&blue_time_h_start);
		eeprom_busy_wait();
		m = eeprom_read_byte(&blue_time_m_start);
		if (h>23){h=0;}
		if (m>59){m=0;}
		h=set_time_h(h,m,"Синий свет","Время включения");
		eeprom_busy_wait();
		eeprom_write_byte(&blue_time_h_start,h);
		m=set_time_m(h,m,"Синий свет","Время включения");
		eeprom_busy_wait();
		eeprom_write_byte(&blue_time_m_start,m);
		
		//Время выключения синего света
		eeprom_busy_wait();
		h = eeprom_read_byte(&blue_time_h_end);
		eeprom_busy_wait();
		m = eeprom_read_byte(&blue_time_m_end);
		if (h>23){h=0;}
		if (m>59){m=0;}
		h=set_time_h(h,m,"Синий свет","Время выключения");
		eeprom_busy_wait();
		eeprom_write_byte(&blue_time_h_end,h);
		m=set_time_m(h,m,"Синий свет","Время выключения");
		eeprom_busy_wait();
		eeprom_write_byte(&blue_time_m_end,m);
		
		//Установка длительности заката/рассвета синего света
		eeprom_busy_wait();
		rising_time = eeprom_read_word(&blue_rising_time);
		if ((rising_time<0)||(rising_time>1440)){rising_time=0;}
		rising_time=set_rising_time(rising_time,"Синий свет",15);
		eeprom_busy_wait();
		eeprom_write_word(&blue_rising_time, rising_time);
	}

//================================================================
// Настройка вентилятора охлаждения
//================================================================
	//Минимальная скорость вращения вентилятора
	eeprom_busy_wait();
	value = eeprom_read_byte(&fan_min_speed);
	value=set_light_max_bright(value,"Мин. скорость","Вентилятор",'f');
	set_PWM_level('f',0);
	eeprom_busy_wait();
	eeprom_write_byte (&fan_min_speed, value);

	//Максимальная скорость вращения вентилятора
	eeprom_busy_wait();
	value = eeprom_read_byte(&fan_max_speed);
	value=set_light_max_bright(value,"Макс. скорость","Вентилятор",'f');
	set_PWM_level('f',0);
	eeprom_busy_wait();
	eeprom_write_byte (&fan_max_speed, value);
	
//================================================================
// Настройка подачи CO2
//================================================================	
	//Время включения подачи CO2
	eeprom_busy_wait();
	h = eeprom_read_byte(&CO2_time_h_start);
	eeprom_busy_wait();
	m = eeprom_read_byte(&CO2_time_m_start);
	if (h>23){h=0;}
	if (m>59){m=0;}
	h=set_time_h(h,m,"Подача CO2","Время включения");
	eeprom_busy_wait();
	eeprom_write_byte(&CO2_time_h_start,h);
	m=set_time_m(h,m,"Подача CO2","Время включения");
	eeprom_busy_wait();
	eeprom_write_byte(&CO2_time_m_start,m);
		
	//Время выключения подачи CO2
	eeprom_busy_wait();
	h = eeprom_read_byte(&CO2_time_h_end);
	eeprom_busy_wait();
	m = eeprom_read_byte(&CO2_time_m_end);
	if (h>23){h=0;}
	if (m>59){m=0;}
	h=set_time_h(h,m,"Подача CO2","Время выключения");
	eeprom_busy_wait();
	eeprom_write_byte(&CO2_time_h_end,h);
	m=set_time_m(h,m,"Подача CO2","Время выключения");
	eeprom_busy_wait();
	eeprom_write_byte(&CO2_time_m_end,m);	
	
//================================================================
	
	//Выход из меню
	ssd1306_clear();
	global_hour=0;
	global_minute=0;	
		
	show_digit('0',0);
	show_digit('0',4);
	show_digit('0',10);
	show_digit('0',14);	
}

void port_init()
{
	 // Настройка портов контроллера
	 DDRD=0b01110000;  //Делаем порт D, как выход, чтобы на выходах порта было напряжение 5В
	 PORTD=0x00; //Выставляем выходы порта D на 0
	 DDRC=0b00000000;  //Делаем порт C, как выход, чтобы на выходах порта было напряжение 5В
	 PORTC=0b00001110; //Выставляем выходы порта C на 0
	 DDRB=0b00001111;  //Делаем порт B, как вход, чтобы на выходах порта было напряжение 5В
	 PORTB=0x00; //Выставляем выходы порта D на 0
}

void init_pwm (void)
{
White_light_level = 0;
Blue_light_level = 0;

ICR1 = 0xFF;
// set TOP to 16bit

OCR1A = 0x3F;
// set PWM for 25% duty cycle @ 16bit

OCR1B = 0x1F;
// set PWM for 75% duty cycle @ 16bit

TCCR1A |= (1 << COM1A1)|(1 << COM1B1);
// set none-inverting mode

TCCR1A |= (1 << WGM10);
TCCR1B |= (1 << WGM12);
// set Fast PWM mode using ICR1 as TOP

TCCR1B |= (1 << CS11);
// START the timer with no prescaler	

//#define Q3 OCR2A
Red_light_level=0;

TCCR2A |= (1 << COM2A1);
// set none-inverting mode

TCCR2A |= (1 << WGM21) | (1 << WGM20);
// set fast PWM Mode

TCCR2B |= (1 << CS21);
// set prescaler to 8 and starts PWM
	
Green_light_level = 0;
Fan_speed = 0;
TCCR0A |= (1 << COM0A1);
TCCR0A |= (1 << COM0B1);
// set none-inverting mode
	
TCCR0A |= (1 << WGM01) | (1 << WGM00);
// set fast PWM Mode

TCCR0B |= (1 << CS01);
// set prescaler to 8 and starts PWM

set_PWM_level('r',0);
set_PWM_level('g',0);
set_PWM_level('b',0);
set_PWM_level('w',0);
set_PWM_level('f',0);
}

void set_PWM_level(char item, char volume)
{
	switch (item)
	{
		case 'r':
		if (Red_light_level != 0 && volume == 0){DDRB &= ~(1<<3);} 
		if (Red_light_level == 0 && volume != 0){DDRB |= 1<<3;}
		Red_light_level = volume;			
		break;
		
		case 'g':
		if (Green_light_level != 0 && volume == 0){DDRD &= ~(1<<6);}
		if (Green_light_level == 0 && volume != 0){DDRD |= 1<<6;}
		Green_light_level = volume;
		break;

		case 'b':
		if (Blue_light_level != 0 && volume == 0){DDRB &= ~(1<<2);}
		if (Blue_light_level == 0 && volume != 0){DDRB |= 1<<2;}
		Blue_light_level = volume;
		break;
		
		case 'w':
		if (White_light_level != 0 && volume == 0){DDRB &= ~(1<<1);}
		if (White_light_level == 0 && volume != 0){DDRB |= 1<<1;}
		White_light_level = volume;
		break;
		
		case 'f':
		if (Fan_speed != 0 && volume == 0){DDRD &= ~(1<<5);}
		if (Fan_speed == 0 && volume != 0){DDRD |= 1<<5;}
		Fan_speed = volume;
		break;
	}
}

void check_jobs(char h, char m)
{
short start_time,end_time,current_time;
unsigned short rising_time; 
current_time=h*60+m;

//Включение/выключение Белого света
start_time = (eeprom_read_byte(&white_time_h_start) * 60) + eeprom_read_byte(&white_time_m_start);
end_time = (eeprom_read_byte(&white_time_h_end) * 60) + eeprom_read_byte(&white_time_m_end);
rising_time = eeprom_read_word(&white_rising_time);

if (start_time < end_time) 
{
	if (current_time >= start_time && current_time < end_time)
	{
		if ((current_time - start_time) < rising_time && (current_time - start_time) >= 0 && rising_time > 0)
		{
			set_PWM_level('w',(eeprom_read_byte(&white_bright) * ((current_time - start_time) + 1)) / rising_time);
		}
		else
		{
			if ((end_time - current_time) < rising_time && (end_time - current_time) >= 0 && rising_time > 0)
			{
				set_PWM_level('w',(eeprom_read_byte(&white_bright) * (end_time - current_time)) / rising_time);
			}
			else
			{
				set_PWM_level('w',eeprom_read_byte(&white_bright));
			}
		}
	}
	else {set_PWM_level('w',0);}
}

if (start_time > end_time)
{	
	if (current_time >= start_time || current_time < end_time)
	{
		if ((current_time - start_time) < rising_time && (current_time - start_time) >= 0 && rising_time > 0)
		{
			set_PWM_level('w',(eeprom_read_byte(&white_bright) * ((current_time - start_time) + 1)) / rising_time);
		}
		else
		{
			if ((end_time - current_time) < rising_time && (end_time - current_time) >= 0 && rising_time > 0)
			{
				set_PWM_level('w',(eeprom_read_byte(&white_bright) * (end_time - current_time)) / rising_time);
			}
			else
			{
				set_PWM_level('w',eeprom_read_byte(&white_bright));
			}
		}
	}
	else {set_PWM_level('w',0);}	
}

if (start_time == end_time)
{
	set_PWM_level('w',0);
}


//Включение/выключение Красного света
start_time = (eeprom_read_byte(&red_time_h_start) * 60) + eeprom_read_byte(&red_time_m_start);
end_time = (eeprom_read_byte(&red_time_h_end) * 60) + eeprom_read_byte(&red_time_m_end);
rising_time = eeprom_read_word(&red_rising_time);

if (start_time < end_time) 
{
	if (current_time >= start_time && current_time < end_time)
	{
		if ((current_time - start_time) < rising_time && (current_time - start_time) >= 0 && rising_time > 0)
		{
			set_PWM_level('r',(eeprom_read_byte(&red_bright) * ((current_time - start_time) + 1)) / rising_time);
		}
		else
		{
			if ((end_time - current_time) < rising_time && (end_time - current_time) >= 0 && rising_time > 0)
			{
				set_PWM_level('r',(eeprom_read_byte(&red_bright) * (end_time - current_time)) / rising_time);
			}
			else
			{
				set_PWM_level('r',eeprom_read_byte(&red_bright));
			}
		}
	}
	else {set_PWM_level('r',0);}
}

if (start_time > end_time)
{	
	if (current_time >= start_time || current_time < end_time)
	{
		if ((current_time - start_time) < rising_time && (current_time - start_time) >= 0 && rising_time > 0)
		{
			set_PWM_level('r',(eeprom_read_byte(&red_bright) * ((current_time - start_time) + 1)) / rising_time);
		}
		else
		{
			if ((end_time - current_time) < rising_time && (end_time - current_time) >= 0 && rising_time > 0)
			{
				set_PWM_level('r',(eeprom_read_byte(&red_bright) * (end_time - current_time)) / rising_time);
			}
			else
			{
				set_PWM_level('r',eeprom_read_byte(&red_bright));
			}
		}
	}
	else {set_PWM_level('r',0);}	
}

if (start_time == end_time)
{
	set_PWM_level('r',0);
}


//Включение/выключение Зелёного света
start_time = (eeprom_read_byte(&green_time_h_start) * 60) + eeprom_read_byte(&green_time_m_start);
end_time = (eeprom_read_byte(&green_time_h_end) * 60) + eeprom_read_byte(&green_time_m_end);
rising_time = eeprom_read_word(&green_rising_time);

if (start_time < end_time) 
{
	if (current_time >= start_time && current_time < end_time)
	{
		if ((current_time - start_time) < rising_time && (current_time - start_time) >= 0 && rising_time > 0)
		{
			set_PWM_level('g',(eeprom_read_byte(&green_bright) * ((current_time - start_time) + 1)) / rising_time);
		}
		else
		{
			if ((end_time - current_time) < rising_time && (end_time - current_time) >= 0 && rising_time > 0)
			{
				set_PWM_level('g',(eeprom_read_byte(&green_bright) * (end_time - current_time)) / rising_time);
			}
			else
			{
				set_PWM_level('g',eeprom_read_byte(&green_bright));
			}
		}
	}
	else {set_PWM_level('g',0);}
}

if (start_time > end_time)
{	
	if (current_time >= start_time || current_time < end_time)
	{
		if ((current_time - start_time) < rising_time && (current_time - start_time) >= 0 && rising_time > 0)
		{
			set_PWM_level('g',(eeprom_read_byte(&green_bright) * ((current_time - start_time) + 1)) / rising_time);
		}
		else
		{
			if ((end_time - current_time) < rising_time && (end_time - current_time) >= 0 && rising_time > 0)
			{
				set_PWM_level('g',(eeprom_read_byte(&green_bright) * (end_time - current_time)) / rising_time);
			}
			else
			{
				set_PWM_level('g',eeprom_read_byte(&green_bright));
			}
		}
	}
	else {set_PWM_level('g',0);}	
}

if (start_time == end_time)
{
	set_PWM_level('g',0);
}


//Включение/выключение Синего света
start_time = (eeprom_read_byte(&blue_time_h_start) * 60) + eeprom_read_byte(&blue_time_m_start);
end_time = (eeprom_read_byte(&blue_time_h_end) * 60) + eeprom_read_byte(&blue_time_m_end);
rising_time = eeprom_read_word(&blue_rising_time);

if (start_time < end_time) 
{
	if (current_time >= start_time && current_time < end_time)
	{
		if ((current_time - start_time) < rising_time && (current_time - start_time) >= 0 && rising_time > 0)
		{
			set_PWM_level('b',(eeprom_read_byte(&blue_bright) * ((current_time - start_time) + 1)) / rising_time);
		}
		else
		{
			if ((end_time - current_time) < rising_time && (end_time - current_time) >= 0 && rising_time > 0)
			{
				set_PWM_level('b',(eeprom_read_byte(&blue_bright) * (end_time - current_time)) / rising_time);
			}
			else
			{
				set_PWM_level('b',eeprom_read_byte(&blue_bright));
			}
		}
	}
	else {set_PWM_level('b',0);}
}

if (start_time > end_time)
{	
	if (current_time >= start_time || current_time < end_time)
	{
		if ((current_time - start_time) < rising_time && (current_time - start_time) >= 0 && rising_time > 0)
		{
			set_PWM_level('b',(eeprom_read_byte(&blue_bright) * ((current_time - start_time) + 1)) / rising_time);
		}
		else
		{
			if ((end_time - current_time) < rising_time && (end_time - current_time) >= 0 && rising_time > 0)
			{
				set_PWM_level('b',(eeprom_read_byte(&blue_bright) * (end_time - current_time)) / rising_time);
			}
			else
			{
				set_PWM_level('b',eeprom_read_byte(&blue_bright));
			}
		}
	}
	else {set_PWM_level('b',0);}	
}

if (start_time == end_time)
{
	set_PWM_level('b',0);
}


//Включение/выключение вентилятора
if (White_light_level > 0 || Red_light_level > 0 || Green_light_level > 0 || Blue_light_level > 0)
	{
		set_PWM_level('f',eeprom_read_byte(&fan_max_speed));
	}
	else 
	{
		set_PWM_level('f',0);
	}

//Включение/выключение CO2
start_time = (eeprom_read_byte(&CO2_time_h_start) * 60) + eeprom_read_byte(&CO2_time_m_start);
end_time = (eeprom_read_byte(&CO2_time_h_end) * 60) + eeprom_read_byte(&CO2_time_m_end);

if (start_time < end_time && current_time >= start_time && current_time < end_time)
{
	CO2_open;
	ssd1306_goto_xy(18, 3);
	send_string("CO2");
} 
else
{
	if ((start_time > end_time) && (current_time >= start_time || current_time < end_time))
	{
		CO2_open;
		ssd1306_goto_xy(18, 3);
		send_string("CO2");
	}
	else
	{
		CO2_close;
		ssd1306_goto_xy(18, 3);
		send_string("   ");
	}
}
}

int main(void)
{
char h, m, temp[3];

global_temperature=0;
port_init();
init_pwm();
_delay_ms(200);
ssd1306_init();
_delay_ms(2000);
DS1307_init();

show_digit('-',0);
show_digit('0',4);
show_digit('0',10);
show_digit('0',14);

while (1)
{
	i2c_restart();
	h=DS1307_get_hours();		
	i2c_restart();
	m=DS1307_get_minutes();

	display_time(h,m);
	clock_dot_flash();

	check_jobs(h,m); //Проверка заданий включения/выключения
	if (check_button_menu()){menu();} //Проверка нажатия кнопки menu
	
	
	itoa(get_temp(),temp,10); //получение температуры и перевод в текст
	if (temp!=global_temperature)
	{	
		ssd1306_goto_xy(18, 0);
		send_string("   ");
		ssd1306_goto_xy(18, 0);
		send_string(temp);
		global_temperature=temp;
		ssd1306_putchar(0xB0); //Значок градуса
	}
}
}

