// ������������� DS18B20
unsigned char DS18B20_init(void)
{
	char OK_Flag;
	
	DDRD |= (1 << PD4); // PD4 - �����
	PORTD &= ~(1 << PD4); // ������������� ������ �������
	_delay_us(490);
	//_delay_us(245);
	DDRD &= ~(1 << PD4); // PD4 - ����
	_delay_us(68);
	//_delay_us(34);
	OK_Flag = (PIND & (1 << PD4)); // ����� ������� ����������� �������
	// ���� OK_Flag = 0 ������ ���������, OK_Flag = 1 ������ �� ���������
	_delay_us(422);
	return OK_Flag;
}

// ������� ������ ����� �� DS18B20
unsigned char DS18B20_read(void)
{
	unsigned char i, data = 0;
	
	for(i = 0; i < 8; i++)
	{
		DDRD |= (1 << PD4); // PD4 - �����
		_delay_us(2);
		DDRD &= ~(1 << PD4); // PD4 - ����
		_delay_us(4);
		data = data >> 1; // ��������� ���
		if(PIND & (1 << PD4)) data |= 0x80;
		_delay_us(62);
	}
	return data;
}

// ������� ������ ����� � DS18B20
void DS18B20_write(unsigned char data)
{
	unsigned char i;
	
	for(i = 0; i < 8; i++)
	{
		DDRD |= (1 << PD4); // PD4 - �����
		_delay_us(2);
		if(data & 0x01) DDRD &= ~(1 << PD4);
		else DDRD |= (1 << PD4);
		data = data >> 1; // ��������� ���
		_delay_us(62);
		DDRD &= ~(1 << PD4); // PD4 - ����
		_delay_us(2);
	}
}

unsigned char get_temp(void)
{
	unsigned char Temp_H, Temp_L, temperature;
	
	DS18B20_init();        // ������������� DS18B20
	DS18B20_write(0xCC);     // �������� ���� �������
	DS18B20_write(0x44);     // ������ �������������� ��������������
	_delay_ms(1000);       // �������� �� ����� �������
	DS18B20_init();        // ������������� DS18B20
	DS18B20_write(0xCC);     // �������� ���� �������
	DS18B20_write(0xBE);     // ��������� ���������� ���
	Temp_L = DS18B20_read(); // ������ ������ 2 ����� ��������
	Temp_H = DS18B20_read();
	temperature = ((Temp_H << 4) & 0x70)|(Temp_L >> 4);
	return temperature;
}