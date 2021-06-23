-------------------------------------------------------
-- �ļ��� : machine_if.lua
-- ˵  �� : ��¼��̨�ӿ��źŶ���
-- Copyright (C), 2020-2030, ���������� www.armfly.com
-------------------------------------------------------
MACHINE_FILE_VER = 1.2  --�ļ��汾

print("load \"machine_if.lua\" ok. version = ",MACHINE_FILE_VER)

MACHINE_INTERFACE = 1	--��¼��̨�ӿ�����

ABORT_ON_ERROR = 1	--1��ʾ����ʱȫ��ͨ������ֹ

MI_STD_OK_NG_DELAY = 200	--��׼�źţ�OK NG�źų���ʱ�䣬ms��0��ʾһֱ����
MI_EOT_TIME1 = 50	--EOT�ź�ʱ���ӳ�1
MI_EOT_TIME2 = 90	--EOT�ź�ʱ���ӳ�2
MI_EOT_TIME3 = 50	--EOT�ź�ʱ���ӳ�3

SWD_BOARD = 0	--��·ģʽת�Ӱ����� 0�Ǽ�棬1����չ��IO��SWDת�Ӱ�
function AutoDetecSwdBorad(void)
	local re = 0
	local i

	uart_cfg(1, 115200, 0, 8, 1)	--����RS232/RS485���ڲ���
	--delayms(20)
	
	for i = 1,1,1 do
		re = re + modbus_write_u16(1, 50, 253, 0x1000 + 3 * (i - 1), 1, 1, 1)
	end
	
	if (re == 0) then	--ͨ��������
		SWD_BOARD = 1  --��̨ר�ð� 1��4ת�Ӱ�
	else
		SWD_BOARD = 0;
	end

	MULTI_MODE = pg_read_c_var("MultiProgMode")

	mi_CfgGpio()	
end

--��START���ŵ�ƽ
function ReadStartLevel(void)
	local x
	
	if MACHINE_INTERFACE > 0 then	
		if (SWD_BOARD == 0) then		
			if (read_analog(4) < TVCC_VOLT * 0.6) then
				x = 1
			else		
				if (MULTI_MODE == 0) then
					x = gpio_read(7)	--D7 = START�ź�(�ڴ�����)
				else
					x = gpio_read(1)	--D1 = START�ź�
				end
			end
		else
			local re
			
			--������ָ���
			--modbus_read_di(port, timeout, addr485, regaddr, regnum)\r\n--parameter erro
			re,x = modbus_read_di(1, 20, 253, 0x1000, 1)	--��0x1000�Ĵ��� START
			if (re == 0) then
				if (x == 0) then
					x = 1
				else
					x = 0
				end
			else
				x = 1
			end
		end
	else
		x = 1
	end
	return x
end
	
--��¼����ʱִ��(c�����е���), 20ms��ִ��һ��
g_last_x = -1
function mi_idle(void)
	local x

	x = ReadStartLevel()	
	if (g_last_x == 1) then
		if (x == 0) then
			put_msg(0x00001001, 0)
		end
	end
	g_last_x = x
end

--��ʼ��� (��lua���� start_prog()����)
function mi_begin(void)
	-- BUSY = 0, OK = 1, NG = 1
	local ch_num
	
	if (MULTI_MODE > 0) then
		ch_num = MULTI_MODE
	else
		ch_num = 1
	end
	
	if (MACHINE_INTERFACE == 1) then
		--BUSY(EOT) = 0  OK = 1  NG = 1
		for i = 1,ch_num,1 do
			mi_SetOutpin(i, 0, 1, 1)
		end
	end
	
	if (MACHINE_INTERFACE == 2) then	--EOT����
		--BUSY(EOT) = 1  OK = 1  NG = 1
		for i = 1,ch_num,1 do
			mi_SetOutpin(i, 1, 1, 1)
		end	
	end
end

--��̽�����ֹ (��lua���� start_prog()����)
--ֻ�г����ͨ������NG��Ч
function mi_abort(err_flag)
	local ch_num
		
	if (MULTI_MODE > 0) then
		ch_num = MULTI_MODE
	else
		ch_num = 1
	end

	if (MACHINE_INTERFACE == 1) then	--��׼�ź� BUSY OK NG
		for i = 1,ch_num,1 do
			if (err_flag[i] == 1) then	--ʧ��
				mi_SetOutpin(i, 1, 1, 0) --BUSY(EOT) = 1  OK = 1  NG = 0
			end
			
			if (err_flag[i] == 0) then	--�ɹ�
				mi_SetOutpin(i, 1, 0, 1) --BUSY(EOT) = 1  OK = 0  NG = 1
			end

			if (err_flag[i] == 2) then  --��⵽IC������δ��ʼ��¼
				mi_SetOutpin(i, 1, 1, 1) --BUSY(EOT) = 1  OK = 1  NG = 1
			end			
		end
		
		if (MI_STD_OK_NG_DELAY > 0) then
			delayms(MI_STD_OK_NG_DELAY)	--�ӳ�200ms	
			for i = 1,ch_num,1 do
				mi_SetOutpin(i, 1, 1, 1) --BUSY(EOT) = 1  OK = 1  NG = 1
			end	
		end
	end

	if (MACHINE_INTERFACE == 2) then	--EOT����  EOT OK NG (ִ��ʱ��140ms)		
		for i = 1,ch_num,1 do
			if (err_flag[i] == 0) then
				mi_SetOutpin(i, 1, 0, 1) --BUSY(EOT) = 1  OK = 0  NG = 1
			end
			
			if (err_flag[i] == 1) then
				mi_SetOutpin(i, 1, 1, 0) --BUSY(EOT) = 1  OK = 1  NG = 0
			end
		end	
		delayms(MI_EOT_TIME1)	--�ӳ�50ms
		for i = 1,ch_num,1 do
			if (err_flag[i] == 0) then
				mi_SetOutpin(i, 0, 0, 1) --BUSY(EOT) = 0  OK = 0  NG = 1
			end
			
			if (err_flag[i] == 1) then
				mi_SetOutpin(i, 0, 1, 0) --BUSY(EOT) = 0  OK = 1  NG = 0
			end
			
			if (err_flag[i] == 2) then	--��⵽IC������δ��ʼ��¼
				mi_SetOutpin(i, 0, 1, 1) --BUSY(EOT) = 0  OK = 1  NG = 1
			end			
		end		
		delayms(MI_EOT_TIME2)	--�ӳ�90ms
		for i = 1,ch_num,1 do
			if (err_flag[i] == 0) then
				mi_SetOutpin(i, 1, 0, 1) --BUSY(EOT) = 1  OK = 0  NG = 1
			end
			
			if (err_flag[i] == 1) then
				mi_SetOutpin(i, 1, 1, 0) --BUSY(EOT) = 1  OK = 1  NG = 0
			end

			if (err_flag[i] == 2) then	--��⵽IC������δ��ʼ��¼
				mi_SetOutpin(i, 1, 1, 1) --BUSY(EOT) = 1  OK = 1  NG = 1
			end				
		end	
		delayms(MI_EOT_TIME3)	--�ӳ�50ms
		for i = 1,ch_num,1 do
			mi_SetOutpin(i, 1, 1, 1) --BUSY(EOT) = 1  OK = 1  NG = 1
		end	
	end		
end

--����GPIO
function mi_CfgGpio(void)
	if (MACHINE_INTERFACE > 0) then
		if (MULTI_MODE == 0) then	--��·ģʽ
			--D7 = START   D1 = BUSY   D2 = OK  D3 = NG
			gpio_cfg(7,0)
			gpio_cfg(1,1)
			gpio_cfg(2,1)
			gpio_cfg(3,1)
		else	--��·ģʽ
			if (SWD_BOARD == 0) then	--���1��4 SWDת�Ӱ�(��֧��1-2·��̨�ź�)
				if (MULTI_MODE <= 2) then
					--1·��2·ģʽ֧�ֻ�̨�ź�
					--D1 = START����  
					--D4 = SWDIO_3	 ���� OK1
					--D6 = SWCLK_3	 ���� NG1
					--D3 = SWDIO_4	 ���� OK2			
					--D2 = SWCLK_4	 ���� NG2
					--TTLTX = BUSY/EOT(����)
					gpio_cfg(1,0)
					gpio_cfg(4,1)
					gpio_cfg(6,1)
					gpio_cfg(3,1)
					gpio_cfg(2,1)		
					gpio_cfg(10,1)	--D10 TTLTX ������ͨGPIO				
				else	
					--3·��4·ģʽ��֧�ֻ�̨�ź�
				end
			end

			if (SWD_BOARD == 1) then	--��̨ר�� 1��4 SWDת�Ӱ� (ͨ�����ڿ���)
				local re
				local i
				
				--100ms��ʱ��485��ַ253, д0x1000�Ĵ���������3��, ����3��
				for i = 1,3,1 do
					re = modbus_write_u16(1, 50, 253, 0x1000 + 3 * (ch - 1), busy, ok, ng)
					if (re == 0) then	--�ɹ�
						break
					end
				end
				if (re ~= 0) then	--ת�Ӱ����
					pg_print_text("SWDת�Ӱ�ͨ�Ź���")
				end
			end		
		end	
	end
end

--�������״̬��ch : 1-4 ��ʾͨ��
function mi_SetOutpin(ch, busy, ok, ng)
	local str
	
	str = string.format("--CH%d: BUSY/EOT=%d OK=%d NG=%d",  ch, busy, ok, ng)
	print(str)
	if (MULTI_MODE == 0) then	--��·ģʽ
		--D0 = START   D1 = BUSY   D2 = OK  D3 = NG
		gpio_write(1, busy)
		gpio_write(2, ok)
		gpio_write(3, ng)	
	else	--��·ģʽ
		if (SWD_BOARD == 0) then	--���1��4 SWDת�Ӱ�(��֧��1-2·��̨�ź�)
			if (MULTI_MODE <= 2) then
				--
				--D1 = START����  
				--D4 = SWDIO_3	 ���� OK1
				--D6 = SWCLK_3	 ���� NG1
				--D3 = SWDIO_4	 ���� OK2			
				--D2 = SWCLK_4	 ���� NG2
				--D10 = BUSY ����BUSY
				if (ch == 1) then
					gpio_write(10, busy)				
					gpio_write(4, ok)
					gpio_write(6, ng)				
				end
				
				if (ch == 2) then
					gpio_write(3, ok)
					gpio_write(2, ng)					
				end				
			else	
				--3·��4·ģʽ��֧�ֻ�̨�ź�
			end
		end

		if (SWD_BOARD == 1) then	--��̨ר�� 1��4 SWDת�Ӱ� (ͨ�����ڿ���)
			local re
			local i
			
			--100ms��ʱ��485��ַ253, д0x1000�Ĵ���������3��, ����3��
			for i = 1,3,1 do
				re = modbus_write_u16(1, 50, 253, 0x1000 + 3 * (ch - 1), busy, ok, ng)
				if (re == 0) then	--�ɹ�
					break
				end
			end
			if (re ~= 0) then	--ת�Ӱ����
				pg_print_text("SWDת�Ӱ�ͨ�Ź���")
			end
		end		
	end	
end

AutoDetecSwdBorad()	--���ת�Ӱ�����, ѡ��lua�ļ�ʱִ��һ�μ���

---------------------------����-----------------------------------
