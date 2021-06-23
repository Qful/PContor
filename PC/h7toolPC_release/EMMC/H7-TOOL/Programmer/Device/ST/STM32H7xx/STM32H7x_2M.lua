-------------------------------------------------------
-- �ļ��� : STM32H7x_2M.lua
-- ��  �� : V1.0  2020-04-23 armfly
-- ˵  �� : STM32H743��STM32H750, 2MB Flash�㷨
-------------------------------------------------------

print("load \"STM32H7x_2M.lua\" ok")

IncludeList = {
	"0:/H7-TOOL/Programmer/Device/ST/Lib/STM32H7_Lib_5C001000.lua"
}

function config_cpu(void)
	CHIP_TYPE = "SWD"		--ָ�������ӿ�����: "SWD", "SWIM", "SPI", "I2C", "UART"

	AlgoFile_FLASH = "0:/H7-TOOL/Programmer/Device/ST/STM32H7xx/STM32H7x_2M.FLM"
	AlgoFile_OTP   = ""			--OTP�㷨�ļ�
	AlgoFile_OPT   = ""			--Option Bytes �㷨�ļ�
	AlgoFile_QSPI  = "0:/H7-TOOL/Programmer/Device/ST/STM32H7xx/STM32H7XX_H7-TOOL_W25Q256.FLM"	--Ƭ��QSPI�㷨�ļ�

	FLASH_ADDRESS = 0x08000000		--CPU�ڲ�FLASH��ʼ��ַ

	RAM_ADDRESS = 0x20000000		--CPU�ڲ�RAM��ʼ��ַ

	--Flash�㷨�ļ������ڴ��ַ�ʹ�С
	AlgoRamAddr = 0x20000000
	AlgoRamSize = 64*1024

	MCU_ID = 0x6BA02477

	UID_ADDR = 0x1FF1E800	   	--UID��ַ����ͬ��CPU��ͬ
	UID_BYTES = 12

	--ȱʡУ��ģʽ
	VERIFY_MODE = 0				-- 0:����У��, 1:���CRC32У��, ����:��չӲ��CRC(��Ҫ��Ƭ��֧�֣�

	ERASE_CHIP_TIME = 12000		--ȫƬ����ʱ��ms�������ڽ���ָʾ)

	OB_ADDRESS     = "5200201C 5200201D 5200201E 5200201F "..
					 "52002028 52002029 5200202A 5200202B "..
					 "52002030 52002031 52002032 52002033 "..
					 "52002038 "..
					 "52002040 52002041 52002042 52002043 "..
					 "52002128 52002129 5200212A 5200212B "..
					 "52002130 52002131 52002132 52002133 "..
					 "52002138"

	OB_SECURE_OFF  = "F0AAC603  FF000000 FF000000 FF 00080000 FF000000 FF000000 FF"	--SECURE_ENABLE = 0ʱ�������Ϻ�д���ֵ
	OB_SECURE_ON   = "F000C603  FF000000 FF000000 FF 00080000 FF000000 FF000000 FF"	--SECURE_ENABLE = 1ʱ�������Ϻ�д���ֵ

	--�ж϶�������д����������(WRP = Write protection)
	OB_WRP_ADDRESS   = {0x5200201D, 0x52002038,0x52002138} 	--�ڴ��ַ
	OB_WRP_MASK  	 = {0xFF, 0xFF, 0xFF}					--�����������������
	OB_WRP_VALUE 	 = {0xAA, 0xFF, 0xFF}					--�����������Ƚϣ���ȱ�ʾû��д����
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
