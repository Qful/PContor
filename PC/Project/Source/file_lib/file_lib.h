/*
*********************************************************************************************************
*
*	ģ������ : �ļ�������
*	�ļ����� : file_lib.h
*	˵    �� : ͷ�ļ�
*
*********************************************************************************************************
*/

#ifndef __FILE_LIB_H
#define __FILE_LIB_H

int ReadFileToMem(UnicodeString _FileName, char *_FileBuf, uint32_t _BufSize);
int SaveMemToFile(UnicodeString _FileName, char *_FileBuf, uint32_t _BufSize);
uint32_t GetFileMD5(UnicodeString _FileName, char *_OutMD5);
uint32_t GetFileSums(UnicodeString _FileName);

String AppExePath(void);

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
