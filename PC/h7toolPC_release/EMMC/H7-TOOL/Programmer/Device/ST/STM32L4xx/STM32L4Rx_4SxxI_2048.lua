-------------------------------------------------------
-- �ļ��� : STM32L4Rx_4SxxI_2048.lua
-- ��  �� : V1.0  2020-04-28
-- ˵  �� :
-------------------------------------------------------

print("load \"STM32L4Rx_4SxxI_2048.lua\" ok")

IncludeList = {
	"0:/H7-TOOL/Programmer/Device/ST/Lib/STM32L4_Lib_E0042000.lua"
}

function config_cpu(void)
	CHIP_TYPE = "SWD"		--ָ�������ӿ�����: "SWD", "SWIM", "SPI", "I2C", "UART"

	AlgoFile_FLASH =  "0:/H7-TOOL/Programmer/Device/ST/STM32L4xx/STM32L4Rx_2048_Dual.FLM"
	AlgoFile_EEPROM = ""
	AlgoFile_OTP   = ""
	AlgoFile_OPT   = "0:/H7-TOOL/Programmer/Device/ST/STM32L4xx/STM32L4Rx_DB_OPT.FLM"
	AlgoFile_QSPI  = ""

	FLASH_ADDRESS = 0x08000000		--CPU�ڲ�FLASH��ʼ��ַ
	FLASH_SIZE = 2 * 1024 * 1024	--оƬʵ��Flash SizeС��FLM�еĶ����size

	RAM_ADDRESS = 0x20000000		--CPU�ڲ�RAM��ʼ��ַ

	--Flash�㷨�ļ������ڴ��ַ�ʹ�С
	AlgoRamAddr = 0x20000000
	AlgoRamSize = 2 * 1024

	MCU_ID = 0x2BA01477

	UID_ADDR = 0x1FFF7590	   	--UID��ַ����ͬ��CPU��ͬ
	UID_BYTES = 12

	ERASE_CHIP_TIME = 2000		--ȫƬ����ʱ��ms�������ڽ���ָʾ)

	--��ַ���е�FFFFFFFF��ʾԭʼ�����в����ϸ��ֽڵķ��� FFFFFFFE��ʾԭʼ�����в���ǰ2���ֽڵķ���
	OB_ADDRESS     = "1FF00000 1FF00001 1FF00002 1FF00003 1FF00008 1FF00009 1FF0000A 1FF0000B "
				   .."1FF00010 1FF00011 1FF00012 1FF00013 1FF00018 1FF00019 1FF0001A 1FF0001B "
 				   .."1FF00020 1FF00021 1FF00022 1FF00023 "

				   .."1FF01008 1FF01009 1FF0100A 1FF0100B "
				   .."1FF01010 1FF01011 1FF01012 1FF01013 "
				   .."1FF01018 1FF01019 1FF0101A 1FF0101B "
				   .."1FF01020 1FF01021 1FF01022 1FF01023 "

	OB_SECURE_OFF  = "AA 70 EF 0F FF FF FF FF 00 00 FF FF FF FF 00 FF "
				   .."FF FF 00 FF "		--SECURE_ENABLE = 0ʱ�������Ϻ�д���ֵ (���������ֽڣ�
				   .."FF FF FF FF 00 00 FF FF FF FF 00 FF FF FF 00 FF "
	OB_SECURE_ON   = "00 70 EF 0F FF FF FF FF 00 00 FF FF FF FF 00 FF "
				   .."FF FF 00 FF "		--SECURE_ENABLE = 1ʱ�������Ϻ�д���ֵ
				   .."FF FF FF FF 00 00 FF FF FF FF 00 FF FF FF 00 FF "

	--�ж϶�������д����������(WRP = Write protection)
	OB_WRP_ADDRESS   = {0x1FF00000,
						0x1FF00008, 0x1FF00010, 0x1FF00018, 0x1FF0001A, 0x1FF00020, 0x1FF00022,
						0x1FF01008, 0x1FF01010, 0x1FF01018, 0x1FF0101A, 0x1FF01020, 0x1FF01022,
						} 	--�ڴ��ַ

	OB_WRP_MASK  	 = {0xFF,
						0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
						0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}		--�����������������
	OB_WRP_VALUE 	 = {0xAA,
						0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
						0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00}		--�����������Ƚϣ���ȱ�ʾû�б���
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
