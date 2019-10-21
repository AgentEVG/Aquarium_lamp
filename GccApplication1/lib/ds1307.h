#include "i2c.h"

#define DS1307_ADR		0xD0 //0x68


// Передача команды часам
BYTE DS1307_send_com(BYTE com)
{
	BYTE ask = ACK;
	
	i2c_start();
	ask = i2c_send_byte(DS1307_ADR);
	ask = i2c_send_byte(0x00);
	ask = i2c_send_byte(com);
	ask = i2c_stop();
	
	return ask;
}


// Передача данных часам
BYTE DS1307_send_data(BYTE data)
{
	BYTE ask = ACK;
	
	i2c_start();
	ask = i2c_send_byte(DS1307_ADR);
	ask = i2c_send_byte(0x40);
	ask = i2c_send_byte(data);
	ask = i2c_stop();
	
	return ask;
}



// Инициализация часов
BYTE DS1307_init(void)
{
	
	BYTE ask = ACK;
	i2c_init();
	_delay_ms(100);
	i2c_start();
	ask = i2c_send_byte(DS1307_ADR);
	ask = i2c_send_byte(0x70);
	ask = i2c_send_byte(1);
	
	ask = i2c_send_byte(0x47);
	ask = i2c_send_byte(1);	
	ask = i2c_send_byte(0x17);
	ask = i2c_send_byte(0);
	ask = i2c_send_byte(0x07);
	ask = i2c_send_byte(0);	
	
	
	ask = i2c_send_byte(0x70);
	ask = i2c_send_byte(0);
	ask = i2c_send_byte(0x62);
	ask = i2c_send_byte(0);	
	ask = i2c_stop();
	return ask;
}


BYTE DS1307_set_hours(BYTE vol)
{
	vol=((vol/10)<<4)+vol%10; // Преобразование в формат BCD
	BYTE ask = ACK;
	i2c_start();
	ask = i2c_send_byte(DS1307_ADR);
	ask = i2c_send_byte(0x02);
	ask = i2c_send_byte(vol);
	ask = i2c_stop();

	return ask;
}

BYTE DS1307_set_minutes(BYTE vol)
{
	vol=((vol/10)<<4)+vol%10; // Преобразование в формат BCD
	BYTE ask = ACK;
	i2c_start();
	ask = i2c_send_byte(DS1307_ADR);
	ask = i2c_send_byte(0x01);
	ask = i2c_send_byte(vol);
	ask = i2c_stop();
	
	return ask;
}

BYTE DS1307_set_seconds(BYTE vol)
{
	vol=((vol/10)<<4)+vol%10; // Преобразование в формат BCD
	BYTE ask = ACK;
	i2c_start();
	ask = i2c_send_byte(DS1307_ADR);
	ask = i2c_send_byte(0x00);
	ask = i2c_send_byte(vol);
	ask = i2c_stop();

	return ask;
}

BYTE DS1307_get_seconds (void)
{
	BYTE ask = ACK;
	i2c_start();
	i2c_send_byte(DS1307_ADR);
	i2c_send_byte(0x00);
	i2c_stop();
	i2c_start();
	i2c_send_byte(0xD1);
	ask = i2c_read_byte(1);
	i2c_stop();
	ask=(((ask&0xF0)>>4)*10)+(ask&0x0F); // Преобразование из BCD формата
	return ask;
}

BYTE DS1307_get_minutes (void)
{
	BYTE ask = ACK;
	i2c_start();
	i2c_send_byte(DS1307_ADR);
	i2c_send_byte(0x01);
	i2c_stop();
	i2c_start();
	i2c_send_byte(0xD1);
	ask = i2c_read_byte(1);
	i2c_stop();
	ask=(((ask&0xF0)>>4)*10)+(ask&0x0F); // Преобразование из BCD формата
	return ask;	
}

BYTE DS1307_get_hours (void)
{
	BYTE ask = ACK;
	i2c_start();
	i2c_send_byte(DS1307_ADR);
	i2c_send_byte(0x02);
	i2c_stop();
	i2c_start();
	i2c_send_byte(0xD1);
	ask = i2c_read_byte(1);
	i2c_stop();
	ask=(((ask&0xF0)>>4)*10)+(ask&0x0F); // Преобразование из BCD формата
	return ask;
}