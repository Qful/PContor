
-------------------------------------------------------
-- �ļ��� : STM32L5xxxC_256.lua
-- ��  �� : V1.0  2020-04-28
-- ˵  �� : H7-TOOL, by armfly
-------------------------------------------------------

print("load \"STM32L5xxxC_256.lua\" ok")

IncludeList = {
	"0:/H7-TOOL/Programmer/Device/ST/Lib/STM32L5_Lib_E0044000.lua"
}

function config_cpu(void)
	CHIP_TYPE = "SWD"		--ָ�������ӿ�����: "SWD", "SWIM", "SPI", "I2C", "UART"

	AlgoFile_FLASH =  "0:/H7-TOOL/Programmer/Device/ST/STM32L5xx/STM32L5x_512_0800.FLM"
	AlgoFile_EEPROM = ""
	AlgoFile_OTP   = ""
	AlgoFile_OPT   = "0:/H7-TOOL/Programmer/Device/ST/STM32L5xx/STM32L5x_OPT.FLM"
	AlgoFile_QSPI  = ""

	FLASH_ADDRESS = 0x08000000		--CPU�ڲ�FLASH��ʼ��ַ
	FLASH_SIZE = 512 * 1024			--оƬʵ��Flash Size

	RAM_ADDRESS = 0x20000000		--CPU�ڲ�RAM��ʼ��ַ

	--Flash�㷨�ļ������ڴ��ַ�ʹ�С
	AlgoRamAddr = 0x20000000
	AlgoRamSize = 4 * 1024

	MCU_ID = 0x0BE12477

	UID_ADDR = 0x0BFA0590	   	--UID��ַ����ͬ��CPU��ͬ
	UID_BYTES = 12

	ERASE_CHIP_TIME = 30000		--ȫƬ����ʱ��ms�������ڽ���ָʾ)

	--��ַ���е�FFFFFFFF��ʾԭʼ�����в����ϸ��ֽڵķ��� FFFFFFFE��ʾԭʼ�����в���ǰ2���ֽڵķ���
	--FLASH_BASE = 0x40022000  FLASH_OPTR = FLASH_BASE + 0x40
	OB_ADDRESS     = "40022040 40022041 40022042 40022043 40022044 40022045 40022046 40022047 "
				   .."40022048 40022049 4002204A 4002204B 4002204C 4002204D 4002204E 4002204F "
 				   .."40022050 40022051 40022052 40022053 40022054 40022055 40022056 40022057 "
				   .."40022058 40022059 4002205A 4002205B 4002205C 4002205D 4002205E 4002205F "
				   .."40022060 40022061 40022062 40022063 40022064 40022065 40022066 40022067 "
				   .."40022068 40022069 4002206A 4002206B 4002206C 4002206D 4002206E 4002206F "

	-- Option Bytes order in buf
         -- secure                   non-secure
    -- 0  FLASH_OPTR                FLASH_OPTR
    -- 4  FLASH_SECWM1R2            DUMMY0 (0)
    -- 8  FLASH_SECWM2R2            DUMMY1 (0)
   -- 12  FLASH_NSBOOTADD0R         FLASH_NSBOOTADD0R
   -- 16  FLASH_NSBOOTADD1R         FLASH_NSBOOTADD1R
   -- 20  FLASH_SECBOOTADD0R        FLASH_SECBOOTADD0R
   -- 24  FLASH_SECWM1R1            DUMMY2 (0)
   -- 28  FLASH_WRP1AR              FLASH_WRP1AR
   -- 32  FLASH_WRP1BR              FLASH_WRP1BR
   -- 36  FLASH_SECWM2R1            DUMMY3 (0)
   -- 40  FLASH_WRP2AR              FLASH_WRP2AR
   -- 44  FLASH_WRP2BR              FLASH_WRP2BR	
   
	--SECURE_ENABLE = 0ʱ�������Ϻ�д���ֵ (���������ֽڣ�
	OB_SECURE_OFF  = "AA F8 EF 7F 7F 00 00 08 7F 00 F9 0B FF FF 80 FF "
				   .."FF FF 80 FF FF FF 80 FF FF FF 80 FF 7C 00 00 0C "		
				   .."80 7F 80 7F 80 7F 80 7F 80 FF FF FF 80 FF FF FF "
	
	--SECURE_ENABLE = 1ʱ�������Ϻ�д���ֵ	
	OB_SECURE_ON   = "00 F8 EF 7F 7F 00 00 08 7F 00 F9 0B FF FF 80 FF "
				   .."FF FF 80 FF FF FF 80 FF FF FF 80 FF 7C 00 00 0C "		
				   .."80 7F 80 7F 80 7F 80 7F 80 FF FF FF 80 FF FF FF "

	--�ж϶�������д����������(WRP = Write protection)
	OB_WRP_ADDRESS   = {0x40022040}
	
	OB_WRP_MASK  	 = {0xFF}	--�����������������
	OB_WRP_VALUE 	 = {0xAA}	--�����������Ƚϣ���ȱ�ʾû�б���
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
