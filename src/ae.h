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
 * anno: reids���������ǵ��͵�reactorģʽ��ʵ�֡�
 *       1. û��buffer�¼������Ҫ�ṩ��������ʵ�֡�
 *       2. ���û�м�ʱʱ�䣬���������û��AE_DONT_WAIT��������Զ�ȴ���
 */

#ifndef __AE_H__
#define __AE_H__

#define AE_OK 0
#define AE_ERR -1

#define AE_NONE 0
/*anno: �ɶ�*/
#define AE_READABLE 1
/*anno: ��д*/
#define AE_WRITABLE 2

#define AE_FILE_EVENTS 1
#define AE_TIME_EVENTS 2
#define AE_ALL_EVENTS (AE_FILE_EVENTS|AE_TIME_EVENTS)
/*anno: ���ö�·���ýӿڣ����û���¼����������̷���*/
#define AE_DONT_WAIT 4

/*anno: û�и����ʱ�¼�,����ɾ�����¼�*/
#define AE_NOMORE -1

/* Macros */
#define AE_NOTUSED(V) ((void) V)

struct aeEventLoop;

/* Types and data structures */
/*anno: �ļ�����������*/
typedef void aeFileProc(struct aeEventLoop *eventLoop, int fd, void *clientData, int mask);
/*anno: ����AE_NOMORE����ɾ���ü�ʱ�¼���������Ϊ�µĺ�������ӵ����¼�*/
typedef int aeTimeProc(struct aeEventLoop *eventLoop, long long id, void *clientData);
/*anno: �¼������ɾ����finalizerProc�ᱻ����*/
typedef void aeEventFinalizerProc(struct aeEventLoop *eventLoop, void *clientData);
/*anno: �¼�ѭ��֮ǰ������*/
typedef void aeBeforeSleepProc(struct aeEventLoop *eventLoop);

/* File event structure */
typedef struct aeFileEvent {
    int mask; /* one of AE_(READABLE|WRITABLE) */
    /*anno: ������*/
    aeFileProc *rfileProc;
    /*anno: д����*/
    aeFileProc *wfileProc;
    void *clientData;
} aeFileEvent;

/* Time event structure */
typedef struct aeTimeEvent {
    long long id; /* time event identifier. */
    /*anno: ����ʱ���Ҫ�������¼�*/
    long when_sec; /* seconds */
    long when_ms; /* milliseconds */
    aeTimeProc *timeProc;
    /*anno: ���¼������ɾ����finalizerProc�ᱻ����*/
    aeEventFinalizerProc *finalizerProc;
    void *clientData;
    /*anno: ��aeEventLoop����һ��β����*/
    struct aeTimeEvent *next;
} aeTimeEvent;

/* A fired event */
typedef struct aeFiredEvent {
    int fd;
    int mask;
} aeFiredEvent;

/* State of an event based program */
typedef struct aeEventLoop {
    int maxfd;   /* anno: �����ļ���������highest file descriptor currently registered */
    int setsize; /* anno: ���ܼ�ص�����ļ���������max number of file descriptors tracked */
    long long timeEventNextId; /*anno: ��ʱ�¼��ļ���������*/
    time_t lastTime;     /* anno: ÿ��ѭ�����µ�ʱ�䡣Used to detect system clock skew */
    aeFileEvent *events; /* anno: ע����¼����������磬�ļ��ȡ�Registered events */
    aeFiredEvent *fired; /* anno: �Ѿ��������ļ���������Fired events */
    aeTimeEvent *timeEventHead; /*anno: ��ʱ�¼���ͷ��*/
    int stop; /*anno: ֹͣ��ʶ*/
    void *apidata; /*anno: ��·���ýӿ�������ݡ� This is used for polling API specific data */
    aeBeforeSleepProc *beforesleep; /*anno: ÿ�ν����¼�ѭ��ʱ�ᱻ����*/
} aeEventLoop;

/* Prototypes */
aeEventLoop *aeCreateEventLoop(int setsize);
void aeDeleteEventLoop(aeEventLoop *eventLoop);
void aeStop(aeEventLoop *eventLoop);
int aeCreateFileEvent(aeEventLoop *eventLoop, int fd, int mask,
        aeFileProc *proc, void *clientData);
/*anno: mask Ϊɾ��������*/
void aeDeleteFileEvent(aeEventLoop *eventLoop, int fd, int mask);
int aeGetFileEvents(aeEventLoop *eventLoop, int fd);
/*anno: ����ֵΪ�¼���ʶID*/
long long aeCreateTimeEvent(aeEventLoop *eventLoop, long long milliseconds,
        aeTimeProc *proc, void *clientData,
        aeEventFinalizerProc *finalizerProc);
int aeDeleteTimeEvent(aeEventLoop *eventLoop, long long id);
int aeProcessEvents(aeEventLoop *eventLoop, int flags);
/*anno �ȴ��¼�������ʹ��pollϵͳ����ʵ�֡�*/
int aeWait(int fd, int mask, long long milliseconds);
void aeMain(aeEventLoop *eventLoop);
char *aeGetApiName(void);
void aeSetBeforeSleepProc(aeEventLoop *eventLoop, aeBeforeSleepProc *beforesleep);
int aeGetSetSize(aeEventLoop *eventLoop);
/*anno: ��̬�������ص�����*/
int aeResizeSetSize(aeEventLoop *eventLoop, int setsize);

#endif
