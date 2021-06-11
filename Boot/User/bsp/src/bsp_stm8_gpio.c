/*
*********************************************************************************************************
*
*	ģ������ : I2C GPIOģ������ģ��
*	�ļ����� : bsp_stm8_gpio.c
*	��    �� : V1.0
*	˵    �� : STM8S i2c GPIOģ��
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		v1.0    2019-04-27 armfly  
*
*	Copyright (C), 2016-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/

/*
	��ֲ����
	1. ����PWM����ĺ�����Ҫ���ġ�  LCD_SetPwmBackLight
	2. FSMC���õ�Ƭѡ��Ҫ����	LCD_FSMCConfig
*/

#include "bsp.h"

static uint8_t I2C_ReadBytes(uint8_t *_pReadBuf, uint16_t _usAddress, uint16_t _usSize);
static uint8_t I2C_WriteBytes(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize);

static uint8_t s_stms_out[2];
static uint16_t s_stms_pwm[2];

/*
*********************************************************************************************************
*	�� �� ��: STM8_InitHard
*	����˵��: ��ʼ��Ӳ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t STM8_InitHard(void)
{
	uint8_t buf[8];
	uint8_t i;
	
	for (i = 0; i < 10; i++)
	{
		memset(buf, 0x55, 8);
		I2C_ReadBytes(buf, REG_ID, 8);
		
		if (buf[0] == 0x75)
		{
			break;
		}
		bsp_DelayUS(10 * 1000);
	}
	
//	/* ����GPIOΪ����� */
//	buf[0] = 0xFF;
//	buf[1] = 0;
//	I2C_WriteBytes(buf, REG_CFG1, 2);
	
//	s_stms_out[0] = 0x12;
//	s_stms_out[1] = 0x34;
//	I2C_WriteBytes(s_stms_out, REG_OUT1, 2);
	return 1;
}

/*
*********************************************************************************************************
*	�� �� ��: STM8_WriteGPIO
*	����˵��: ����GPIO���״̬
*	��    ��: _pin : 0-7, 8-13
*			  _value : 0,1
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void STM8_WriteGPIO(uint8_t _pin, uint8_t _value)
{
	if (_pin < 8)
	{
		if (_value == 1)
		{
			s_stms_out[0] |= (1 << _pin);
		}
		else
		{
			s_stms_out[0] &= (~(1 << _pin));
		}
	}
	else
	{
		if (_value == 1)
		{
			s_stms_out[1] |= (1 << (_pin - 8));
		}
		else
		{
			s_stms_out[1] &= (~(1 << (_pin - 8)));
		}
	}
	I2C_WriteBytes(s_stms_out, REG_OUT1, 2);
}


/*
*********************************************************************************************************
*	�� �� ��: STM8_WriteGPIO
*	����˵��: ����GPIO���״̬
*	��    ��: _ch : ͨ��1,2
*			  _value : PWMֵ (0-4095)
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void STM8_WritePWM(uint8_t _ch, uint16_t _value)
{
	uint8_t buf[2];
	
	if (_ch == 1)
	{
		s_stms_pwm[0] = _value;
		buf[0] = _value >> 8;
		buf[1] = _value;
		I2C_WriteBytes(buf, REG_PWM1_H, 2);
	}
	else
	{
		s_stms_pwm[1] = _value;
		buf[0] = _value >> 8;
		buf[1] = _value;
		I2C_WriteBytes(buf, REG_PWM2_H, 2);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: I2C_ReadBytes
*	����˵��: �Ӵӻ�ָ����ַ����ʼ��ȡ��������
*	��    ��:  _usAddress : ��ʼ��ַ
*			 _usSize : ���ݳ��ȣ���λΪ�ֽ�
*			 _pReadBuf : ��Ŷ��������ݵĻ�����ָ��
*	�� �� ֵ: 0 ��ʾʧ�ܣ�1��ʾ�ɹ�
*********************************************************************************************************
*/
static uint8_t I2C_ReadBytes(uint8_t *_pReadBuf, uint16_t _usAddress, uint16_t _usSize)
{
	uint16_t i;

	/* ���ô���EEPROM�漴��ȡָ�����У�������ȡ�����ֽ� */

	/* ��1��������I2C���������ź� */
	i2c_Start();
	
	/* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
	i2c_SendByte(I2C_DEV_ADDR | I2C_WR);	/* �˴���дָ�� */

	/* ��3��������ACK */
	if (i2c_WaitAck() != 0)
	{
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}

	/* ��4���������ֽڵ�ַ��24C02ֻ��256�ֽڣ����1���ֽھ͹��ˣ������24C04���ϣ���ô�˴���Ҫ���������ַ */
	if (I2C_ADDR_BYTES == 1)
	{
		i2c_SendByte((uint8_t)_usAddress);
		if (i2c_WaitAck() != 0)
		{
			goto cmd_fail;	/* EEPROM������Ӧ�� */
		}
	}
	else
	{
		i2c_SendByte(_usAddress >> 8);
		if (i2c_WaitAck() != 0)
		{
			goto cmd_fail;	/* EEPROM������Ӧ�� */
		}

		i2c_SendByte(_usAddress);
		if (i2c_WaitAck() != 0)
		{
			goto cmd_fail;	/* EEPROM������Ӧ�� */
		}
	}

	/* ��6������������I2C���ߡ����濪ʼ��ȡ���� */
	i2c_Start();

	/* ��7������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
	i2c_SendByte(I2C_DEV_ADDR | I2C_RD);	/* �˴��Ƕ�ָ�� */

	/* ��8��������ACK */
	if (i2c_WaitAck() != 0)
	{
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}

	/* ��9����ѭ����ȡ���� */
	for (i = 0; i < _usSize; i++)
	{
		_pReadBuf[i] = i2c_ReadByte();	/* ��1���ֽ� */

		/* ÿ����1���ֽں���Ҫ����Ack�� ���һ���ֽڲ���ҪAck����Nack */
		if (i != _usSize - 1)
		{
			i2c_Ack();	/* �м��ֽڶ����CPU����ACK�ź�(����SDA = 0) */
		}
		else
		{
			i2c_NAck();	/* ���1���ֽڶ����CPU����NACK�ź�(����SDA = 1) */
		}
	}
	/* ����I2C����ֹͣ�ź� */
	i2c_Stop();
	return 1;	/* ִ�гɹ� */

cmd_fail: /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
	/* ����I2C����ֹͣ�ź� */
	i2c_Stop();
	return 0;
}

/*
*********************************************************************************************************
*	�� �� ��: I2C_WriteBytes
*	����˵��: ��ӻ�ָ����ַд���������ݣ�����ҳд�������д��Ч��
*	��    ��:  _usAddress : ��ʼ��ַ
*			 _usSize : ���ݳ��ȣ���λΪ�ֽ�
*			 _pWriteBuf : ��Ŷ��������ݵĻ�����ָ��
*	�� �� ֵ: 0 ��ʾʧ�ܣ�1��ʾ�ɹ�
*********************************************************************************************************
*/
uint8_t I2C_WriteBytes(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize)
{
	uint16_t i,m;
	uint16_t usAddr;

	/*
		д����EEPROM�������������������ȡ�ܶ��ֽڣ�ÿ��д����ֻ����ͬһ��page��
		����24xx02��page size = 8
		�򵥵Ĵ�����Ϊ�����ֽ�д����ģʽ��ÿд1���ֽڣ������͵�ַ
		Ϊ���������д��Ч��: ����������page wirte������
	*/

	usAddr = _usAddress;
	for (i = 0; i < _usSize; i++)
	{
		/* �����͵�1���ֽڻ���ҳ���׵�ַʱ����Ҫ���·��������źź͵�ַ */
		if ((i == 0))
		{
			/*���ڣ�������ֹͣ�źţ������ڲ�д������*/
			i2c_Stop();

			/* ͨ���������Ӧ��ķ�ʽ���ж��ڲ�д�����Ƿ����, һ��С�� 10ms
				CLKƵ��Ϊ200KHzʱ����ѯ����Ϊ30������
			*/
			for (m = 0; m < 1000; m++)
			{
				/* ��1��������I2C���������ź� */
				i2c_Start();

				/* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
				i2c_SendByte(I2C_DEV_ADDR | I2C_WR);	/* �˴���дָ�� */

				/* ��3��������һ��ʱ�ӣ��ж������Ƿ���ȷӦ�� */
				if (i2c_WaitAck() == 0)
				{
					break;
				}
			}
			if (m  == 1000)
			{
				goto cmd_fail;	/* EEPROM����д��ʱ */
			}

			/* ��4���������ֽڵ�ַ��24C02ֻ��256�ֽڣ����1���ֽھ͹��ˣ������24C04���ϣ���ô�˴���Ҫ���������ַ */
			if (I2C_ADDR_BYTES == 1)
			{	
				i2c_SendByte((uint8_t)usAddr);
				if (i2c_WaitAck() != 0)
				{
					goto cmd_fail;	/* EEPROM������Ӧ�� */
				}
			}
			else
			{
				i2c_SendByte(usAddr >> 8);
				if (i2c_WaitAck() != 0)
				{
					goto cmd_fail;	/* EEPROM������Ӧ�� */
				}

				i2c_SendByte(usAddr);
				if (i2c_WaitAck() != 0)
				{
					goto cmd_fail;	/* EEPROM������Ӧ�� */
				}
			}
		}

		/* ��6������ʼд������ */
		i2c_SendByte(_pWriteBuf[i]);

		/* ��7��������ACK */
		if (i2c_WaitAck() != 0)
		{
			goto cmd_fail;	/* EEPROM������Ӧ�� */
		}

		usAddr++;	/* ��ַ��1 */
	}

	/* ����ִ�гɹ�������I2C����ֹͣ�ź� */
	i2c_Stop();
	return 1;

cmd_fail: /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
	/* ����I2C����ֹͣ�ź� */
	i2c_Stop();
	return 0;
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
