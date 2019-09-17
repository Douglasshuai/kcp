//=====================================================================
//
// KCP - A Better ARQ Protocol Implementation
// skywind3000 (at) gmail.com, 2010-2011
//  
// Features:
// + Average RTT reduce 30% - 40% vs traditional ARQ like tcp.
// + Maximum RTT reduce three times vs tcp.
// + Lightweight, distributed as a single source file.
//
//=====================================================================
#ifndef __IKCP_H__
#define __IKCP_H__

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


//=====================================================================
// 32BIT INTEGER DEFINITION 
//=====================================================================
#ifndef __INTEGER_32_BITS__
#define __INTEGER_32_BITS__
#if defined(_WIN64) || defined(WIN64) || defined(__amd64__) || \
	defined(__x86_64) || defined(__x86_64__) || defined(_M_IA64) || \
	defined(_M_AMD64)
	typedef unsigned int ISTDUINT32;
	typedef int ISTDINT32;
#elif defined(_WIN32) || defined(WIN32) || defined(__i386__) || \
	defined(__i386) || defined(_M_X86)
	typedef unsigned long ISTDUINT32;
	typedef long ISTDINT32;
#elif defined(__MACOS__)
	typedef UInt32 ISTDUINT32;
	typedef SInt32 ISTDINT32;
#elif defined(__APPLE__) && defined(__MACH__)
	#include <sys/types.h>
	typedef u_int32_t ISTDUINT32;
	typedef int32_t ISTDINT32;
#elif defined(__BEOS__)
	#include <sys/inttypes.h>
	typedef u_int32_t ISTDUINT32;
	typedef int32_t ISTDINT32;
#elif (defined(_MSC_VER) || defined(__BORLANDC__)) && (!defined(__MSDOS__))
	typedef unsigned __int32 ISTDUINT32;
	typedef __int32 ISTDINT32;
#elif defined(__GNUC__)
	#include <stdint.h>
	typedef uint32_t ISTDUINT32;
	typedef int32_t ISTDINT32;
#else 
	typedef unsigned long ISTDUINT32; 
	typedef long ISTDINT32;
#endif
#endif


//=====================================================================
// Integer Definition
//=====================================================================
#ifndef __IINT8_DEFINED
#define __IINT8_DEFINED
typedef char IINT8;
#endif

#ifndef __IUINT8_DEFINED
#define __IUINT8_DEFINED
typedef unsigned char IUINT8;
#endif

#ifndef __IUINT16_DEFINED
#define __IUINT16_DEFINED
typedef unsigned short IUINT16;
#endif

#ifndef __IINT16_DEFINED
#define __IINT16_DEFINED
typedef short IINT16;
#endif

#ifndef __IINT32_DEFINED
#define __IINT32_DEFINED
typedef ISTDINT32 IINT32;
#endif

#ifndef __IUINT32_DEFINED
#define __IUINT32_DEFINED
typedef ISTDUINT32 IUINT32;
#endif

#ifndef __IINT64_DEFINED
#define __IINT64_DEFINED
#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef __int64 IINT64;
#else
typedef long long IINT64;
#endif
#endif

#ifndef __IUINT64_DEFINED
#define __IUINT64_DEFINED
#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef unsigned __int64 IUINT64;
#else
typedef unsigned long long IUINT64;
#endif
#endif

#ifndef INLINE
#if defined(__GNUC__)

#if (__GNUC__ > 3) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1))
#define INLINE         __inline__ __attribute__((always_inline))
#else
#define INLINE         __inline__
#endif

#elif (defined(_MSC_VER) || defined(__BORLANDC__) || defined(__WATCOMC__))
#define INLINE __inline
#else
#define INLINE 
#endif
#endif

#ifndef inline
#define inline INLINE
#endif


//=====================================================================
// QUEUE DEFINITION                                                  
//=====================================================================
#ifndef __IQUEUE_DEF__
#define __IQUEUE_DEF__

struct IQUEUEHEAD {
	struct IQUEUEHEAD *next, *prev;
};

typedef struct IQUEUEHEAD iqueue_head;


//---------------------------------------------------------------------
// queue init                                                         
//---------------------------------------------------------------------
#define IQUEUE_HEAD_INIT(name) { &(name), &(name) }
#define IQUEUE_HEAD(name) \
	struct IQUEUEHEAD name = IQUEUE_HEAD_INIT(name)

#define IQUEUE_INIT(ptr) ( \
	(ptr)->next = (ptr), (ptr)->prev = (ptr))

#define IOFFSETOF(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define ICONTAINEROF(ptr, type, member) ( \
		(type*)( ((char*)((type*)ptr)) - IOFFSETOF(type, member)) )

#define IQUEUE_ENTRY(ptr, type, member) ICONTAINEROF(ptr, type, member)


//---------------------------------------------------------------------
// queue operation                     
//---------------------------------------------------------------------
#define IQUEUE_ADD(node, head) ( \
	(node)->prev = (head), (node)->next = (head)->next, \
	(head)->next->prev = (node), (head)->next = (node))

#define IQUEUE_ADD_TAIL(node, head) ( \
	(node)->prev = (head)->prev, (node)->next = (head), \
	(head)->prev->next = (node), (head)->prev = (node))

#define IQUEUE_DEL_BETWEEN(p, n) ((n)->prev = (p), (p)->next = (n))

#define IQUEUE_DEL(entry) (\
	(entry)->next->prev = (entry)->prev, \
	(entry)->prev->next = (entry)->next, \
	(entry)->next = 0, (entry)->prev = 0)

#define IQUEUE_DEL_INIT(entry) do { \
	IQUEUE_DEL(entry); IQUEUE_INIT(entry); } while (0)

#define IQUEUE_IS_EMPTY(entry) ((entry) == (entry)->next)

#define iqueue_init		IQUEUE_INIT
#define iqueue_entry	IQUEUE_ENTRY
#define iqueue_add		IQUEUE_ADD
#define iqueue_add_tail	IQUEUE_ADD_TAIL
#define iqueue_del		IQUEUE_DEL
#define iqueue_del_init	IQUEUE_DEL_INIT
#define iqueue_is_empty IQUEUE_IS_EMPTY

#define IQUEUE_FOREACH(iterator, head, TYPE, MEMBER) \
	for ((iterator) = iqueue_entry((head)->next, TYPE, MEMBER); \
		&((iterator)->MEMBER) != (head); \
		(iterator) = iqueue_entry((iterator)->MEMBER.next, TYPE, MEMBER))

#define iqueue_foreach(iterator, head, TYPE, MEMBER) \
	IQUEUE_FOREACH(iterator, head, TYPE, MEMBER)

#define iqueue_foreach_entry(pos, head) \
	for( (pos) = (head)->next; (pos) != (head) ; (pos) = (pos)->next )
	

#define __iqueue_splice(list, head) do {	\
		iqueue_head *first = (list)->next, *last = (list)->prev; \
		iqueue_head *at = (head)->next; \
		(first)->prev = (head), (head)->next = (first);		\
		(last)->next = (at), (at)->prev = (last); }	while (0)

#define iqueue_splice(list, head) do { \
	if (!iqueue_is_empty(list)) __iqueue_splice(list, head); } while (0)

#define iqueue_splice_init(list, head) do {	\
	iqueue_splice(list, head);	iqueue_init(list); } while (0)


#ifdef _MSC_VER
#pragma warning(disable:4311)
#pragma warning(disable:4312)
#pragma warning(disable:4996)
#endif

#endif


//---------------------------------------------------------------------
// WORD ORDER
//---------------------------------------------------------------------
#ifndef IWORDS_BIG_ENDIAN
    #ifdef _BIG_ENDIAN_
        #if _BIG_ENDIAN_
            #define IWORDS_BIG_ENDIAN 1
        #endif
    #endif
    #ifndef IWORDS_BIG_ENDIAN
        #if defined(__hppa__) || \
            defined(__m68k__) || defined(mc68000) || defined(_M_M68K) || \
            (defined(__MIPS__) && defined(__MISPEB__)) || \
            defined(__ppc__) || defined(__POWERPC__) || defined(_M_PPC) || \
            defined(__sparc__) 
            #define IWORDS_BIG_ENDIAN 1
        #endif
    #endif
    #ifndef IWORDS_BIG_ENDIAN
        #define IWORDS_BIG_ENDIAN  0
    #endif
#endif


//=====================================================================
// SEGMENT
//=====================================================================
struct IKCPSEG
{
	struct IQUEUEHEAD node;
	IUINT32 conv; //表示一条连接
	IUINT32 cmd;  //4种 数据包、确认包. 请求远端窗口大小包. 应答窗口大小包
	IUINT32 frg;  //分片
	IUINT32 wnd;
	IUINT32 ts;  //timestamp
	IUINT32 sn;  //序列号
	IUINT32 una; //此编号之前的包都确认过，但不包括此包
	IUINT32 len; //data size
	IUINT32 resendts; //重传时间
	IUINT32 rto; //rto时间
	IUINT32 fastack; //被跳过的次数
	IUINT32 xmit;
	char data[1];
};


//---------------------------------------------------------------------
// IKCPCB
//---------------------------------------------------------------------
struct IKCPCB
{
	IUINT32 state;
	IUINT32 ts_recent, ts_lastack;
	IUINT32 dead_link;
	IUINT32 incr;

	IUINT32 xmit;

	IUINT32 rx_srtt; //rtt时间
	IUINT32 rx_rttval; //和rx_srtt决定rx_rto时间
	IUINT32 rx_minrto; //最小rto时间
	IUINT32 rx_rto; //当前rto时间

	IUINT32 conv; //表示一条连接
	IUINT32 mss; //最大报文段
	IUINT32 mtu; //最大传输单元

	IUINT32 ts_flush; //update下次刷新时间戳
	IUINT32 interval; //心跳间隔
	IUINT32 updated; //是否心跳
	IUINT32 nodelay; //影响rto

	IUINT32 ts_probe; //探测等待时间戳
	IUINT32 probe_wait; //探测等待时间

	IUINT32 current; //当前时间戳

	IUINT32 snd_una; //目前已确认过的编号，不包括此编号
	IUINT32 snd_nxt; //下一个要发送的编号
	IUINT32 rcv_nxt; //下一个要接收的编号
	IUINT32 probe; //探测远端窗口大小
	IUINT32 ssthresh; //慢启动阈值

	IUINT32 cwnd; //当前发送窗口大小
	IUINT32 rmt_wnd; //远端窗口大小
    const IUINT32 snd_wnd; //发送缓存大小
    IUINT32 rcv_wnd; //接收窗口大小

	//接受数据 data -> rcv_buf -> rcv_queue ; ikcp_input
	//取数据 -> rcv_queue -> buffer ; ikcp_recv
	//写数据 buffer -> snd_queue; ikcp_send
	//发送数据 snd_queue -> snd_buf; ikcp_update
	IUINT32 nrcv_buf;
	struct IQUEUEHEAD rcv_buf;
	IUINT32 nrcv_que;
	struct IQUEUEHEAD rcv_queue;

	IUINT32 nsnd_que;
	struct IQUEUEHEAD snd_queue;
	IUINT32 nsnd_buf;
	struct IQUEUEHEAD snd_buf;
	
	IUINT32 *acklist; //收到数据包需要进行确认的序列号和时间戳
	IUINT32 ackcount; //收到数据包需要进行确认的个数
	IUINT32 ackblock; //内存申请的大小capacity
	void *user;
	char *buffer; //组装发送数据的缓冲区
	int fastresend; //开启快速重传
	int nocwnd; //1 表示拥塞算法不影响窗口大小
	int logmask;
	int (*output)(const char *buf, int len, struct IKCPCB *kcp, void *user);
	void (*writelog)(const char *log, struct IKCPCB *kcp, void *user);
};

typedef struct IKCPCB ikcpcb;

#define IKCP_LOG_OUTPUT			1
#define IKCP_LOG_INPUT			2
#define IKCP_LOG_SEND			4
#define IKCP_LOG_RECV			8
#define IKCP_LOG_IN_DATA		16
#define IKCP_LOG_IN_ACK			32
#define IKCP_LOG_IN_PROBE		64
#define IKCP_LOG_IN_WINS		128
#define IKCP_LOG_OUT_DATA		256
#define IKCP_LOG_OUT_ACK		512
#define IKCP_LOG_OUT_PROBE		1024
#define IKCP_LOG_OUT_WINS		2048

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------
// interface
//---------------------------------------------------------------------

// create a new kcp control object, 'conv' must equal in two endpoint
// from the same connection. 'user' will be passed to the output callback
// output callback can be setup like this: 'kcp->output = my_udp_output'
ikcpcb* ikcp_create(IUINT32 conv, void *user);

// release kcp control object
void ikcp_release(ikcpcb *kcp);

//把rcv_queue的一个数据报放入buffer缓冲中
//返回值为数据报的大小
//应用层调用此方法可以处理buffer里的内容 = recv
// user/upper level recv: returns size, returns below zero for EAGAIN
int ikcp_recv(ikcpcb *kcp, char *buffer, int len);

//把buffer里的数据放入snd_queue队列里
// user/upper level send, returns below zero for error
int ikcp_send(ikcpcb *kcp, const char *buffer, int len);

// update state (call it repeatedly, every 10ms-100ms), or you can ask 
// ikcp_check when to call it again (without ikcp_input/_send calling).
// 'current' - current timestamp in millisec. 
void ikcp_update(ikcpcb *kcp, IUINT32 current);

// Determine when should you invoke ikcp_update:
// returns when you should invoke ikcp_update in millisec, if there 
// is no ikcp_input/_send calling. you can call ikcp_update in that
// time, instead of call update repeatly.
// Important to reduce unnacessary ikcp_update invoking. use it to 
// schedule ikcp_update (eg. implementing an epoll-like mechanism, 
// or optimize ikcp_update when handling massive kcp connections)
IUINT32 ikcp_check(const ikcpcb *kcp, IUINT32 current);

//把data数据放入rec_buf, rec_buf -> rec_que
// when you received a low level packet (eg. UDP packet), call it
int ikcp_input(ikcpcb *kcp, const char *data, long size);

void ikcp_flush(ikcpcb *kcp);

int ikcp_peeksize(const ikcpcb *kcp);

// change MTU size, default is 1400
int ikcp_setmtu(ikcpcb *kcp, int mtu);

// set maximum window size: sndwnd=32, rcvwnd=32 by default
int ikcp_wndsize(ikcpcb *kcp, int sndwnd, int rcvwnd);

// get how many packet is waiting to be sent
int ikcp_waitsnd(const ikcpcb *kcp);

// fastest: ikcp_nodelay(kcp, 1, 20, 2, 1)
// nodelay: 0:disable(default), 1:enable  影响RTO不翻倍
// interval: internal update timer interval in millisec, default is 100ms 
// resend: 0:disable fast resend(default), 1:enable fast resend
// nc: 0:normal congestion control(default), 1:disable congestion control
int ikcp_nodelay(ikcpcb *kcp, int nodelay, int interval, int resend, int nc);

int ikcp_rcvbuf_count(const ikcpcb *kcp);
int ikcp_sndbuf_count(const ikcpcb *kcp);

void ikcp_log(ikcpcb *kcp, int mask, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif


