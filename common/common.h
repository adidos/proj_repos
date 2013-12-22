//============================================================
// common.h : common includes files, type definitions, etc
//                          
// Author: JeffLuo
// Created: 2006-10-21
//============================================================

#ifndef _COMMON_HEADER_H_20061021_16
#define  _COMMON_HEADER_H_20061021_16

/* C++ support */
#ifdef __cplusplus
extern "C" {
#endif

#ifndef _DEBUG
#define assert()
#endif

#ifdef WIN32
#pragma warning(disable:4275)
//#pragma warning(disable:4275)
#pragma warning(disable:4786)
#include <winsock2.h>
#include <windows.h>
#else



#endif

//公用部分
#include	<stdio.h>
#include	<stdlib.h>
#include	<stdarg.h>
#include	<string.h>
#include	<ctype.h>
#include	<fcntl.h>
#include	<signal.h>
#include	<time.h>
#include <assert.h>

#ifndef NUL
#define NUL '\0'
#endif
#ifndef NULL
#define NULL (void *)0
#endif


/* bitmask generation */
#define V_BIT(n) (1<<(n))
#define INIT_PROCESS_PID (1)
#define MAX_MLINE_LEN (4096)
#define PAGE_SIZE (4096)
#define PIPE_BUF_ATOMIC	(512)
#define MAX_BUFSIZE (65536)
#define MAX_LINE_LEN (1024)
#define ADDITIONAL_SAFE_LEN (1024)

#ifdef WIN32
//window定义部分
#define MSG_NOSIGNAL 0
#define socklen_t int
#define SHM_KEY_WORD  std::string
typedef unsigned __int64 __u64;
typedef unsigned __int64 _u64;
typedef unsigned int _u32;
typedef unsigned short _u16;
typedef unsigned long ulong;

#else



typedef unsigned long long __u64;
typedef unsigned long long _u64;
typedef unsigned int  _u32;
typedef unsigned short _u16;

typedef long long __int64;
typedef int __int32;

typedef unsigned char byte;

typedef bool boolean;
typedef	uint64_t	network64_t;
typedef	uint32_t	network32_t;
typedef	uint16_t	network16_t;
typedef	uint64_t	host64_t;
typedef	uint32_t	host32_t;
typedef	uint16_t	host16_t;

#define _snprintf snprintf

#endif


typedef wchar_t WCHAR;
#define LINUX_INT_LEN 4
#define LINUX_BOOLEAN_LEN 1
#define LINUX_BYTE_LEN 1
#define LINUX_SHORT_LEN 2
#define LINUX_FLOAT_LEN 4
#define LINUX_DOUBLE_LEN 8
#define LINUX_LLONG_LEN 8

/**  macros **/
#define MAX_CID_LEN		20
#define MAX_CID_HEX_LEN 40
#define MAX_PATH_LEN	256
#define MAX_THUNDER_VER_LEN  16
#define MAX_PEERID_LEN	20
#define MAX_FILE_BLOCKS	128


const static int MAX_CMD_LEN = 32 * 1024;
const static int MAX_CMDNAME_LEN = 1024;
const static int MAX_ZIP_LEN = 32 * 1024;
const static int MAX_PATH_SIZE = 256;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

//#define COMMON_FILE_MODE	S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH

#ifdef __cplusplus
}
#endif

#endif // #ifndef _COMMON_HEADER_H_20061021_16

