-------------------------------------------------------
-- �ļ��� : fix_data.lua
-- ˵  �� : ������¼��Ʒ��š�����UID�����ַ����Զ����ַ���
-- Copyright (C), 2020-2030, ���������� www.armfly.com
-------------------------------------------------------
FIX_DATA_FILE_VER = 1.1  --�ļ��汾

print("load \"fix_data.lua\" ok. version = ", FIX_DATA_FILE_VER)

--����C�����õ�table���ṹΪ {
--  �ļ����,ͨ����,��ַ,����������
--  1, 0, 0x08000000, "1234567",   0��ʾȫ��ͨ������һ���� 1��ʾ��1ͨ��4��ʾ4ͨ��
--  1, 1, 0x08000200, "AAA",
--  1, 2, 0x08000300, "BBBB",
-- }

FIX_TABLE = {}		--ȫ�ֱ���
	
--��������ǰִ��һ��
function fix_data_begin(void)
	local idx = 1
	local ch_num
	local i
	local j
	local k

	if (MULTI_MODE > 0) then
		ch_num = MULTI_MODE
	else
		ch_num = 1
	end

	--�����SN
	if (SN_ENABLE >= 1) then
		for i = 1, ch_num, 1 do
			SN_DATA[i] = sn_new()	--�����ϴ�SN�����µ�SN
			FIX_TABLE[idx] = SN_ENABLE		--�ļ����
			idx = idx + 1
			FIX_TABLE[idx] = i		--ͨ����1-4
			idx = idx + 1
			FIX_TABLE[idx] = SN_ADDR
			idx = idx + 1
			FIX_TABLE[idx] = SN_DATA[i]
			idx = idx + 1
		end
	end

--	CONST_CFG = {
--		1, 0x00000010, "12 34",	--ʹ��λ, �洢��ַ, �ַ�������
--		1, 0x00000020, "34 56",
--		1, 0x00000030, "56",
--		1, 0x00000040, "88"
--	}
	--����
	for i = 1, 12, 3 do
		if (CONST_CFG[i] >= 1) then
			FIX_TABLE[idx] = CONST_CFG[i]		--�ļ����
			idx = idx + 1		
			FIX_TABLE[idx] = 0	--ͨ����0
			idx = idx + 1
			FIX_TABLE[idx] = CONST_CFG[i+1]
			idx = idx + 1
			FIX_TABLE[idx] = hex_to_bin(CONST_CFG[i+2])
			idx = idx + 1
		end
	end

	--�������������
	if (DATE_ENABLE >= 1) then
		DATE_DATA = date_new()
		FIX_TABLE[idx] = DATE_ENABLE		--�ļ����
		idx = idx + 1	
		FIX_TABLE[idx] = 0		--ͨ����0 ��ʾ����ͨ��һ��
		idx = idx + 1
		FIX_TABLE[idx] = DATE_ADDR
		idx = idx + 1
		FIX_TABLE[idx] = DATE_DATA
		idx = idx + 1
	end

	--��������
	if (TOOL_FACTORY_ENABLE >= 1) then
		local tid = pg_read_c_var("FactoryId")

		print("FactoryId = ", tid)
		for i = 1, ch_num, 1 do
			FIX_TABLE[idx] = TOOL_FACTORY_ENABLE		--�ļ����
			idx = idx + 1			
			FIX_TABLE[idx] = 0
			idx = idx + 1
			FIX_TABLE[idx] = TOOL_FACTORY_ADDR
			idx = idx + 1
			FIX_TABLE[idx] = string.char(tid & 0xFF)
			idx = idx + 1
		end
	end

	--��¼�����
	if (TOOL_SN_ENABLE >= 1) then
		local tid = pg_read_c_var("ToolSn")

		print("ToolSn = ", tid)
		for i = 1, ch_num, 1 do
			FIX_TABLE[idx] = TOOL_SN_ENABLE		--�ļ����
			idx = idx + 1		
			FIX_TABLE[idx] = 0
			idx = idx + 1
			FIX_TABLE[idx] = TOOL_SN_ADDR
			idx = idx + 1
			FIX_TABLE[idx] = string.char(tid & 0xFF)
			idx = idx + 1
		end
	end

	--��¼��ͨ��(1-4)
	if (TOOL_CHANNAL_ENABLE >= 1) then
		for i = 1, ch_num, 1 do
			FIX_TABLE[idx] = TOOL_CHANNAL_ENABLE		--�ļ����
			idx = idx + 1		
			FIX_TABLE[idx] = i
			idx = idx + 1
			FIX_TABLE[idx] = TOOL_CHANNAL_ADDR
			idx = idx + 1
			FIX_TABLE[idx] = string.char(i)
			idx = idx + 1
		end
	end

	--	RNG_CONST_DATA = ""
	--	RNG_CONST_ADDR = 0x08000000
	if (RNG_CONST_DATA ~= "") then
		if (ENCRYPT_RND_IDX == 0) then
			for i = 1, ch_num, 1 do
				ENCRYPT_RND_DATA[i] = hex_to_bin(RNG_CONST_DATA)	--�������ݣ����ں������
			end
		end
	end

	--	RNG_TAB = {
	--		0,0x08000000,4, --�����#1
	--		0,0x08000000,4, --�����#2
	--		0,0x08000000,4, --�����#3
	--		0,0x08000000,4, --�����#4
	--	}
	k = 1
	for j = 1,4,1 do
		if (RNG_TAB[k] >= 1) then
			for i = 1, ch_num, 1 do
				FIX_TABLE[idx] = RNG_TAB[k]		--�ļ����
				idx = idx + 1
				FIX_TABLE[idx] = i		--ͨ����
				idx = idx + 1
				FIX_TABLE[idx] = RNG_TAB[k + 1]		--��ַ
				idx = idx + 1
				FIX_TABLE[idx] = get_rng(RNG_TAB[k + 2])	--����

				if (ENCRYPT_RND_IDX == j) then
					for i = 1, ch_num, 1 do
						ENCRYPT_RND_DATA[i] = FIX_TABLE[idx]	--�������ݣ����ں������
					end
				end

				idx = idx + 1
			end
		end
		k = k + 3
	end

	--��uid
	local uid
	if (MCU_ReadUID == nil) then
		UID = {pg_read_mem(UID_ADDR, UID_BYTES)}
	else
		UID = {MCU_ReadUID(UID_ADDR, UID_BYTES)}
	end

	if (UID[1] == 1) then
		local str
		for i = 1,ch_num,1 do
			str = string.format("uid%d = ", i)
			str = str..bin2hex(UID[i+1]) print(str)

			if (ENCRYPT_ENABLE >= 1) then
				FIX_TABLE[idx] = ENCRYPT_ENABLE		--�ļ����
				idx = idx + 1			
				FIX_TABLE[idx] = i		--ͨ����
				idx = idx + 1
				FIX_TABLE[idx] = ENCRYPT_ADDR	--��ַ
				idx = idx + 1
				FIX_TABLE[idx] = string.sub(encrypt(UID[i + 1], ENCRYPT_RND_DATA[i]), 1,ENCRYPT_LEN)	--����
				idx = idx + 1
			end
		end
	end
end

--��ӡ�������
function PringFixData(void)
	print("FIX_TABLE count = ", #FIX_TABLE)

	local i
	local str
	for i = 1,#FIX_TABLE, 4 do
	   str = string.format("%d, %d, 0x%08X,", FIX_TABLE[i], FIX_TABLE[i + 1], FIX_TABLE[i+2])
	   str = str.." "..bin2hex(FIX_TABLE[i+3])
	   print(str)
	end
end

--��Ʒ���SN���ɺ��� ��last_sn��һ��UINT32�������룩����Ƕ������ַ���
function sn_new(void)
	local bin = {}
	local out = {}
	local sn1
	local s
	local i

	if (SN_TYPE == 0) then	--��ͨ����
		sn1 = pg_read_sn()	--���ϴ�SN ��������������
		str = string.format("Last SN. = %d", sn1) print(str)
		pg_write_sn(sn1 + 1)	--��̳ɹ���Żᱣ�汾��SN
	else
		--1970������
		sn1 = os.time()
	end

	if (SN_FORMAT == 0) then
		s = ""
		for i = 1,SN_LEN,1 do
			s = s..string.char((sn1 >> (8 * (i - 1))) & 0xFF)
		end
	else
		if (SN_FORMAT == 1) then
			s = ""
			for i = 1,SN_LEN,1 do
				s = s..string.char((sn1 >> (8 * (SN_LEN - i))) & 0xFF)
			end
		else
			local c
			s = ""
			for i = 1,SN_LEN,1 do
				c = math.floor(sn1 / (10 ^ (SN_LEN - i)))
				c = c % 10
				print(c)
				s = s..string.format("%d", c)
			end
		end
	end

	return s
end

--��������
function date_new(void)
	local s
	local tt

	tt = os.date("DateTime %Y-%m-%d %H:%M:%S")
	print(tt)

	if (DATE_FORMAT == 0) then
		tt = os.date("%Y%m%d%H%M%S")	--����ASCII�ַ��� 2020 01 21 23 25 01
		s = tt
	else if (DATE_FORMAT == 1) then	--1 : 1970��׼����, 4�ֽ�,С��
			tt = os.time()
			s = string.char((tt >> 24) & 0xFF)..string.char((tt >> 16) & 0xFF)
				..string.char((tt >> 8) & 0xFF)..string.char((tt >> 0) & 0xFF)
	else if (DATE_FORMAT == 2) then	--2 : 1970��׼����, 4�ֽ�,���
			tt = os.time()
			s = string.char((tt >> 24) & 0xFF)..string.char((tt >> 16) & 0xFF)
				..string.char((tt >> 8) & 0xFF)..string.char((tt >> 0) & 0xFF)
	else if (DATE_FORMAT == 3) then	--3 : 1970��׼����, 8�ֽ�,С��
			tt = os.time()
			s = string.char((tt >> 0) & 0xFF)..string.char((tt >> 8) & 0xFF)
				..string.char((tt >> 16) & 0xFF)..string.char((tt >> 24) & 0xFF)
				..string.char((tt >> 32) & 0xFF)..string.char((tt >> 40) & 0xFF)
				..string.char((tt >> 48) & 0xFF)..string.char((tt >> 56) & 0xFF)
	else if (DATE_FORMAT == 4) then	--4 : 1970��׼����, 4�ֽ�,���
			tt = os.time()
			s = string.char((tt >> 56) & 0xFF)..string.char((tt >> 48) & 0xFF)
				..string.char((tt >> 40) & 0xFF)..string.char((tt >> 32) & 0xFF)
				..string.char((tt >> 24) & 0xFF)..string.char((tt >> 16) & 0xFF)
				..string.char((tt >> 8) & 0xFF)..string.char((tt >> 0) & 0xFF)
		else
			s = ""
		end
		end
		end
		end
	end

	return s
end

---------------------------����-----------------------------------
