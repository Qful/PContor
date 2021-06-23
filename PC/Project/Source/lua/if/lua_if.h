﻿/*
*********************************************************************************************************
*
*    模块名称 : lua接口模块
*    文件名称 : lua_if.h
*    版    本 : V1.0
*    说    明 : 。
*    修改记录 :
*        版本号  日期       作者    说明
*        v1.0    2015-04-25 armfly  ST固件库版本 V2.1.0
*
*    Copyright (C), 2014-2015, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __LUA_IF_H
#define __LUA_IF_H

#include "lua.hpp"
#include "time.h"

#include "lua_if_uart.h"

#define LUA_PROG_LEN_MAX    (32 * 1024)

#define LUA_READ_LEN_MAX    (2 * 1024)

//#define LUA_ERR_PRINT(...)
#define LUA_ERR_PARAM_PRINT(format, ...) {char buf[128]; sprintf(buf, format, __VA_ARGS__); log_Print(buf);}

void lua_Test(void);
void lua_Init(void);
void lua_do(char *_str);
void lua_abort(void);

void lua_DeInit(void);
void lua_DownLoad(uint32_t _addr, uint8_t *_buf, uint32_t _len, uint32_t _total_len);
void lua_Run(void);
uint8_t lua_66H_Write(uint32_t _addr, uint8_t *_buf, uint32_t _len);
uint8_t lua_67H_Read(uint32_t _addr, uint8_t *_buf, uint32_t _len);
void lua_Idle(void);

extern lua_State *g_Lua;
extern  uint8_t s_lua_quit;

extern uint8_t s_lua_read_buf[LUA_READ_LEN_MAX];
extern uint8_t s_lua_read_len;

extern char s_lua_prog_buf[LUA_PROG_LEN_MAX];
extern uint32_t s_lua_prog_len;

void log_Print(char *_str);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
