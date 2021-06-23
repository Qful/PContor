-------------------------------------------------------
-- �ļ��� : STM32F750x8_64.lua
-- ��  �� : V1.0  2020-04-28
-- ˵  �� :
-------------------------------------------------------

print("load \"STM32F750x8_64.lua\" ok")

IncludeList = {
	"0:/H7-TOOL/Programmer/Device/ST/Lib/STM32F7_Lib_E0042000.lua"
}

function config_cpu(void)
	CHIP_TYPE = "SWD"		--ָ�������ӿ�����: "SWD", "SWIM", "SPI", "I2C", "UART"

	AlgoFile_FLASH = "0:/H7-TOOL/Programmer/Device/ST/STM32F7xx/STM32F75x_64_AXI.FLM"
	AlgoFile_EEPROM = ""
	AlgoFile_OTP   = "0:/H7-TOOL/Programmer/Device/ST/STM32F7xx/STM32F7xx_OTP.FLM"
	AlgoFile_OPT   = "0:/H7-TOOL/Programmer/Device/ST/STM32F7xx/STM32F74x_75x_OPT.FLM"
	AlgoFile_QSPI  = ""

	FLASH_ADDRESS = 0x08000000		--CPU�ڲ�FLASH��ʼ��ַ
	FLASH_SIZE = 64 * 1024		--����FLM�е� Device Size

	--EEPROM_ADDRESS = 0				--CPU�ڲ�EEPROM��ʼ��ַ
	--EEPROM_SIZE = 0

	OTP_ADDRESS		= 0x1FF0F000
	OTP_SIZE		= 0x0410

	RAM_ADDRESS = 0x20000000		--CPU�ڲ�RAM��ʼ��ַ

	--Flash�㷨�ļ������ڴ��ַ�ʹ�С
	AlgoRamAddr = RAM_ADDRESS
	AlgoRamSize = 4 * 1024

	MCU_ID = 0x5BA02477

	UID_ADDR = 0x1FF0F420	   	--UID��ַ����ͬ��CPU��ͬ
	UID_BYTES = 12

	--ȱʡУ��ģʽ
	VERIFY_MODE = 0				-- 0:����У��, 1:���CRC32У��, ����:��չӲ��CRC(��Ҫ��Ƭ��֧�֣�

	ERASE_CHIP_TIME = 8000		--ȫƬ����ʱ��ms�������ڽ���ָʾ)

	--��ַ���е�FFFFFFFF��ʾԭʼ�����в���һ��ȡ�����ֽ�
	OB_ADDRESS     = "1FFF0000 1FFF0001 1FFF0008 1FFF0009 1FFF0010 1FFF0011 1FFF0018 1FFF0019"

	OB_SECURE_OFF  = "FF AA FF FF 80 00 40 00"	--SECURE_ENABLE = 0ʱ�������Ϻ�д���ֵ
	OB_SECURE_ON   = "FF 00 FF FF 80 00 40 00"	--SECURE_ENABLE = 1ʱ�������Ϻ�д���ֵ

	--�ж϶�������д����������(WRP = Write protection)
	OB_WRP_ADDRESS   = {0x1FFF0001, 0x1FFF0008} 	--�ڴ��ַ
	OB_WRP_MASK  	 = {0xFF, 0xFF}					--�����������������
	OB_WRP_VALUE 	 = {0xAA, 0xFF}					--�����������Ƚϣ���ȱ�ʾû�б���
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
