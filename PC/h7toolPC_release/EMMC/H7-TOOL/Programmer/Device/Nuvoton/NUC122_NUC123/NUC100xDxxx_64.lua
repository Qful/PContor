
-------------------------------------------------------
-- �ļ��� : NUC100xDxxx_64.lua
-- ��  �� : V1.0  2020-04-28
-- ˵  �� : MM32F003TW, MM32F003NW
-------------------------------------------------------
function config_cpu(void)
	CHIP_TYPE = "SWD"		--ָ�������ӿ�����: "SWD", "SWIM", "SPI", "I2C", "UART"

	AlgoFile_AP	   = "0:/H7-TOOL/Programmer/Device/Nuvoton/FLM/NUC100_AP_64.FLM"
	AlgoFile_LD	   = "0:/H7-TOOL/Programmer/Device/Nuvoton/FLM/NUC100_LD_4.FLM"
	AlgoFile_DAT   = "0:/H7-TOOL/Programmer/Device/Nuvoton/FLM/NUC100_DAT.FLM"
	AlgoFile_CFG   = "0:/H7-TOOL/Programmer/Device/Nuvoton/FLM/NUC100_CFG.FLM"

	AlgoFile_FLASH = AlgoFile_AP
	AlgoFile_OPT = AlgoFile_CFG

	AlgoFile_OTP   = ""
	AlgoFile_QSPI  = ""

	FLASH_ADDRESS = 0x00000000		--CPU�ڲ�FLASH��ʼ��ַ

	RAM_ADDRESS = 0x20000000		--CPU�ڲ�RAM��ʼ��ַ

	--Flash�㷨�ļ������ڴ��ַ�ʹ�С
	AlgoRamAddr = RAM_ADDRESS
	AlgoRamSize = 4 * 1024

	MCU_ID = 0x0BB11477

	UID_ADDR = 0x1FFFF7AC	   	--UID��ַ����ͬ��CPU��ͬ
	UID_BYTES = 12

	--ȱʡУ��ģʽ
	VERIFY_MODE = 0				-- 0:����У��, 1:���CRC32У��, ����:��չӲ��CRC(��Ҫ��Ƭ��֧�֣�

	ERASE_CHIP_TIME = 5000		--ȫƬ����ʱ��ms�������ڽ���ָʾ)

	OB_ADDRESS     = "00300000 00300001 00300002 00300003 00300004 00300005 00300006 00300007"

	OB_SECURE_OFF  = "FF FF FF FF FF FF FF FF"	--SECURE_ENABLE = 0ʱ�������Ϻ�д���ֵ(�������)
	OB_SECURE_ON   = "FD FF FF FF FF FF FF FF"	--SECURE_ENABLE = 1ʱ�������Ϻ�д���ֵ(оƬ����)

	--�ж϶�������д����������(WRP = Write protection)
	OB_WRP_ADDRESS   = {0x00300000} 	--�ڴ��ַ
	OB_WRP_MASK  	 = {0x02}		--�����������������
	OB_WRP_VALUE 	 = {0x02}		--�����������Ƚϣ���ȱ�ʾû�б���

	if (MCU_Init ~= nil) then
		MCU_Init()
	end
end

---------------------------����-----------------------------------
