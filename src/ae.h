/* A simple event-driven programming library. Originally I wrote this code
 * for the Jim's event-loop (Jim is a Tcl interpreter) but later translated
 * it in form of a library for easy reuse.
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
 * anno: reids的网络框架是典型的reactor模式的实现。
 *       1. 没有buffer事件，如果要提供则需另外实现。
 *       2. 如果没有计时时间，如果传参数没有AE_DONT_WAIT，可能永远等待。
 */

#ifndef __AE_H__
#define __AE_H__

#define AE_OK 0
#define AE_ERR -1

#define AE_NONE 0
/*anno: 可读*/
#define AE_READABLE 1
/*anno: 可写*/
#define AE_WRITABLE 2

#define AE_FILE_EVENTS 1
#define AE_TIME_EVENTS 2
#define AE_ALL_EVENTS (AE_FILE_EVENTS|AE_TIME_EVENTS)
/*anno: 调用多路复用接口，如果没有事件就绪，立刻返回*/
#define AE_DONT_WAIT 4

/*anno: 没有更多计时事件,用完删除该事件*/
#define AE_NOMORE -1

/* Macros */
#define AE_NOTUSED(V) ((void) V)

struct aeEventLoop;

/* Types and data structures */
/*anno: 文件就绪被调用*/
typedef void aeFileProc(struct aeEventLoop *eventLoop, int fd, void *clientData, int mask);
/*anno: 返回AE_NOMORE，则删除该计时事件。否则，作为新的毫秒数添加到新事件*/
typedef int aeTimeProc(struct aeEventLoop *eventLoop, long long id, void *clientData);
/*anno: 事件如果被删除，finalizerProc会被调用*/
typedef void aeEventFinalizerProc(struct aeEventLoop *eventLoop, void *clientData);
/*anno: 事件循环之前被调用*/
typedef void aeBeforeSleepProc(struct aeEventLoop *eventLoop);

/* File event structure */
typedef struct aeFileEvent {
    int mask; /* one of AE_(READABLE|WRITABLE) */
    /*anno: 读函数*/
    aeFileProc *rfileProc;
    /*anno: 写函数*/
    aeFileProc *wfileProc;
    void *clientData;
} aeFileEvent;

/* Time event structure */
typedef struct aeTimeEvent {
    long long id; /* time event identifier. */
    /*anno: 多少时间后要触发该事件*/
    long when_sec; /* seconds */
    long when_ms; /* milliseconds */
    aeTimeProc *timeProc;
    /*anno: 该事件如果被删除，finalizerProc会被调用*/
    aeEventFinalizerProc *finalizerProc;
    void *clientData;
    /*anno: 与aeEventLoop构成一个尾队列*/
    struct aeTimeEvent *next;
} aeTimeEvent;

/* A fired event */
typedef struct aeFiredEvent {
    int fd;
    int mask;
} aeFiredEvent;

/* State of an event based program */
typedef struct aeEventLoop {
    int maxfd;   /* anno: 最大的文件描述符。highest file descriptor currently registered */
    int setsize; /* anno: 所能监控的最大文件描述符。max number of file descriptors tracked */
    long long timeEventNextId; /*anno: 计时事件的计数产生器*/
    time_t lastTime;     /* anno: 每次循环更新的时间。Used to detect system clock skew */
    aeFileEvent *events; /* anno: 注册的事件，包括网络，文件等。Registered events */
    aeFiredEvent *fired; /* anno: 已经就绪的文件描述符。Fired events */
    aeTimeEvent *timeEventHead; /*anno: 计时事件表头。*/
    int stop; /*anno: 停止标识*/
    void *apidata; /*anno: 多路复用接口相关数据。 This is used for polling API specific data */
    aeBeforeSleepProc *beforesleep; /*anno: 每次进入事件循环时会被调用*/
} aeEventLoop;

/* Prototypes */
aeEventLoop *aeCreateEventLoop(int setsize);
void aeDeleteEventLoop(aeEventLoop *eventLoop);
void aeStop(aeEventLoop *eventLoop);
int aeCreateFileEvent(aeEventLoop *eventLoop, int fd, int mask,
        aeFileProc *proc, void *clientData);
/*anno: mask 为删除的掩码*/
void aeDeleteFileEvent(aeEventLoop *eventLoop, int fd, int mask);
int aeGetFileEvents(aeEventLoop *eventLoop, int fd);
/*anno: 返回值为事件标识ID*/
long long aeCreateTimeEvent(aeEventLoop *eventLoop, long long milliseconds,
        aeTimeProc *proc, void *clientData,
        aeEventFinalizerProc *finalizerProc);
int aeDeleteTimeEvent(aeEventLoop *eventLoop, long long id);
int aeProcessEvents(aeEventLoop *eventLoop, int flags);
/*anno 等待事件发生，使用poll系统调用实现。*/
int aeWait(int fd, int mask, long long milliseconds);
void aeMain(aeEventLoop *eventLoop);
char *aeGetApiName(void);
void aeSetBeforeSleepProc(aeEventLoop *eventLoop, aeBeforeSleepProc *beforesleep);
int aeGetSetSize(aeEventLoop *eventLoop);
/*anno: 动态变更最大监控的数量*/
int aeResizeSetSize(aeEventLoop *eventLoop, int setsize);

#endif
