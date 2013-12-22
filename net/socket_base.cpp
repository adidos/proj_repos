#include "socket_base.h"

/*
* 设置socket的阻塞模式
* @param bNoBlock[in] true 非阻塞,false阻塞
* @return 0 成功, -1 失败
*/
int SocketBase::set_noblock(bool bNoBlock)
{
	
	
	return 0;
}

/*
* 设置socket发送数据无延时
* @return 0 成功, -1 失败
*/
int SocketBase::set_nodelay()
{

	return 0;
}

/*
* 设置socket的心跳间隔时间
* @param interval_sec[in] 间隔时间,单位秒
* @return 0 成功, -1 失败
*/
int SocketBase::set_KeepAlive(int interval_sec)
{

}

/*
* 设置socket的发送缓冲区大小
* @param buf_size[in] 发送缓冲区大小,单位字节
* @return 0 成功, -1 失败
*/
int SocketBase::set_send_buf(int buf_size)
{


}

/*
* 设置socket的接受缓冲区大小
* @param buf_size[in] 发送缓冲区大小,单位字节
* @return 0 成功, -1 失败
*/
int SocketBase::set_recv_buf(int buf_size)
{


}