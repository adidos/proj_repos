#include "socket_base.h"

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

/*
* 设置socket的阻塞模式
* @param bNoBlock[in] true 非阻塞,false阻塞
* @return 0 成功, -1 失败
*/
int SocketBase::set_noblock(bool bNoBlock)
{
    int val = fcntl(fd_, F_GETFL, 0);
    
    if(bNoBlock)
    {
        fcntl(fd_, F_SETFL, val | O_NONBLOCK);
    }
    else
        fcntl(fd_, F_SETFL, val & ~O_NONBLOCK);
	
	return 0;
}

/*
* 设置socket发送数据无延时
* @return 0 成功, -1 失败
*/
int SocketBase::set_nodelay()
{
    int val = 1;
    int rst = setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, (void*)&val, sizeof(val));
    if(rst != 0)
    {
        perror("setsockopt:nodelay");
        return -1;
    }

	return 0;
}

/*
* 启用socket的keepalive选项
* 
* @return 0 成功, -1 失败
*/
int SocketBase::set_KeepAlive()
{
    int val = 1;
    int rst = setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, (void*)&val, sizeof(val));
    if(0 != rst)
    {
        perror("setsockopt:keepalive");
        return -1;
    }

    return 0;
}

/*
* 设置socket的发送缓冲区大小
* @param buf_size[in] 发送缓冲区大小,单位字节
* @return 0 成功, -1 失败
*/
int SocketBase::set_send_buf(int buf_size)
{
    if(buf_size < 0)
        return -1;

    int rst = setsockopt(fd_, SOL_SOCKET, SO_SNDBUF, (void*)&buf_size, sizeof(buf_size));
 
    return rst;
}

/*
* 设置socket的接受缓冲区大小
* @param buf_size[in] 发送缓冲区大小,单位字节
* @return 0 成功, -1 失败
*/
int SocketBase::set_recv_buf(int buf_size)
{
    if(buf_size < 0)
        return -1;

    int rst = setsockopt(fd_, SOL_SOCKET, SO_RCVBUF, (void*)&buf_size, sizeof(buf_size));
 
    return rst;
}

/**
* @brief: set_send_timeout 
*
* @Param: time_out
*
* @Returns: 
*/
int SocketBase::set_send_timeout(int time_out)
{
    if(time_out < 0)
        return -1;

    struct timeval time;
    time.tv_sec = time_out;
    time.tv_usec = 0;
    int rst = setsockopt(fd_, SOL_SOCKET, SO_SNDTIMEO, (void*)&time, sizeof(time));

    return rst;
}


/**
* @brief: set_recv_timeout 
*
* @Param: time_out
*
* @Returns: 0成功 -1 失败
*/
int SocketBase::set_recv_timeout(int time_out)
{
    if(time_out < 0)
        return -1;

    struct timeval time;
    time.tv_sec = time_out;
    time.tv_usec = 0;
    int rst = setsockopt(fd_, SOL_SOCKET, SO_RCVTIMEO, (void*)&time, sizeof(time));
    
    return rst;
}
