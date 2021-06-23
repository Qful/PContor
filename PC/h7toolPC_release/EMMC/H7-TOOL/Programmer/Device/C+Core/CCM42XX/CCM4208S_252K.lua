-------------------------------------------------------
-- �ļ��� : CCM4208S_252K.lua
-- ��  �� : V1.0  2020-12-12
-- ˵  �� : ���ݹ�оM4, �����QSPI Flash
-------------------------------------------------------

print("load \"CCM4208S_256K.lua\" ok")

IncludeList = {
	"0:/H7-TOOL/Programmer/Device/C+Core/Lib/CCM4_Lib.lua"
}

function config_cpu(void)
	CHIP_TYPE = "SWD"		--ָ�������ӿ�����: "SWD", "SWIM", "SPI", "I2C", "UART"

	AlgoFile_FLASH = "0:/H7-TOOL/Programmer/Device/C+Core/FLM/CCM4208S_MASS_4k.FLM"
	AlgoFile_OTP   = ""
	AlgoFile_OPT   = ""
	CCM4208S_QSPI  = "0:/H7-TOOL/Programmer/Device/C+Core/FLM/CCM4208S_QSPI.FLM"

	FLASH_ADDRESS = 0x08000000		--CPU�ڲ�FLASH��ʼ��ַ
	FLASH_SIZE = 252 * 1024
	
	RAM_ADDRESS = 0x20000000		--CPU�ڲ�RAM��ʼ��ַ

	--Flash�㷨�ļ������ڴ��ַ�ʹ�С
	AlgoRamAddr = 0x20000000
	AlgoRamSize = 16 * 1024

	MCU_ID = 0x0BB11477

	UID_ADDR = 0x203E4	   	--UID��ַ����ͬ��CPU��ͬ
	UID_BYTES = 12

	--ȱʡУ��ģʽ
	VERIFY_MODE = 0				-- 0:����У��, 1:���CRC32У��, ����:��չӲ��CRC(��Ҫ��Ƭ��֧�֣�

	ERASE_CHIP_TIME = 20000		--ȫƬ����ʱ��ms�������ڽ���ָʾ)

	OB_ADDRESS     =  "00020400 FFFFFFFF 00020402 FFFFFFFF 00020404 FFFFFFFF 00020406 FFFFFFFF 00020408 FFFFFFFF 0002040A FFFFFFFF 0002040C FFFFFFFF 0002040E FFFFFFFF"
					.."00020410 FFFFFFFF 00020412 FFFFFFFF 00020414 FFFFFFFF 00020416 FFFFFFFF"
					
	OB_SECURE_OFF  = "A5 FF FF FF FF FF FF FF"	--SECURE_ENABLE = 0ʱ�������Ϻ�д���ֵ(�������)
	OB_SECURE_ON   = "00 FF FF FF FF FF FF FF"	--SECURE_ENABLE = 1ʱ�������Ϻ�д���ֵ(оƬ����)

	--�ж϶�������д����������(WRP = Write protection)
	OB_WRP_ADDRESS   = {0x00020400, 0x00020408, 0x0002040A, 0x0002040C, 0x0002040E} 	--�ڴ��ַ
	OB_WRP_MASK  	 = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF}		--�����������������
	OB_WRP_VALUE 	 = {0xA5, 0xFF, 0xFF, 0xFF, 0xFF}		--�����������Ƚϣ���ȱ�ʾû�б���
end

--����PC���, ����ȱʡ���ò���
function pc_default(void)
	TVCC_VOLT = 3.3			--����CPU�����ѹTVCC
	
	VERIFY_MODE = 1			--У��ģʽ: 0:�Զ�(FLM�ṩУ�麯�������) 1:����  2:���CRC32  3:STM32Ӳ��CRC32

	REMOVE_RDP_POWEROFF = 0 --д��OB����Ҫ�ϵ�
	POWEROFF_TIME1 = 0   	--д��OB�ӳ�ʱ��ms
	POWEROFF_TIME2 = 100   	--�ϵ�ʱ��ms
	POWEROFF_TIME3 = 20   	--�ϵ��ȴ�ʱ��ms

	SWD_CLOCK_DELAY_0 = 0 	--��·��¼ʱ��ʱ���ӳ�
	SWD_CLOCK_DELAY_1 = 0 	--��·��¼ʱ��ʱ���ӳ�
	
	AUTO_REMOVE_PROTECT = 0 --�Զ����������

	NOTE_PC = "��֧��OB������,��֧�ֽ������"

	--��PC���ѡ���㷨���б�
	AlgoFile_List = {
		"AlgoFile_FLASH",
		"CCM4208S_QSPI",	
	}		
end

---------------------------����-----------------------------------
