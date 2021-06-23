
-------------------------------------------------------
-- �ļ��� : STM8S207_208.lua
-- ��  �� : V1.0  2020-04-28
-- ˵  �� :
-------------------------------------------------------
function config_cpu(void)
	DeviceList = {
	
	typedef struct {
    char *name;                                  /**< flash chip name */
    uint8_t mf_id;                               /**< manufacturer ID */
    uint8_t type_id;                             /**< memory type ID */
    uint8_t capacity_id;                         /**< capacity ID */
    uint32_t capacity;                           /**< flash capacity (bytes) */
    uint16_t write_mode;                         /**< write mode @see sfud_write_mode */
    uint32_t erase_gran;                         /**< erase granularity (bytes) */
    uint8_t erase_gran_cmd;                      /**< erase granularity size block command */
} sfud_flash_chip;

#define SFUD_FLASH_CHIP_TABLE                                                                                       \
{                                                                                                                   \
    {"AT45DB161E", SFUD_MF_ID_ATMEL, 0x26, 0x00, 2L*1024L*1024L, SFUD_WM_BYTE|SFUD_WM_DUAL_BUFFER, 512, 0x81},      \
    {"W25Q40BV", SFUD_MF_ID_WINBOND, 0x40, 0x13, 512L*1024L, SFUD_WM_PAGE_256B, 4096, 0x20},                        \
    {"W25Q16BV", SFUD_MF_ID_WINBOND, 0x40, 0x15, 2L*1024L*1024L, SFUD_WM_PAGE_256B, 4096, 0x20},                    \
    {"W25Q32BV", SFUD_MF_ID_WINBOND, 0x40, 0x16, 4L*1024L*1024L, SFUD_WM_PAGE_256B, 4096, 0x20},                    \
    {"W25Q64CV", SFUD_MF_ID_WINBOND, 0x40, 0x17, 8L*1024L*1024L, SFUD_WM_PAGE_256B, 4096, 0x20},                    \
    {"W25Q64DW", SFUD_MF_ID_WINBOND, 0x60, 0x17, 8L*1024L*1024L, SFUD_WM_PAGE_256B, 4096, 0x20},                    \
    {"W25Q128BV", SFUD_MF_ID_WINBOND, 0x40, 0x18, 16L*1024L*1024L, SFUD_WM_PAGE_256B, 4096, 0x20},                  \
    {"W25Q256FV", SFUD_MF_ID_WINBOND, 0x40, 0x19, 32L*1024L*1024L, SFUD_WM_PAGE_256B, 4096, 0x20},                  \
    {"SST25VF080B", SFUD_MF_ID_SST, 0x25, 0x8E, 1L*1024L*1024L, SFUD_WM_BYTE|SFUD_WM_AAI, 4096, 0x20},              \
    {"SST25VF016B", SFUD_MF_ID_SST, 0x25, 0x41, 2L*1024L*1024L, SFUD_WM_BYTE|SFUD_WM_AAI, 4096, 0x20},              \
    {"M25P32", SFUD_MF_ID_MICRON, 0x20, 0x16, 4L*1024L*1024L, SFUD_WM_PAGE_256B, 64L*1024L, 0xD8},                  \
    {"M25P80", SFUD_MF_ID_MICRON, 0x20, 0x14, 1L*1024L*1024L, SFUD_WM_PAGE_256B, 64L*1024L, 0xD8},                  \
    {"M25P40", SFUD_MF_ID_MICRON, 0x20, 0x13, 512L*1024L, SFUD_WM_PAGE_256B, 64L*1024L, 0xD8},                      \
    {"EN25Q32B", SFUD_MF_ID_EON, 0x30, 0x16, 4L*1024L*1024L, SFUD_WM_PAGE_256B, 4096, 0x20},                        \
    {"GD25Q64B", SFUD_MF_ID_GIGADEVICE, 0x40, 0x17, 8L*1024L*1024L, SFUD_WM_PAGE_256B, 4096, 0x20},                 \
    {"GD25Q16B", SFUD_MF_ID_GIGADEVICE, 0x40, 0x15, 2L*1024L*1024L, SFUD_WM_PAGE_256B, 4096, 0x20},                 \
    {"S25FL216K", SFUD_MF_ID_CYPRESS, 0x40, 0x15, 2L*1024L*1024L, SFUD_WM_PAGE_256B, 4096, 0x20},                   \
    {"S25FL032P", SFUD_MF_ID_CYPRESS, 0x02, 0x15, 4L*1024L*1024L, SFUD_WM_PAGE_256B, 4096, 0x20},                   \
    {"A25L080", SFUD_MF_ID_AMIC, 0x30, 0x14, 1L*1024L*1024L, SFUD_WM_PAGE_256B, 4096, 0x20},                        \
    {"F25L004", SFUD_MF_ID_ESMT, 0x20, 0x13, 512L*1024L, SFUD_WM_BYTE|SFUD_WM_AAI, 4096, 0x20},                     \
    {"PCT25VF016B", SFUD_MF_ID_SST, 0x25, 0x41, 2L*1024L*1024L, SFUD_WM_BYTE|SFUD_WM_AAI, 4096, 0x20},              \
}

	}

	CHIP_TYPE = "SWIM"			--ָ�������ӿ�����: "SWD", "SWIM", "SPI", "I2C"

	STM8_SERIAL = "STM8S"		--ѡ��2��ϵ��: "STM8S" �� "STM8L"

	FLASH_ADDRESS = 0x008000	--����FLASH��ʼ��ַ

	EEPROM_ADDRESS = 0x004000 	--����FLASH��ʼ��ַ(STM8S��STM8L��ͬ��

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

	UID_ADDR = 0x48CD			--UID��ַ����ͬ��CPU��ͬ

	OB_ADDRESS     = "4800 4801 FFFF 4803 FFFF 4805 FFFF 4807 FFFF 4809 FFFF 480B FFFF 480D FFFF 487E FFFF"

	OB_SECURE_OFF  = "00 00 00 00 00 00 00 00 00"	--SECURE_ENABLE = 0ʱ�������Ϻ�д���ֵ (���������ֽڣ�
	OB_SECURE_ON   = "AA 00 00 00 00 00 00 00 00"	--SECURE_ENABLE = 1ʱ�������Ϻ�д���ֵ
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
