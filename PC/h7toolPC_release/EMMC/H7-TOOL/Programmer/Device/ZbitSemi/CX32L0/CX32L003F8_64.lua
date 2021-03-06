-------------------------------------------------------
-- 文件名 : CX32L003F8_64.lua
-- 版  本 : V1.0  2020-09-29
-- 说  明 :
-------------------------------------------------------

print("load \"CX32L003F8_64.lua\" ok")

IncludeList = {
	"0:/H7-TOOL/Programmer/Device/ZbitSemi/Lib/CX32L0_Lib.lua"
}

function config_cpu(void)
	CHIP_TYPE = "SWD"		--指定器件接口类型: "SWD", "SWIM", "SPI", "I2C", "UART"

	AlgoFile_FLASH = "0:/H7-TOOL/Programmer/Device/ZbitSemi/FLM/CX32L003F8.FLM"
	AlgoFile_OTP   = ""
	AlgoFile_OPT   = "0:/H7-TOOL/Programmer/Device/ZbitSemi/FLM/CX32L003_OB.FLM"
	AlgoFile_QSPI  = ""

	FLASH_ADDRESS = 0x00000000		--CPU内部FLASH起始地址

	RAM_ADDRESS = 0x20000000		--CPU内部RAM起始地址

	--Flash算法文件加载内存地址和大小
	AlgoRamAddr = RAM_ADDRESS
	AlgoRamSize = 4 * 1024

	MCU_ID = 0x0BC11477

	UID_ADDR = 0x180000F0	  	--UID地址，16字节
	UID_BYTES = 16

	--缺省校验模式
	VERIFY_MODE = 0				--校验模式: 0:自动(FLM提供校验函数或读回) 1:读回  2:软件CRC32  3:STM32硬件CRC32

	ERASE_CHIP_TIME = 500		--全片擦除时间ms（仅用于进度指示)

	OB_ADDRESS     =  "08000000 08000001 08000002 08000003 "
					.."08000004 08000005 08000006 08000007 "
					.."08000008 08000009 0800000A 0800000B "
					
	OB_SECURE_OFF  = "FF FF FF FF FF FF FF FF FF FF FF FF"	--SECURE_ENABLE = 0时，编程完毕后写入该值(解除加密)
	OB_SECURE_ON   = "FF FF FF FF FE FF FF FF FF FF FF FF"	--SECURE_ENABLE = 1时，编程完毕后写入该值(芯片加密)

	--判断读保护和写保护的条件(WRP = Write protection)
	OB_WRP_ADDRESS   = {0x08000004} 	--内存地址
	OB_WRP_MASK  	 = {0x01}		--读出数据与此数相与
	OB_WRP_VALUE 	 = {0x01}		--相与后与此数比较，相等表示没有保护
end

--用于PC软件, 设置缺省配置参数
function pc_default(void)
	TVCC_VOLT = 3.3			--定义CPU供电电压TVCC
	
	VERIFY_MODE = 1			--校验模式: 0:自动(FLM提供校验函数或读回) 1:读回  2:软件CRC32  3:STM32硬件CRC32

	REMOVE_RDP_POWEROFF = 0 --写完OB后需要断电
	POWEROFF_TIME1 = 0   	--写完OB延迟时间ms
	POWEROFF_TIME2 = 100   	--断电时间ms
	POWEROFF_TIME3 = 20   	--上电后等待时间ms

	SWD_CLOCK_DELAY_0 = 1 	--单路烧录时的时钟延迟
	SWD_CLOCK_DELAY_1 = 1 	--多路烧录时的时钟延迟
	
	AUTO_REMOVE_PROTECT = 1 --自动解除读保护

	NOTE_PC = ""
	
	--供PC软件选择算法的列表
	AlgoFile_List = {
		"AlgoFile_FLASH"
	}
end

---------------------------结束-----------------------------------
