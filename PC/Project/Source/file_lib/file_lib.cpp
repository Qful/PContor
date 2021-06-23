/*
*********************************************************************************************************
*
*	ģ������ : HEX��ʽתΪbin��ʽ
*	�ļ����� : hex2bin.c
*	��    �� : V1.0
*	˵    �� : ����������ԭ��
*   �޸ļ�¼ :
* 			V1.0  2020-06-07  �װ�
*
*********************************************************************************************************
*/

#include "bsp.h"
#include "md5.h"
/*
*********************************************************************************************************
*	�� �� ��: ReadFileToMem
*	����˵��: ���ļ���������
*	��    ��:
*	�� �� ֵ: 0 ����, 1 �ɹ�
*********************************************************************************************************
*/
int ReadFileToMem(UnicodeString _FileName, char *_FileBuf, uint32_t _BufSize)
{
	int iFileHandle;
	int iBytesRead;
	int iFileLength = 0;

	iFileHandle = FileOpen(_FileName, fmOpenRead);
	if (iFileHandle > 0)
	{
		iFileLength = FileSeek(iFileHandle, 0, 2);

		if (iFileLength < _BufSize)
		{
			FileSeek(iFileHandle, 0, 0);
			FileRead(iFileHandle, _FileBuf, iFileLength);
		}
		else
		{
			;
		}
		FileClose(iFileHandle);
	}
	return iFileLength;
}

/*
*********************************************************************************************************
*	�� �� ��: SaveMemToFile
*	����˵��: ���浽�ļ�
*	��    ��:
*	�� �� ֵ: 0 ����, > 1��ʾbin����
*********************************************************************************************************
*/
int SaveMemToFile(UnicodeString _FileName, char *_FileBuf, uint32_t _BufSize)
{
	int iFileHandle;
	int iBytesRead;
	int iFileLength = 0;

	iFileHandle = FileCreate(_FileName);
	if (iFileHandle > 0)
	{
		iFileLength = FileWrite(iFileHandle, _FileBuf, _BufSize);
		FileClose(iFileHandle);
	}
	return iFileLength;
}

/*
*********************************************************************************************************
*    �� �� ��: GetFileMD5
*    ����˵��: �õ��ļ���MD5��
*    ��    ��: _Path : �ļ���
*              _OutMD5 : ������ 16�ֽ�
*             _FileSize : �ļ���С
*    �� �� ֵ: �ļ���С. -1��ʾ���ļ�ʧ��
*********************************************************************************************************
*/
uint32_t GetFileMD5(UnicodeString _FileName, char *_OutMD5)
{
    uint32_t bytes;
    uint32_t offset = 0;
    MD5_CTX md5;
	int iFileHandle;
	int iBytesRead;
	int iFileLength = 0;
	char FsReadBuf[32*1024];

    memset(_OutMD5, 0, 16);

	MD5Init(&md5);

	iFileHandle = FileOpen(_FileName, fmOpenRead);
	if (iFileHandle > 0)
	{
		iFileLength = FileSeek(iFileHandle, 0, 2);

		FileSeek(iFileHandle, 0, 0);

		while (1)
		{
			bytes = FileRead(iFileHandle, FsReadBuf, sizeof(FsReadBuf));
			if (bytes > 0)
			{
				MD5Update(&md5, (uint8_t *)FsReadBuf, bytes);
			}
			else
			{
				break;
			}
		}
		FileClose(iFileHandle);
	}
	MD5Final(&md5, (uint8_t *)_OutMD5);
	return iFileLength;
}


/*
*********************************************************************************************************
*    �� �� ��: GetFileSums
*    ����˵��: �õ��ļ����ۼ�У���
*    ��    ��: _Path : �ļ���
*              _OutMD5 : ������ 16�ֽ�
*             _FileSize : �ļ���С
*    �� �� ֵ: У��� uint32_t
*********************************************************************************************************
*/
uint32_t GetFileSums(UnicodeString _FileName)
{
    uint32_t bytes;
	uint32_t offset = 0;

	int iFileHandle;
	int iBytesRead;
	int iFileLength = 0;
	char FsReadBuf[32*1024];
	uint32_t sum = 0;
	uint32_t i;

	iFileHandle = FileOpen(_FileName, fmOpenRead);
	if (iFileHandle > 0)
	{
		iFileLength = FileSeek(iFileHandle, 0, 2);

		FileSeek(iFileHandle, 0, 0);

		while (1)
		{
			bytes = FileRead(iFileHandle, FsReadBuf, sizeof(FsReadBuf));
			if (bytes > 0)
			{
				for (i = 0; i < bytes; i++)
				{
					sum += FsReadBuf[i];
				}
			}
			else
			{
				break;
			}
		}
		FileClose(iFileHandle);
	}

	return sum;
}

/*
*********************************************************************************************************
*    �� �� ��: AppExePath
*    ����˵��: �õ�Ӧ�ó���·���� windows
*    ��    ��:  ��
*    �� �� ֵ: exe�ļ�·��
*********************************************************************************************************
*/
String AppExePath(void)
{
	AnsiString str;
	int len;
	char *p;
	char *p0;
	char buf[256];

	str = ExtractFilePath(Application->ExeName);

	strcpy(buf, str.c_str());
	p = strstr(buf, "\\..\\");
	if (p > 0)
	{
		p0 = p;
		for (int i = 0; i < 100; i++)
		{
			p0--;
			if (*p0 == '\\')
			{
				strcpy(p0, p+3);
				break;
			}
		}
	}

	len = strlen(buf);
	if (buf[len - 1] == '\\')
	{
		buf[len - 1] = 0;
	}

	str = (AnsiString)buf;

	return str;
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
