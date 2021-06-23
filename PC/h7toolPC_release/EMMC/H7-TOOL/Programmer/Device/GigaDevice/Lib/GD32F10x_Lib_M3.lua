
-------------------------------------------------------
-- �ļ��� : GD32F10x_Lib_M3.lua
-- ��  �� : V1.0  2020-04-28
-- ˵  �� : ���ڽ���������Ŀ�
-------------------------------------------------------

print("load \"GD32F10x_Lib_M3.lua\" ok")

FLASH_KEYR		= 0x40022004
FLASH_OPTKEYR   = 0x40022008

FLASH_KEY1      = 0x45670123
FLASH_KEY2      = 0xCDEF89AB

FMC_STAT		= 0x4002200C
FMC_CTL         = 0x40022010
FMC_CTL_OBPG    = 0x00000010
FMC_CTL_OBER    = 0x00000020
FMC_CTL_START   = 0x00000040

FMC_STAT_BUSY   = 0x00000001
FMC_STAT_PGERR  = 0x00000004
FMC_STAT_PGAERR = 0x00000008
FMC_STAT_WPERR  = 0x00000010
FMC_STAT_ENDF   = 0x00000020

--�ж�data�����־��ȫ��Ϊ0���˳�
function CheckFlagQuit0(data, mask)
	local j
	local ret
	local ch_num

	if (MULTI_MODE > 0) then
		ch_num = MULTI_MODE
	else
		ch_num = 1
	end

	ret = 0
	for j = 1,ch_num,1 do
		ret = ret | (data[j] & mask)
	end

	return ret
end

--оƬר�еĽ����������
function MCU_RemoveProtect(void)
	return MCU_ProgOptionBytes(OB_SECURE_OFF)
end

function FMC_WaitBusy(t100ms)
	local i
	local reg = {}
	local re = "error"
	local ch_num

	if (MULTI_MODE > 0) then
		ch_num = MULTI_MODE
	else
		ch_num = 1
	end	
	
	if (t100ms == nil) then
		t100ms = 50
	end
	
 	for i = 1, t100ms, 1 do
 		reg = {pg_read32(FMC_STAT)}

		--��������������ж�״̬�ֻ�ʧ�ܣ�����ɹ���ȡ������жϱ�־
		if (reg[ch_num + 1] == 1) then 
			if (CheckFlagQuit0(reg, FMC_STAT_BUSY) == 0) then
				re = "OK"
				break
			end
		end
		delayms(100)
	end
	return re
end

--û��FLM��MCU���ýű�ʵ�ֱ��OB�� ���� "OK" or "error"
function MCU_ProgOptionBytes(ob)
	local i
	local j
	local reg
	local ob_8
	local ob_16
	local ob_read = {}
	local err = "OK"
	local ch_num

	if (MULTI_MODE > 0) then
		ch_num = MULTI_MODE
	else
		ch_num = 1
	end

		
	pg_write32(FLASH_KEYR, FLASH_KEY1)
 	pg_write32(FLASH_KEYR, FLASH_KEY2)
	pg_write32(FLASH_OPTKEYR, FLASH_KEY1)
 	pg_write32(FLASH_OPTKEYR, FLASH_KEY2)

	--start erase the option bytes
	print("---Erase the option bytes---")
 	pg_write32(FMC_CTL, pg_read32(FMC_CTL) | FMC_CTL_OBER)
 	pg_write32(FMC_CTL, pg_read32(FMC_CTL) | FMC_CTL_START)

	if (FMC_WaitBusy(5) ~= "OK") then
		print("Erase the option bytes error")
		goto quit_err
	end
	
	reg = pg_read32(FMC_CTL)
	reg = reg & ~FMC_CTL_OBER
	pg_write32(FMC_CTL, reg)	--reset the OBER bit
	
	pg_write32(FMC_CTL, pg_read32(FMC_CTL) | FMC_CTL_OBPG)	--set the OBPG bi

	ob_8 = hex_to_bin(ob)	--hex�ַ���תΪ����������
	
	for i = 0, 7, 1 do
		ob_16 = string.byte(ob_8, i + 1) + (((~string.byte(ob_8, i + 1)) << 8) & 0xFF00)		
		
		pg_write16(0x1FFFF800 + 2 * i, ob_16)

		delayms(10)
		
		if (FMC_WaitBusy() ~= "OK") then
			goto quit_err
		end
		

	end
	
	pg_write32(FMC_CTL, pg_read32(FMC_CTL) & ~FMC_CTL_OBPG)	--reset the OBPG bit

	--У��
	print("---Verify OB---")
	for i = 0, 7, 1 do
		ob_16 = string.byte(ob_8, i + 1) + (((~string.byte(ob_8, i + 1)) << 8) & 0xFF00)
		ob_read = {pg_read16(0x1FFFF800 + 2 * i)}
		
		for j = 1,ch_num,1 do
			if (ob_16 ~= ob_read[j]) then
				err = "error"
			end
		end
	end

::quit_err::
	return err
end

--��F1����λ�ڼ�ִ�еĺ���. Ŀ��:debug�ڼ䶳�ῴ�Ź�ʱ��,�͹���ģʽ����HCLK��FCLK
function InitUnderReset(void)
	local str = "error"

	print("InitUnderReset()")

	if (pg_write32(0xE0042004, 0x00000307) == 0) then
		goto quit_err
	end

	if (ReadDeviceID() ~= 0) then
		goto quit_err
	end

::quit_ok::
	str = "OK"

::quit_err::
	print(str)

	return str
end

--��оƬID
function ReadDeviceID(void)
	local str
	local err = 0
	local j
	local ch_num

	if (MULTI_MODE == 0) then
		ch_num = 1
	else
		ch_num = MULTI_MODE
	end
	
	g_DevID = {pg_read32(0xE0042000)} --ȫ�ֱ���g_DevID[]

	str = "..DeviceID = "
	for j = 1, ch_num, 1 do
		str = str..string.format("%08X ", g_DevID[j])
		if (g_DevID[j] == 0) then
			err = 1
		end
	end

	print(str)
	return err
end

---------------------------����-----------------------------------
