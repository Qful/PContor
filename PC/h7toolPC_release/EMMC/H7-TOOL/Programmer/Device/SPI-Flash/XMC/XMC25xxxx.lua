-------------------------------------------------------
-- �ļ��� : XMC25xxxx.lua
-- ��  �� : V1.0  2020-11-06
-- ˵  �� : 
-------------------------------------------------------
function config_cpu(void)
	DeviceList = {
		--��������, ����(KB),   ����������С,   ����ָ��, 	ȫƬ������ʱ, 	���AAIģʽ(1��ʾAAI, 0��ʾpage 256д)	
		"XMC25QH32",   4 * 1024, 	64 * 1024,  	0xD8,		10000,		0,   
		"XMC25QH64",   8 * 1024, 	64 * 1024,  	0xD8,		20000,   	0,		
		"XMC25QH128",  16 * 1024, 	64 * 1024,  	0xD8,		50000,   	0,    
		"XMC25QH256",  32 * 1024, 	64 * 1024,  	0xD8,		100000,   	0,
		
		"XMC25QW16",   2 * 1024, 	64 * 1024,  	0xD8,		5000,		0,   
		"XMC25QW32",   4 * 1024, 	64 * 1024,  	0xD8,		10000,		0,   
		"XMC25QW64",   8 * 1024, 	64 * 1024,  	0xD8,		20000,   	0,		
		"XMC25QW128",  16 * 1024, 	64 * 1024,  	0xD8,		50000,   	0,    
		"XMC25QW256",  32 * 1024, 	64 * 1024,  	0xD8,		100000,   	0, 

		"XMC25QU16",   2 * 1024, 	64 * 1024,  	0xD8,		5000,		0,   
		"XMC25QU32",   4 * 1024, 	64 * 1024,  	0xD8,		10000,		0,   
		"XMC25QU64",   8 * 1024, 	64 * 1024,  	0xD8,		20000,   	0,		
		"XMC25QU128",  16 * 1024, 	64 * 1024,  	0xD8,		50000,   	0,    
		"XMC25QU256",  32 * 1024, 	64 * 1024,  	0xD8,		100000,   	0,  	
	}

	CHIP_TYPE = "SPI"			--ָ�������ӿ�����: "SWD", "SWIM", "SPI", "I2C"

	FLASH_ADDRESS = 0x00000000	--����FLASH��ʼ��ַ

	UNLOCK_CMD = 0x00		--0x00 ��ʾд״̬��Ϊ0x00����  0x98��ʾ��0x98ָ�����
	
	ERASE_CHIP_CMD = 0xC7	--����ȫƬ��ָ�ȱʡ��0xC7
	
	for i = 1, #DeviceList, 6 do
		if (DEVICE == DeviceList[i]) then
			FLASH_SIZE  = DeviceList[i + 1] * 1024	--FLASH������
			SECTOR_SIZE = DeviceList[i + 2]
			ERASE_SECTOR_CMD = DeviceList[i + 3]
			ERASE_SECTOR_TIMEOUT = 1000	--����������ʱ�̶�1��
			ERASE_CHIP_TIMEOUT = DeviceList[i + 4]	--ȫƬ������ʱ
			PROG_AAI = DeviceList[i + 5]
			break
		end
	end

	--�����ֶ�����
	UID_ADDR = 0x00000			--UID��ַ����ͬ��CPU��ͬ

	OB_ADDRESS     = "00000000 00000000 00000000 00000000"

	OB_SECURE_OFF  = "FF FF FF FF"	--SECURE_ENABLE = 0ʱ�������Ϻ�д���ֵ (���������ֽڣ�
	OB_SECURE_ON   = "FF FF FF FF"	--SECURE_ENABLE = 1ʱ�������Ϻ�д���ֵ
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

	ERASE_CHIP_MODE = 0		--0���������� 1����ȫƬ  

	if (FLASH_SIZE >= 1024 * 1024) then
		NOTE_PC = string.format("���� : %d MB", FLASH_SIZE / (1024 * 1024))
	else
		NOTE_PC = string.format("���� : %d KB", FLASH_SIZE / 1024)
	end

	--��PC���ѡ���㷨���б�
	AlgoFile_List = {
		"FLASH",
	}	
end

---------------------------����-----------------------------------
