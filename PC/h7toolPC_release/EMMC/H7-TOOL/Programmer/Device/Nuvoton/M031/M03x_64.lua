-------------------------------------------------------
-- �ļ��� : M03x_64.lua
-- ��  �� : V1.0  2020-04-28
-- ˵  �� : 
-------------------------------------------------------

print("load \"M03x_64.lua\" ok")

IncludeList = {
	"0:/H7-TOOL/Programmer/Device/Nuvoton/Lib/cfg_m0.lua"
}

function config_cpu(void)
	CHIP_TYPE = "SWD"		--ָ�������ӿ�����: "SWD", "SWIM", "SPI", "I2C", "UART"

	AlgoFile_AP	   = "0:/H7-TOOL/Programmer/Device/Nuvoton/FLM/M031_AP_64.FLM"
	AlgoFile_LD	   = "0:/H7-TOOL/Programmer/Device/Nuvoton/FLM/M031_LD_2.FLM"
	AlgoFile_DAT   = ""
	AlgoFile_CFG   = ""

	AlgoFile_FLASH = AlgoFile_AP
	AlgoFile_OPT = AlgoFile_CFG

	AlgoFile_OTP   = ""
	AlgoFile_QSPI  = ""

	FLASH_ADDRESS = 0x00000000		--CPU�ڲ�FLASH��ʼ��ַ

	RAM_ADDRESS = 0x20000000		--CPU�ڲ�RAM��ʼ��ַ

	--Flash�㷨�ļ������ڴ��ַ�ʹ�С
	AlgoRamAddr = RAM_ADDRESS
	AlgoRamSize = 4 * 1024

	MCU_ID = 0x2BA01477

	UID_ADDR = 0x1FFFF7AC	   	--UID��ַ����ͬ��CPU��ͬ
	UID_BYTES = 12

	--ȱʡУ��ģʽ
	VERIFY_MODE = 0				-- 0:����У��, 1:���CRC32У��, ����:��չӲ��CRC(��Ҫ��Ƭ��֧�֣�

	ERASE_CHIP_TIME = 5000		--ȫƬ����ʱ��ms�������ڽ���ָʾ)

	OB_ADDRESS     = "00300000 00300001 00300002 00300003 00300004 00300005 00300006 00300007"

	OB_SECURE_OFF  = "FF FF FF FF FF FF FF FF"	--SECURE_ENABLE = 0ʱ�������Ϻ�д���ֵ(�������)
	OB_SECURE_ON   = "FD FF FF FF FF FF FF FF"	--SECURE_ENABLE = 1ʱ�������Ϻ�д���ֵ(оƬ����)
	-- 0x7FFFFFE7

	--�ж϶�������д����������(WRP = Write protection)
	OB_WRP_ADDRESS   = {0x00300000} 	--�ڴ��ַ
	OB_WRP_MASK  	 = {0x02}		--�����������������
	OB_WRP_VALUE 	 = {0x02}		--�����������Ƚϣ���ȱ�ʾû�б���

	if (MCU_Init ~= nil) then
		MCU_Init()
	end
end

--����PC���, ����ȱʡ���ò���
function pc_default(void)
	TVCC_VOLT = 3.3			--����CPU�����ѹTVCC
	
	VERIFY_MODE = 1			--У��ģʽ: 0:�Զ�(FLM�ṩУ�麯�������) 1:����  2:���CRC32  3:STM32Ӳ��CRC32

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
