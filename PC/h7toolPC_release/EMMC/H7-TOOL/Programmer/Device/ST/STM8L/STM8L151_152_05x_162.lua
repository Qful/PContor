
-------------------------------------------------------
-- �ļ��� : STM8L151_152.lua
-- ��  �� : V1.0  2020-04-28
-- ˵  �� :
-------------------------------------------------------
function config_cpu(void)
	DeviceList = {
		"STM8L151F2",  4 * 1024, 256,
		"STM8L151G2",  4 * 1024, 256,
		"STM8L151K2",  4 * 1024, 256,
		"STM8L151C2",  4 * 1024, 256,

		"STM8L151F3",  8 * 1024, 256,
		"STM8L151G3",  8 * 1024, 256,
		"STM8L151K3",  8 * 1024, 256,
		"STM8L151C3",  8 * 1024, 256,

		"STM8L151G4",  16 * 1024, 1 * 1024,
		"STM8L151G4",  16 * 1024, 1 * 1024,
		"STM8L151C4",  16 * 1024, 1 * 1024,

		"STM8L151G6",  32 * 1024, 1 * 1024,
		"STM8L151G6",  32 * 1024, 1 * 1024,
		"STM8L151C6",  32 * 1024, 1 * 1024,

		"STM8L151C8",  64 * 1024, 2 * 1024,
		"STM8L151K8",  64 * 1024, 2 * 1024,
		"STM8L151R8",  64 * 1024, 2 * 1024,
		"STM8L151M8",  64 * 1024, 2 * 1024,
		"STM8L151R6",  32 * 1024, 2 * 1024,

		"STM8L152C4",  16 * 1024, 1 * 1024,
		"STM8L152C6",  32 * 1024, 1 * 1024,
		"STM8L152C8",  64 * 1024, 2 * 1024,
		"STM8L152K4",  16 * 1024, 1 * 1024,
		"STM8L152K6",  32 * 1024, 1 * 1024,
		"STM8L152K8",  64 * 1024, 2 * 1024,
		"STM8L152R8",  64 * 1024, 2 * 1024,
		"STM8L152M8",  64 * 1024, 2 * 1024,
		"STM8L152R6",  32 * 1024, 2 * 1024,

		"STM8L162R8",  64 * 1024, 2 * 1024,
		"STM8L162M6",  64 * 1024, 2 * 1024,

		"STM8L050J3",  8 * 1024,  256,
		"STM8L051F3",  8 * 1024,  256,
		"STM8L052R8",  64 * 1024, 256,
		"STM8L052C6",  32 * 1024, 256,
	}

	CHIP_TYPE = "SWIM"			--ָ�������ӿ�����: "SWD", "SWIM", "SPI", "I2C"

	STM8_SERIAL = "STM8L"		--ѡ��2��ϵ��: "STM8S" �� "STM8L"

	FLASH_ADDRESS = 0x008000	--����FLASH��ʼ��ַ

	EEPROM_ADDRESS = 0x001000 	--����EEPROM��ʼ��ַ(STM8S��STM8L��ͬ��

	for i = 1, #DeviceList, 3 do
		if (DEVICE == DeviceList[i]) then
			FLASH_SIZE  = DeviceList[i + 1]	--FLASH������
			EEPROM_SIZE = DeviceList[i + 2]	--EEPROM����

			--����BLOCK SIZE, ֻ��64��128����
			if (FLASH_SIZE <= 8 * 1024) then
				FLASH_BLOCK_SIZE = 64
			else
				FLASH_BLOCK_SIZE = 128
			end

			break
		end
	end

	UID_ADDR = 0x4926			--UID��ַ����ͬ��CPU��ͬ

	OB_ADDRESS     = "4800 4802 4807 4808 4809 480A 480B 480C"

	OB_SECURE_OFF  = "AA 00 00 00 00 00 00 00 00"	--SECURE_ENABLE = 0ʱ�������Ϻ�д���ֵ (���������ֽڣ�
	OB_SECURE_ON   = "00 00 00 00 00 00 00 00 00"	--SECURE_ENABLE = 1ʱ�������Ϻ�д���ֵ

	MCU_REMOVE_PROTECT = 1		--1��ʾʹ�� MCU_RemoveProtect() �������
end

-- STM8L05x/15x, medium density STM8L05x/15x and STM8AL31xx/STM8AL3Lxx and high density STM8L05x/15x/16x microcontrollers,
-- ��Ҫ����2��д����ܽ������
function MCU_RemoveProtect(void)
	pg_prog_buf_ob("4800", "AA")
	delayms(5)
	pg_prog_buf_ob("4800", "AA")
	delayms(5)
	pg_reset()
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

	--��PC���ѡ���㷨���б�
	AlgoFile_List = {
		"FLASH",
		"EEPROM",
	}	
end

---------------------------����-----------------------------------
