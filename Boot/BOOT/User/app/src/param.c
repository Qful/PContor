/*
*********************************************************************************************************
*
*	ģ������ : Ӧ�ó������ģ��
*	�ļ����� : param.c
*	��    �� : V1.0
*	˵    �� : ��ȡ�ͱ���Ӧ�ó���Ĳ���
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2013-01-01 armfly  ��ʽ����
*
*	Copyright (C), 2012-2013, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"
#include "param.h"
#include "modbus_reg_addr.h"

PARAM_T g_tParam;			/* �������� */
CALIB_T g_tCalib;			/* У׼���� */
VAR_T g_tVar;				/* ȫ�ֱ��� */

void LoadCalibParam(void);

/*
*********************************************************************************************************
*	�� �� ��: LoadParam
*	����˵��: ��eeprom��������g_tParam
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LoadParam(void)
{

	/* ��ȡEEPROM�еĲ��� */
	ee_ReadBytes((uint8_t *)&g_tParam, PARAM_ADDR, sizeof(PARAM_T));

	if (sizeof(PARAM_T) > PARAM_SIZE)
	{
		/* ������������ռ䲻�� */
		while(1);
	}
	
	if (g_tParam.ParamVer != PARAM_VER)
	{
		g_tParam.ParamVer = PARAM_VER;
		
		g_tParam.Addr485 = 1;
		
		/* ��̫����� */		
		g_tParam.LocalIPAddr[0] = 192;		/* ����IP��ַ */
		g_tParam.LocalIPAddr[1] = 168;
		g_tParam.LocalIPAddr[2] = 1;
		g_tParam.LocalIPAddr[3] = 211;
		g_tParam.LocalTcpPort = 30010;		/* ����TCP����˿ں� */
		
		g_tParam.NetMask[0] = 255;		/* �������� */
		g_tParam.NetMask[1] = 255;
		g_tParam.NetMask[2] = 255;
		g_tParam.NetMask[3] = 0;
		
		g_tParam.Gateway[0] = 192;		/* ���� */
		g_tParam.Gateway[1] = 168;
		g_tParam.Gateway[2] = 1;
		g_tParam.Gateway[3] = 1;

		g_tParam.RemoteIPAddr[0] = 192;		/* Զ��IP��ַ */
		g_tParam.RemoteIPAddr[1] = 168;
		g_tParam.RemoteIPAddr[2] = 1;
		g_tParam.RemoteIPAddr[3] = 213;
		g_tParam.RemoteTcpPort = 30000;		/* Զ��TCP����˿ں� */			
		
		g_tParam.WorkMode = 0;	
		
		g_tParam.NtcType = 0;	
		
		SaveParam();							/* ���²���д��Flash */				
	}
		
	bsp_GetCpuID(g_tVar.CPU_Sn);	/* ��ȡCPU ID */
	
	/* �Զ�������̫��MAC */
	g_tVar.MACaddr[0] = 0xC8;
	g_tVar.MACaddr[1] = 0xF4;
	g_tVar.MACaddr[2] = 0x8D;
	g_tVar.MACaddr[3] = g_tVar.CPU_Sn[2] >> 16;
	g_tVar.MACaddr[4] = g_tVar.CPU_Sn[2] >> 8;
	g_tVar.MACaddr[5] = g_tVar.CPU_Sn[2];
	
	LoadCalibParam();
}

/*
*********************************************************************************************************
*	�� �� ��: SaveParam
*	����˵��: ��ȫ�ֱ���g_tParam д�뵽eeprom
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void SaveParam(void)
{
	/* ��ȫ�ֵĲ����������浽EEPROM */
	ee_WriteBytes((uint8_t *)&g_tParam, PARAM_ADDR, sizeof(PARAM_T));
}

/*
*********************************************************************************************************
*	�� �� ��: LoadCalibParam
*	����˵��: ��ȫ�ֱ���g_tParam д�뵽CPU�ڲ�Flash
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LoadCalibParam(void)
{
	if (sizeof(g_tCalib) > PARAM_CALIB_SIZE)
	{
		/* У׼��������ռ䲻�� */
		while(1);
	}	
	
	/* ��ȡEEPROM�еĲ��� */
	ee_ReadBytes((uint8_t *)&g_tCalib, PARAM_CALIB_ADDR, sizeof(g_tCalib));	

	if (g_tCalib.CalibVer != CALIB_VER)
	{
		g_tCalib.CalibVer = CALIB_VER;
		
		InitCalibParam();	/* ��ʼ��У׼���� */
	}	
}

/*
*********************************************************************************************************
*	�� �� ��: WriteParamUint16
*	����˵��: д������16bit
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void WriteParamUint16(uint16_t _addr, uint16_t _value)
{
	ee_WriteBytes((uint8_t *)&_value, PARAM_CALIB_ADDR + _addr, 2);
}

/*
*********************************************************************************************************
*	�� �� ��: SaveCalibParam
*	����˵��: ��ȫ�ֱ���g_tCalibд�뵽eeprom
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void SaveCalibParam(void)
{
	/* ��ȫ�ֵĲ����������浽EEPROM */
	ee_WriteBytes((uint8_t *)&g_tCalib, PARAM_CALIB_ADDR, sizeof(g_tCalib));
}

/*
*********************************************************************************************************
*	�� �� ��: InitCalibParam
*	����˵��: ��ʼ��У׼����Ϊȱʡֵ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void InitCalibParam(void)
{
	uint8_t i;
	
	g_tCalib.CalibVer = CALIB_VER;
	for (i = 0; i < 8; i++)
	{
		g_tCalib.CH1[i].x1 = 0;
		g_tCalib.CH1[i].y1 = -14.3678 / (1 << i);	/* -14.3678V */
		g_tCalib.CH1[i].x2 = 65535;
		g_tCalib.CH1[i].y2 = 14.3678 / (1 << i);	/* +14.3678V */

		g_tCalib.CH2[i].x1 = 0;
		g_tCalib.CH2[i].y1 = -14.3678 / (1 << i);	/* -14.3678V */
		g_tCalib.CH2[i].x2 = 65535;
		g_tCalib.CH2[i].y2 = 14.3678 / (1 << i);	/* +14.3678V */
	}

	g_tCalib.LoadVolt.x1 = 0;
	g_tCalib.LoadVolt.y1 = 0;
	g_tCalib.LoadVolt.x2 = 65535;
	g_tCalib.LoadVolt.y2 = 32.167;			/* 32.167V */

	g_tCalib.LoadCurr[0].x1 = 0;			/* ���ص��� С���� */
	g_tCalib.LoadCurr[0].y1 = 0;
	g_tCalib.LoadCurr[0].x2 = 4380;
	g_tCalib.LoadCurr[0].y2 = 30;		/* 65535 = 123.934mA */
	g_tCalib.LoadCurr[0].x3 = 4380;
	g_tCalib.LoadCurr[0].y3 = 30;		/* 65535 = 123.934mA */
	g_tCalib.LoadCurr[0].x4 = 4380;
	g_tCalib.LoadCurr[0].y4 = 30;		/* 65535 = 123.934mA */	

	g_tCalib.LoadCurr[1].x1 = 0;			/* ���ص��� ������ */
	g_tCalib.LoadCurr[1].y1 = 0;
	g_tCalib.LoadCurr[1].x2 = 65535;
	g_tCalib.LoadCurr[1].y2 = 1250.0;		/* 1250mA */
	g_tCalib.LoadCurr[1].x3 = 65535;
	g_tCalib.LoadCurr[1].y3 = 1250.0;		/* 1250mA */
	g_tCalib.LoadCurr[1].x4 = 65535;
	g_tCalib.LoadCurr[1].y4 = 1250.0;		/* 1250mA */	
	
	g_tCalib.TVCCVolt.x1 = 0;
	g_tCalib.TVCCVolt.y1 = 0;
	g_tCalib.TVCCVolt.x2 = 65535;
	g_tCalib.TVCCVolt.y2 = 6.25;	/* ��߲���6.25V */

	g_tCalib.TVCCCurr.x1 = 0;
	g_tCalib.TVCCCurr.y1 = 0;
	g_tCalib.TVCCCurr.x2 = 16720;
	g_tCalib.TVCCCurr.y2 = 30;	/* 65535 = ��߲���6.25V */
	g_tCalib.TVCCCurr.x3 = 16720;
	g_tCalib.TVCCCurr.y3 = 30;	/* 65535 = ��߲���6.25V */
	g_tCalib.TVCCCurr.x4 = 16720;
	g_tCalib.TVCCCurr.y4 = 30;	/* 65535 = ��߲���6.25V */	

	//g_tCalib.RefResistor = 5.1;
	g_tCalib.NtcRes.x1 = 32768;
	g_tCalib.NtcRes.y1 = 5.1;
	g_tCalib.NtcRes.x2 = 0;
	g_tCalib.NtcRes.y2 = 0;

	g_tCalib.TVCCSet.x1 = 127;
	g_tCalib.TVCCSet.y1 = 1265;
	g_tCalib.TVCCSet.x2 = 34;
	g_tCalib.TVCCSet.y2 = 4687;	
		
	g_tCalib.Dac10V.x1 = 0;
	g_tCalib.Dac10V.y1 = -12200;		/* -12200mV */
	g_tCalib.Dac10V.x2 = 2047;
	g_tCalib.Dac10V.y2 = 0;				/* 0mV */	
	g_tCalib.Dac10V.x3 = 3071;
	g_tCalib.Dac10V.y3 = 6100;			/* 6100mV */
	g_tCalib.Dac10V.x4 = 4095;
	g_tCalib.Dac10V.y4 = 12200;		/* 12200mV */	

	g_tCalib.Dac20mA.x1 = 0;
	g_tCalib.Dac20mA.y1 = 0;
	g_tCalib.Dac20mA.x2 = 2047;
	g_tCalib.Dac20mA.y2 = 10500;		/* 10500uA */
	g_tCalib.Dac20mA.x3 = 3071;
	g_tCalib.Dac20mA.y3 = 15821;		/* 15821uA */	
	g_tCalib.Dac20mA.x4 = 4095;
	g_tCalib.Dac20mA.y4 = 21142;		/* 21142uA */
	
	g_tCalib.USBVolt.x1 = 0;
	g_tCalib.USBVolt.y1 = 0;
	g_tCalib.USBVolt.x2 = 65535;
	g_tCalib.USBVolt.y2 = 6.25;	/* ��߲���6.25V */
	
	g_tCalib.ExtPowerVolt.x1 = 0;
	g_tCalib.ExtPowerVolt.y1 = 0;
	g_tCalib.ExtPowerVolt.x2 = 65535;
	g_tCalib.ExtPowerVolt.y2 = 6.25;	/* ��߲���6.25V */		
	
	SaveCalibParam();
}
		
/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
