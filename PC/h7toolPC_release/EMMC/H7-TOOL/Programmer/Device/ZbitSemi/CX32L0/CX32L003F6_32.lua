-------------------------------------------------------
-- �ļ��� : CX32L003F6_32.lua
-- ��  �� : V1.0  2020-09-29
-- ˵  �� :
-------------------------------------------------------

print("load \"CX32L003F6_32.lua\" ok")

IncludeList = {
	"0:/H7-TOOL/Programmer/Device/ZbitSemi/Lib/CX32L0_Lib.lua"
}

function config_cpu(void)
	CHIP_TYPE = "SWD"		--ָ�������ӿ�����: "SWD", "SWIM", "SPI", "I2C", "UART"

	AlgoFile_FLASH = "0:/H7-TOOL/Programmer/Device/ZbitSemi/FLM/CX32L003F6.FLM"
	AlgoFile_OTP   = ""
	AlgoFile_OPT   = "0:/H7-TOOL/Programmer/Device/ZbitSemi/FLM/CX32L003_OB.FLM"
	AlgoFile_QSPI  = ""

	FLASH_ADDRESS = 0x00000000		--CPU�ڲ�FLASH��ʼ��ַ

	RAM_ADDRESS = 0x20000000		--CPU�ڲ�RAM��ʼ��ַ

	--Flash�㷨�ļ������ڴ��ַ�ʹ�С
	AlgoRamAddr = RAM_ADDRESS
	AlgoRamSize = 4 * 1024

	MCU_ID = 0x0BC11477

	UID_ADDR = 0x180000F0	  	--UID��ַ��16�ֽ�
	UID_BYTES = 16

	--ȱʡУ��ģʽ
	VERIFY_MODE = 0				--У��ģʽ: 0:�Զ�(FLM�ṩУ�麯�������) 1:����  2:���CRC32  3:STM32Ӳ��CRC32

	ERASE_CHIP_TIME = 500		--ȫƬ����ʱ��ms�������ڽ���ָʾ)

	OB_ADDRESS     =  "08000000 08000001 08000002 08000003 "
					.."08000004 08000005 08000006 08000007 "
					.."08000008 08000009 0800000A 0800000B "
					
	OB_SECURE_OFF  = "FF FF FF FF FF FF FF FF FF FF FF FF"	--SECURE_ENABLE = 0ʱ�������Ϻ�д���ֵ(�������)
	OB_SECURE_ON   = "FF FF FF FF FE FF FF FF FF FF FF FF"	--SECURE_ENABLE = 1ʱ�������Ϻ�д���ֵ(оƬ����)

	--�ж϶�������д����������(WRP = Write protection)
	OB_WRP_ADDRESS   = {0x08000004} 	--�ڴ��ַ
	OB_WRP_MASK  	 = {0x01}		--�����������������
	OB_WRP_VALUE 	 = {0x01}		--�����������Ƚϣ���ȱ�ʾû�б���
end

--����PC���, ����ȱʡ���ò���
function pc_default(void)
	TVCC_VOLT = 3.3			--����CPU�����ѹTVCC
	
	VERIFY_MODE = 1			--У��ģʽ: 0:�Զ�(FLM�ṩУ�麯�������) 1:����  2:���CRC32  3:STM32Ӳ��CRC32

	REMOVE_RDP_POWEROFF = 0 --д��OB����Ҫ�ϵ�
	POWEROFF_TIME1 = 0   	--д��OB�ӳ�ʱ��ms
	POWEROFF_TIME2 = 100   	--�ϵ�ʱ��ms
	POWEROFF_TIME3 = 20   	--�ϵ��ȴ�ʱ��ms

	SWD_CLOCK_DELAY_0 = 1 	--��·��¼ʱ��ʱ���ӳ�
	SWD_CLOCK_DELAY_1 = 1 	--��·��¼ʱ��ʱ���ӳ�
	
	AUTO_REMOVE_PROTECT = 1 --�Զ����������

	NOTE_PC = ""
	
	--��PC���ѡ���㷨���б�
	AlgoFile_List = {
		"AlgoFile_FLASH"
	}
end

---------------------------����-----------------------------------
