-------------------------------------------------------
-- �ļ��� : MIMXRT105x_QSPI.lua
-- ��  �� : V1.0  2020-04-28
-- ˵  �� :
-------------------------------------------------------

print("load \"MIMXRT105x_QSPI.lua\" ok")

IncludeList = {
	"0:/H7-TOOL/Programmer/Device/NXP/Lib/RT105x_Lib.lua"
}

function config_cpu(void)
	CHIP_TYPE = "SWD"		--ָ�������ӿ�����: "SWD", "SWIM", "SPI", "I2C", "UART"

	AlgoFile_FLASH = "0:/H7-TOOL/Programmer/Device/NXP/RT105x/MIMXRT105x_QuadSPI_4KB_SEC.FLM"
	AlgoFile_OTP   = ""
	AlgoFile_OPT   = ""
	AlgoFile_QSPI  = ""

	FLASH_ADDRESS = 0x60000000		--CPU�ڲ�FLASH��ʼ��ַ
	FLASH_SIZE = 32 * 1024 * 1024
	
	RAM_ADDRESS = 0x20000000		--CPU�ڲ�RAM��ʼ��ַ

	--Flash�㷨�ļ������ڴ��ַ�ʹ�С
	AlgoRamAddr = RAM_ADDRESS
	AlgoRamSize = 32 * 1024

	MCU_ID = 0x0BD11477 

	UID_ADDR = 0x401F4410	   	--UID��ַ����ͬ��CPU��ͬ
	UID_BYTES = 8

	--ȱʡУ��ģʽ
	VERIFY_MODE = 2				-- 0:����У��, 1:���CRC32У��, ����:��չӲ��CRC(��Ҫ��Ƭ��֧�֣�

	--PYPER Flash  2MB ����8.3s
	ERASE_CHIP_TIME = 250000		--ȫƬ����ʱ��ms�������ڽ���ָʾ)

	OB_ADDRESS     = "1FFFF800 FFFFFFFF 1FFFF802 FFFFFFFF 1FFFF804 FFFFFFFF 1FFFF806 FFFFFFFF 1FFFF808 FFFFFFFF 1FFFF80A FFFFFFFF 1FFFF80C FFFFFFFF 1FFFF80E FFFFFFFF"

	OB_SECURE_OFF  = "AA FF FF FF FF FF FF FF"	--SECURE_ENABLE = 0ʱ�������Ϻ�д���ֵ(�������)
	OB_SECURE_ON   = "00 FF FF FF FF FF FF FF"	--SECURE_ENABLE = 1ʱ�������Ϻ�д���ֵ(оƬ����)

	--�ж϶�������д����������(WRP = Write protection)
	OB_WRP_ADDRESS   = {} 	--�ڴ��ַ
	OB_WRP_MASK  	 = {}		--�����������������
	OB_WRP_VALUE 	 = {}		--�����������Ƚϣ���ȱ�ʾû�б���
end

--����PC���, ����ȱʡ���ò���
function pc_default(void)
	TVCC_VOLT = 3.3			--����CPU�����ѹTVCC
	
	VERIFY_MODE = 2			--У��ģʽ: 0:�Զ�(FLM�ṩУ�麯�������) 1:����  2:���CRC32  3:STM32Ӳ��CRC32

	REMOVE_RDP_POWEROFF = 1 --д��OB����Ҫ�ϵ�
	POWEROFF_TIME1 = 0   	--д��OB�ӳ�ʱ��ms
	POWEROFF_TIME2 = 100   	--�ϵ�ʱ��ms
	POWEROFF_TIME3 = 20   	--�ϵ��ȴ�ʱ��ms

	SWD_CLOCK_DELAY_0 = 0 	--��·��¼ʱ��ʱ���ӳ�
	SWD_CLOCK_DELAY_1 = 0 	--��·��¼ʱ��ʱ���ӳ�
	
	AUTO_REMOVE_PROTECT = 0 --�Զ����������

	NOTE_PC = "Ŀ������й��磬TVCC����Ͽ�,�����RESET����"

	SWD_RESET_DELAY = 500	--���ְ�����Ӳ����λ��Ҫ�ϳ����ӳ�ʱ�� ms (��д��20ms)	
	
	RESET_MODE = 0  		--0:�Զ�ģʽ,  1:���ģʽ  2:Ӳ��UnderReset
	
	ERASE_CHIP_MODE = 0		--0������������1����Ƭ����
end

---------------------------����-----------------------------------
