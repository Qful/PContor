--�汾 V1.1  2020-06-28
--F01=���β���,TestOnce()
--F02=�Զ�����,AutoTest()

COM_485 = 1
RS485_ADDR = 1
RS485_TimeOut = 300
MODEL = ""
SOFT_VER = ""
g_PassedCount = 0

beep()

function TestErr(void)
	print("����ʧ��")
	beep()
	delayms(100)
	beep()
	delayms(100)
	beep()
end

function TestOk(void)
	g_PassedCount = g_PassedCount + 1
	print("����ͨ��  ", g_PassedCount)
	beep()
end

function InitRS485(void)
	local Parity = 0
	local DataBits = 8
	local StopBits = 1

	uart_cfg(COM_485, 9600, Parity, DataBits, StopBits)
end

--����ͺ�
function CheckModel(void)
	local err = 0
	local rx_len
	local modle = ""

	uart_send(COM_485, "$001,02&")
	rx_len, modle = uart_recive(COM_485, 20, 100)

	if (modle == "LED-485-034" or modle == "LED-485-043" or modle == "LED-485-054" or modle == "LED-485-083"
		 or modle == "LED-485-184"  or modle == "LED-485-504" or modle == "LED-485-2342" or modle == "LED-485-1842") then
		return modle
	else
		return ""
	end
end

--�������汾
function CheckSoftVer(void)
	local err = 0
	local rx_len
	local version = ""

	uart_send(COM_485, "$001,03&")
	rx_len, version = uart_recive(COM_485, 20, 50)

	return version
end

--�Զ�����
function AutoTest(void)

	local inserted = 0
	local key
	local err
	local flag = 0

	print("�����Զ�����ģʽ")

	InitRS485()

	while(1)
	do
		if (inserted == 0) then
			MODEL = CheckModel()
			if (MODEL ~= "") then
				inserted = 1
				TestOnce()

				flag = 0
			end
		else
			if (flag < 10) then
				err = modbus_write_u16(COM_485, RS485_TimeOut, RS485_ADDR, 0x0070, 0x382E, 0x382E,0x382E, 0x382E, 0, 0)
			else
				if (flag < 20) then
					CheckSoftVer()
				else
					flag = 0
				end
			end

			flag = flag + 1
			if (err > 0) then
				inserted = 0
			end
		end

		delayms(50)

	    --��������˳�ѭ��
	    key = get_key()
	    if (key > 0) then
	    	print("�û�����lua����")
	    	return
	    end
	end
end

function TestOnce(void)
	InitRS485()
	MODEL = CheckModel()

	delayms(250)
	if (MODEL == "") then
		print("RS485ͨ�Ŵ���")
		TestErr()
	end

	if (MODEL == "LED-485-184" or MODEL == "LED-485-504") then
		TestLED184()
	end

	if (MODEL == "LED-485-034" or MODEL == "LED-485-054") then
		TestLED034()
	end

	if (MODEL == "LED-485-043" or MODEL == "LED-485-083") then
		TestLED083()
	end
end

--����LED-485-034  LED-485-054
function TestLED034(void)
	local err = 0
	local str
	local ret = "OK"
	local i
	local value
	local count

	SOFT_VER = CheckSoftVer()

	print("��ʼ����")
	print(MODEL, SOFT_VER)

	delayms(300)

	--ͨ��RS485�����
	err = modbus_write_u16(COM_485, RS485_TimeOut, RS485_ADDR, 0x0088, 1234)
	if (err > 0) then
		print("RS485ͨ�Ŵ���")
		goto quit_err
	end

	--����ȫ��LED
	uart_send(COM_485, "$001,8.8.8.8.#")

::quit_err::
	if (err == 0) then
		TestOk()
	else
		TestErr()
	end

	return ret
end

--����LED-485-034  LED-485-054
function TestLED083(void)
	local err = 0
	local str
	local ret = "OK"
	local i
	local value
	local count

	SOFT_VER = CheckSoftVer()

	print("��ʼ����")
	print(MODEL, SOFT_VER)

	delayms(300)

	--ͨ��RS485�����
	err = modbus_write_u16(COM_485, RS485_TimeOut, RS485_ADDR, 0x0088, 123)
	if (err > 0) then
		print("RS485ͨ�Ŵ���")
		goto quit_err
	end

	--����ȫ��LED
	uart_send(COM_485, "$001,8.8.8.#")

::quit_err::
	if (err == 0) then
		TestOk()
	else
		TestErr()
	end

	return ret
end

--����LED-485-034  LED-485-054
function TestLED043(void)
	local err = 0
	local str
	local ret = "OK"
	local i
	local value
	local count

	SOFT_VER = CheckSoftVer()

	print("��ʼ����")
	print(MODEL, SOFT_VER)

	--ͨ��RS485�����
	err = modbus_write_u16(COM_485, RS485_TimeOut, RS485_ADDR, 0x0088, 123)
	if (err > 0) then
		print("RS485ͨ�Ŵ���")
		goto quit_err
	end

	--����ȫ��LED
	uart_send(COM_485, "$001,8.8.8.#")

::quit_err::
	if (err == 0) then
		TestOk()
	else
		TestErr()
	end

	return ret
end

--����LED-485-184
function TestLED184(void)
	local err = 0
	local str
	local ret = "OK"
	local i
	local value
	local count

	SOFT_VER = CheckSoftVer()

	print("��ʼ����")
	print(MODEL, SOFT_VER)

	set_tvcc(5)
	gpio_cfg(2, 1)
	gpio_cfg(3, 1)
	gpio_cfg(4, 1)

	--CLR��
	gpio_write(4, 1)
	delayms(100)
	gpio_write(4, 0)

	--INC��ִ��3��
	for i = 1, 3, 1 do
		gpio_write(2, 1)
		delayms(70)
		gpio_write(2, 0)
		delayms(70)
	end

	--ͨ��RS485�����
	err, count = modbus_read_u16(COM_485, RS485_TimeOut, RS485_ADDR, 0x001D, 1)
	if (err > 0) then
		print("RS485ͨ�Ŵ���1")
		goto quit_err
	else
		if (count ~= 3) then
			print("INC������")
			goto quit_err
		end
	end

	--DEC��ִ��2��
	for i = 1, 2, 1 do
		gpio_write(3, 1)
		delayms(70)
		gpio_write(3, 0)
		delayms(70)
	end

	--ͨ��RS485�����
	err, count = modbus_read_u16(COM_485, RS485_TimeOut, RS485_ADDR, 0x001D, 1)
	if (err > 0) then
		print("RS485ͨ�Ŵ���")
		goto quit_err
	else
		if (count ~= 1) then
			print("DEC������2")
			goto quit_err
		end
	end

	--CLR��
	gpio_write(4, 1)
	delayms(100)
	gpio_write(4, 0)

	--ͨ��RS485�����
	err, count = modbus_read_u16(COM_485, RS485_TimeOut, RS485_ADDR, 0x001D, 1)
	if (err > 0) then
		print("RS485ͨ�Ŵ���3")
		goto quit_err
	else
		if (count ~= 0) then
			print("CLR������")
			goto quit_err
		end
	end

	--����ȫ��LED
	uart_send(COM_485, "$001,8.8.8.8.#")

::quit_err::
	if (err == 0) then
		TestOk()
	else
		TestErr()
	end

	set_tvcc(3.3)

	return ret
end

