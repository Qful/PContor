-------------------------------------------------------
-- �ļ��� : N76E003.lua
-- ��  �� : V1.0  2020-04-28
-- ˵  �� : 
-------------------------------------------------------

print("load \"N76E003.lua\" ok")

IncludeList = {
	"0:/H7-TOOL/Programmer/Device/Nuvoton/Lib/cfg_8051.lua"
}

function config_cpu(void)
	DeviceList = {
		--��������, APROM���ֵ,  LDROM���ֵ,  SPROM���ֵ
		"N76E003",  18 * 1024, 		4 * 1024,	0,
		"N76E616",  18 * 1024, 		4 * 1024,	0,
		"N76E885",  18 * 1024, 		4 * 1024,	0,
		
		--����δ����
		"N79E715",  18 * 1024, 		4 * 1024,	0,
		"N79E844",  18 * 1024, 		4 * 1024,	0,
		"N79E8432", 18 * 1024, 		4 * 1024,	0,
		"N79E825",  18 * 1024, 		4 * 1024,	0,
		"N79E824",  18 * 1024, 		4 * 1024,	0,
		"N79E823",  18 * 1024, 		4 * 1024,	0,
		"N79E822",  18 * 1024, 		4 * 1024,	0,
	}

	CHIP_TYPE = "NUVOTON_8051"		--ָ�������ӿ�����: "SWD", "SWIM", "SPI", "I2C", "UART"

	APROM_ADDRESS = 0x00000000
	LDROM_ADDRESS = 0x00010000	--�����ַ��+0x10000
	SPROM_ADDRESS = 0x00020000	--�����ַ��+0x20000

	FLASH_ADDRESS = APROM_ADDRESS
	EEPROM_ADDRESS = 0x004000

	for i = 1, #DeviceList, 4 do
		if (DEVICE == DeviceList[i]) then
			APROM_SIZE = DeviceList[i + 1]	--FLASH������, APROM
			LDROM_SIZE = DeviceList[i + 2]	--EEPROM����, LDROM
			SPROM_SIZE = DeviceList[i + 3]	--EEPROM����, LDROM			

			break
		end
	end

	UID_ADDR = 0			--UID��ַ����ͬ��CPU��ͬ

	OB_ADDRESS     = "0000 0001 0002 0003 0004 0005 0006 0007"

	OB_SECURE_OFF  = "FF FF FF FF FF FF FF FF"	--SECURE_ENABLE = 0ʱ�������Ϻ�д���ֵ (���������ֽڣ�
	OB_SECURE_ON   = "FD FF FF FF FF FF FF FF"	--SECURE_ENABLE = 1ʱ�������Ϻ�д���ֵ
	
	--�ж϶�������д����������(WRP = Write protection)
	OB_WRP_ADDRESS   = {0x00000000} 	--�ڴ��ַ
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

	NOTE_PC = "LDROMҲ��APROM�㷨,�û��Լ��������¼��ַ"	
	
	--��PC���ѡ���㷨���б�
	AlgoFile_List = {
		"APROM",
		--"LDROM",	LDROM���ݿ�����APROM�㷨һ��д��
		--"SPROM",
	}	
end

---------------------------����-----------------------------------
