
-------------------------------------------------------
-- �ļ��� : MM32_Lib_M0.lua
-- ��  �� : V1.0  2020-04-28
-- ˵  �� : 
-------------------------------------------------------

print("load \"MM32_Lib_M0.lua\" ok")

--��λ�ڼ�ִ�еĺ���. Ŀ��:debug�ڼ䶳�ῴ�Ź�ʱ��,�͹���ģʽ����HCLK��FCLK
function InitUnderReset(void)
	local str = "error"

	print("InitUnderReset()")

	if (pg_write32(0x40013404, 0x00000307) == 0) then
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
	
	g_DevID = {pg_read32(0x40013400)} --ȫ�ֱ���g_DevID[]

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
