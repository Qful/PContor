-------------------------------------------------------
-- �ļ��� : STM32G47x_48xxC_256.lua
-- ��  �� : V1.0  2020-04-28
-- ˵  �� :
-------------------------------------------------------

print("load \"STM32G47x_48xxC_256.lua\" ok")

IncludeList = {
	"0:/H7-TOOL/Programmer/Device/ST/Lib/STM32G4_Lib_E0042000.lua"
}

function config_cpu(void)
	CHIP_TYPE = "SWD"		--ָ�������ӿ�����: "SWD", "SWIM", "SPI", "I2C", "UART"

	AlgoFile_FLASH = "0:/H7-TOOL/Programmer/Device/ST/STM32G4xx/STM32G4xx_256.FLM"
	AlgoFile_OTP   = ""
	AlgoFile_OPT   = "0:/H7-TOOL/Programmer/Device/ST/STM32G4xx/STM32G4xx_CAT3_OPT.FLM"
	AlgoFile_QSPI  = ""

	FLASH_ADDRESS = 0x08000000		--CPU�ڲ�FLASH��ʼ��ַ

	RAM_ADDRESS = 0x20000000		--CPU�ڲ�RAM��ʼ��ַ

	--Flash�㷨�ļ������ڴ��ַ�ʹ�С
	AlgoRamAddr = RAM_ADDRESS
	AlgoRamSize = 4 * 1024

	MCU_ID = 0x2BA01477

	UID_ADDR = 0x1FFF7590	   	--UID��ַ��STM32G0 �ֲ���û��UID
	UID_BYTES = 12

	--ȱʡУ��ģʽ
	VERIFY_MODE = 0				-- 0:����У��, 1:���CRC32У��, ����:��չӲ��CRC(��Ҫ��Ƭ��֧�֣�

	ERASE_CHIP_TIME = 500		--ȫƬ����ʱ��ms�������ڽ���ָʾ)

	OB_ADDRESS     =  "1FFF7800 1FFF7801 1FFF7802 1FFF7803 "
					.."1FFF7808 1FFF7809 1FFF780A 1FFF780B "
					.."1FFF7810 1FFF7811 1FFF7812 1FFF7813 "
					.."1FFF7818 1FFF7819 1FFF781A 1FFF781B "
					.."1FFF7820 1FFF7821 1FFF7822 1FFF7823 "
					.."1FFF7828 1FFF7829 1FFF782A 1FFF782B "

					.."1FFFF808 1FFFF809 1FFFF80A 1FFFF80B "
					.."1FFFF810 1FFFF811 1FFFF812 1FFFF813 "
					.."1FFFF818 1FFFF819 1FFFF81A 1FFFF81B "
					.."1FFFF820 1FFFF821 1FFFF822 1FFFF823 "					
					.."1FFFF828 1FFFF829 1FFFF82A 1FFFF82B "

	--SECURE_ENABLE = 0ʱ�������Ϻ�д���ֵ(�������)
	OB_SECURE_OFF  =  "AA F8 FE FF "
					.."FF FF FF FF "
					.."00 00 FF 00 "
					.."FF FF 00 FF "	
					.."FF FF 00 FF "
					.."00 FF 00 FF "
					
					.."FF FF FF FF "
					.."00 00 FF 00 "
					.."FF FF 00 FF "
					.."FF FF 00 FF "
					.."00 FF 00 FF "

	--SECURE_ENABLE = 1ʱ�������Ϻ�д���ֵ(оƬ����)
	OB_SECURE_ON   =  "BB F8 FE FF "
					.."FF FF FF FF "
					.."00 00 FF 00 "
					.."FF FF 00 FF "	
					.."FF FF 00 FF "
					.."00 FF 00 FF "
					
					.."FF FF FF FF "
					.."00 00 FF 00 "
					.."FF FF 00 FF "
					.."FF FF 00 FF "
					.."00 FF 00 FF "

	--�ж϶�������д����������(WRP = Write protection)
	OB_WRP_ADDRESS   = {0x1FFF7800}  --�ڴ��ַ
	OB_WRP_MASK  	 = {0xFF}		--�����������������
	OB_WRP_VALUE 	 = {0xAA}		--�����������Ƚϣ���ȱ�ʾû�б���
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
