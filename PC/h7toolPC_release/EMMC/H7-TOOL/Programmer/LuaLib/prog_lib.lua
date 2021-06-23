-------------------------------------------------------
-- �ļ��� : prog_lib.lua
-- ˵  �� : ����lua�ӳ���
-- Copyright (C), 2020-2030, ���������� www.armfly.com
-------------------------------------------------------
PROG_LIB_FILE_VER = 1.6  --�ļ��汾

print("load \"prog_lib.lua\" ok. version = ",PROG_LIB_FILE_VER)

--������
function start_prog(void)
	return prog_or_erase(0)
end

--�������
function erase_chip_mcu(void)
	return prog_or_erase(1)
end

--�������
function erase_chip_eeprom(void)
	return prog_or_erase(2)
end

--��̻��߲�����������
function prog_or_erase(mode)
	local err = ""
	local str
	local mi_error = {0,0,0,0}  --��̨��¼�����ź�
	
	g_id = {0,0,0,0}	--ȫ�ֱ�����оƬcore id

	pg_reload_var("StarPorgTime")	--���ڸ�����¼��ʼʱ��

	if (mi_begin ~= nil) then
		mi_begin() --��̨�����ź�
	end
	
	if (MULTI_MODE == 0) then
		print("��·��¼")
	elseif (MULTI_MODE == 1) then
		print("��·��¼ 1·")
	elseif (MULTI_MODE == 2) then
		print("��·��¼ 1-2·")
	elseif (MULTI_MODE == 3) then
		print("��·��¼ 1-3·")
	elseif (MULTI_MODE == 4) then
		print("��·��¼ 1-4·")
	else
		print("��·��¼, ͨ��������������", MULTI_MODE)
		err = "ͨ����������"
		goto quit		
	end

	config_chip1()		--������¼����

	if (CHIP_TYPE == "SWD") then
		print("SWCLKʱ���ӳ�: ", SWD_CLOCK_DELAY)
	else if (CHIP_TYPE == "SWIM") then
		print("Device  = "..DEVICE)
		print(" flash  size = ", FLASH_SIZE)
		print(" eeprom size = ", EEPROM_SIZE)
		if (FLASH_SIZE == nil or EEPROM_SIZE == nil) then
			err = "chip name is invalid"
			goto quit
		end
	end
	end

--	set_tvcc(0)			--�ϵ�
--	delayms(20)
	set_tvcc(TVCC_VOLT)	--����TVCC��ѹ
--	delayms(20)

	if (RemoveSwdLock ~= nil)  then
		RemoveSwdLock()
	end
	
	if (MULTI_MODE > 0) then
		local i
		local CanProg = 0

		g_id = {pg_detect_ic()}
		str = string.format("core_id: = 0x%08X 0x%08X 0x%08X 0x%08X", g_id[1], g_id[2], g_id[3], g_id[4])
		print(str)
		
		if (ABORT_ON_ERROR == 1) then	--�����⵽ȫ��IC
			CanProg = 1
			for i = 1, MULTI_MODE, 1 do
				if (g_id[i] == 0) then
					CanProg = 0
					break
				end
			end
		else	--��⵽����1��IC���ɼ������
			CanProg = 0
			for i = 1, MULTI_MODE, 1 do
				if (g_id[i] > 0) then
					CanProg = 1
					break
				end
			end
		end
		
		if (CanProg == 1) then
			if (mode == 0) then --���
				config_fix_data()	--���û�lua���򲿷�ʵ���������
				if (CHIP_TYPE == "SWD") then
					err = swd_start_prog()	--���ARM (SWD)
				else
					G_CHIP_ERASED = 0	--�����Ƭ�Ѳ������������ļ�ʱ�����ٲ���
					err = swim_start_prog()	--���STM8 (SWIM) ����51
				end
			else	--mode == 1 ����
				if (mode == 1) then
					if (MCU_EraseMass == nil) then
						err = erase_chip(FLASH_ADDRESS)	--����CPU Flash
					else
						err = MCU_EraseMass()		--����arm��Ҫ��������Ƭ����ָ��
					end
				else
					if (EEPROM_ADDRESS ~= nil) then
						err = erase_chip(EEPROM_ADDRESS)	--����CPU EEPROM
					else
						print("MCUδ����EEPROM")
					end
				end
			end
			if (err ~= "OK") then
				mi_error = {pg_read_c_var("MultiProgError")}
				goto quit 
			end
			
			goto quit
		end

		err = "δ��⵽IC"
		for i = 1, MULTI_MODE, 1 do
			if (g_id[i] == 0) then
				err = string.format("%s #%d", err, i)
				mi_error[i] = 1		--��NG�ź�
			else
				--��ֹ�����NG�źŲ���, 0ȫ����NG  1������ͨ����NG
				if (ABORT_NG_MODE == 1) then
					mi_error[i] = 2	--2������ֵ������NG�ź�Ҳ����OK�ź�
				else
					mi_error[i] = 1	--��NG�ź�
				end
			end
		end

	else
		g_id = {pg_detect_ic()}
		if (g_id[1] > 0) then
			str = string.format("core_id = 0x%08X", g_id[1])
			print(str)

			--�˶�core id
			if (CHECK_MCU_ID == 1) then
				if (g_id[1] ~= MCU_ID) then
					err = "MCU ID����ȷ"
					pg_print_text(err)
					goto quit
				end
			end

			if (mode == 0) then --���
				if (CHIP_TYPE == "SWD") then
					err = swd_start_prog()	--���ARM (SWD)
				else
					err = swim_start_prog()	--���STM8 (SWD)
				end
			else	--����
				if (mode == 1) then
					err = erase_chip(FLASH_ADDRESS)	--����CPU Flash
				else
					if (EEPROM_ADDRESS ~= nil) then
						err = erase_chip(EEPROM_ADDRESS)	--����CPU EEPROM
					else
						print("MCUδ����EEPROM")
					end
				end
			end
			if (err ~= "OK") then 
				goto quit 
			end

			goto quit
		end

		mi_error[0] = 1
		err = "δ��⵽IC"
	end

::quit::
	if (err == "OK") then
		local i
		local MulErr
		
		mi_error = {pg_read_c_var("MultiProgError")}
	
		MulErr = 0
		for i = 1, 4, 1 do
			if (mi_error[i] == 1) then
				MulErr = 1
			end
		end
	
		if (MulErr == 1) then
			beep(20,1,1) --�ɹ���1�Σ�����
		else
			beep() --�ɹ���1��
		end
		
		if (MULTI_MODE == 0) then
			pg_print_text("��̳ɹ�")
		else
			str = "�ɹ�:"
			for i = 1,MULTI_MODE,1 do
				if (mi_error[i] == 0) then
					str = string.format("%s#%d", str, i)
				end
			end

			if (MulErr == 1) then
				str = str.." ʧ��:"
				for i = 1,MULTI_MODE,1 do
					if (mi_error[i] ~= 0) then
						str = string.format("%s#%d", str, i)
					end
				end			 
			end
			
			pg_print_text(str)
		end
	else		
		beep(5, 5, 3) --ʧ�ܽ�3��
		if (err ~= "error") then
			pg_print_text(err)
		end
	end
		
	if (mi_abort ~= nil) then
		mi_abort(mi_error)	--��¼��ֹ, ��1��ʧ�ܣ�����ֹ
	end	
	return err
end

--�ж�оƬ�Ƴ�������������¼��
function CheckChipRemove(void)
	if (MULTI_MODE > 0) then
		local id1
		local id2
		local id3
		local id4

		id1,id2,id3,id4 = pg_detect_ic(1)
		if (MULTI_MODE == 1) then
			if (id1 == 0) then
				return "removed"
			end
		end
		if (MULTI_MODE == 2) then
			if (id1 == 0 and id2 == 0) then
				return "removed"
			end
		end
		if (MULTI_MODE == 3) then
			if (id1 == 0 and id2 == 0 and id3 == 0) then
				return "removed"
			end
		end
		if (MULTI_MODE == 4) then
			if (id1 == 0 and id2 == 0 and id3 == 0 and id4 == 0) then
				return "removed"
			end
		end
	else
		local core_id

		core_id = pg_detect_ic(1)
		if (core_id == 0) then
			return "removed"
		end
	end

	return "no"
end

--�ж�оƬ���루����������¼��
function CheckChipInsert(void)
	config_chip1()

	if (MULTI_MODE > 0) then
		local id
		local id1
		local id2
		local id3
		local id4

		id1,id2,id3,id4 = pg_detect_ic(1)
		if (MULTI_MODE == 1) then
			if (id1 > 0) then
				return "inserted"
			end
		end
		if (MULTI_MODE == 2) then
			if (id1 > 0 and id2 > 0) then
				return "inserted"
			end
		end
		if (MULTI_MODE == 3) then
			if (id1 > 0 and id2 > 0 and id3 > 0) then
				return "inserted"
			end
		end
		if (MULTI_MODE == 4) then
			if (id1 > 0 and id2 > 0 and id3 > 0 and id4 > 0) then
				return "inserted"
			end
		end
	else
		local core_id

		core_id = pg_detect_ic(1)
		if (core_id > 0) then
			return "inserted"
		end
	end

	return "no"
end

-------------------------------------------------------
-- ����Ĵ���һ�������޸�
-------------------------------------------------------

--��ʼ���SWD�ӿ�оƬ
--����-����ȫƬ-��̬����SN��UID�������ݡ��û�����
--����ļ�(�Զ���������̡�У��)
--дOPTION BYTES
function swd_start_prog(void)
	local err = "OK"
	local re
	local core_id
	local uid_bin
	local str
	local mcu_uid
	local ob_data = {}
	local i
	local ch_num
		
	if (MULTI_MODE > 0) then
		ch_num = MULTI_MODE
	else
		ch_num = 1
	end

	if (check_limit_1() == 1) then
		err = "error"
		goto quit
	end

	--�ж϶�������д�����������������ִ�н�������
	if (AUTO_REMOVE_PROTECT == 1) then
		local remove_protect

		print("����д����...")
				
		remove_protect = 0;
		for i = 1, #OB_WRP_ADDRESS, 1 do
			local wrp
			local prot

			if (MCU_ReadOptionByte ~= nil) then
				ob_data = {MCU_ReadOptionByte(OB_WRP_ADDRESS[i])}
			else
				ob_data = {pg_read_mem(OB_WRP_ADDRESS[i], 1)}
			end

			if (ob_data[1] == 0) then
				print("  ���Ĵ���ʧ��")
				pg_print_text("  �ѱ��������ö�����")
				remove_protect = 1
				break
			else
				str = string.format("  0x%08X ��", OB_WRP_ADDRESS[i])
				prot = 0
				for j = 1, ch_num, 1 do
					if (MCU_ReadOptionByte ~= nil) then
						wrp = ob_data[j + 1]
					else
						wrp = tonumber(string.byte(ob_data[j + 1],1,1))
					end
					if (g_id[j] > 0 and ((wrp & OB_WRP_MASK[i]) ~= OB_WRP_VALUE[i])) then
						prot = 1
						remove_protect = 1
					end
					
					str = str..string.format("0x%02X ", wrp)
				end
				str = str..string.format("& 0x%02X == 0x%02X", OB_WRP_MASK[i], OB_WRP_VALUE[i])
				if (prot == 1) then
					str = str.."(�ѱ���)"
				end
				print(str)
			end
		end

		if (remove_protect == 1) then
			pg_print_text("���ڽ������...")
			
			if (MCU_RemoveProtect ~= nil) then
				print("MCU_RemoveProtect()")
				MCU_RemoveProtect()
			end
	
			err = set_read_protect(0)		--������������ڲ��и�λ����					
			if (err ~= "OK") then
				goto quit
			end
		else
			print("  �ޱ���")
		end
	end

	fix_data_begin()			--��ʼ��̬���SN UID USR����
	pg_reload_var("FixData")  	--���ڸ���c�����ȫ�ֱ���

	if (check_limit_2() ~= 0) then
		pg_print_text("UIDδ��Ȩ")
		err = "error"
		goto quit
	end

	for i = 1, #TaskList, 4 do		
		if (TaskList[i] ~= "") then
			local EndAddress
			local j
			
			print("------------------------")
			str = string.format("FLM : %s", TaskList[i])  print(str)
			str = string.format("Data: %s", TaskList[i + 1]) print(str)
			str = string.format("Addr: 0x%08X, %X", TaskList[i + 2], TaskList[i + 3]) print(str)

			--����flash�㷨�ļ�

			re = pg_load_algo_file(TaskList[i], AlgoRamAddr, AlgoRamSize)
			if (re == 0) then
				err = "����flash�㷨ʧ��"  goto quit
			end

			--�β�����FLASH������ַ����������ظ�д��
			EndAddress = 0xFFFFFFFF
			for j = 1, #TaskList, 4 do
				if (TaskList[i] ~= TaskList[j]) then
					if (TaskList[j + 2] < EndAddress) then
						EndAddress = TaskList[j + 2]
					end
				end
			end		

			--pg_prog_file(�����ļ�, Ŀ���ַ, ������ַ����Ƭ�������ļ����)
			re = pg_prog_file(TaskList[i + 1], TaskList[i + 2], EndAddress, TaskList[i + 3], i, TaskList[i])

			if (re == 0) then
				err = "error" goto quit 	--pg_prog_file�ڲ�����ʾ������Ϣ
			end
		else
			err = "�㷨�ļ�Ϊ��"  goto quit
		end
	end

	--дOPTION BYTES (������Ҳ���ڣ�
	if (OB_ENABLE == 1) then
		--pg_print_text("дoption bytes")
		if (SECURE_ENABLE == 0) then
			set_read_protect(0)
		else
			set_read_protect(1)
		end
		if (re == 0) then
			err = "дѡ����ʧ��"  goto quit
			goto quit
		else

		end
	end

	--��λ
	if (RESET_AFTER_COMPLETE == 1) then
		pg_reset()
	end

::quit::
	return err
end

--��ʼ���,���裺
--����-����ȫƬ-��̬����SN��UID�������ݡ��û�����
--����ļ�(�Զ���������̡�У��)
--дOPTION BYTES
function swim_start_prog(void)
	local err = "OK"
	local re
	local uid_bin
	local str
	local mcu_uid
	local i
	local bin
	local ff
	local ch_num
		
	if (MULTI_MODE > 0) then
		ch_num = MULTI_MODE
	else
		ch_num = 1
	end
	
	--����TVCC��ѹ
	set_tvcc(TVCC_VOLT)
	delayms(20)

	pg_init()

	if (check_limit_1() == 1) then
		err = "error"
		goto quit
	end

	--�ж϶�������д�����������������ִ�н�������
	if (AUTO_REMOVE_PROTECT == 1) then
	if (CHIP_TYPE == "SWIM") then
		--�����ö��������ٽ�����������Զ�����ȫƬ��
		pg_print_text("����ȫƬ")
		--�ȼӱ����ٽ���������ﵽ���оƬ��Ŀ��
		set_read_protect(1)
		pg_init()
		set_read_protect(0)	
	else
		local remove_protect
		local ob_data = {}
					
		print("����д����...")
		remove_protect = 0;
		for i = 1, #OB_WRP_ADDRESS, 1 do
			local wrp
			local prot

			if (MCU_ReadOptionByte ~= nil) then
				ob_data = {MCU_ReadOptionByte(OB_WRP_ADDRESS[i])}
			else
				ob_data = {pg_read_mem(OB_WRP_ADDRESS[i], 1)}
			end

			if (ob_data[1] == 0) then
				print("  ���Ĵ���ʧ��")
				pg_print_text("  �ѱ��������ö�����")
				remove_protect = 1
				break
			else
				str = string.format("  0x%08X ��", OB_WRP_ADDRESS[i])
				prot = 0
				for j = 1, ch_num, 1 do
					if (MCU_ReadOptionByte ~= nil) then
						wrp = ob_data[j + 1]
					else
						wrp = tonumber(string.byte(ob_data[j + 1],1,1))
					end

					if ((wrp & OB_WRP_MASK[i]) ~= OB_WRP_VALUE[i]) then
						prot = 1
						remove_protect = 1
					end
					
					str = str..string.format("0x%02X ", wrp)
				end
				str = str..string.format("& 0x%02X == 0x%02X", OB_WRP_MASK[i], OB_WRP_VALUE[i])
				if (prot == 1) then
					str = str.."(�ѱ���)"
				end
				print(str)
			end
		end

		if (remove_protect == 1) then
			pg_print_text("���ڽ������...")
			
			err = set_read_protect(0)		--������������ڲ��и�λ����					
			if (err ~= "OK") then
				goto quit
			end
			
			G_CHIP_ERASED = 1	--��Ƭ�Ѳ������������ļ�ʱ�����ٲ���
		else
			print("  �ޱ���")
		end		
		
	end	--if (CHIP_TYPE == "SWIM")		
	end --if (AUTO_REMOVE_PROTECT == 1)

	--��̬���SN UID USR����
	fix_data_begin()
	pg_reload_var("FixData")  	--���ڸ���c�����ȫ�ֱ���
	
	if (check_limit_2() ~= 0) then
		pg_print_text("UIDδ��Ȩ")
		err = "error"
		goto quit
	end

	--����ļ�����ա���������̡�У�飩
	for i = 1, #TaskList, 4 do
		if (TaskList[i+1] ~= "") then
			local EndAddress
			local j
			
			print("------------------------")
			
			if (CHIP_TYPE == "SWD") then
				str = string.format("FLM : %s", TaskList[i])  print(str)
				str = string.format("Data: %s", TaskList[i + 1]) print(str)
				str = string.format("Addr: 0x%08X, %X", TaskList[i + 2], TaskList[i + 3]) print(str)

				pg_print_text("����ļ�")
				--����flash�㷨�ļ�

				re = pg_load_algo_file(TaskList[i], AlgoRamAddr, AlgoRamSize)
				if (re == 0) then
					err = "����flash�㷨ʧ��"  goto quit
				end
			else
				str = string.format("File : %s", TaskList[i + 1])  print(str)
			end
			
			--�β�����FLASH������ַ����������ظ�д��
			EndAddress = 0xFFFFFFFF
			for j = 1, #TaskList, 4 do
				if (TaskList[i] ~= TaskList[j]) then
					if (TaskList[j + 2] < EndAddress) then
						EndAddress = TaskList[j + 2]
					end
				end
			end	
			--pg_prog_file(�����ļ�, Ŀ���ַ, ������ַ����Ƭ���������֣��ļ����, �㷨��)
			re = pg_prog_file(TaskList[i + 1], TaskList[i + 2], EndAddress, TaskList[i + 3], i, TaskList[i])
			if (re == 0) then
				err = "���ʧ��"  goto quit
				goto quit
			end
		end
	end

	--дOPTION BYTES (������Ҳ���ڣ�
	if (OB_ENABLE == 1) then
		print("------------------------")
		pg_print_text("дoption bytes")
		if (SECURE_ENABLE == 0) then
			set_read_protect(0)
		else
			set_read_protect(1)
		end
		if (re == 0) then
			err = "дOPTION BYTESʧ��"  goto quit
			goto quit
		else

		end
	end

	--��λ
	if (RESET_AFTER_COMPLETE == 1) then
		pg_reset()
	end

::quit::
	return err
end

--�����������1,����0��ʾOK, 1��ʾ��ֹ��¼ [��������¼�������]
function check_limit_1(uid)
  local ret = 0

  if (DEADLINE_ENABLE == 1) then
    local now = os.date("%Y-%m-%d %H:%M:%S")
    if (now >= DEADLINE_DATE) then
      ret = 1
      pg_print_text("�ѵ���ֹʱ��")
    end

    if (now < "2020-08-01 00:00:00") then
      ret = 1
      pg_print_text("��¼��ʱ�Ӳ���ȷ")
    end	
  end

  return ret
end

--�����������2, ����0��ʾOK, 1��ʾ��ֹ��¼ [������оƬ������������]
function check_limit_2(void)
	local ret = 0
	local ch_num
	local uid

	if (MULTI_MODE > 0) then
		ch_num = MULTI_MODE
	else
		ch_num = 1
	end

	--UIDȫ�ֱ���
	if (UID_CHECK_ENABLE == 1) then
		local mask = {}
		local xid = ""
		local yid = ""
		local i
		local j
		local m

		if (UID[1] == 0) then
			ret = 1
			goto quit
		end

		for m = 1,ch_num,1 do
			ret = 1
			uid = UID[m + 1]
			for i = 1,#UID_TABLE,2 do
				mask = hex_to_bin(UID_TABLE[i])
				yid = hex_to_bin(UID_TABLE[i+1])
				xid = ""
				for j = 1,#uid,1 do
					xid = xid..string.char((string.byte(uid, j) & string.byte(mask, j)))
				end
				if (xid == yid) then
					ret = 0
					break
				end
			end

			if (ret ~= 0) then
				print("оƬUIDδ����Ȩ", m)
				break
			end
		end
	end

::quit::
	return ret
end

--�������ַ���ת���ɼ���hex�ַ���
function bin2hex(s)
	s = string.gsub(s,"(.)", function (x) return string.format("%02X ",string.byte(x)) end)
	return s
end

function hex2bin( hexstr )
	local h2b = {
	    ["0"] = 0,
	    ["1"] = 1,
	    ["2"] = 2,
	    ["3"] = 3,
	    ["4"] = 4,
	    ["5"] = 5,
	    ["6"] = 6,
	    ["7"] = 7,
	    ["8"] = 8,
	    ["9"] = 9,
	    ["A"] = 10,
	    ["B"] = 11,
	    ["C"] = 12,
	    ["D"] = 13,
	    ["E"] = 14,
	    ["F"] = 15
	}

    local s = string.gsub(hexstr, "(.)(.)%s", function ( h, l )
         return string.char(h2b[h]*16+h2b[l])
    end)
    return s
end

function bin2hex(bin)
	local s
	local i

	s = ""
	for i=1,#bin,1 do
		s = s..string.format("%02X ", string.byte(bin,i))
	end
	return s
end

--��ӡ�ڴ�����
function print_flash(_addr, _len, width, dispaddr, _delay)
	local re
	local bin
	local bin1
	local bin2
	local bin3
	local bin4
	local str
	local core_id
	local i
	local addr
	local len
	local n

	--����TVCC��ѹ
	set_tvcc(TVCC_VOLT)
	delayms(20)

	if (RemoveSwdLock ~= nil)  then
		RemoveSwdLock()
	end
	
	pg_init()

	n = (_len + 1023) / 1024
	addr = _addr

	for i = 1, n, 1 do
		if (_len > 1024) then
			len = 1024
		else
			len = _len
		end

		if (MULTI_MODE > 0) then
			re,bin1,bin2,bin3,bin4 = pg_read_mem(addr, len)
			if (re == 1) then
				if (MULTI_MODE >= 1) then
					str = string.format("#1 address = 0x%08X, len = %d", addr, len)
					print(str)
					if (width == nil) then
						print_hex(bin1,16)
					else
						if (dispaddr == nil) then
							print_hex(bin1,width)
						else
							print_hex(bin1,width, dispaddr)
						end
					end
					delayms(5)
				end

				if (MULTI_MODE >= 2) then
					str = string.format("#2 address = 0x%08X, len = %d", addr, len)
					print(str)
					if (width == nil) then
						print_hex(bin2,16)
					else
						if (dispaddr == nil) then
							print_hex(bin2,width)
						else
							print_hex(bin2,width, dispaddr)
						end
					end
					delayms(5)
				end

				if (MULTI_MODE >= 3) then
					str = string.format("#3 address = 0x%08X, len = %d", addr, len)
					print(str)
					if (width == nil) then
						print_hex(bin3,16)
					else
						if (dispaddr == nil) then
							print_hex(bin3,width)
						else
							print_hex(bin3,width, dispaddr)
						end
					end
					delayms(5)
				end

				if (MULTI_MODE == 4) then
					str = string.format("#4 address = 0x%08X, len = %d", addr, len)
					print(str)
					if (width == nil) then
						print_hex(bin4,16)
					else
						if (dispaddr == nil) then
							print_hex(bin4,width)
						else
							print_hex(bin4,width, dispaddr)
						end
					end
					delayms(5)
				end
			else
				str = "error"
				print(str)
			end
		else
			re,bin = pg_read_mem(addr, len)
			if (re == 1) then
				str = string.format("address = 0x%08X, len = %d", addr, len)
				print(str)
				if (width == nil) then
					print_hex(bin,16)
				else
					if (dispaddr == nil) then
						print_hex(bin,width)
					else
						print_hex(bin,width, dispaddr)
					end
				end
			else
				str = "error"
				print(str)
			end
		end

		_len = _len - len
		addr = addr + len

		if (dispaddr ~= nil) then
			dispaddr = addr
		end

		if (_delay ~= nil) then
			delayms(_delay)
		else
			delayms(20)
		end
	end
end

--��ӡOPTION BYTES
function print_option_bytes(void)

	print_core_id()

	if (MCU_Init~= nil) then
		MCU_Init()
	end
	
	print("Option bytes Address:")
	print(OB_ADDRESS)
		
	if (MULTI_MODE > 0) then
		local re
		local bin1
		local bin2
		local bin3
		local bin4

		if (MCU_ReadOB ~= nil) then
			re,bin1,bin2,bin3,bin4 = MCU_ReadOB()
		else
			re,bin1,bin2,bin3,bin4 = pg_read_ob(OB_ADDRESS)
		end

		if (re == 1) then
			if (MULTI_MODE >= 1) then
				print("#1 Option bytes data:")
				print_hex(bin1)
			end

			if (MULTI_MODE >= 2) then
				print("#2 Option bytes data:")
				print_hex(bin2)
			end

			if (MULTI_MODE >= 3) then
				print("#3 Option bytes data:")
				print_hex(bin3)
			end

			if (MULTI_MODE >= 4) then
				print("#4 Option bytes data:")
				print_hex(bin4)
			end
		else
			print("error")
		end
	else
		local re
		local bin

		print("Option bytes data:")
		
		if (MCU_ReadOB ~= nil) then
			re,bin = MCU_ReadOB()
		else
			re,bin = pg_read_ob(OB_ADDRESS)
		end

		if (re == 1) then
			print_hex(bin)
		else
			print("error")
		end
	end
end

--���ö����� 0 ��ʾ�����������1��ʾ���ö�����
function set_read_protect(on)
	local re
	local err = "OK"
	local time1
	local time2
	local str

	if (REMOVE_RDP_POWEROFF == nil) then
		REMOVE_RDP_POWEROFF = 0
	end

	print("\r\nдѡ����...")

	time1 = get_runtime()

--	--����TVCC��ѹ
--	set_tvcc(TVCC_VOLT)
--	delayms(20)
--
--	--���IC,��ӡ�ں�ID
--	local core_id = pg_detect_ic()
--	if (core_id == 0) then
--		err = "δ��⵽IC"  print(err) return err
--	else
--		str = string.format("core_id = 0x%08X", core_id)
--		print(str)
--	end

	if (CHIP_TYPE == "SWD") then
		if (MCU_ProgOptionBytes == nil) then
			if (AlgoFile_OPT == "") then
				err = "û��OPT�㷨�ļ�"  print(err) return err
			end

			--����flash�㷨�ļ�
			re = pg_load_algo_file(AlgoFile_OPT, AlgoRamAddr, AlgoRamSize)
			if (re == 0) then
				err = "����flash�㷨ʧ��"  print(err) return err
			end
		end
	else
		--STM8 N76Eר�õĽ������
		if (MCU_RemoveProtect ~= nil) then
			if (on == 0) then
				print("MCU_RemoveProtect()")
				MCU_RemoveProtect()
			end
		end
	end

	if (on == 0) then
		local i

		print("OB_SECURE_OFF = ", OB_SECURE_OFF)
		for i = 1, 3, 1 do
			if (MCU_ProgOptionBytes ~= nil) then
				err = MCU_ProgOptionBytes(OB_SECURE_OFF)
				if (err == "OK") then
					re = 1
				else
					re = 0
				end
			else
				re = pg_prog_buf_ob(OB_ADDRESS, OB_SECURE_OFF)
			end

			if (re == 0) then		--����оƬ��Ҫ2�β�����ȷ���Ƿ�ɹ� ��STM32L051)
				if (REMOVE_RDP_POWEROFF > 0) then
					print("  prog ob failed", i)
					delayms(POWEROFF_TIME1)
					print("  �ϵ�")
					set_tvcc(0)			--�ϵ�
					delayms(POWEROFF_TIME2)
					set_tvcc(TVCC_VOLT) --�ϵ�
					pg_reset()
					delayms(POWEROFF_TIME3)
					core_id = pg_detect_ic()	--MM32��λ������һ��ID���ܷ����ڴ�
				else
					print("  prog ob failed", i)
					delayms(100)
					pg_reset()
					delayms(100)
					core_id = pg_detect_ic()	--MM32��λ������һ��ID���ܷ����ڴ�
				end
			else	--�ɹ�
				if (REMOVE_RDP_POWEROFF > 0) then
					delayms(POWEROFF_TIME1)
					print("  �ϵ�")
					set_tvcc(0)			--�ϵ�
					delayms(POWEROFF_TIME2)
					set_tvcc(TVCC_VOLT) --�ϵ�
					pg_reset()
					delayms(POWEROFF_TIME3)
					core_id = pg_detect_ic()	--MM32��λ������һ��ID���ܷ����ڴ�
				else
					pg_reset()
					--delayms(100)
					core_id = pg_detect_ic()	--MM32��λ������һ��ID���ܷ����ڴ�					
				end				
				break
			end
		end
	else
		if (on == 1) then
			print("OB_SECURE_ON = ", OB_SECURE_ON)
			if (MCU_ProgOptionBytes ~= nil) then
				err = MCU_ProgOptionBytes(OB_SECURE_ON)
				if (err == "OK") then
					re = 1
				else
					re = 0
				end
			else
				re = pg_prog_buf_ob(OB_ADDRESS, OB_SECURE_ON)
			end
		end
	end

::quit::
	if (re == 0) then
		err = "дѡ����ʧ��"
	end
	time2 = get_runtime()

	if (err == "OK") then
		str = string.format("дѡ���ֳɹ�, ��ʱ = %d ms", time2 - time1);
		print(str)
	else

	end

	return err
end

--����оƬ=����оƬ
function erase_chip(FlashAddr)
	local re
	local err = "OK"
	local time1
	local time2
	local str

	pg_print_text("����...")

	if (CHIP_TYPE == "SWD") then
		if (FlashAddr == FLASH_ADDRESS) then
			re = pg_load_algo_file(AlgoFile_FLASH, AlgoRamAddr, AlgoRamSize)	--����flash�㷨�ļ�
		else
			re = pg_load_algo_file(AlgoFile_EEPROM, AlgoRamAddr, AlgoRamSize)	--����eeprom�㷨�ļ�
		end
		if (re == 0) then
			err = "����flash�㷨ʧ��"  print(str) return err
		end
	else
		if (CHIP_TYPE == "SWIM") then
			if (FlashAddr == FLASH_ADDRESS) then
				str = string.format("��ʼ����flash. ��ַ : 0x%X ���� : %dKB ", FlashAddr, FLASH_SIZE / 1024)
			else
				str = string.format("��ʼ����eeprom. ��ַ : 0x%X ���� : %dB ", FlashAddr, EEPROM_SIZE)
			end
			print(str)
		else
			print("δ֪�ӿ�")
		end
	end

	time1 = get_runtime()

	--����TVCC��ѹ
	set_tvcc(TVCC_VOLT)
	delayms(20)

	pg_init()

	re = pg_erase_chip(FlashAddr)
	if (re == 1) then
		pg_print_text("�����ɹ�")
	else
		pg_print_text("����ʧ��")
		err = "err"
	end
	time2 = get_runtime()

	str = string.format("ִ��ʱ�� = %d ms", time2 - time1);
	print(str)
	return err
end

--��ʾ�ں�id,
function print_core_id(void)
	local id
	local id1
	local id2
	local id3
	local id4
	local str

	set_tvcc(TVCC_VOLT)

	if (RemoveSwdLock ~= nil)  then
		RemoveSwdLock()
	end

	if (MULTI_MODE > 0) then
		id1,id2,id3,id4 = pg_detect_ic()
		str = string.format("core_id1 = 0x%08X", id1) print(str) delayms(5)
		str = string.format("core_id2 = 0x%08X", id2) print(str) delayms(5)
		str = string.format("core_id3 = 0x%08X", id3) print(str) delayms(5)
		str = string.format("core_id4 = 0x%08X", id4) print(str) delayms(5)
	else
		core_id = pg_detect_ic()
		if (core_id == 0) then
			print("δ��⵽IC")
		else
			str = string.format("core_id = 0x%08X", core_id) print(str)
		end
	end
end

--��ʾUID
function print_uid(void)
	local uid1,uid2,uid3,uid4
	local re
	local str

	str = string.format("UID Address = 0x%08X, Length = %d", UID_ADDR, UID_BYTES)
	print(str)

	if (MCU_Init~= nil) then
		MCU_Init()
	end

	if (MULTI_MODE > 0) then
		if (MCU_ReadUID == nil) then
			re,uid1,uid2,uid3,uid4 = pg_read_mem(UID_ADDR, UID_BYTES)
		else
			re,uid1,uid2,uid3,uid4 = MCU_ReadUID(UID_ADDR, UID_BYTES)
		end
		if (re == 1) then
			str = "uid1 = "..bin2hex(uid1) print(str) delayms(5)
			str = "uid2 = "..bin2hex(uid2) print(str) delayms(5)
			str = "uid3 = "..bin2hex(uid3) print(str) delayms(5)
			str = "uid4 = "..bin2hex(uid4) print(str) delayms(5)
		end
	else
		if (MCU_ReadUID == nil) then
			re,uid1 = pg_read_mem(UID_ADDR, UID_BYTES)
		else
			re,uid1 = MCU_ReadUID(UID_ADDR, UID_BYTES)
		end
		if (re == 1) then
			str = "uid1 = "..bin2hex(uid1) print(str) delayms(5)
		end
	end
end

--����CPUƬ��QSPI FLASH
function erase_chip_qspi(void)
	local core_id
	local str
	local addr
	local i
	local nSector
	local percent
	local time1
	local time2

	if (AlgoFile_QSPI == nil or AlgoFile_QSPI == "") then
		print("δ����QSPI Flash")
		return
	end

	print("��ʼ����QSPI Flash...")

	time1 = get_runtime()

	config_chip1()		--������¼����
	--set_tvcc(0)			--�ϵ�
	--delayms(20)
	set_tvcc(TVCC_VOLT)	--����TVCC��ѹ

	core_id = pg_detect_ic()
	if (core_id > 0) then
		str = string.format("swd : core_id = 0x%08X", core_id)
		print(str)
	end

	--����flash�㷨�ļ�
	re = pg_load_algo_file(AlgoFile_QSPI, AlgoRamAddr, AlgoRamSize)

	addr = 0x90000000
	nSector = 32 * 1024 / 64
	for i = 1, nSector,1 do
		pg_erase_sector(addr)

		percent = 100 * i / nSector;
		str = string.format("erase 0x%08X, %0.2f%%", addr, percent)
		print(str)
		addr = addr + 64 * 1024
	end

	time2 = check_runtime(time1)
	str = string.format("��������  %0.3f ��", time2 / 1000)
	print(str)
end

---------------------------����-----------------------------------
