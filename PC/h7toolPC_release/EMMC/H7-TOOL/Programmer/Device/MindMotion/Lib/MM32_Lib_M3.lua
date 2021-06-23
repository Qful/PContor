
-------------------------------------------------------
-- �ļ��� : MM32_Lib_M3.lua
-- ��  �� : V1.0  2020-04-28
-- ˵  �� : 
-------------------------------------------------------

print("load \"MM32_Lib_M3.lua\" ok")

--��λ�ڼ�ִ�еĺ���. Ŀ��:debug�ڼ䶳�ῴ�Ź�ʱ��,�͹���ģʽ����HCLK��FCLK
function InitUnderReset(void)
	local str = "error"
	local RCC_APB1ENR_M3M0_M3N4 =  0x4002101C

	print("InitUnderReset()")

	--M3ϵ��ȱʡû�п�PWR��־λ
	if (pg_write32(RCC_APB1ENR_M3M0_M3N4, 0x10000000) == 0) then
		goto quit_err
	end
	
	if (pg_write32(0x40007084, 0x00000307) == 0) then
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
	
	g_DevID = {pg_read32(0x40007080)} --ȫ�ֱ���g_DevID[]

	str = "..DeviceID = "
	for j = 1, ch_num, 1 do
		str = str..string.format("%08X ", g_DevID[j])
		if (g_DevID[j] == 0) then
			--err = 1	--M3ʼ�ն�����
		end
	end

	print(str)
	return err
end

---------------------------����-----------------------------------
