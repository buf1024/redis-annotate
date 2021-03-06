/* anet.c -- Basic TCP socket stuff made a bit less boring
 *
 * Copyright (c) 2006-2012, Salvatore Sanfilippo <antirez at gmail dot com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*
 * anno: TCP简单的封装，同时支持IPV4和IPV6。
 *       anetTcpNonBlockConnect如果加入多路复用中，
 *       需检测getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &errlen),
 *       因为出错或成功，fd都是可读写的。
 */
#ifndef ANET_H
#define ANET_H

#define ANET_OK 0
#define ANET_ERR -1
#define ANET_ERR_LEN 256

/* Flags used with certain functions. */
#define ANET_NONE 0
#define ANET_IP_ONLY (1<<0)

#if defined(__sun)
#define AF_LOCAL AF_UNIX
#endif

/*anno: 使用getaddrinfo获取地址结构，与IPV4/IPV6无关*/
int anetTcpConnect(char *err, char *addr, int port);
int anetTcpNonBlockConnect(char *err, char *addr, int port);
int anetUnixConnect(char *err, char *path);
int anetUnixNonBlockConnect(char *err, char *path);
/*anno: 一直读到规定的字节数，或者没数据。*/
int anetRead(int fd, char *buf, int count);
/*anno: resolve host信息。*/
int anetResolve(char *err, char *host, char *ipbuf, size_t ipbuf_len);
int anetResolveIP(char *err, char *host, char *ipbuf, size_t ipbuf_len);
/*anno: server信息，没有设置非阻塞信息。*/
int anetTcpServer(char *err, int port, char *bindaddr, int backlog);
int anetTcp6Server(char *err, int port, char *bindaddr, int backlog);
int anetUnixServer(char *err, char *path, mode_t perm, int backlog);
/*anno: 也是没有设置非阻塞信息。*/
int anetTcpAccept(char *err, int serversock, char *ip, size_t ip_len, int *port);
int anetUnixAccept(char *err, int serversock);
/*anno: 一直写到规定的字节数，或者没数据。*/
int anetWrite(int fd, char *buf, int count);
/*anno: 设置TCP/IP选项。*/
int anetNonBlock(char *err, int fd);
/*anno: tcp noday, 单报文比较小时，放送该报文需要的头部数据比较多，
 * 如果不设置该选项，系统会缓存到一定数量的报文再发送
 */
int anetEnableTcpNoDelay(char *err, int fd);
int anetDisableTcpNoDelay(char *err, int fd);
int anetTcpKeepAlive(char *err, int fd);
int anetKeepAlive(char *err, int fd, int interval);
/*anno: 获取对端的信息。*/
int anetPeerToString(int fd, char *ip, size_t ip_len, int *port);
/*anno: 获取fd的信息。*/
int anetSockName(int fd, char *ip, size_t ip_len, int *port);

#endif
