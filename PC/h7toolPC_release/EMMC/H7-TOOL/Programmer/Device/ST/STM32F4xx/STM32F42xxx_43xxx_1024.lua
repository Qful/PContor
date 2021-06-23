-------------------------------------------------------
-- �ļ��� : STM32F42xxx_43xxx_1024.lua
-- ��  �� : V1.0  2020-04-28
-- ˵  �� :
-------------------------------------------------------

print("load \"STM32F42xxx_43xxx_1024.lua\" ok")

IncludeList = {
	"0:/H7-TOOL/Programmer/Device/ST/Lib/STM32F4_Lib_E0042000.lua"
}

function config_cpu(void)
	CHIP_TYPE = "SWD"		--ָ�������ӿ�����: "SWD", "SWIM", "SPI", "I2C", "UART"

	AlgoFile_FLASH = "0:/H7-TOOL/Programmer/Device/ST/STM32F4xx/STM32F4xx_1024.FLM"
	AlgoFile_OTP   = "0:/H7-TOOL/Programmer/Device/ST/STM32F4xx/STM32F4xx_OTP.FLM"
	AlgoFile_OPT   = "0:/H7-TOOL/Programmer/Device/ST/STM32F4xx/STM32F42xxx_43xxx_OPT.FLM"
	AlgoFile_QSPI  = ""

	FLASH_ADDRESS = 0x08000000		--CPU�ڲ�FLASH��ʼ��ַ

	OTP_ADDRESS	= 0x1FFF7800		--CPU�ڲ�OTP(1�οɱ��)��ʼ��ַ

	RAM_ADDRESS = 0x20000000		--CPU�ڲ�RAM��ʼ��ַ

	--Flash�㷨�ļ������ڴ��ַ�ʹ�С
	AlgoRamAddr = RAM_ADDRESS
	AlgoRamSize = 8 * 1024

	MCU_ID = 0x2BA01477

	UID_ADDR = 0x1FFF7A10	   	--UID��ַ����ͬ��CPU��ͬ
	UID_BYTES = 12

	--ȱʡУ��ģʽ
	VERIFY_MODE = 0				-- 0:����У��, 1:���CRC32У��, ����:��չӲ��CRC(��Ҫ��Ƭ��֧�֣�

	ERASE_CHIP_TIME = 16000		--ȫƬ����ʱ��ms�������ڽ���ָʾ)

	OB_ADDRESS     = "1FFFC000 1FFFC001 1FFFC008 1FFFC009 1FFEC000 1FFEC001 1FFEC008 1FFEC009"

	OB_SECURE_OFF  = "EF AA FF 3F FF FF FF FF"	--SECURE_ENABLE = 0ʱ�������Ϻ�д���ֵ
	OB_SECURE_ON   = "EF 00 FF 3F FF FF FF FF"	--SECURE_ENABLE = 1ʱ�������Ϻ�д���ֵ

	--�ж϶�������д����������(WRP = Write protection)
	OB_WRP_ADDRESS   = {0x1FFFC001, 0x1FFFC008, 0x1FFFC009, 0x1FFEC008, 0x1FFEC009} 	--�ڴ��ַ
	OB_WRP_MASK  	 = {0xFF, 0xFF, 0x0F, 0xFF, 0x0F}					--�����������������
	OB_WRP_VALUE 	 = {0xAA, 0xFF, 0x0F, 0xFF, 0x0F}					--�����������Ƚϣ���ȱ�ʾû�б���
end

--����PC���, ����ȱʡ���ò���
function pc_default(void)
	TVCC_VOLT = 3.3			--����CPU�����ѹTVCC
	
	VERIFY_MODE = 0			--У��ģʽ: 0:�Զ�(FLM�ṩУ�麯�������) 1:����  2:���CRC32  3:STM32Ӳ��CRC32

	REMOVE_RDP_POWEROFF = 1 --д��OB����Ҫ�ϵ�
	POWEROFF_TIME1 = 0   	--д��OB�ӳ�ʱ��ms
	POWEROFF_TIME2 = 100   	--�ϵ�ʱ��ms
	POWEROFF_TIME3 = 20   	--�ϵ��ȴ�ʱ��ms

	SWD_CLOCK_DELAY_0 = 0 	--��·��¼ʱ��ʱ���ӳ�
	SWD_CLOCK_DELAY_1 = 0 	--��·��¼ʱ��ʱ���ӳ�
	
	AUTO_REMOVE_PROTECT = 1 --�Զ����������

	NOTE_PC = ""	
end

---------------------------����-----------------------------------
