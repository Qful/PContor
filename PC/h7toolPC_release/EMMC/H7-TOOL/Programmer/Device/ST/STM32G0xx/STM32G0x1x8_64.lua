-------------------------------------------------------
-- �ļ��� : STM32G0x1x8_64.lua
-- ��  �� : V1.0  2020-04-28
-- ˵  �� :
-------------------------------------------------------

print("load \"STM32G0x1x8_64.lua\" ok")

IncludeList = {
	"0:/H7-TOOL/Programmer/Device/ST/Lib/STM32G0_Lib_40015800.lua"
}

function config_cpu(void)
	CHIP_TYPE = "SWD"		--ָ�������ӿ�����: "SWD", "SWIM", "SPI", "I2C", "UART"

	AlgoFile_FLASH = "0:/H7-TOOL/Programmer/Device/ST/STM32G0xx/STM32G0xx_64.FLM"
	AlgoFile_OTP   = "0:/H7-TOOL/Programmer/Device/ST/STM32G0xx/STM32G0xx_OTP.FLM"
	AlgoFile_OPT   = "0:/H7-TOOL/Programmer/Device/ST/STM32G0xx/STM32G0x1_OPT.FLM"
	AlgoFile_QSPI  = ""

	FLASH_ADDRESS = 0x08000000		--CPU�ڲ�FLASH��ʼ��ַ

	OTP_ADDRESS = 0x1FFF7000		--CPU�ڲ�OTP����ַ
	OTP_SIZE = 1024

	RAM_ADDRESS = 0x20000000		--CPU�ڲ�RAM��ʼ��ַ

	--Flash�㷨�ļ������ڴ��ַ�ʹ�С
	AlgoRamAddr = RAM_ADDRESS
	AlgoRamSize = 4 * 1024

	MCU_ID = 0x0BC11477

	UID_ADDR = 0x1FFF7590	   	--UID��ַ��STM32G0 �ֲ���û��UID
	UID_BYTES = 12

	--ȱʡУ��ģʽ
	VERIFY_MODE = 0				-- 0:����У��, 1:���CRC32У��, ����:��չӲ��CRC(��Ҫ��Ƭ��֧�֣�

	ERASE_CHIP_TIME = 500		--ȫƬ����ʱ��ms�������ڽ���ָʾ)

	OB_ADDRESS     =  "1FFF7800 1FFF7801 1FFF7802 1FFF7803 "		--FFFF FEAA
					.."1FFF7808 1FFF7809 1FFF780A 1FFF780B "		--FFFF FFFF
					.."1FFF7810 1FFF7811 1FFF7812 1FFF7813 "		--0000 0000
					.."1FFF7818 1FFF7819 1FFF781A 1FFF781B "		--0000 000F
					.."1FFF7820 1FFF7821 1FFF7822 1FFF7823 "		--0000 000F
					.."1FFF7828 1FFF7829 1FFF782A 1FFF782B "		--FFFF FFFF
					.."1FFF7830 1FFF7831 1FFF7832 1FFF7833 "		--0000 0000
					.."1FFF7870 1FFF7871 1FFF7872 1FFF7873 "		--0000 0000

	OB_SECURE_OFF  = "AA FE FF FF  FF FF FF FF  00 00 00 00  0F 00 00 00  0F 00 00 00  FF FF FF FF 00 00 00 00  00 00 00 00 "	--SECURE_ENABLE = 0ʱ�������Ϻ�д���ֵ(�������)

	OB_SECURE_ON   = "00 FE FF FF  FF FF FF FF  00 00 00 00  0F 00 00 00  0F 00 00 00  FF FF FF FF 00 00 00 00  00 00 00 00  "	--SECURE_ENABLE = 1ʱ�������Ϻ�д���ֵ(оƬ����)

	--�ж϶�������д����������(WRP = Write protection)
	OB_WRP_ADDRESS   = {0x1FFF7800, 0x1FFF7818, 0x1FFF781A, 0x1FFF7820, 0x1FFF7822} 	--�ڴ��ַ
	OB_WRP_MASK  	 = {0xFF, 0x3F, 0x3F, 0x3F, 0x3F}		--�����������������
	OB_WRP_VALUE 	 = {0xAA, 0x1F, 0x00, 0x1F, 0x00}		--�����������Ƚϣ���ȱ�ʾû�б���
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
