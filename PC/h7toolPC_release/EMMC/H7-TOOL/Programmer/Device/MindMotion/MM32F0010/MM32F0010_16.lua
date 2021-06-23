-------------------------------------------------------
-- �ļ��� : MM32F0010_16.lua
-- ��  �� : V1.1  2020-12-16 ����OPT�㷨�ļ�
-- ˵  �� : MM32F0010A1T, MM32F0010A1N
-------------------------------------------------------

print("load \"MM32F0010_16.lua\" ok")

IncludeList = {
	"0:/H7-TOOL/Programmer/Device/MindMotion/Lib/MM32_Lib_M0.lua"
}

function config_cpu(void)
	CHIP_TYPE = "SWD"		--ָ�������ӿ�����: "SWD", "SWIM", "SPI", "I2C", "UART"

	AlgoFile_FLASH = "0:/H7-TOOL/Programmer/Device/MindMotion/MM32F0010/MM32F0010_16.FLM"
	AlgoFile_OTP   = ""
	AlgoFile_OPT   = "0:/H7-TOOL/Programmer/Device/MindMotion/OPT/MM32_M0_F010_OPT.FLM"
	AlgoFile_QSPI  = ""

	FLASH_ADDRESS = 0x08000000		--CPU�ڲ�FLASH��ʼ��ַ

	RAM_ADDRESS = 0x20000000		--CPU�ڲ�RAM��ʼ��ַ

	--Flash�㷨�ļ������ڴ��ַ�ʹ�С
	AlgoRamAddr = RAM_ADDRESS
	AlgoRamSize = 2 * 1024

	MCU_ID = 0x0BB11477

	UID_ADDR = 0x1FFFF7E8	   	--UID��ַ����ͬ��CPU��ͬ
	UID_BYTES = 12

	--ȱʡУ��ģʽ
	VERIFY_MODE = 0				-- 0:����У��, 1:���CRC32У��, ����:��չӲ��CRC(��Ҫ��Ƭ��֧�֣�

	--��ַ���е�FFFFFFFF��ʾԭʼ�����в���һ��ȡ�����ֽ�
	OB_ADDRESS     = "1FFFF800 FFFFFFFF 1FFFF802 FFFFFFFF 1FFFF804 FFFFFFFF 1FFFF806 FFFFFFFF 1FFFF808 FFFFFFFF 1FFFF80A FFFFFFFF 1FFFF80C FFFFFFFF FFFFFFFE FFFFFFFF "
	OB_SECURE_OFF  = "A5 FF FF FF FF FF FF FF "	--SECURE_ENABLE = 0ʱ�������Ϻ�д���ֵ
	OB_SECURE_ON   = "80 FF FF FF FF FF FF FF "	--SECURE_ENABLE = 1ʱ�������Ϻ�д���ֵ

	--�ж϶�������д����������(WRP = Write protection)
	OB_WRP_ADDRESS   = {0x1FFFF800} 	--�ڴ��ַ
	OB_WRP_MASK  	 = {0xFF}		--�����������������
	OB_WRP_VALUE 	 = {0xA5}		--�����������Ƚϣ���ȱ�ʾû�б���
end

--����PC���, ����ȱʡ���ò���
function pc_default(void)
	TVCC_VOLT = 3.3			--����CPU�����ѹTVCC
	
	VERIFY_MODE = 1			--У��ģʽ: 0:�Զ�(FLM�ṩУ�麯�������) 1:����  2:���CRC32  3:STM32Ӳ��CRC32

	REMOVE_RDP_POWEROFF = 0 --д��OB����Ҫ�ϵ�
	POWEROFF_TIME1 = 200   	--д��OB�ӳ�ʱ��ms
	POWEROFF_TIME2 = 200   	--�ϵ�ʱ��ms
	POWEROFF_TIME3 = 20   	--�ϵ��ȴ�ʱ��ms

	SWD_CLOCK_DELAY_0 = 0 	--��·��¼ʱ��ʱ���ӳ�
	SWD_CLOCK_DELAY_1 = 0 	--��·��¼ʱ��ʱ���ӳ�
	
	AUTO_REMOVE_PROTECT = 1 --�Զ����������

	NOTE_PC = ""	
end

---------------------------����-----------------------------------
