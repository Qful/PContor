--F01=�Զ�����,AutoTestUart()
--F02=��������,TestUart()

--�ɹ���һ����ʧ�ܽ�����
function TestErr(void)
	print("����ʧ��")
	beep()
	delayms(100)
	beep()
	delayms(100)
	beep()	
end

function TestOk(void)
	print("����ͨ��")
	beep()
end

--���Դ���Ӳ������
function TestUart(void)
	local COM_TTL = 7
	local COM_485 = 1
	local Parity = 0
	local DataBits = 8
	local StopBits = 1
	local tx_str = "H7-TOOL"
	local rx_str
	local rx_len
	local str
	local curr
	local volt
	local ret = "OK"
	
	print("��ʼ����")
	
	write_reg16(0x01FF,2)
	
	gpio_cfg(0, 5)	-- ����D0ΪUART����
	gpio_cfg(1, 5)	-- ����D1ΪUART����
	
	uart_cfg(COM_TTL, 115200, Parity, DataBits, StopBits)
	uart_cfg(COM_485, 115200, Parity, DataBits, StopBits)

	volt = read_analog(4)
	curr = read_analog(5)
	str = string.format("��ѹ %0.1fV ���� %0.1fmA", volt, curr)
	print(str)
	
	--print("RS485 -> TTL")
	uart_send(COM_485, tx_str)
	rx_len, rx_str = uart_recive(COM_TTL, 7, 100)
	if (rx_str == tx_str) then
		print("RS485 -> TTL : OK")
	else
		print("RS485 -> TTL : ERROR")
		ret = "ERROR"
		goto quit_err
	end

	--print("����   TTL -> RS485")
	uart_send(COM_TTL, tx_str)
	rx_len, rx_str = uart_recive(COM_485, 7, 100)
	if (rx_str == tx_str) then
		print("TTL -> RS485 : OK")
	else
		print("TTL -> RS485 : ERROR")
		ret = "ERROR"
		goto quit_err
	end

::quit_err::
	if (ret == "OK") then
		TestOk()
	else
		TestErr()
	end
	
	return ret
end

--������Զ�����
function AutoTestUart(void)

	local now_curr = 0
	local count = 0
	local inserted = 0
	local key
	
	print("���� RS485 -> TTL")
	
	write_reg16(0x01FF, 2)	--ADC�л������ٶ�ͨ��ɨ��ģʽ
	while(1)
	do
		now_curr = read_analog(5)
		
		if (inserted == 0) then
			if (now_curr > 20) then
				delayms(50)
				TestUart()
				inserted = 1
				count = count + 1
				print("�Ѳ���", count)
			end
		else
			if (now_curr < 5) then
				inserted = 0
			end
		end
		
		delayms(100)
	    
	    --��������˳�ѭ��
	    key = get_key()
	    if (key > 0) then
	    	return
	    end
	    
	end
end

--����GPIO����,D0��D1�̽Ӻ���Կ�ͨ��
function TestGPIO(void)
	local y
	local err = 0
	
	print("")
	print("����GPIO")
	
	gpio_cfg(0, 1)
	gpio_cfg(1, 0)

	-- D0 = 0
	gpio_write(0, 0) 
	delayms(100)	
	y = gpio_read(1) 	
	if (y ~= 0) then
		err = err + 1
	end

	-- D0 = 1
	gpio_write(0, 1) 
	delayms(100)	
	y = gpio_read(1) 	
	if (y ~= 1) then
		err = err + 1
	end	
				
	if (err == 0) then
		TestOk()
	else
		TestErr()
	end
end
