/*
*********************************************************************************************************
*
*	ģ������ : UDP Modbus �Ĵ�����ַ����
*	�ļ����� : udp_modbus_addr.h
*
*********************************************************************************************************
*/
#include "bsp.h"

#ifndef _UDP_MODBUS_ADDR_H
#define _UDP_MODBUS_ADDR_H

#define HARD_MODEL		0x0750
#define BOOT_VERSION	0x0100
#define APP_VERSION		0x0100

/* Ӳ������: DO,DI,AI,AO����
AI : ʾ���� CH1 ��ֵ, mV��λ
	 ʾ���� CH2 ��ֵ, mV��λ
	 �߶˵������-��ѹֵ  ��ֵ, mV��λ
	 �߶˵������-����ֵ  ��ֵ, uA��λ
	 USB 5V��ѹ ��ֵ, mV��λ
	 �ⲿ12V ��ѹ ��ֵ, mV��λ
	 TVCC��ѹ ��ֵ, mV��λ
*/
#define MODS_DO_NUM	9
#define MODS_DI_NUM	10
#define MODS_AI_NUM	7
#define MODS_AO_NUM	2

/* ���������״̬�Ĵ�����01Hָ�����Ȧ״̬ 05H��0FH��д��Ȧ״̬ */
#define	REG01_Y00				0x0000	/* ��1·���״̬��0��ʾ�Ͽ���1��ʾ�պ� */
//#define	REG01_Y01				0x0001	/* ��2·���״̬ */
//#define	REG01_Y02				0x0002	/* ��3·���״̬ */
//#define	REG01_Y03				0x0003	/* ��4·���״̬ */
//#define	REG01_Y04				0x0004	/* ��5·���״̬ */
//#define	REG01_Y05				0x0005	/* ��6·���״̬ */
//#define	REG01_Y06				0x0006	/* ��7·���״̬ */
//#define	REG01_Y07				0x0007	/* ��8·���״̬ */
//#define	REG01_Y08				0x0008	/* ��9·���״̬ */
//#define REG01_MAX				REG01_Y09

/* ����������Ĵ�����02Hָ��  ������˿�״̬ */
#define	REG02_X00				0x0000	/* ��1·����״̬��0��ʾ�͵�ƽ 1��ʾ�ߵ�ƽ */
//#define	REG02_X01				0x0001	/* ��2·����״̬ */
//#define	REG02_X02				0x0002	/* ��3·����״̬ */
//#define	REG02_X03				0x0003	/* ��4·����״̬ */
//#define	REG02_X04				0x0004	/* ��5·����״̬ */
//#define	REG02_X05				0x0005	/* ��6·����״̬ */
//#define	REG02_X06				0x0006	/* ��7·����״̬ */
//#define	REG02_X07				0x0007	/* ��8·����״̬ */
//#define	REG02_X08				0x0008	/* ��9·����״̬ */
//#define	REG02_X09				0x0009	/* ��10·����״̬ */
//#define REG02_MAX				REG02_X09

/* 03Hָ��  ���ּĴ����� 06��10H����д */
#define REG03_DEV_ID0				0x0000	/*�豸�����룺12���ֽڣ�96λ����6���Ĵ�����ֻ������ȫ��Ψһ��ID*/
#define REG03_DEV_ID1				0x0001
#define REG03_DEV_ID2				0x0002
#define REG03_DEV_ID3				0x0003
#define REG03_DEV_ID4				0x0004
#define REG03_DEV_ID5				0x0005

#define REG03_HARD_MODEL			0x0006	/* Ӳ���ͺţ�ֻ�������ڲ��ͺŴ��� 0750 */
#define REG03_APP_VER				0x0007 	/* APP�̼��汾��0x0102 ��ʾV1.02 */

#define REG03_X15_00				0x0008	/* DI��ʵʱ״̬��ֻ������Bit0��ʾD0ͨ����Bit15��ʾD15ͨ�� */
#define REG03_X31_16				0x0009	/* DI��ʵʱ״̬��ֻ������ */
#define REG03_Y15_00				0x000A	/* DO��ʵʱ״̬��ֻ������ */
#define REG03_Y31_16				0x000B	/* DO��ʵʱ״̬��ֻ������ */

/* У׼���ģ���� */
#define REG03_AI_CH1				0x000C	/* 4�ֽڸ�����, ʾ����CH1ͨ����ѹֵ��ֵ����λV */
#define REG03_AI_CH2				0x000E	/* 4�ֽڸ�����, ʾ����CH2ͨ����ѹֵ��ֵ����λV */
#define REG03_AI_HIGH_SIDE_VOLT		0x0010	/* 4�ֽڸ�����, �߶˵������-��ѹֵ  ��ֵ����λV */
#define REG03_AI_HIGH_SIDE_CURR		0x0012	/* 4�ֽڸ�����, �߶˵������-����ֵ  ��ֵ, ��λA */
#define REG03_AI_TVCC_VOLT			0x0014	/* 4�ֽڸ�����, Ŀ����ѹ ��ֵ, ��λV */
#define REG03_AI_TVCC_CURR			0x0016	/* 4�ֽڸ�����, Ŀ����ѹ ��ֵ, ��λV */
#define REG03_AI_NTC_RES			0x0018	/* NTC����������ֵ���, ��λŷķ */
#define REG03_AI_NTC_TEMP			0x001A	/* 4�ֽڸ����� NTC�¶ȣ������ */
#define REG03_AI_USB_5V				0x001C	/* 4�ֽڸ�����, USB 5V��ѹ  ��ֵ, ��λV */
#define REG03_AI_EXT_POWER			0x001E	/* 4�ֽڸ�����, �ⲿ12V��Դ��ѹ��ֵ, ��λV */

/* �յ�Ԥ�� */

#define REG03_ADC_CH1				0x0040	/* 4�ֽڸ�����, ʾ����CH1ͨ�� adc ��ֵ */
#define REG03_ADC_CH2				0x0042	/* 4�ֽڸ�����, ʾ����CH2ͨ�� adc ��ֵ */
#define REG03_ADC_HIGH_SIDE_VOLT	0x0044	/* 4�ֽڸ�����, �߶˵������-��ѹֵ adc ��ֵ */
#define REG03_ADC_HIGH_SIDE_CURR	0x0046	/* 4�ֽڸ�����, �߶˵������-����ֵ  adc ��ֵ */
#define REG03_ADC_TVCC_VOLT			0x0048	/* 4�ֽڸ�����, Ŀ����ѹ adc ��ֵ */
#define REG03_ADC_TVCC_CURR			0x004A	/* 4�ֽڸ�����, Ŀ����ѹ adc ��ֵ */
#define REG03_ADC_NTC_RES			0x004C	/* NTC����������ֵ��� adc ��ֵ*/
#define REG03_ADC_USB_5V			0x004E	/* 4�ֽڸ�����, USB 5V��ѹ  adc ��ֵ */
#define REG03_ADC_EXT_POWER			0x0050	/* 4�ֽڸ�����, �ⲿ12V��Դ��ѹ  adc ��ֵ */
#define REG03_ADC_PF4				0x0052	/* ������Ԫ */

#define REG03_OUT_VOLT_DAC			0x0080	/* 2�ֽ����� ���������ѹ�� DACֵ */
#define REG03_OUT_VOLT_MV			0x0081	/* 2�ֽ����� ���������ѹ�� mVֵ */

#define REG03_OUT_CURR_DAC			0x0082	/* 2�ֽ����� ������������� DACֵ */
#define REG03_OUT_CURR_UA			0x0083	/* 2�ֽ����� ������������� uAֵ */

#define REG03_OUT_TVCC_DAC			0x0084	/* 2�ֽ����� ����TVCC��ѹ�ĵ�λֵ��0-127�� */
#define REG03_OUT_TVCC_MV			0x0085	/* 2�ֽ����� mVֵ 1260 - 5000mV */

/* �յ�Ԥ�� */

/* DAC������ο���
	0�������ƽ
	1���������
	2���������
	3��������ǲ�
*/
#define REG03_DAC_WAVE_VOLT_RANGE	0x0100	/* ��ѹ���̣��������á��̶�����10V */
#define REG03_DAC_WAVE_TYPE			0x0101	/* DAC�������� */
#define REG03_DAC_WAVE_VOLT_MIN		0x0102	/* ������С��ѹ��mV */
#define REG03_DAC_WAVE_VOLT_MAX		0x0103	/* ��������ѹ��mV */
#define REG03_DAC_WAVE_FREQ			0x0104	/* 32bit  ����Ƶ�ʣ�Hz */
#define REG03_DAC_WAVE_FREQ_Low		0x0105	/* 32bit  ����Ƶ�ʣ�Hz */
#define REG03_DAC_WAVE_DUTY			0x0106	/* ����ռ�ձ� �ٷֱ� */
#define REG03_DAC_WAVE_COUNT_SET	0x0107	/* 32bit  ���θ������� ��16bit */
#define REG03_DAC_WAVE_COUNT_SET0	0x0108	/* 32bit  ���θ������� ��16bit */
#define REG03_DAC_WAVE_START		0x0109	/* DAC��������ֹͣ���� */

/* �յ�����δ�� */

#define REG03_D0_GPIO_MODE			0x0140	/* DO�ڵ�GPIOģʽ - 0=���룬1=��� 2=�������⹦�� */
#define REG03_D1_GPIO_MODE			0x0141
#define REG03_D2_GPIO_MODE			0x0142
#define REG03_D3_GPIO_MODE			0x0143
#define REG03_D4_GPIO_MODE			0x0144
#define REG03_D5_GPIO_MODE			0x0145
#define REG03_D6_GPIO_MODE			0x0146
#define REG03_D7_GPIO_MODE			0x0147
#define REG03_D8_GPIO_MODE			0x0148
#define REG03_D9_GPIO_MODE			0x0149

#define REG03_D10_GPIO_MODE			0x014A	/* TTL-TX */
#define REG03_D11_GPIO_MODE			0x014B	/* TTL-RX */
#define REG03_D12_GPIO_MODE			0x014C	/* CAN-TX */
#define REG03_D13_GPIO_MODE			0x014D	/* CAN-RX */

/* �յ�����δ�� */

#define REG03_RTC_YEAR				0x0180	/* �豸ʱ�� */
#define REG03_RTC_MON				0x0181
#define REG03_RTC_DAY				0x0182
#define REG03_RTC_HOUR				0x0183
#define REG03_RTC_MIN				0x0184
#define REG03_RTC_SEC				0x0185

#define REG03_NTC_COEF_K			0x0186	/* NTC�¶ȴ������¶�������ʽ y = kx + b ֮Kֵ ���� ȱʡ = 1 */
#define REG03_NTC_COEF_B			0x0187	/* NTC�¶ȴ������¶�������ʽ y = kx + b ֮Bֵ ���� ȱʡ = 0 */

/* ADC ʾ�������� */
#define REG03_DSO_MEASURE_MODE		0x01FF	/* 1��ʾ�������ģʽ */
#define REG03_CH1_DC				0x0200	/* CH1ͨ����AC/DC����л� 1��ʾDC */
#define REG03_CH2_DC				0x0201	/* CH2ͨ����AC/DC����л� */
#define REG03_CH1_GAIN				0x0202	/* CH1ͨ���������л�0-3 */
#define REG03_CH2_GAIN				0x0203	/* CH2ͨ���������л�0-3 */
#define REG03_CH1_OFFSET			0x0204	/* CH1ͨ����ֱ��ƫֵ��0-2500mV ��������֧��) */
#define REG03_CH2_OFFSET			0x0205	/* CH2ͨ����ֱ��ƫֵ��0-2500mV ��������֧��) */
#define REG03_DSO_FREQ_ID			0x0206	/* ʾ��������Ƶ�ʵ�λ  */
	#define DSO_FREQ_100	0
	#define DSO_FREQ_200	1
	#define DSO_FREQ_500	2
	#define DSO_FREQ_1K		3
	#define DSO_FREQ_2K		4
	#define DSO_FREQ_5K		5
	#define DSO_FREQ_10K	6
	#define DSO_FREQ_20K	7
	#define DSO_FREQ_50K	8
	#define DSO_FREQ_100K	9
	#define DSO_FREQ_200K	10
	#define DSO_FREQ_500K	11
	#define DSO_FREQ_1M		12
	#define DSO_FREQ_2M		13
	#define DSO_FREQ_5M		14
	#define DSO_FREQ_10M	15
	#define DSO_FREQ_20M	16

#define REG03_DSO_SAMPLE_SIZE		0x0207	/* ������ȵ�λ */
	#define DSO_SIZE_1K		0
	#define DSO_SIZE_2K		1
	#define DSO_SIZE_4K		2
	#define DSO_SIZE_8K		3
	#define DSO_SIZE_16K	4
	#define DSO_SIZE_32K	5
	#define DSO_SIZE_64K	6
	#define DSO_SIZE_128K	7
	#define DSO_FREQ_512K	8

#define REG03_DSO_TRIG_LEVEL		0x0208	/* ������ƽ��0-65535 */
#define REG03_DSO_TRIG_POS			0x0209	/* ����λ�� 0-100 �ٷֱ� */
#define REG03_DSO_TRIG_MODE			0x020A	/* ����ģʽ 0=�Զ� 1=��ͨ 2=���� */
#define REG03_DSO_TRIG_CHAN			0x020B	/* ����ͨ�� 0=CH1  1=CH2*/
#define REG03_DSO_TRIG_EDGE			0x020C	/* �������� 0�½� 1���� */
#define REG03_DSO_CHAN_EN			0x020D	/* ͨ��ʹ�ܿ��� bit0 = CH1  bit1 = CH2  bit2 = CH2 - CH1 */
#define REG03_DSO_RUN				0x020E	/* ʾ�����ɼ�����,0��ֹͣ 1: ���� */

#define REG03_DSO_CURR_GAIN			0x0211	/* 0��ʾ200mA, 1��ʾ2A���� */

#define REG03_WAVE_LOCK				0x021C	/* �����������ȴ���ȡ */

#define REG03_LUA_CMD				0x0300	/* LUA����ָ�� */
#define REG03_LUA_STATE				0x0301	/* LUA����״̬ */


#define REG03_WORK_MODE				0x1000	/* �ն˹���ģʽ��1 �����湤��ģʽ������ֵ������������Ӧ�� */
#define REG03_RS485_ADDR			0x1001	/* ����RS485 MODBUS��ַ��1��254��*/
#define REG03_RS485_BAUD			0x1002	/* RS485�����ʣ�0��1200 , 1��2400��2 : 4800��3 : 9600 , 4��19200 , 5��38400��6��57600��7��115200��*/
#define REG03_RS485_PARITY			0x1003	/* RS485��żУ��λ��0������żУ�飬 1����У�飬2��żУ�飩*/

#define REG03_IPAddr_Format			0x1100	/*��̫��IP��ַ��ʽ��IPv4��IPv6		0 ��ʾIPv4		1 ��ʾIPv6*/
#define REG03_DHCP					0x1101	/*��̫��DHCP���á�0��ʾ��ֹ��1��ʾ���á�*/
#define REG03_LocalIPAddr_H			0x1102	/*IPv4����IP��ַ����̬��ַ����4�ֽڡ�ȱʡ 192.168.1.105*/
#define REG03_LocalIPAddr_L			0x1103	/*IPv4����IP��ַ����̬��ַ����4�ֽڡ�ȱʡ 192.168.1.105*/
#define REG03_GatewayAddr_H			0x1104	/*IPv4���ص�ַ��4�ֽڡ�ȱʡ192.168.1.1*/
#define REG03_GatewayAddr_L			0x1105	/*IPv4���ص�ַ��4�ֽڡ�ȱʡ192.168.1.1*/
#define REG03_SubMask_H				0x1106	/*IPv4�������룬4�ֽڡ�ȱʡ255.255.255.0*/
#define REG03_SubMask_L				0x1107	/*IPv4�������룬4�ֽڡ�ȱʡ255.255.255.0*/
#define REG03_TCP_PORT				0x1108	/*����TCP����˿ںţ�ȱʡֵ30010*/
#define REG03_MAC12					0x1109	/*MAC��ַ1��2�ֽ�*/
#define REG03_MAC34					0x110A	/*MAC��ַ3��4�ֽ�*/
#define REG03_MAC56					0x110B	/*MAC��ַ5��6�ֽ�*/

/* ���ڶ�ȡ�������ݵļĴ������ */
#define REG03_CH1_WAVE_0			0x2000	/* CH1�������� ��ڼĴ����� ���⴦��1K�����ֽڶ�ȡ */
#define REG03_CH1_WAVE_1			0x2001	/* CH1�������� ��2�� 1K�ֽ� */
#define REG03_CH1_WAVE_2			0x2002	/* CH1�������� ��3�� 1K�ֽ� */
#define REG03_CH1_WAVE_3			0x2003	/* CH1�������� ��4�� 1K�ֽ� */

#define REG03_CH2_WAVE_0			0x4000	/* CH2�������� ��ڼĴ����� ���⴦��1K�����ֽڶ�ȡ  */


/********************************** У׼���� ***********************************/
/* У׼���أ�
	1      = �����޸�У׼������
	0      = ��ֹ�޸�У׼����
	0x5AA5 = ��ʼ��У׼����   InitCalibParam();��ʼ��У׼����

*/
#define REG03_CALIB_KEY				0xBFFF

/* ����ģ����У׼ */
#define REG03_CALIB_CH1_R1_X1		0xC000		/* CH1 X1��λ У׼������X1 Y1 X2 Y2 ��Ϊ������. ÿ��������ռ��2���Ĵ��� */
#define REG03_CALIB_CH1_R1_Y1		0xC002
#define REG03_CALIB_CH1_R1_X2		0xC004
#define REG03_CALIB_CH1_R1_Y2		0xC006
	#define REG03_CALIB_CH1_R2_X1	0xC008		/* CH1 X2��λ У׼������X1 Y1 X2 Y2 ��Ϊ������. ÿ��������ռ��2���Ĵ��� */
	#define REG03_CALIB_CH1_R3_X1	0xC010		/* CH1 X4��λ У׼������X1 Y1 X2 Y2 ��Ϊ������. ÿ��������ռ��2���Ĵ��� */
	#define REG03_CALIB_CH1_R4_X1	0xC018		/* CH1 X8��λ У׼���� */
	#define REG03_CALIB_CH1_R5_X1	0xC020		/* CH1 X16��λ У׼���� */
	#define REG03_CALIB_CH1_R6_X1	0xC028		/* CH1 X32��λ У׼���� */
	#define REG03_CALIB_CH1_R7_X1	0xC030		/* CH1 X64��λ У׼���� */
	#define REG03_CALIB_CH1_R8_X1	0xC038		/* CH1 X128��λ У׼���� */

#define REG03_CALIB_CH2_R1_X1		0xC040		/* CH2 X1��λ У׼������X1 Y1 X2 Y2 ��Ϊ������. ÿ��������ռ��2���Ĵ��� */
#define REG03_CALIB_CH2_R1_Y1		0xC042
#define REG03_CALIB_CH2_R1_X2		0xC044
#define REG03_CALIB_CH2_R1_Y2		0xC046
	#define REG03_CALIB_CH2_R2_X1	0xC048		/* CH2 X2��λ У׼������X1 Y1 X2 Y2 ��Ϊ������. ÿ��������ռ��2���Ĵ��� */
	#define REG03_CALIB_CH2_R3_X1	0xC050		/* CH2 X4��λ У׼������X1 Y1 X2 Y2 ��Ϊ������. ÿ��������ռ��2���Ĵ��� */
	#define REG03_CALIB_CH2_R4_X1	0xC058		/* CH2 X8��λ У׼���� */
	#define REG03_CALIB_CH2_R5_X1	0xC060		/* CH2 X16��λ У׼���� */
	#define REG03_CALIB_CH2_R6_X1	0xC068		/* CH2 X32��λ У׼���� */
	#define REG03_CALIB_CH2_R7_X1	0xC070		/* CH2 X64��λ У׼���� */
	#define REG03_CALIB_CH2_R8_X1	0xC078		/* CH2 X128��λ У׼���� */

#define REG03_CALIB_LOAD_VOLT_X1	0xC080	/* �߶˵������-��ѹֵ У׼������X1 Y1 X2 Y2 ��Ϊ������. ÿ��������ռ��2���Ĵ��� */
#define REG03_CALIB_LOAD_VOLT_Y1	0xC082
#define REG03_CALIB_LOAD_VOLT_X2	0xC084
#define REG03_CALIB_LOAD_VOLT_Y2	0xC086

#define REG03_CALIB_LOAD_CURR1_X1	0xC088	/* �߶˵������(С����)-����ֵ У׼������X1 Y1 X2 Y2 ��Ϊ������. ÿ��������ռ��2���Ĵ��� */
#define REG03_CALIB_LOAD_CURR1_Y1	0xC08A
#define REG03_CALIB_LOAD_CURR1_X2	0xC08C
#define REG03_CALIB_LOAD_CURR1_Y2	0xC08E
#define REG03_CALIB_LOAD_CURR1_X3	0xC090
#define REG03_CALIB_LOAD_CURR1_Y3	0xC092
#define REG03_CALIB_LOAD_CURR1_X4	0xC094
#define REG03_CALIB_LOAD_CURR1_Y4	0xC096

#define REG03_CALIB_LOAD_CURR2_X1	0xC098	/* �߶˵������(С����)-����ֵ У׼������X1 Y1 X2 Y2 ��Ϊ������. ÿ��������ռ��2���Ĵ��� */
#define REG03_CALIB_LOAD_CURR2_Y1	0xC09A
#define REG03_CALIB_LOAD_CURR2_X2	0xC09C
#define REG03_CALIB_LOAD_CURR2_Y2	0xC09E
#define REG03_CALIB_LOAD_CURR2_X3	0xC0A0
#define REG03_CALIB_LOAD_CURR2_Y3	0xC0A2
#define REG03_CALIB_LOAD_CURR2_X4	0xC0A4
#define REG03_CALIB_LOAD_CURR2_Y4	0xC0A6

#define REG03_CALIB_TVCC_VOLT_X1	0xC0A8	/* TVCC-��ѹֵ У׼������X1 Y1 X2 Y2 ��Ϊ������. ÿ��������ռ��2���Ĵ��� */
#define REG03_CALIB_TVCC_VOLT_Y1	0xC0AA
#define REG03_CALIB_TVCC_VOLT_X2	0xC0AC
#define REG03_CALIB_TVCC_VOLT_Y2	0xC0AE

#define REG03_CALIB_TVCC_CURR_X1	0xC0B0	/* TVCC-����ֵ У׼������X1 Y1 X2 Y2 ��Ϊ������. ÿ��������ռ��2���Ĵ��� */
#define REG03_CALIB_TVCC_CURR_Y1	0xC0B2
#define REG03_CALIB_TVCC_CURR_X2	0xC0B4
#define REG03_CALIB_TVCC_CURR_Y2	0xC0B6
#define REG03_CALIB_TVCC_CURR_X3	0xC0B8
#define REG03_CALIB_TVCC_CURR_Y3	0xC0BA
#define REG03_CALIB_TVCC_CURR_X4	0xC0BC
#define REG03_CALIB_TVCC_CURR_Y4	0xC0BE

/* ���ģ����У׼ */
#define REG03_CALIB_TVCC_SET_X1		0xC0C0	/* TVCC�����ѹ�趨 У׼������X1 Y1 X2 Y2 ��Ϊ������. ÿ��������ռ��2���Ĵ��� */
#define REG03_CALIB_TVCC_SET_Y1		0xC0C2
#define REG03_CALIB_TVCC_SET_X2		0xC0C4
#define REG03_CALIB_TVCC_SET_Y2		0xC0C6

#define REG03_CALIB_DAC_VOLT_X1		0xC0C8	/* DAC�����ѹ�趨 У׼�������з������� */
#define REG03_CALIB_DAC_VOLT_Y1		0xC0C9
#define REG03_CALIB_DAC_VOLT_X2		0xC0CA
#define REG03_CALIB_DAC_VOLT_Y2		0xC0CB
#define REG03_CALIB_DAC_VOLT_X3		0xC0CC
#define REG03_CALIB_DAC_VOLT_Y3		0xC0CD
#define REG03_CALIB_DAC_VOLT_X4		0xC0CE
#define REG03_CALIB_DAC_VOLT_Y4		0xC0CF

#define REG03_CALIB_DAC_CURR_X1		0xC0D0	/* DAC�����ѹ�趨 У׼�������з�������  */
#define REG03_CALIB_DAC_CURR_Y1		0xC0D1
#define REG03_CALIB_DAC_CURR_X2		0xC0D2
#define REG03_CALIB_DAC_CURR_Y2		0xC0D3
#define REG03_CALIB_DAC_CURR_X3		0xC0D4
#define REG03_CALIB_DAC_CURR_Y3		0xC0D5
#define REG03_CALIB_DAC_CURR_X4		0xC0D6
#define REG03_CALIB_DAC_CURR_Y4		0xC0D7

#define REG03_CALIB_NTC_RES_X1		0xC0D8	/* NTC���� У׼������X1 Y1 X2 Y2 ��Ϊ������. ÿ��������ռ��2���Ĵ��� */
#define REG03_CALIB_NTC_RES_Y1		0xC0DA
#define REG03_CALIB_NTC_RES_X2		0xC0DC
#define REG03_CALIB_NTC_RES_Y2		0xC0DE
#define REG03_CALIB_NTC_RES_X3		0xC0E0
#define REG03_CALIB_NTC_RES_Y3		0xC0E2
#define REG03_CALIB_NTC_RES_X4		0xC0E4
#define REG03_CALIB_NTC_RES_Y4		0xC0E6


#define REG03_CALIB_PARAM_END		REG03_CALIB_NTC_RES_Y2 + 1		/* У׼���������1���Ĵ��� */

#define REG03_BOOT_VER				0xE000 	/* BOOT �̼��汾��0x0102 ��ʾV1.02 */

/* ��ӳ�䲿�ּĴ�����ר����UDP��·���� */
#define REG03_NET_CPU_ID0			0xFF00
#define REG03_NET_CPU_ID1			0xFF01
#define REG03_NET_CPU_ID2			0xFF02
#define REG03_NET_CPU_ID3			0xFF03
#define REG03_NET_CPU_ID4			0xFF04
#define REG03_NET_CPU_ID5			0xFF05
#define REG03_NET_HARD_VER			0xFF06	/* Ӳ���ͺţ�ֻ�������ڲ��ͺŴ��룬���ͺ�ȫ�ƣ�0xC200 ��ʾTC200*/
#define REG03_NET_SOFT_VER			0xFF07	/* Ӳ���汾��ֻ��������0x0100����ʾH1.00 */
#define REG03_NET_MAC12				0xFF08	/* MAC */
#define REG03_NET_MAC34				0xFF09	/* MAC */
#define REG03_NET_MAC56				0xFF0A	/* MAC */
#define REG03_NET_LOCAL_IP_H		0xFF0B	/* IPv4����IP��ַ����̬��ַ������2�ֽ� */
#define REG03_NET_LOCAL_IP_L		0xFF0C	/* IPv4����IP��ַ����̬��ַ������2�ֽ� */
#define REG03_NET_NETMASK_H			0xFF0D	/* IPv4�������룬��2�ֽڡ� */
#define REG03_NET_NETMASK_L			0xFF0E	/* IPv4�������룬��2�ֽڡ� */
#define REG03_NET_GATEWAY_H			0xFF0F	/* IPv4���ص�ַ����2�ֽڡ� */
#define REG03_NET_GATEWAY_L			0xFF10	/* IPv4���ص�ַ����2�ֽڡ� */
#define REG03_NET_LOCAL_PORT		0xFF11	/* ���ض˿� */
#define REG03_NET_SYSTEM_IP_H		0xFF12	/* ����ϵͳIP��ַ��λ */
#define REG03_NET_SYSTEM_IP_L		0xFF13	/* ����ϵͳIP��ַ��λ  */
#define REG03_NET_SYSTEM_PORT	 	0xFF14	/* Զ�̷������˿� */

#define REG03_SEND_KEY          	0xFF7E  /* ����Ĵ�����ģ��һ���������� */
#define REG03_RESET_TO_BOOT     	0xFF7F  /* ����Ĵ�������λ����BOOT
	1 - ��λ����APP
	2 - ��λ����BOOT
    3 - ����DAP
    4 - ����U��ģʽ
    5 - ����APP��������00
*/
    #define JUMP_TO_APP    1
    #define JUMP_TO_BOOT   2
    #define JUMP_TO_DAP    3
    #define JUMP_TO_EMMC   4
	#define JUMP_TO_LINK   5

/* BOOT �����õļĴ��� */
#define REG03_BOOT_CPU_ID0			0xFF80
#define REG03_BOOT_CPU_ID1			0xFF81
#define REG03_BOOT_CPU_ID2			0xFF82
#define REG03_BOOT_CPU_ID3			0xFF83
#define REG03_BOOT_CPU_ID4			0xFF84
#define REG03_BOOT_CPU_ID5			0xFF85
#define REG03_BOOT_HARD_VER			0xFF86	/* Ӳ���ͺţ�ֻ�������ڲ��ͺŴ��룬���ͺ�ȫ�ƣ�0xC200 ��ʾTC200*/
#define REG03_BOOT_SOFT_VER			0xFF87	/* Ӳ���汾��ֻ��������0x0100����ʾH1.00 */

#define REG03_BOOT_PROG_TYPE		0xFF88	/* APP��������: 0 = CPU�ڲ�Flash ; 1 = QSPI Flash */
#define REG03_BOOT_PROG_ADDR		0xFF89	/* APP�����ַ 32λ */
#define REG03_BOOT_PROG_SIZE		0xFF8B	/* APP���򳤶� 32λ */
/* ϵͳ���Ʋ���
	1 - ֪ͨ��ʼ�������豸��ʼ����flash. ����ǰ��2���Ĵ������������ռ�
	2 - ֪ͨ�豸�����������
	3 - ֪ͨϵͳ��λ
*/
#define REG03_BOOT_ERASE_SECTOR		0xFF8D	/* �������������0��ʼ��ÿ����128K */
#define REG03_BOOT_TRANS_END		0xFF8E	/* ������� 1 */

#define REG03_BOOT_STATUS			0xFFA0	/* �豸״̬�Ĵ���  0���У�1��ʾ��æ */
#define REG03_BOOT_CRC32_HIGH		0xFFA1	/* ������CRC32У�� - δ�� */
#define REG03_BOOT_CRC32_LOW		0xFFA2	/* ������CRC32У�� - δ�� */

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
