beep()
print("Load usb_i2c_host.lua ok")

function bin2hex(bin)
	local s
	local i

	s = ""
	for i=1,#bin,1 do
		s = s..string.format("%02X ", string.byte(bin,i))
	end
	return s
end

----------I2C����---------------

function I2C_ScanDevice(freq)
	local i
	local re
	local str
	
	beep()
	
	I2C_InitSlave(freq)
	
	print("ɨ��I2C�����豸...")
	
	for i = 0,255,2 do
		re = i2c_bus("check", i)
		if (re == 0) then
			str = string.format("0x%02X : ACK OK", i)			
			print(str)
		end
		delayms(5)
	end
	
	print("ɨ�����")
end

--��ʼ��I2C����(��վ)��Ƶ��
function I2C_InitSlave(freq)
	local str
	
	i2c_bus("init",freq)
	
	str = string.format("I2C����Ƶ��������Ϊ %dKHz", freq / 1000)
	print(str)
end

--д����
function I2C_WriteData(addr, data)
	local str
	local ack
	local i
	local len
	local err
	
	err = string.format("I2C send 0x%02X, %s", addr, bin2hex(data))
	
	i2c_bus("start")	
	ack = i2c_bus("send", addr)
	if (ack ~= 0) then
		err = err.." error"
		goto quit
	end
	
	ack = i2c_bus("send", data)
	if (ack ~= 0) then
		err = err.." error"
		goto quit
	end	
	
	err = err.." ok"
::quit::
	i2c_bus("stop")
	print(err)
end

--������,оƬ��ַ�� ��������ʾ���
function I2C_ReadData(addr, num, dispwidth)
	local str
	local ack
	local i
	local err
	local rd
	
	err = string.format("I2C read 0x%02X, %d", addr + 1, num)
	
	i2c_bus("start")	
	ack = i2c_bus("send", addr | 1)
	if (ack ~= 0) then
		err = err.." error"
		print(err)
		goto quit
	end

	rd = i2c_bus("recive", num)
	i2c_bus("stop")

	err = err.." ok"
	print(err)

	print_hex(rd, dispwidth, 0)
	
::quit::
	i2c_bus("stop")
end

----------SPI����---------------
-- D5 : SPI_CLK
-- D2 : SPI_MOSI
-- D3 : SPI_MISO
-- D0 : CS0
-- D1 : CS1
--��ʼ������(��վ)
function SPI_Init(freqid, phase, polarity)
	local str
	local Baud = {"390.625K", "781.25K", "1.5625M", "3.125M", "6.25M", "12.5M", "25M", "50M"}

	gpio_cfg(0, 1)	--D0�����CS0
	gpio_cfg(1, 1)	--D1�����CS1
	
	if (freqid < 8) then
		spi_bus("init", freqid, phase, polarity)
		print("SPI����Ƶ��������Ϊ : "..Baud[freqid+1]) delayms(5)

		if (phase == 0) then
			if (polarity == 0) then
				print("ʱ����λ: ������ǰ1��ʱ���زɼ�(������)")
			else
				print("ʱ����λ: ������ǰ1��ʱ���زɼ�(������)")
			end
		else
			if (polarity == 0) then
				print("ʱ����λ: �����ں�1��ʱ���زɼ�(������)")
			else
				print("ʱ����λ: �����ں�1��ʱ���زɼ�(������)")
			end
		end
		 delayms(5)
		if (polarity == 0) then
			print("ʱ�Ӽ���: ����ʱΪ0")
		else
			print("ʱ�Ӽ���: ����ʱΪ1")
		end

	else
		print("SPI����Ƶ�ʲ�֧��")
	end	
end

--����Ƭѡ�� ch: ͨ��, level:��ƽ
function SPI_SetCS(ch, level)
	if (ch == 0) then
		gpio_write(0, level)
	else if (ch == 1) then
		gpio_write(1, level)
		else
		end
	end	
end

--д����ch:ͨ��(-1��ʾ������), data:����������
function SPI_WriteData(ch, data)
	local str
	local err
	
	err = string.format("SPI send %s", bin2hex(data))
	
	SPI_SetCS(ch, 0)
	spi_bus("send", data)     --���Ͷ������ַ���
	SPI_SetCS(ch, 1)
	
	err = err.." ok"
	print(err)
end

--������, chͨ���� ��������ʾ���
function SPI_ReadData(ch, num, dispwidth)
	local str
	local err
	local rd
	
	err = string.format("SPI read  %d",  num)

	SPI_SetCS(ch, 0)
	rd = spi_bus("recive", num)
	SPI_SetCS(ch, 1)
	
	err = err.." ok"
	print(err)

	print_hex(rd, dispwidth, 0)
end


--д����ch:ͨ��(-1��ʾ������), data:����������
function SPI_WriteReadData(ch, data, num)
	local str
	local err
	local rd
	
	err = string.format("SPI send and recive %s, %d", bin2hex(data), num)
	
	SPI_SetCS(ch, 0)
	rd = spi_bus("sendrecive", data, num)
	SPI_SetCS(ch, 1)
	
	err = err.." ok"
	print(err)

	print_hex(rd, dispwidth, 0)
end


