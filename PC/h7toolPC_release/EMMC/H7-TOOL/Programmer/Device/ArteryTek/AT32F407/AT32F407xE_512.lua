-------------------------------------------------------
-- �ļ��� : AT32F407xE_512.lua
-- ��  �� : V1.0  2020-09-23
-- ˵  �� :
-------------------------------------------------------

print("load \"AT32F407xE_512.lua\" ok")

IncludeList = {
	"0:/H7-TOOL/Programmer/Device/ArteryTek/Lib/AT32_Lib_E0042000_PWR.lua"
}

function config_cpu(void)
	CHIP_TYPE = "SWD"		--ָ�������ӿ�����: "SWD", "SWIM", "SPI", "I2C", "UART"

	AlgoFile_FLASH = "0:/H7-TOOL/Programmer/Device/ArteryTek/FLM/AT32F407_512.FLM"
	AlgoFile_OTP   = ""
	AlgoFile_OPT   = "0:/H7-TOOL/Programmer/Device/ArteryTek/FLM/AT32F407_UOB.FLM"
	AlgoFile_QSPI  = ""
	
	Algo_EXT_TYPE1_REAMP0  = "0:/H7-TOOL/Programmer/Device/ArteryTek/FLM/AT32F407_EXT_TYPE1_REAMP0_GENERAL.FLM"
	Algo_EXT_TYPE1_REAMP1  = "0:/H7-TOOL/Programmer/Device/ArteryTek/FLM/AT32F407_EXT_TYPE1_REAMP1_GENERAL.FLM"
	Algo_EXT_TYPE2_REAMP0  = "0:/H7-TOOL/Programmer/Device/ArteryTek/FLM/AT32F407_EXT_TYPE2_REAMP0_GENERAL.FLM"
	Algo_EXT_TYPE2_REAMP1  = "0:/H7-TOOL/Programmer/Device/ArteryTek/FLM/AT32F407_EXT_TYPE2_REAMP1_GENERAL.FLM"
	
	FLASH_ADDRESS = 0x08000000		--CPU�ڲ�FLASH��ʼ��ַ

	RAM_ADDRESS = 0x20000000		--CPU�ڲ�RAM��ʼ��ַ

	--Flash�㷨�ļ������ڴ��ַ�ʹ�С
	AlgoRamAddr = RAM_ADDRESS
	AlgoRamSize = 4 * 1024

	MCU_ID = 0x2BA01477

	UID_ADDR = 0x1FFFF7E8	   	--UID��ַ����ͬ��CPU��ͬ
	UID_BYTES = 12

	--ȱʡУ��ģʽ
	VERIFY_MODE = 0				-- 0:����У��, 1:���CRC32У��, ����:��չӲ��CRC(��Ҫ��Ƭ��֧�֣�

	ERASE_CHIP_TIME = 4000		--ȫƬ����ʱ��ms�������ڽ���ָʾ)

	--��ַ���е�FFFFFFFF��ʾԭʼ�����в���һ��ȡ�����ֽ�
	OB_ADDRESS     =  "1FFFF800 FFFFFFFF 1FFFF802 FFFFFFFF 1FFFF804 FFFFFFFF 1FFFF806 FFFFFFFF 1FFFF808 FFFFFFFF 1FFFF80A FFFFFFFF 1FFFF80C FFFFFFFF 1FFFF80E FFFFFFFF "
					.."1FFFF810 FFFFFFFF 1FFFF812 FFFFFFFF 1FFFF814 FFFFFFFF 1FFFF816 FFFFFFFF 1FFFF818 FFFFFFFF 1FFFF81A FFFFFFFF 1FFFF81C FFFFFFFF 1FFFF81E FFFFFFFF "
					.."1FFFF820 FFFFFFFF 1FFFF822 FFFFFFFF 1FFFF824 FFFFFFFF 1FFFF826 FFFFFFFF 1FFFF828 FFFFFFFF 1FFFF82A FFFFFFFF 1FFFF82C FFFFFFFF 1FFFF82E FFFFFFFF "
	OB_SECURE_OFF  =  "A5 FF FF FF FF FF FF FF "	--SECURE_ENABLE = 0ʱ�������Ϻ�д���ֵ
					.."FF FF FF FF FF FF FF FF "
					.."FF FF FF FF FF FF FF FF"
	OB_SECURE_ON   =  "00 FF FF FF FF FF FF FF "	--SECURE_ENABLE = 1ʱ�������Ϻ�д���ֵ
					.."FF FF FF FF FF FF FF FF "
					.."FF FF FF FF FF FF FF FF"	

	--�ж϶�������д����������(WRP = Write protection)
	OB_WRP_ADDRESS   = {0x1FFFF800, 0x1FFFF808, 0x1FFFF80A, 0x1FFFF80C, 0x1FFFF80E} 	--�ڴ��ַ
	OB_WRP_MASK  	 = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF}		--�����������������
	OB_WRP_VALUE 	 = {0xA5, 0xFF, 0xFF, 0xFF, 0xFF}		--�����������Ƚϣ���ȱ�ʾû�б���
end

--����PC���, ����ȱʡ���ò���
function pc_default(void)
	TVCC_VOLT = 3.3			--����CPU�����ѹTVCC
	
	VERIFY_MODE = 3			--У��ģʽ: 0:�Զ�(FLM�ṩУ�麯�������) 1:����  2:���CRC32  3:STM32Ӳ��CRC32

	REMOVE_RDP_POWEROFF = 1 --д��OB����Ҫ�ϵ�
	POWEROFF_TIME1 = 0   	--д��OB�ӳ�ʱ��ms
	POWEROFF_TIME2 = 100   	--�ϵ�ʱ��ms
	POWEROFF_TIME3 = 20   	--�ϵ��ȴ�ʱ��ms

	SWD_CLOCK_DELAY_0 = 0 	--��·��¼ʱ��ʱ���ӳ�
	SWD_CLOCK_DELAY_1 = 0 	--��·��¼ʱ��ʱ���ӳ�
	
	AUTO_REMOVE_PROTECT = 1 --�Զ����������

	NOTE_PC = ""
	
	--��PC���ѡ���㷨���б�
	AlgoFile_List = {
		"AlgoFile_FLASH",
		"Algo_EXT_TYPE1_REAMP0",
		"Algo_EXT_TYPE1_REAMP1",
		"Algo_EXT_TYPE2_REAMP0",
		"Algo_EXT_TYPE2_REAMP1",
	}	
end

---------------------------����-----------------------------------
