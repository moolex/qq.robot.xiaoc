/*
 *  QQAPIServer.c
 *
 *  main() QQRobot API.
 *
 *  Copyright (C) 2010  Moyo (moyo.live@gmail.com)
 *
 *  Notice: This program is based on MyQQ3.19 by Huang Guan (gdxxhg@gmail.com)
 *
 *  2008-01-31 Created.
 *  ~~
 *  2010-8-17  Rebuild by Moyo.
 *
 *  Description: This file mainly includes the functions about
 *  API Command.
 *
 *  Warning: this file should be in UTF-8.
 *
 */


#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#ifdef __WIN32__
#include <conio.h>
#include <winsock.h>
#include <wininet.h>
#include <windows.h>
#else
#include <termios.h> //read password
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif
#include "qqclient.h"
#include "buddy.h"
#include "qun.h"
#include "group.h"
#include "debug.h"
#include "memory.h"
#include "utf8.h"
#include "config.h"
#include "qqsocket.h"
#include "api.h"

#ifndef MSG
#define MSG need_reset = 1; setcolor( color_index ); printf
#endif

static char *qun_buf, *buddy_buf, *print_buf;
//static uint to_uid = 0;		//send message to that guy.
//static uint qun_int_uid;		//internal qun id if entered.
//static char input[1024];
//static int enter = 0;
static qqclient* qq;
static apiclient* apis;
//static apicall* apic;
//static int need_reset, no_color = 0;

#ifdef __WIN32__
#define CCOL_GREEN	FOREGROUND_GREEN
#define CCOL_LIGHTBLUE	FOREGROUND_BLUE | FOREGROUND_GREEN
#define CCOL_REDGREEN	FOREGROUND_RED | FOREGROUND_GREEN
#define CCOL_NONE		FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
//static int color_index = CCOL_NONE;
/*
static void charcolor( int col )
{
	color_index = col;
}
static void setcolor( int col )
{
	if(!no_color)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY | col);
}
*/
static void clear_screen()
{
	system("cls");
}
#else

#define CCOL_NONE		"\033[0m"
#define CCOL_BLACK	"\033[0;30m"
#define CCOL_DARKGRAY	"\033[1;30m"
#define CCOL_BLUE		"\033[0;34m"
#define CCOL_LIGHTBLUE	"\033[1;34m"
#define CCOL_GREEN	"\033[0;32m"
#define CCOL_LIGHTGREEN	"\033[1;32m"
#define CCOL_CYAN		"\033[0;36m"
#define CCOL_LIGHTCYAN	"\033[1;36m"
#define CCOL_RED		"\033[0;31m"
#define CCOL_LIGHTRED	"\033[1;31m"
#define CCOL_PURPLE	"\033[0;35m"
#define CCOL_LIGHTPURPLE	"\033[1;35m"
#define CCOL_LIGHTBROWN	"\033[0;33m"
#define CCOL_YELLOW	"\033[1;33m"
#define CCOL_LIGHTGRAY	"\033[0;37m"
#define CCOL_WHITE	"\033[1;37m"
#define CCOL_REDGREEN	CCOL_YELLOW
static char* color_index = CCOL_NONE;
static void charcolor( const char* col )
{
	color_index = (char*)col;
}
static void setcolor( const char* col )
{
	if(!no_color)
		printf( col );
}
static void clear_screen()
{
	//system("clear");
	printf("\033[2J   \033[0;0f");
}
#endif

//含颜色控制
#define RESET_INPUT \
	if( need_reset ){\
		charcolor( CCOL_NONE );\
	if( enter ){ \
		MSG("In {%s}> ", _TEXT( myqq_get_qun_name( qq, qun_int_uid ) ) ); \
	}else{ \
		MSG("To [%s]> ", _TEXT( myqq_get_buddy_name( qq, to_uid ) ) );} \
	fflush( stdout ); \
	need_reset = 0;}




#ifdef __WIN32__
#define _TEXT to_gb_force
static char* to_gb_force( char* s )
{
//	memset( print_buf, 0, PRINT_BUF_SIZE );
	utf8_to_gb( s, print_buf, PRINT_BUF_SIZE-1 );
	return print_buf;
}
/*
static char* to_utf8( char* s )
{
//	memset( print_buf, 0, PRINT_BUF_SIZE );
	gb_to_utf8( s, print_buf, PRINT_BUF_SIZE-1 );
	return print_buf;
}
*/

#else
#define _TEXT
#endif
/*
static int inputline(char *s, int lim) {
	char *t;
	int c;

	t = s;
	while (--lim>1 && (c=getchar()) != EOF && c != '\n')
		*s++ = c;
	*s = '\0';
	return s - t;
}
*/

static char* mode_string( int mode )
{
	switch( mode ){
	case QQ_ONLINE:
		return "Online";
	case QQ_AWAY:
		return "Away";
	case QQ_BUSY:
		return "Busy";
	case QQ_OFFLINE:
		return "Offline";
	case QQ_HIDDEN:
		return "Hidden";
	case QQ_KILLME:
		return "KillMe";
	case QQ_QUIET:
		return "Quiet";
	}
	return "Unknown";
}
/*static char* skip_line( char* p, int ln )
{
	while( *p && ln-- )
	{
		p ++;
		while( *p && *p!='\n' ) p++;
	}
	return p;
}
*/
char* myqq_login(uint uid, char* password)
{
    if ( qq->process == P_LOGIN ) return "ALREADY LOGIN";
    qqclient_create(qq, uid, password);
    qqclient_login(qq);
    for (; qq->process != P_LOGIN; qqclient_wait(qq, 1))
    {
        switch ( qq->process )
        {
            case P_LOGGING: continue;
            case P_VERIFYING:
                return "NEED VERIFY";
            case P_ERROR:
                qqclient_logout(qq);
                qqclient_cleanup(qq);
                return "NETWORK ERROR";
            case P_DENIED:
                qqclient_logout(qq);
                qqclient_cleanup(qq);
                return "NEED ACTIVATE";
            case P_WRONGPASS:
                qqclient_logout(qq);
                qqclient_cleanup(qq);
                return "PASSWORD ERROR";
            default:
                return "UNKNOWN ERROR";
        }
    }
    return "LOGIN SUCCESS";
}
char* myqq_logout()
{
    if ( qq->process != P_LOGIN ) return "NOT LOGIN";
    OPS("### @ LOGIN : QQClient logout Runing...");
    qqclient_logout( qq );
    OPS("### @ LOGIN : QQClient cleanup Runing...");
	qqclient_cleanup( qq );
	return "LOGOUT SUCCESS";
}
char* myqq_resume_login()
{
    for (; qq->process != P_LOGIN; qqclient_wait(qq, 1))
    {
        switch ( qq->process )
        {
            case P_LOGGING: continue;
            case P_VERIFYING:
                return "NEED VERIFY";
            case P_ERROR:
                qqclient_logout(qq);
                qqclient_cleanup(qq);
                return "NETWORK ERROR";
            case P_DENIED:
                qqclient_logout(qq);
                qqclient_cleanup(qq);
                return "NEED ACTIVATE";
            case P_WRONGPASS:
                qqclient_logout(qq);
                qqclient_cleanup(qq);
                return "PASSWORD ERROR";
            default:
                return "UNKNOWN ERROR";
        }
    }
    return "LOGIN SUCCESS";
}
char* myqq_check_login(qqclient* qq)
{
    switch ( qq->process )
    {
        case P_LOGIN:
            return "ALREADY LOGIN";
        case P_LOGGING:
            return "PROCESS LOGGING";
        case P_VERIFYING:
            return "NEED VERIFY";
        case P_ERROR:
            qqclient_logout(qq);
            qqclient_cleanup(qq);
            return "NETWORK ERROR";
        case P_DENIED:
            qqclient_logout(qq);
            qqclient_cleanup(qq);
            return "NEED ACTIVATE";
        case P_WRONGPASS:
            qqclient_logout(qq);
            qqclient_cleanup(qq);
            return "PASSWORD ERROR";
        default:
            return "NOT LOGIN";
    }
}
char* myqq_get_buddy_name( qqclient* qq, uint uid )
{
    if (qq->process != P_LOGIN) return "Nobody";
	static char tmp[16];
	qqbuddy* b = buddy_get( qq, uid, 0 );
	if( b )
		return b->nickname;
	if( uid == 10000 )
		return "System";
	if( uid != 0 ){
		sprintf( tmp, "%u" , uid );
		return tmp;
	}
	return "Nobody";
}
char* myqq_get_qun_name( qqclient* qq, uint uid )
{
    if (qq->process != P_LOGIN) return "[q==NULL]";
	static char tmp[16];
	qqqun* q = qun_get( qq, uid, 1 );
	if( q )
		return q->name;
	if( uid != 0 ){
		sprintf( tmp, "%u" , uid );
		return tmp;
	}
	return "[q==NULL]";
}
char* myqq_get_qun_member_name( qqclient* qq, uint int_uid, uint uid )
{
    if (qq->process != P_LOGIN) return "[q==NULL]";
	static char tmp[16];
	qqqun* q = qun_get( qq, int_uid, 0 );
	if( q ){
		qunmember* m = qun_member_get( qq, q, uid, 0 );
		if( m )
			return m->nickname;
		if( uid != 0 ){
			sprintf( tmp, "%u" , uid );
			return tmp;
		}
		return "[m==NULL]";
	}
	return "[q==NULL]";
}
int myqq_send_im_to_qun( qqclient* qq, uint int_uid, char* msg, int wait )
{
    if (qq->process != P_LOGIN) return 0;
    qq->send_qun_count ++ ;
	qun_send_message( qq, int_uid, msg );
	if( wait )
	{
		if( qqclient_wait( qq, 5 ) < 0 )
			return -1;
	}
	return 0;
}
int myqq_send_im_to_buddy( qqclient* qq, uint int_uid, char* msg, int wait )
{
    if (qq->process != P_LOGIN) return 0;
    qq->send_budy_count ++ ;
	buddy_send_message( qq, int_uid, msg );
	if( wait )
	{
		if( qqclient_wait( qq, 5 ) < 0 )
			return -1;
	}
	return 0;
}

int myqq_get_buddy_info( qqclient* qq, uint uid, char* buf, int size )
{
    if (qq->process != P_LOGIN) return 0;
	qqbuddy *b = buddy_get( qq, uid, 0 );
	if( size < 256 )
		return -1;
	if( b == NULL )
		return -2;
	int len;
	len = sprintf( buf,	"好友昵称\t%s\n"
				"用户账号\t%d\n"
				"签名\t\t%s\n"
				"备注\t\t%s\n"
				"手机\t\t%s\n"
				"邮箱\t\t%s\n"
				"职业\t\t%s\n"
				"主页\t\t%s\n"
				"毕业学院\t%s\n"
				"来自\t\t%s %s %s\n"
				"通讯地址\t%s\n"
				"自我介绍\t%s\n"
				"头像\t\t%d\n"
				"年龄\t\t%d\n"
				"性别\t\t%s\n"
				"状态\t\t%s\n",
		b->nickname, b->number, b->signature, b->alias, b->mobilephone,
		b->email, b->occupation, b->homepage, b->school, b->country, b->province, b->city,
		b->address, b->brief, b->face, b->age,
		(b->sex==0x00)?"Male": (b->sex==0x01)?"Female":"Asexual", mode_string(b->status) );
	return len;
}


//Note: this function change the source string directly.
static char* util_escape( char* str )
{
	unsigned char* ch;
	ch = (unsigned char*)str;
	while( *ch )
	{
		if( *ch < 0x80 ) //ASCII??
		{
			if( !isprint(*ch) ) //if not printable??
				*ch = ' ';	//use space instead..
			ch ++;	//skip one
		}else{
			ch +=2; //skip two
		}
	}
	return str;
}

/*   The output buf looks like this:
L8bit  L16bit		L16bit		L32bit
1	  357339036	 online		Xiao xia
2	  273310179	 offline	Huang Guan
*/
//Note: size must be big enough
int myqq_get_buddy_list( qqclient* qq, char* buf, int size, char online )
{
    if (qq->process != P_LOGIN) return 0;
	int i, len = 0;
	//avoid editing the array
	buf[0] = 0;
	pthread_mutex_lock( &qq->buddy_list.mutex );
	int ln = 1;
	for( i=0; i<qq->buddy_list.count; i++ )
	{
		qqbuddy * b = (qqbuddy*)qq->buddy_list.items[i];
		if( online && b->status == QQ_OFFLINE ) continue;
		if( *b->alias ){
			len = sprintf( buf, "%s%-8d%-16d%-16s%-16.64s\n", buf, ln ++, b->number,
				mode_string( (int) b->status ), util_escape( b->alias ) );
		}else{
			len = sprintf( buf, "%s%-8d%-16d%-16s%-16.64s\n", buf, ln ++, b->number,
				mode_string( (int) b->status ), util_escape( b->nickname ) );
		}
		if( len + 80 > size ) break;
	}
	pthread_mutex_unlock( &qq->buddy_list.mutex );
	return len;
}

/*   The output buf looks like this:
L8bit  L16bit		L16bit		L32bit
1	  467788923	 12118724	Xinxing Experimental School
2	  489234223	 13223423	SGOS2007
*/
//Note: size must be big enough
int myqq_get_qun_list( qqclient* qq, char* buf, int size )
{
    if (qq->process != P_LOGIN) return 0;
	int i, len = 0, ln=1;
	//avoid editing the array
	buf[0] = 0;
	pthread_mutex_lock( &qq->qun_list.mutex );
	for( i=0; i<qq->qun_list.count; i++ )
	{
		qqqun * q = (qqqun *)qq->qun_list.items[i];
		len = sprintf( buf, "%s%-8d%-16d%-16d%-16.64s\n", buf, ln ++, q->number,
			q->ext_number, util_escape( q->name ) );
		if( len + 80 > size ) break;
	}
	pthread_mutex_unlock( &qq->qun_list.mutex );
	return len;
}

/*   The output buf looks like this:
L8bit  L16bit		L16bit		L32bit
1	  357339036	 Manager	Xiaoxia
2	  273310179	 Fellow		Huang Guan
*/
//Note: size must be big enough
int myqq_get_qun_member_list( qqclient* qq, uint int_uid, char* buf, int size, char online )
{
    if (qq->process != P_LOGIN) return 0;
	qqqun * q = qun_get( qq, int_uid, 0 );
	if( !q )return 0;
	//Hope the Qun won't get removed while we are using it.
	int i, len = 0, ln = 1;
	buf[0] = 0;
	pthread_mutex_lock( &q->member_list.mutex );
	for( i=0; i<q->member_list.count; i++ )
	{
		qunmember * m = (qunmember *)q->member_list.items[i];
		if( online && m->status == QQ_OFFLINE ) continue;
		len = sprintf( buf, "%s%-8d%-16d%-16s%-16.64s\n", buf, ln++, m->number,
			(m->role&1)?"Admin":"Fellow", util_escape( m->nickname ) );
		if( len + 80 > size )
			break;
	}
	pthread_mutex_unlock( &q->member_list.mutex );
	return len;
}

//interface for getting qun information
/* Output style:
*/
int myqq_get_qun_info( qqclient* qq, uint int_uid, char* buf, int size )
{
    if (qq->process != P_LOGIN) return 0;
	char cate_str[4][10] = {"Classmate", "Friend", "Workmate", "Other" };
	qqqun *q = qun_get( qq, int_uid, 0 );
	if( !q )return 0;
	int len;
	if( size < 256 )
		return -1;

	if( q == NULL )
		return -2;
	len = sprintf( buf, "名称\t\t%s\n"
				"内部号码\t%d\n"
				"群号码\t\t%d\n"
				"群类型\t\t%s\n"
				"加入验证\t%s\n"
				"群分类\t\t%s\n"
				"创建人\t\t%d\n"
				"成员数量\t%d\n"
				"群的简介\n%s\n"
				"群的公告\n%s\n",
		q->name, q->number, q->ext_number, (q->type==0x01)?"Normal":"Special",
		(q->auth_type==0x01)?"No": (q->auth_type==0x02)?"Yes":
			(q->auth_type==0x03)?"RejectAll":"Unknown",
		q->category > 3 ? cate_str[3] : cate_str[(int)q->category],
		q->owner, q->member_list.count, q->intro, q->ann );
	return len;
}

// 自动回复
void myqq_auto_reply(int power)
{
    if (power)
    {
        gb_to_utf8(config_readstr(g_conf, "APIAutoReply"), qq->auto_reply, AUTO_REPLY_LEN);
    }
    else
    {
        memset(qq->auto_reply, 0, 0);
    }
}

//更改签名
void myqq_modify_signiture( struct qqclient* qq, char* sign )
{
    if (qq->process != P_LOGIN) return;
	qqpacket* p;
	p = packetmgr_new_send( qq, QQ_CMD_GET_BUDDY_SIGN );
	if( !p ) return;
	bytebuffer *buf = p->buf;

	put_byte( buf, 0x01 );//subcommand
	put_byte( buf, 0x01 );//??
	int len = strlen(sign);
	put_byte( buf, len );//sign len
	put_data( buf, (uchar*)sign, len );//sign
	post_packet( qq, p, SESSION_KEY );
}

// 个人信息通知
void buddy_msg_callback ( qqclient* qq, uint uid, time_t t, char* msg )
{
    if (qq->process != P_LOGIN) return;
    qq->recv_buddy_count ++ ;
    if (apis->APISleep)
    {
        char *sudo;
        NEW(sudo, 12);
        sprintf(sudo, "<%d>", uid);
        if ( strstr(apis->APISudo, sudo)==0 )
        {
            DEL(sudo);
            return;
        }
        DEL(sudo);
    }
    if ( uid > 10000 )
    {
        t += qq->local_time_movs;
        if ( apis->APIIgnoreOfflineMsg && (time(NULL) - t) > 60 )
        {
            OPS("### @ [Offline Msg].@Buddy.Ignore[%s]", _TEXT(msg));
            return;
        }
        if (strlen(msg) > HTTP_MESSAGE_SIZE)
        {
            OPS("### @ [Message too long].@Buddy.Ignore[%d > %d]", strlen(msg), HTTP_MESSAGE_SIZE);
            return;
        }
    }
    char *event;
    NEW(event, 18);
    char *data;
    NEW(data, HTTP_POST_SIZE);
    switch (uid)
    {
        case 100:
            strcpy(event, "buddy.operate");
            break;
        case 101:
            strcpy(event, "buddy.operate");
            break;
        case 10000:
            strcpy(event, "system.message");
            break;
        default:
            strcpy(event, "buddy.message");
    }
    sprintf( data,
        "event=%s&"
        "uid=%d&"
        "message=%s"
        , event, uid, url_encode(msg)
    );
    OPS("Callback Runing @ Buddy[%s]=>%s...", _TEXT(myqq_get_buddy_name(qq, uid)), _TEXT(msg));
    api_callback(data);
    DEL(event);
    DEL(data);
}

// 群信息通知
void qun_msg_callback ( qqclient* qq, uint uid, uint int_uid, time_t t, char* msg )
{
    if (qq->process != P_LOGIN) return;
    qq->recv_qun_count ++ ;
    if (apis->APISleep)
    {
        char *sudo;
        NEW(sudo, 12);
        sprintf(sudo, "<%d>", uid);
        if ( strstr(apis->APISudo, sudo)==0 )
        {
            DEL(sudo);
            return;
        }
        DEL(sudo);
    }
    t += qq->local_time_movs;
    if ( apis->APIIgnoreOfflineMsg && (time(NULL) - t) > 60 )
    {
        OPS("### @ [Offline Msg].@Qun.Ignore[%s]", _TEXT(msg));
        return;
    }
    char *GetKey;
    NEW(GetKey, apis->APIEventKeyMaxLen);
    char *FoundKey;
    NEW(FoundKey, apis->APIEventKeyMaxLen)
    BOOL found = FALSE;
    int GKi;
    for (GKi=0; GKi<apis->APIEventKeyCount; GKi++)
    {
        strncpy(GetKey, msg, strlen(apis->APIEventKey[GKi]));
        GetKey = strlwr(GetKey);
        if ( strstr(GetKey, apis->APIEventKey[GKi])>0 )
        {
            found = TRUE;
            strcat(FoundKey, apis->APIEventKey[GKi]);
            break;
        }
    }
    DEL(GetKey);
    if (!found) return;
    if (strlen(msg) > HTTP_MESSAGE_SIZE)
    {
        OPS("### @ [Message too long].@Qun.Ignore[%d > %d]", strlen(msg), HTTP_MESSAGE_SIZE);
        return;
    }
    char *data;
    NEW(data, HTTP_POST_SIZE);
    sprintf( data,
        "event=qun.message&"
        "gid=%d&"
        "uid=%d&"
        "message=%s"
        , int_uid, uid, url_encode(msg)
    );
    OPS("Callback Runing @ Qun[%s]=>[%s]:[%s]=>%s", _TEXT(myqq_get_qun_name(qq, int_uid)), _TEXT(myqq_get_qun_member_name(qq, int_uid, uid)), _TEXT(FoundKey), _TEXT(msg));
    api_callback(data);
    DEL(FoundKey);
    DEL(data);
}

// 签名变更通知
void buddy_sign_callback( qqclient* qq, uint uid, time_t t, char* msg)
{
    if (qq->process != P_LOGIN) return;
    qq->recv_sign_count ++ ;
    char *data;
    NEW(data, HTTP_POST_SIZE);
    sprintf( data,
        "event=buddy.sign&"
        "uid=%d&"
        "message=%s"
        , uid, url_encode(msg)
    );
    OPS("Callback Runing @ Sign[%s]=>%s", _TEXT(myqq_get_buddy_name(qq, uid)), _TEXT(msg));
    api_callback(data);
    DEL(data);
}

// muti threads callback
void api_callback(char *data)
{
    // 锁定callback数据
    pthread_mutex_lock(&apis->mxlock);
    qq->api_callback_count ++ ;
    strcpy(apis->data, data);
    void* arg = apis;
	pthread_create(&apis->pid, &apis->pconf, http_callback, arg);
}

void api_callback_build(char *event, char *message)
{
    // 锁定callback数据
    pthread_mutex_lock(&apis->mxlock);
    qq->api_callback_build_count ++ ;
    // build callback content
    char *data;
    NEW(data, HTTP_POST_SIZE);
    sprintf( data,
        "event=%s&"
        "uid=robot.apiz.org&"
        "message=%s"
        , event, url_encode(message)
    );
    strcpy(apis->data, data);
    DEL(data);
    void* arg = apis;
	pthread_create(&apis->pid, &apis->pconf, http_callback, arg);
}

#ifndef __WIN32__
int read_password(char   *lineptr )
{
	struct termios old, new;
	int nread;
	/* Turn echoing off and fail if we can't. */
	if (tcgetattr (fileno (stdout), &old) != 0)
        return -1;
	new = old;
	new.c_lflag &= ~ECHO;
	if (tcsetattr (fileno (stdout), TCSAFLUSH, &new) != 0)
		return -1;
	/* Read the password. */
	nread = scanf ("%31s", lineptr);
	/* Restore terminal. */
	(void) tcsetattr (fileno (stdout), TCSAFLUSH, &old);
	return nread;
}
#endif

int main(int argc, char** argv)
{
    if ( argc<2 )
    {
        MSG("Missing config file!");
        return -1;
    }
    clear_screen();
    // begin
	srand(time(NULL));
	//init data
	config_init(argv[1]);
	// apply memory
	NEW( qun_buf, QUN_BUF_SIZE );
	NEW( buddy_buf, BUDDY_BUF_SIZE );
	NEW( print_buf, PRINT_BUF_SIZE );
	NEW( qq, sizeof(qqclient) );
	NEW( apis, sizeof(apis) );
	if( !qun_buf || !buddy_buf || !print_buf || !qq || !apis )
	{
		MSG("no enough memory.");
		return -1;
	}
	qqsocket_init();
    // Defines
    short api_port = config_readint(g_conf, "APIPort");
    printf("APIServer.define Port @ %d\n", api_port);
	// API Server
	NEW(apis->APISeckey, 255);
	strcpy(apis->APISeckey, config_readstr(g_conf, "APISeckey"));
	printf("APIServer.load Seckey : %s\n", apis->APISeckey);
	// API EventKey
	char *APIEventKey, *eventKey;
	NEW(APIEventKey, 96);
	NEW(eventKey, 8);
	gb_to_utf8(config_readstr(g_conf, "APIEventKey"), APIEventKey, 96);
	int AEKi = 0;
    printf("APIServer.load Eventkeys | ");
	while ( strlen(APIEventKey)>2 )
	{
	    APIEventKey = mid_value(APIEventKey, "<", ">", eventKey, 8);
	    printf("%s | ", _TEXT(eventKey));
	    NEW(apis->APIEventKey[AEKi], 8);
        strcpy(apis->APIEventKey[AEKi], eventKey);
        if ( strlen(eventKey) > apis->APIEventKeyMaxLen )
        {
            apis->APIEventKeyMaxLen = strlen(eventKey);
        }
	    AEKi++;
	}
	//DEL(APIEventKey);
	DEL(eventKey);
	apis->APIEventKeyCount = AEKi;
	printf(" Done.\n");
	// Sleep
	apis->APISleep = 0;
	// Sudo
	NEW(apis->APISudo, 255);
	strcpy(apis->APISudo, config_readstr(g_conf, "APISudo"));
	printf("APIServer.define Sudo : %s\n", apis->APISudo);
	// Callback
	NEW(apis->APICallback, 255);
    strcpy(apis->APICallback, config_readstr(g_conf, "APICallback"));
    printf("APIServer.define Callback : %s\n", apis->APICallback);
	// Ignore Offline Message
	apis->APIIgnoreOfflineMsg = config_readint(g_conf, "APIIgnoreOfflineMsg");
	// apply memory for apis
	NEW(apis->data, HTTP_POST_SIZE);
	// api output switch
    uint apiops = config_readint( g_conf, "APIOutput" );
	if( apiops ){
		api_ops_on();
	}else{
		api_ops_off();
	}
    // Starting
    printf("Config load finish.. Server starting...\n\n");
    // licecnce check
    printf("Check Client Licence...\n");
    char *licence = api_check_licence(apis);
    printf("------------------------\n");
    printf("%s\n", _TEXT(licence));
    printf("------------------------\n");
    DEL(licence);
    if ( apis->Licenced )
    {
        printf("Licences allowed.\n\n");
    }
    else
    {
        printf("\n!!! @ Not Allowed Client @ !!!\n\n");
        printf("Please goto %s\n\n", LICENCE_CONSOLE_OL);
        printf("Exited.");
        return -1;
    }
    // socket
    int sock;
    printf("Initing Socket...");
    // 初始化 socket
    if ( (sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) <=0 )
    {
        printf(" Failed.[%s]\n", strerror(errno));
        return -1;
    }
    printf(" Done.[Sock:%d]\n", sock);

    // 端口复用？
    BOOL bReuseaddr = TRUE;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&bReuseaddr, sizeof(BOOL));

    // 非阻塞
    unsigned long ul = 1;
    ioctlsocket(sock, FIONBIO, (unsigned long*)&ul);

    // 本地地址
    struct sockaddr_in local_addr;
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY; // or inet_addr("ip_address")
    //local_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    local_addr.sin_port = htons(api_port);

    printf("Binding Socket Port...");
    // 绑定端口
    if ( bind(sock, (struct sockaddr*)&local_addr, sizeof(local_addr)) <0 )
    {
        printf(" Failed.[%s]\n", strerror(errno));
        return -1;
    }
    printf(" Done.[Port:%d]\n", api_port);

    printf("Initing Listener...");

    // 启用监听
    if ( listen(sock, 128) <0 )
    {
        printf(" Failed.[%s]\n", strerror(errno));
        return -1;
    }
    printf(" Done\n");
    printf("------ ### [http://xiaoc.uuland.org/] ### ------\n");
    printf("API Server Runs.\n");

    // 启动数据包管理器守护线程
    pthread_create( &qq->packetmgr.daemon, NULL, packetmgr_daemon, (void*)qq );

    // 启动授权更新守护线程
    //pthread_create( &apis->licence_daemon, NULL, api_licence_daemon, (void*)apis );

    // client信息
    int client;
    struct sockaddr_in client_addr;
    int sock_size = sizeof(struct sockaddr_in);

    // select 模型
    fd_set fdRead;
    struct timeval fdTV = {1, 0};

    // 初始化 apis 线程
    pthread_attr_init(&apis->pconf);
    pthread_attr_setscope(&apis->pconf, PTHREAD_SCOPE_PROCESS);
    pthread_attr_setdetachstate(&apis->pconf, PTHREAD_CREATE_DETACHED);

    // 初始化 apis 互斥锁
    pthread_mutex_init(&apis->mxlock, NULL);
    pthread_mutex_init(&apis->mxsock, NULL);

    // 循环接收连接请求
    while (1 && apis->Licenced)
    {
        FD_ZERO(&fdRead);
        FD_SET(sock, &fdRead);
        switch ( select(sock, &fdRead, NULL, NULL, &fdTV) )
        {
            default:
            if ( FD_ISSET(sock, &fdRead) )
            {
                client = accept(sock, (struct sockaddr*)&client_addr, &sock_size);
                // 锁定 socket 连接
                pthread_mutex_lock(&apis->mxsock);
                apis->sock = client;
                void* arg[3] = {qq, apis};
                pthread_create(&apis->pid, &apis->pconf, APISelect, arg);
            }
        }
    }
    // Close
    shutdown(sock, SD_BOTH);
    closesocket(sock);
    qqsocket_end();
    // Clear
    pthread_mutex_destroy(&apis->mxlock);
    pthread_mutex_destroy(&apis->mxsock);
    pthread_attr_destroy(&apis->pconf);
    DEL(apis->pconf);
    DEL(apis);
    DEL(qq);
    //DEL(sock);
}
