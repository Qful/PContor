--F01=����RC602,AutoTestRC608(8)
--F02=ʶ��+����602,AutoDetect()
--F03=RC402 У׼+���, AutoCalibTest()
--F04=RC402 У׼ģ����,Calib402()
--F05=RC402 �Զ����,AutoTestRC202()
--F06=RC402 ����Ϊ0-20mA,ConfigChanRange(0)
--F07=RC402 ����0-10V,ConfigChanRange(1)

--xF05=��6ͨ��ģ����,ReadAllChan(0)
--xF07=����ͨ��1��ѹ,SetCh1VoltMV(5000)
--xF08=����ͨ��2��ѹ,SetCh2VoltMV(10000)
--xF09=����ͨ��1����,SetCh1CurrMA(4.0)
--xF10=����ͨ��2����,SetCh2CurrMA(19.0)
--xF11=��NTC�¶�,ReadTemp()

--xF13=����0-10V���Զ�,TestChVolt(1)
--xF14=����0-20mA���Զ�,TestChCurr(1)

print("����IOģ������Ѽ���")

local Addr485 = 1
local COMx = 1
local TimeOut = 1000
local RC604Addr	= 250
local RC302Addr	= 251

local RC202_NORMAL	= 0xC202
local RD202_20MA    = 0x202A
local RD202_18B20   = 0x202B
local RD202_PULSE	= 0x202C

local REG03_AO_VOLT_01 = 0x0420		--RC302 V1�����ѹ mV
local REG03_AO_CURR_01 = 0x0440		--RC302 ������� uA

local CALIB_VOTL1 = 4000		--500mV  У׼��ѹ1
local CALIB_VOTL2 = 9500	--95000mV У׼��ѹ2

local CALIB_CURR1 = 4000	--4mA  У׼����1
local CALIB_CURR2 = 19000	--19mA У׼����2

local REG06_CALIB_KEY = 0x2FF0		--У׼���ء�д�� 0x55AA ����дУ׼����������ֵ��ֹд��
local REG06_CALIB_SET_RANGE = 0x2FF1		--��ʱ����ģ��ͨ�����̵�λ�� ���ֽ�ͨ���ţ����ֽ��ǵ�λ

local CALIB_REG_STEP 	= 0x100   	--ͨ���Ĵ�������
local RANGE_REG_STEP 	= 12		--���̼Ĵ�������
local REG03_CC01_K0 	= 0x3000	--ͨ��1 ��ѹ��1��  Kֵ
	local CC01_K_0 		= 0		--Kֵ
	local CC01_B_0 		= 2		--Bֵ
	local CC01_T_5K_0 	= 4		--У׼ʱ�����¶�ֵ - NTC��������
	local CC01_COEF_K_0 = 6		--�¶Ȳ���ϵ��K
	local CC01_COEF_B_0 = 8		--�¶Ȳ���ϵ��B
	local CC01_ZERO_ADC_0 = 10		--��������ADC

	local CC01_K_1 		= 12		--Kֵ
	local CC01_B_1	 	= 14		--Bֵ
	local CC01_T_5K_1 	= 16		--У׼ʱ�����¶�ֵ
	local CC01_COEF_K_1	 = 18		--У׼ʱ��λADC
	local CC01_COEF_B_1	 = 20		--�¶�ϵ�� 0=������
	local CC01_ZERO_ADC_1 = 22		--��������ADC

local REG03_CC02_K0	= 0x3100		--ͨ��2 ��ѹ��1��  Kֵ

--
--ģ��ͨ���������ֵ output �� int32 ��ʽ. 32��ͨ������64���Ĵ���
local REG04_CH01_OUT_I = 0x0010		--ͨ��1-32 ���ֵ���з���32λ��������2�ֽ���ǰ��2���Ĵ���

--ģ��ͨ���������ֵ output �� float ��ʽ. 32��ͨ������64���Ĵ���
local REG04_CH01_OUT_F = 0x0050		--ͨ��1-32 ���ֵ������������2�ֽ���ǰ��2���Ĵ���

--ͨ��1 - 8 ADCֵ ��Чֵ
local REG04_CH01_ADC_RMS = 0x0090		--ͨ��1-32 ADCֵ(NTC�Ǿ�ֵ����������ʱRMSֵ��, ����������2�ֽ���ǰ��2���Ĵ���

--ͨ��1 - 8 �������ֵ��float��ʽ
local REG04_CH01_VALUE	= 0x00D0		--ͨ��1-32 ����ֵ������ ��ѹ ������������������2�ֽ���ǰ��2���Ĵ���

local REG04_CH01_ADC_AVG = 0x0110		--ͨ��1-32 ADC��ֵ

local REG04_CH01_SIGNAL_TYPE = 0x0150		--ͨ��1-32 �ź����ͣ��������䣬�����ȡ�������ֽ�bit0��ʾ���� ���ã����ֽڱ�ʾ����


local REG03_CF01_EN = 0x2000		--ͨ��ʹ�� 0��ʾ��ʹ��  1��ʾʹ��
	local CF01_EN			= 0x00		--ͨ��ʹ�� 0��ʾ��ʹ��  1��ʾʹ��
	local CF01_SIGN_TYPE		= 0x01		--�������������ź����� ͳһ���� ����������ѹ���ȵ��裩
	local CF01_FILT_MODE		= 0x02		--�˲�ģʽ 0 ��������
	local CF01_FILT_TIME		= 0x03		--�˲�ʱ�� ��λ0.1s
	local CF01_CORR_K			= 0x04		--�������� y=kx+b ֮k�� ��������4�ֽ�
	local CF01_CORR_B			= 0x06		--�������� y=kx+b ֮b�� ��������4�ֽ�
	local CF01_LIMIT_LOWER	= 0x08		--�������ޣ�������
	local CF01_LIMIT_UPPER	= 0x0A		--�������ޣ�������
	local CF01_CUTOFF			= 0x0C		--С�ź��г���ֱ�  .*/
	local CF01_ALARM_EN		= 0x0D		--����ʹ�� 1��ʾʹ�� 0 ��ֹ
	local CF01_ALARM_LOWER	= 0x0E		--�������� ������
	local CF01_ALARM_UPPER	= 0x10		--��������
	local CF01_ALARM_BACK		= 0x12		--�����ز� ������
	local CF01_ALARM_ACTION	= 0x14		--����ʱ���� Y1-Y24 �ĸ��̵���״̬Ϊ1��������ͨ��ӳ�䵽һ���̵���������ܱ�����ǣ�*/
	local CF01_TEMP_CORR		= 0x15		--�ȵ�ż����¶������� �з���16bit���� 0.1���϶�


uart_cfg(COMx, 9600, 0, 8, 1)
beep()

--ʧ��
function PrintError(void)
	print("-------����ʧ��----------")
	beep(5,5,3)
end

--�ɹ�
function PrintOk(void)
	print("-------���Գɹ�----------")
	beep()
end

--�Զ�У׼+����
function AutoCalibTest(void)
	if (Calib402() == 0) then
		print("")
		AutoTestRC202()
	end
end

--��ģ�������
function ReadAllChan(n)
	local err, y1,y2,y3,y4,y5,y6,y7
	local str

	err,y1,y2,y3,y4,y5,y6,y7 = modbus_read_float(COMx,TimeOut,Addr485,REG04_CH01_OUT_F,7)

	if (err == 0) then
		str = string.format("%d %f, %f, %f, %f, %f, %f, %f", n, y1,y2,y3,y4,y5,y6,y7) print(str)
	else
		str = string.format("������� = %d", err) print(str)
	end
end

--��NTC�¶�
function ReadTemp(void)
	local err, y1
	local str

	err, y1 = modbus_read_float(COMx,TimeOut,Addr485,REG04_CH01_OUT_F + 12,1)
	if (err == 0) then
		str = string.format("�¶� = %f ��", y1) print(str)
	else
		str = string.format("������� = %d", err) print(str)
	end
end

--��CH1�����ѹ
function SetCh1VoltMV(volt)
	local err, y1
	local str

	--�л��̵���,ͨ��1��ѹ
	err = modbus_write_do(COMx,TimeOut,RC604Addr,1,1,0,0,0)

	--����DAC��ѹ - У׼��1
	err = err + modbus_write_u16(COMx,TimeOut,RC302Addr,REG03_AO_VOLT_01, volt)
	if (err == 0) then
		str = string.format("CH1 ��ѹ = %d mV", volt) print(str)
	else
		str = string.format("������� = %d", err) print(str)
	end
end

--��CH2�����ѹ
function SetCh2VoltMV(volt)
	local err, y1
	local str

	--�л��̵���,ͨ��2��ѹ
	err = modbus_write_do(COMx,TimeOut,RC604Addr,1,0,1,0,0)

	--����DAC��ѹ - У׼��1
	err = err + modbus_write_u16(COMx,TimeOut,RC302Addr,REG03_AO_VOLT_01, volt)
	if (err == 0) then
		str = string.format("CH2 ��ѹ = %d mV", volt) print(str)
	else
		str = string.format("������� = %d", err) print(str)
	end
end

--��CH1�������
function SetCh1CurrMA(curr)
	local err, y1
	local str

	--�л��̵���,ͨ��1��ѹ
	err = modbus_write_do(COMx,TimeOut,RC604Addr,1,0,0,1,0)

	--����DAC��ѹ - У׼��1
	err = err + modbus_write_u16(COMx,TimeOut,RC302Addr,REG03_AO_CURR_01, curr * 1000)
	if (err == 0) then
		str = string.format("CH1 ���� = %f mA", curr) print(str)
	else
		str = string.format("������� = %d", err) print(str)
	end
end

--��CH2�������
function SetCh2CurrMA(curr)
	local err, y1
	local str

	--�л��̵���,ͨ��1��ѹ
	err = modbus_write_do(COMx,TimeOut,RC604Addr,1,0,0,0,1)

	--����DAC��ѹ - У׼��1
	err = err + modbus_write_u16(COMx,TimeOut,RC302Addr,REG03_AO_CURR_01, curr * 1000)
	if (err == 0) then
		str = string.format("CH2 ���� = %f mA", curr) print(str)
	else
		str = string.format("������� = %d", err) print(str)
	end
end

--����ͨ������Ϊ����ģʽ
function ConfigChanRange(_rg)
	local err, y1
	local str

	--ͨ��1
	err = modbus_write_u16(COMx,TimeOut,Addr485,REG03_CF01_EN + CF01_SIGN_TYPE, _rg)
	if (err > 0) then goto quit end

	--ͨ��2
	err = err + modbus_write_u16(COMx,TimeOut,Addr485,REG03_CF01_EN + CF01_SIGN_TYPE + 0x20, _rg)
	if (err > 0) then goto quit end

::quit::
	if (err == 0) then
		if (_rg == 0) then
			str = string.format("����2��ͨ��Ϊ0-20mA�ɹ�") print(str)
		else
			str = string.format("����2��ͨ��Ϊ0-10V�ɹ�") print(str)
		end
	else
		str = string.format("������� = %d", err) print(str)
	end
end

--����¶ȷ�Χ�� ����1��ʾOK
function Check18B20(temp1, temp2)
	local diff

	if (temp1 < 5 or temp2 < 5 or temp1 > 40  or temp2 > 40) then
		return 0
	end

	if (temp1 > temp2) then
		diff = temp1 - temp2
	else
		diff = temp2 - temp1
	end

	if (diff > 2) then
		return 0
	else
		return 1
	end
end

--�Զ�����
function AutoTestRC202(void)
	local err,y1,y2,y3,y4
	local str
	local time1,time2
	local Model
	local RegAddr

	print("-------��ʼ����------------------------------------")
	time1 = get_runtime()

	--��Ӳ���ͺźͰ汾
	err,y1,y2 = modbus_read_u16(COMx,TimeOut,Addr485,0x9000,2)
	if (err > 0) then goto quit end
	str = string.format("�ͺ� = %04X �̼��汾 = V%X.%02X", y1, y2 >> 8, y2 & 0xFF) print(str)

	Model = y1

	if (Model == RC202_NORMAL) then
		str = string.format("��ʶ�����: RC602")  print(str)
	else
		if (Model == RD202_20MA) then
			str = string.format("��ʶ�����: RD202-20MA")  print(str)
		else
			if (Model == RD202_18B20) then
				str = string.format("��ʶ�����: RD202-18B20")  print(str)
			else
				if (Model == RD202_PULSE) then
					str = string.format("��ʶ�����: RD202-PULSE")  print(str)
				else
					if (Model == 0xC402) then
						str = string.format("��ʶ�����: RC402")  print(str)
					else
						str = string.format("δ֪����")  print(str)
						goto quit
					end
				end
			end
		end
	end

	--����DS18B20����
	if (Model == RC202_NORMAL or Model == RD202_18B20) then
		str = string.format("\r\n��1������DS18B20����")  print(str)
		if (Model == RC202_NORMAL) then
			RegAddr = REG04_CH01_OUT_F + 8
		else
			RegAddr = REG04_CH01_OUT_Fl
		end
		err,y1,y2 = modbus_read_float(COMx,TimeOut,Addr485,RegAddr,2)
		if (err > 0) then
			goto quit
		end

		if (Check18B20(y1,y2) == 1) then
			str = string.format(" %0.1f��, %0.1f��  -- OK ", y1, y2)  print(str)
		else
			str = string.format(" %0.1f��, %0.1f��  -- Err", y1, y2)  print(str)
			err = 9
			goto quit
		end
	end

	--�������幦��
	if (Model == RC202_NORMAL or Model == RD202_PULSE) then
		str = string.format("\r\n��2���������幦��")  print(str)
		if (Model == RC202_NORMAL) then
			RegAddr = REG04_CH01_OUT_F + 4
		else
			RegAddr = REG04_CH01_OUT_F
		end
		err,y1,y2 = modbus_read_float(COMx,TimeOut,Addr485,RegAddr,2)
		if (err > 0) then
			goto quit
		end

		if (y1 > 90 and y2 > 90 and y1 < 110 and y2 < 110 ) then
			str = string.format(" %0.2fHz, %0.2fHz  -- OK ", y1, y2)  print(str)
		else
			str = string.format(" %0.2fHz, %0.2fHz   -- Err", y1, y2)  print(str)
			err = 9
			goto quit
		end
	end

	--����ģ������⹦��
	if (Model == RC202_NORMAL or Model == RD202_20MA or Model == 0xC402) then
		str = string.format("\r\n��4������ģ��������")  print(str)

		ConfigChanRange(1)	--�л�����ѹ��λ

		--����DAC��ѹ
		err = modbus_write_u16(COMx,TimeOut,RC302Addr,REG03_AO_VOLT_01, 5000)
		if (err > 0) then goto quit end
		--�л��̵���
		err = modbus_write_do(COMx,TimeOut,RC604Addr,1, 1, 0,0,0)
		if (err > 0) then goto quit end
		delayms(1000)
		err,y1 = modbus_read_float(COMx,TimeOut,Addr485, REG04_CH01_OUT_F, 1)
		if (err > 0) then goto quit end
		if (math.abs(y1 - 5.0) < 0.02) then
			str = string.format(" CH1 5.000V, %0.3fV  -- OK ", y1)   print(str)
		else
			str = string.format(" CH1 5.000V, %0.3fV  -- Err ", y1)  print(str)
			err = 9
			goto quit
		end

		--����DAC��ѹ
		err = modbus_write_u16(COMx,TimeOut,RC302Addr,REG03_AO_VOLT_01, 6000)
		if (err > 0) then goto quit end
		--�л��̵���
		err = modbus_write_do(COMx,TimeOut,RC604Addr,1, 0, 1,0,0)
		if (err > 0) then goto quit end
		delayms(1000)
		err,y1 = modbus_read_float(COMx,TimeOut,Addr485, REG04_CH01_OUT_F + 2, 1)
		if (err > 0) then goto quit end
		if (math.abs(y1 - 6.0) < 0.02) then
			str = string.format(" CH2 6.000V, %0.3fV  -- OK ", y1)   print(str)
		else
			str = string.format(" CH2 6.000V, %0.3fV  -- Err ", y1)  print(str)
			err = 9
			goto quit
		end

		print("")
		ConfigChanRange(0) --�л���������λ

		--����DAC����
		err = modbus_write_u16(COMx,TimeOut,RC302Addr,REG03_AO_CURR_01, 12000)
		if (err > 0) then goto quit end
		--�л��̵���
		err = modbus_write_do(COMx,TimeOut,RC604Addr,1, 0, 0,1,0)
		if (err > 0) then goto quit end
		delayms(1000)
		err,y1 = modbus_read_float(COMx,TimeOut,Addr485, REG04_CH01_OUT_F, 1)
		if (err > 0) then goto quit end
		if (math.abs(y1 - 12.0) < 0.02) then
			str = string.format(" CH1 12.000mA, %0.3fmA  -- OK ", y1)   print(str)
		else
			str = string.format(" CH1 12.000mA, %0.3fmA  -- Err ", y1)  print(str)
			err = 9
			goto quit
		end

		--����DAC����
		err = modbus_write_u16(COMx,TimeOut,RC302Addr,REG03_AO_CURR_01, 10000)
		if (err > 0) then goto quit end
		--�л��̵���
		err = modbus_write_do(COMx,TimeOut,RC604Addr,1, 0, 0,0,1)
		if (err > 0) then goto quit end
		delayms(1000)
		err,y1 = modbus_read_float(COMx,TimeOut,Addr485, REG04_CH01_OUT_F + 2, 1)
		if (err > 0) then goto quit end
		if (math.abs(y1 - 10.0) < 0.02) then
			str = string.format(" CH2 10.000mA, %0.3fmA  -- OK ", y1)   print(str)
		else
			str = string.format(" CH2 10.000mA, %0.3fmA  -- Err ", y1)  print(str)
			err = 9
			goto quit
		end
	end

	--���Լ̵�������
	if (Model == RC202_NORMAL or Model == RD202_PULSE) then
		str = string.format("\r\n��3�����Լ̵�������")  print(str)

		err = modbus_write_do(COMx,TimeOut,Addr485,1,0,0)
		if (err > 0) then goto quit end
		delayms(100)
		err,y1,y2 = modbus_read_di(COMx,TimeOut,RC604Addr,1,2)
		if (y1 == 0 and y2 == 0) then
			str = string.format(" y1 = 0(%d)�� y2 = 0(%d)  -- Ok ", y1, y2)  print(str)
		else
			str = string.format(" y1 = 0(%d)�� y2 = 0(%d)  -- Err ", y1, y2)  print(str)
			err = 9
			goto quit
		end

		err = modbus_write_do(COMx,TimeOut,Addr485,1,1,0)
		if (err > 0) then goto quit end
		delayms(100)
		err,y1,y2 = modbus_read_di(COMx,TimeOut,RC604Addr,1,2)
		if (y1 == 1 and y2 == 0) then
			str = string.format(" y1 = 1(%d)�� y2 = 0(%d)  -- Ok ", y1, y2)  print(str)
		else
			str = string.format(" y1 = 1(%d)�� y2 = 0(%d)  -- Err ", y1, y2)  print(str)
			err = 9
			goto quit
		end

		err = modbus_write_do(COMx,TimeOut,Addr485,1,0,1)
		if (err > 0) then goto quit end
		delayms(100)
		err,y1,y2 = modbus_read_di(COMx,TimeOut,RC604Addr,1,2)
		if (y1 == 0 and y2 == 1) then
			str = string.format(" y1 = 0(%d)�� y2 = 1(%d)  -- Ok ", y1, y2)  print(str)
		else
			str = string.format(" y1 = 0(%d)�� y2 = 1(%d)  -- Err ", y1, y2)  print(str)
			err = 9
			goto quit
		end

		err = modbus_write_do(COMx,TimeOut,Addr485,1,0,0)
		if (err > 0) then goto quit end
	end

::quit::
	if (err == 0) then
		PrintOk()
		time2 = get_runtime()
		time1 = (time2 - time1) / 1000
		str = string.format("  ִ��ʱ�� = %0.1f ��", time1)	   print(str)
	else
		if (err == 6) then
			print("RS485ͨ�Ŵ���")
		else
			str = string.format("������� = %d", err)
			print(str)
		end
		PrintError()
	end
end

function AutoTestRC608(void)
	local err,y1,y2,y3,y4
	local str
	local time1,time2
	local Model
	local SoftVer
	local RFHard
	local RFSoft
	local ModelStr = "xxxxx"
	local RegAddr
	local ch_num

	print("\r\n-------��ʼ����---------")
	time1 = get_runtime()

	uart_clear_rx(COMx)

	err =  modbus_write_u16(COMx, 1000, 1, 0x1007, 1)

	--��Ӳ���ͺźͰ汾
	err,Model,SoftVer = modbus_read_u16(COMx,TimeOut,Addr485,0x9000,2)
	if (err > 0) then goto quit end

	uart_clear_rx(COMx)
	err, RFHard, RFSoft = modbus_read_u16(COMx,TimeOut,Addr485,0x9009,2)
	if (err > 0) then goto quit end

	if (Model == 0xC602) then
		ModelStr = "RC602"
		ch_num = 2
	else
		if (Model == 0xC604) then
			ModelStr = "RC604"
			ch_num = 4
		else
			if (Model == 0xC608) then
				ModelStr = "RC608"
				ch_num = 8
			else
				if (Model == 0xC612) then
					ModelStr = "RC612"
					ch_num = 12
				else
					if (Model == 0xC616) then
						ModelStr = "RC616"
						ch_num = 16
					else
						if (Model == 0xC632) then
							ModelStr = "RC632"
							ch_num = 32
						else
							if (Model == 0xC601) then
								ModelStr = "RC601"
								ch_num = 1
							else
								ch_num = -1
							end
						end
					end
				end
			end
		end
	end

	if (RFHard == 0) then ModelStr = ModelStr.."-XXX" end
	if (RFHard == 1) then ModelStr = ModelStr.."-485" end
	if (RFHard == 2) then ModelStr = ModelStr.."-232" end
	if (RFHard == 0x30) then ModelStr = ModelStr.."-2K" end
	if (RFHard == 0x31) then ModelStr = ModelStr.."-8K" end

	if (RFHard == 0x32) then
		if ((RFSoft & 0xFF) == 0x14) then
			ModelStr = ModelStr.."-3K"
		else
			if ((RFSoft & 0xFF) == 0x1E) then
				ModelStr = ModelStr.."-7K"
			else
				ModelStr = ModelStr.."-xx"
			end
		end
	end

	if (RFHard == 0x82) then ModelStr = ModelStr.."-WiFi" end
	if (RFHard == 0x83) then ModelStr = ModelStr.."-NBIoT" end

	str = string.format("�ͺ�: %s  V%X.%02X", ModelStr, SoftVer >> 8, SoftVer & 0xFF) print(str)

	if (ch_num > 0) then
		TestRC608(ch_num)
	else

	end

	if (RFHard == 0x30 or RFHard == 0x32) then
		delayms(500)
		Test433M()
	end

::quit::
	if (err == 0) then
		time2 = get_runtime()
		time1 = (time2 - time1) / 1000
		str = string.format("  ִ��ʱ�� = %0.1f ��", time1)	   print(str)
	else
		if (err == 6) then
			print("RS485ͨ�Ŵ���")
		end
		PrintError()
	end
end

--�Զ�ʶ���ͺ�
function AutoDetect(void)
	local err,y1
	local i

	err = 0
	err = err + modbus_write_u16(COMx, 1000, Addr485, 0x9100, 4)
	if (err > 0) then goto quit end
	delayms(100)

	for i=1,50,1 do
		err,y1 = modbus_read_u16(COMx,1000, Addr485,0x9100, 1)
		if (err == 0) then
			if (y1 == 0) then
				break
			end
		end

		delayms(100)
	end

::quit::
	if (err > 0) then
		print("-------ʶ��ʧ��----------")
		beep(5,5,3)
	else
		print("-------ʶ��OK----------")
		AutoTestRC608()
	end
end

--�Զ�����RC601 RC602 604 608 612 616 632
function TestRC608(ch_num)
	local err,y1,y2,y3,y4,y5,y6,y7,y8
	local str
	local time1,time2
	local Model
	local RegAddr
	local S = {0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0}
	local Y = {0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0}
	local i
	local terr = 0
	local strerr = ""

	time1 = get_runtime()

	uart_clear_rx(COMx)
	err = modbus_write_do(COMx,TimeOut,Addr485,1, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0)
	if (err > 0) then goto quit end
	delayms(60)
	for i = 0, ch_num - 1, 1 do
		RegAddr = 1 + i

		uart_clear_rx(COMx)
		err, y1 = modbus_read_di(COMx,TimeOut,RC604Addr, RegAddr, 1)
		if (err > 0) then goto quit end

		uart_clear_rx(COMx)
		err = modbus_write_do(COMx,TimeOut,Addr485,RegAddr, 1)
		if (err > 0) then goto quit end
		delayms(60)
		uart_clear_rx(COMx)
		err, y2 = modbus_read_di(COMx,TimeOut,RC604Addr, RegAddr, 1)
		if (err > 0) then goto quit end

		if (y1 == 0 and y2 == 1) then
			--str = string.format(" Y%d  -- Ok ", i + 1)  print(str)
		else
			--str = string.format(" Y%d  -- Err %d,%d", i + 1, y1, y2)  print(str)
			err = 9
			terr = terr + 1
			strerr = strerr..string.format("Y%d ", i + 1)
		end
	end

	print("")
	uart_clear_rx(COMx)
	err = modbus_write_do(COMx,TimeOut,RC604Addr,1, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0)
	if (err > 0) then goto quit end
	delayms(60)
	for i = 0, ch_num - 1, 1 do
		RegAddr = 1 + i
		uart_clear_rx(COMx)
		err, y1 = modbus_read_di(COMx,TimeOut,Addr485, RegAddr, 1)
		if (err > 0) then goto quit end

		uart_clear_rx(COMx)
		err = modbus_write_do(COMx,TimeOut,RC604Addr,RegAddr, 1)
		if (err > 0) then goto quit end
		delayms(60)
		uart_clear_rx(COMx)
		err, y2 = modbus_read_di(COMx,TimeOut,Addr485, RegAddr, 1)
		if (err > 0) then goto quit end

		if (y1 == 0 and y2 == 1) then
			--str = string.format(" X%d  -- Ok ", i + 1)  print(str)
		else
			--str = string.format(" X%d  -- Err %d,%d", i + 1, y1, y2)  print(str)
			err = 9
			terr = terr + 1
			strerr = strerr..string.format("X%d ", i + 1)
		end
	end

::quit::
	if (err == 0 and terr == 0) then
		print("-------IO����OK----------")
		beep()
	else
		if (err == 6) then
			print("RS485ͨ�Ŵ���")
		else
			if (terr > 0) then
				str = "����:"..strerr	print(str)
			else
				str = string.format(" ������� = %d", terr)	print(str)
			end
		end

		print("-------IO����ʧ��----------")
		beep(5,5,3)
	end
end

--���433M��Ƶ����
function Test433M(void)
	local err,y1,y2,y3,y4
	local str
	local COM_TTL = 7
	local rx_len = 0
	local rx_str = ""

	err = 0
	err = err + modbus_write_u16(COMx, 1000, 1, 0x1007, 1)

	gpio_cfg(0, 5)	-- ����D0ΪUART����
	gpio_cfg(1, 5)	-- ����D1ΪUART����
	uart_cfg(COM_TTL, 9600, 0, 8, 1)

	--D2 D3 = M0 M1 = 1 ����ָ��ģʽ
	gpio_cfg(2,1)
	gpio_cfg(3,1)
	gpio_write(2,1)
	gpio_write(3,1)

	delayms(20)

	uart_clear_rx(COM_TTL)

	--0001 1111    0000 0000
	uart_send(COM_TTL, "\xC2\xFF\xFF\x1F\x17\x00")
	rx_len, rx_str = uart_recive(COM_TTL, 6, 100)
	print_hex(rx_len, rx_str)

	--M0 M1 = 0 ���빤��״̬
	gpio_write(2,0)
	gpio_write(3,0)
	delayms(20)

	err = err + modbus_write_do(COM_TTL,1000, 1, 1, 1)
	delayms(100)
	err = err + modbus_write_do(COM_TTL,1000, 1, 1, 0)

	err = err + modbus_write_u16(COMx, 1000, 1, 0x1007, 3)

	if (err > 0) then
		print("-------433M����ʧ��----------")
		beep(5,5,3)
	else
		print("-------433M����OK----------")
		beep()
	end
end

------------------------------------------------------

--���һ��ֵ�Ƿ��ڹ��Χ 1��ʾerr 0��ʾok
function check_err(data, mid, diff)
	local re
	local dd

	if (mid < 0) then
		dd = -mid * diff
	else
		dd = mid * diff
	end

	if ((data >= mid - dd) and  (data <= mid + dd)) then
		re = 0
	else
		re = 1
	end
	return re
end

--У׼  ����0��ʾ0K, 1��ʾ����
function Calib402(void)
	local Model
	local err,y1,y2,y3,y4
	local x1,x2,K,B
	local str
	local time1,time2
	local RegAddr
	local adc1
	local adc2
	local Temp
	local i
	local VoltAdc1 = 1202
	local VoltAdc2 = 2860
	local CurrAdc1 = 615
	local CurrAdc2 = 2923
	local DIFF = 0.05

	print("-------��ʼУ׼------------------------------------")
	time1 = get_runtime()

	err,y1,y2 = modbus_read_u16(COMx,TimeOut,RC604Addr,0x9000,2)
	if (err > 0) then
		str = string.format("δ�ҵ���װRC604") print(str)
		goto quit
	end

	err,y1,y2 = modbus_read_u16(COMx,TimeOut,RC302Addr,0x9000,2)
	if (err > 0) then
		str = string.format("δ�ҵ���װRC302") print(str)
		goto quit
	end

	--��ʼ������������ģ��ͨ�����ò���
	err = modbus_write_u16(COMx,2000,Addr485,0x9100,0x5A50)
	if (err > 0) then
		str = string.format("��ʼ������������ģ��ͨ�����ò���ʧ��") print(str)
		goto quit
	else
		str = string.format("��ʼ������������ģ��ͨ�����ò��� ok") print(str)
	end

	--��ʼ��ģ����У׼����
	err = modbus_write_u16(COMx,3000,Addr485,0x9100,0x5AA0)
	if (err > 0) then
		str = string.format("��ʼ��ģ����У׼����ʧ��") print(str)
		goto quit
	else
		str = string.format("��ʼ��ģ����У׼���� ok") print(str)
	end
	print("")

	--��Ӳ���ͺźͰ汾
	err,y1,y2 = modbus_read_u16(COMx,TimeOut,Addr485,0x9000,2)
	if (err > 0) then goto quit end
	str = string.format("�ͺ� = %04X �̼��汾 = V%X.%02X", y1, y2 >> 8, y2 & 0xFF) print(str)

	Model = y1

	if (Model == RC202_NORMAL) then
		str = string.format("��ʶ�����: RC602")  print(str)
	else
		if (Model == RD202_20MA) then
			str = string.format("��ʶ�����: RD202-20MA")  print(str)
		else
			if (Model == RD202_18B20) then
				str = string.format("��ʶ�����: RD202-18B20")  print(str)
			else
				if (Model == RD202_PULSE) then
					str = string.format("��ʶ�����: RD202-PULSE")  print(str)
				else
					if (Model == 0xC402) then
						str = string.format("��ʶ�����: RC402")  print(str)
					else
						str = string.format("δ֪����")  print(str)
						goto quit
					end
				end
			end
		end
	end

	if (Model ~= RC202_NORMAL and Model ~= RD202_20MA and Model ~= 0xC402) then
		str = string.format(" ���ͺ�����ģ����У׼")  print(str)
		goto quit
	end

	err = modbus_write_u16(COMx,TimeOut,Addr485,REG06_CALIB_KEY, 0x55AA)
	if (err > 0) then goto quit end
	--print("У׼�����Ѵ�")

	--print("--У׼��λ...")
		--�л��̵���
		err = modbus_write_do(COMx,TimeOut,RC604Addr,1,0,0,0,0)
		if (err > 0) then goto quit end

		delayms(1000)

		err,y1,y2 = modbus_read_float(COMx,TimeOut,Addr485,REG04_CH01_ADC_AVG, 2)
		if (err > 0) then goto quit end
		if (check_err(y1, 0.5, 1) == 1) then
			str = string.format(" CH1 ��λADC = %f --- Err", y1) print(str)
			err = 9
			goto quit
		end
		if (check_err(y2, 0.5, 1) == 1) then
			str = string.format(" CH2 ��λADC = %f --- Err", y1) print(str)
			err = 9
			goto quit
		end

		--дͨ��1��λADC,2������ADCһ��
		RegAddr = 	REG03_CC01_K0 + CC01_ZERO_ADC_0;
		err = modbus_write_float(COMx,TimeOut,Addr485, RegAddr, y1)
		if (err > 0) then goto quit end

		RegAddr = 	REG03_CC01_K0 + CC01_ZERO_ADC_0 + RANGE_REG_STEP;
		err = modbus_write_float(COMx,TimeOut,Addr485, RegAddr, y1)
		if (err > 0) then goto quit end

		--дͨ��2��λADC,2������ADCһ��
		RegAddr = 	REG03_CC01_K0 + CALIB_REG_STEP + CC01_ZERO_ADC_0;
		err = modbus_write_float(COMx,TimeOut,Addr485, RegAddr, y2)
		if (err > 0) then goto quit end

		RegAddr = 	REG03_CC01_K0 + CALIB_REG_STEP + CC01_ZERO_ADC_0 + RANGE_REG_STEP;
		err = modbus_write_float(COMx,TimeOut,Addr485, RegAddr, y2)
		if (err > 0) then goto quit end

		str = string.format(" CH1 CH2 ��λADC = %f, %f --OK", y1, y2) print(str)

	--��ȡNTC�¶�
	err, Temp = modbus_read_float(COMx,TimeOut,Addr485,REG04_CH01_OUT_F + 12,1)
	if (err > 0) then goto quit end
	str = string.format(" NTC�¶� = %f ��", Temp) print(str)

	--У׼2��ͨ���ĵ�ѹ��λ
	for i = 0, 1, 1 do

		str = string.format("\r\nУ׼ͨ��%d��ѹ����1�� %dmV, ��2�� %dmV", i + 1, CALIB_VOTL1, CALIB_VOTL2)
		print(str)

		--�л��̵��� ��ѹ
		err = modbus_write_do(COMx,TimeOut,RC604Addr,1, 1 - i, i,0,0)
		if (err > 0) then
			goto quit
		end

		--����DAC��ѹ - У׼��1
		err = modbus_write_u16(COMx,TimeOut,RC302Addr,REG03_AO_VOLT_01, CALIB_VOTL1)
		if (err > 0) then
			goto quit
		end

		delayms(1000)

		err,y1 = modbus_read_float(COMx,TimeOut, Addr485,REG04_CH01_ADC_RMS + 2 * i, 2)
		if (err > 0) then goto quit end
		--str = string.format("  RMS_ADC = %f", y1) print(str)

		--�����1��adc
		adc1 = y1
		if (check_err(adc1, VoltAdc1, DIFF) == 1) then
			str = string.format(" CH%d��ѹAdc1 = %f -- Err (ok = %f)", i + 1, y1, VoltAdc1) print(str)
			err = 9
			goto quit
		else
			str = string.format(" CH%d��ѹAdc1 = %f -- Ok", i + 1, y1) print(str)
		end

		--str = string.format("\r\nУ׼ͨ��%d��ѹ����2�� %dmV", i + 1, CALIB_VOTL2) print(str)
		--����DAC��ѹ - У׼��2
		err = modbus_write_u16(COMx,TimeOut,RC302Addr,REG03_AO_VOLT_01, CALIB_VOTL2)
		if (err > 0) then
			goto quit
		end

		delayms(1000)

		err,y1 = modbus_read_float(COMx,TimeOut,Addr485,REG04_CH01_ADC_RMS + 2 * i, 2)
		if (err > 0) then goto quit end
		--str = string.format("  RMS_ADC = %f", y1) print(str)

		--�����2��adc
		adc2 = y1
		if (check_err(adc2, VoltAdc2, DIFF) == 1) then
			str = string.format(" CH%d��ѹAdc2 = %f -- Err (ok = %f)", i + 1, y1, VoltAdc2) print(str)
			err = 9
			goto quit
		else
			str = string.format(" CH%d��ѹAdc2 = %f -- Ok", i + 1, y1) print(str)
		end

		--����K,B
		x1 = adc1
		y1 = CALIB_VOTL1 / 1000
		x2 = adc2
		y2 = CALIB_VOTL2 / 1000

		K = (y2 - y1) / (x2 - x1);
		B = (x2 * y1 - x1 * y2)/(x2 - x1);

		str = string.format(" X1 = %f, X2 = %f, X2 = %f, Y2 = %f",x1,y1,x2,y2) print(str)
		str = string.format(" K = %f, B = %f", K, B) print(str)

		RegAddr = REG03_CC01_K0 + i * CALIB_REG_STEP + RANGE_REG_STEP + CC01_K_0
		err = modbus_write_float(COMx,TimeOut,Addr485, RegAddr, K,B,Temp,0,0)
		if (err > 0) then goto quit end
		str = string.format(" ͨ��%d��ѹ��У׼����д��ɹ�", i + 1) print(str)
	end

	--У׼������λ
	print("")

	for i = 0, 1, 1 do
		--У׼ͨ��1����
		str = string.format("\r\nУ׼ͨ��%d��������1�� %fmA  ��2�� %fmA", i + 1, CALIB_CURR1 / 1000, CALIB_CURR2 / 1000)
		print(str)

		--�л��̵��� ��ѹ
		err = modbus_write_do(COMx,TimeOut,RC604Addr,1, 0, 0, 1 - i, i)
		if (err > 0) then
			goto quit
		end

		--����DAC��ѹ - У׼��1
		err = modbus_write_u16(COMx,TimeOut,RC302Addr,REG03_AO_CURR_01, CALIB_CURR1)
		if (err > 0) then
			goto quit
		end

		delayms(1000)

		err,y1 = modbus_read_float(COMx,TimeOut, Addr485,REG04_CH01_ADC_RMS + 2 * i, 2)
		if (err > 0) then goto quit end
		--str = string.format("  RMS_ADC = %f", y1) print(str)

		--�����1��adc
		adc1 = y1
		if (check_err(adc1, CurrAdc1, DIFF) == 1) then
			str = string.format("  CH%d����Adc1 = %f -- Err (ok = %f)", i + 1, y1, CurrAdc1) print(str)
			err = 9
			goto quit
		else
			str = string.format("  CH%d����Adc1 = %f -- Ok", i + 1, y1) print(str)
		end

		--str = string.format("\r\nУ׼ͨ��%d��������1�� %duA", i + 1, CALIB_CURR2)	print(str)
		--����DAC��ѹ - У׼��2
		err = modbus_write_u16(COMx,TimeOut,RC302Addr,REG03_AO_CURR_01, CALIB_CURR2)
		if (err > 0) then
			goto quit
		end

		delayms(1000)

		err,y1 = modbus_read_float(COMx,TimeOut,Addr485,REG04_CH01_ADC_RMS + 2 * i, 2)
		if (err > 0) then goto quit end
		--str = string.format("  RMS_ADC = %f", y1) print(str)

		--�����2��adc
		adc2 = y1
		if (check_err(adc2, CurrAdc2, DIFF) == 1) then
			str = string.format("  CH%d����Adc2 = %f -- Err (ok = %f)", i + 1, y1, CurrAdc2) print(str)
			err = 9
			goto quit
		else
			str = string.format("  CH%d����Adc2 = %f -- Ok", i + 1, y1) print(str)
		end

		--����K,B
		x1 = adc1
		y1 = CALIB_CURR1 / 1000
		x2 = adc2
		y2 = CALIB_CURR2 / 1000

		K = (y2 - y1) / (x2 - x1);
		B = (x2 * y1 - x1 * y2)/(x2 - x1);

		str = string.format(" X1 = %f, X2 = %f, X2 = %f, Y2 = %f",x1,y1,x2,y2) print(str)
		str = string.format(" K = %f, B = %f", K, B) print(str)

		RegAddr = REG03_CC01_K0 + i * CALIB_REG_STEP + 0 * RANGE_REG_STEP + CC01_K_0
		err = modbus_write_float(COMx,TimeOut,Addr485, RegAddr, K,B,Temp,0,0)
		if (err > 0) then goto quit end
		str = string.format("--ͨ��%d������У׼����д��ɹ�", i + 1) print(str)
	end

::quit::
	if (err == 0) then
		print("-------У׼�ɹ�------------------------------------")
		time2 = get_runtime()
		time1 = (time2 - time1) / 1000
		str = string.format("  ִ��ʱ�� = %0.1f ��", time1)	   print(str)
		return 0
	else
		if (err == 6) then
			print("RS485ͨ�Ŵ���")
		else
			str = string.format("������� = %d", err)
			print(str)
		end
		PrintError()
		return 1
	end
end
