/*
 *  api.h
 *
 *  QQRobot API Server.
 *
 *  Copyright (C) 2010  Moyo (moyo.live@gmail.com)
 *
 *  Description: This file mainly includes the functions about
 *  API Server
 *
 */

#ifdef __WIN32__
#include <winsock.h>
#else
#include <sys/socket.h>
#endif

#include <stdio.h>
#include <winsock.h>
#include <pthread.h>
#include "qqsocket.h"
#include "debug.h"
#include "qqclient.h"
#include "memory.h"
#include "api.h"
#include "md5.h"


#define API_BUFFER 128
#define REQUEST_BUFFER 4096
#define RESPONSE_BUFFER 2048+80*500
#define SEND_MESSAGE_SIZE 3000

void http_send(int sock, char *message)
{
    OPS("### @ HTTP : Send Message: %s", message);
    // build
    char *http;
    NEW(http, 1024);
    //memset(http, 0, 1024);
    strcat(http, "HTTP/1.1 200 OK\r\n");
    strcat(http, "Content-Type: text/html\r\n");
    strcat(http, "\r\n");
    strcat(http, message);
    // Send
    send(sock, http, strlen(http), 0);
    // Close
    shutdown(sock, SD_BOTH);
    closesocket(sock);
    // Clear
    DEL(http);
    //DEL(sock);
}

void http_send_file(int sock, char *file)
{
    FILE *fp;
    fp = fopen(file, "rb");
    if( fp != NULL )
    {
        OPS("### @ HTTP : Send File : %s", file);
        // header
        char *http;
        NEW(http, 256);
        //memset(http, 0, 128);
        strcat(http, "HTTP/1.1 200 OK\r\n");
        strcat(http, "Content-Type: image/png\r\n");
        strcat(http, "\r\n");
        // send
        send(sock, http, strlen(http), 0);
        // body
        char *buffer;
        NEW(buffer, 512);
        while( !feof(fp) )
        {
            fread(buffer, 512, 1, fp);
            send(sock, buffer, 512, 0);
        }
        fclose(fp);
        // Close
        shutdown(sock, SD_BOTH);
        closesocket(sock);
        // Clear
        DEL(http);
        DEL(buffer);
        //DEL(sock);
        //DEL(fp);
     }
     else
     {
         OPS("### @ HTTP : File Not Found : %s", file);
         //DEL(sock);
         DEL(fp);
         http_send(sock, "FILE NOT FOUND");
     }
}

void APISelect(void *data)
{
    OPS("### @ API : Accept Success - Thread[%d]", pthread_self());
    // struct data
    qqclient* qq = ((void**)data)[0];
    qq->api_select_count ++ ;
    apiclient* apis = ((void**)data)[1];
    int client = apis->sock;
    pthread_mutex_unlock(&apis->mxsock);
    // Request
    char *buffer;
    NEW(buffer, REQUEST_BUFFER);
    // Select 模型读取请求
    fd_set fdRead;
    struct timeval fdRTV = {1, 0};

    FD_ZERO(&fdRead);
    FD_SET(client, &fdRead);
    switch ( select(client, &fdRead, NULL, NULL, &fdRTV) )
    {
        default:
        if ( FD_ISSET(client, &fdRead) )
        {
            recv(client, buffer, REQUEST_BUFFER, 0);
        }
    }

    if ( strlen(buffer) < 9 )
    {
        OPS("### @ API : Request Unavailable.");
        DEL(buffer);
        return;
    }

    // Request Test
    DBG("##### Request Begin #####\n%s\n##### Request End #####\n", buffer);

    char *http;
    NEW(http, 16);
    mid_value(buffer, "GET ", " HTTP/1.1", http, 16);
    if ( strstr(http, "/v?") >0 )
    {
        // 验证码读取
        char *uid, *file;
        NEW(uid, 18);
        NEW(file, 32);
        if ( strstr(http, "&") > 0 )
        {
            mid_value(http, "/v?", "&", uid, 18);
        }
        else
        {
            mid_value(http, "/v?", NULL, uid, 18);
        }
        sprintf(file, "./verify/%s.png", uid);
        http_send_file(client, file);
        DEL(uid);
        DEL(file);
        DEL(http);
        DEL(buffer);
        return;
    }
    else if ( strstr(http, "/send?") > 0 )
    {
        // 发送自定义信息
        char *msg;
        NEW(msg, REQUEST_BUFFER);
        mid_value(buffer, "GET /send?", " HTTP/1.1", msg, REQUEST_BUFFER);
        api_callback_build("http.send", msg);
        DEL(msg);
        http_send(client, "Message Sends OK.");
        DEL(http);
        DEL(buffer);
        return;
    }
    else if ( strlen(http) > 0 )
    {
        http_send(client, "QQRobot API Server.");
        DEL(http);
        DEL(buffer);
        return;
    }
    DEL(http);

    char *api;
    NEW(api, API_BUFFER);
    mid_value(buffer, "API ", " MOYO/1.1", api, API_BUFFER);
    if (strlen(api) == 0)
    {
        http_send(client, "API IS EMPTY");
        DEL(api);
        DEL(buffer);
        return;
    }

    // API Commands
    char *response;
    NEW(response, RESPONSE_BUFFER);
    // Check SecKey
    char *seckey;
    NEW(seckey, 255);
    mid_value(buffer, "<seckey>", "</seckey>", seckey, 255);
    if ( strcmp(apis->APISeckey, seckey)!=0 )
    {
        strcat(response, "ACCESS DENIED");
        OPS("### @ API : Denied of : %s", api);
        DEL(seckey);
        goto API_RESPONSE;
    }
    DEL(seckey);
    // API Runs
    OPS("### @ API : Command of : %s ...Runs ...", api);
    // ##### API of login #####
    if ( stricmp(api, "login.create")==0 )
    {
        char *uid, *password;
        NEW(uid, 10);
        NEW(password, 32);
        mid_value(buffer, "<uid>", "</uid>", uid, 10);
        mid_value(buffer, "<password>", "</password>", password, 32);
        strcat(response, myqq_login(atoi(uid), password));
        //printf("Done of %s.\n", api);
        DEL(uid);
        DEL(password);
    }
    else if ( stricmp(api, "login.destroy")==0 )
    {
        strcat(response, myqq_logout());
        //printf("Done of %s.\n", api);
    }
    else if ( stricmp(api, "login.verify")==0 )
    {
        char *vcode;
        NEW(vcode, 4);
        mid_value(buffer, "<verify>", "</verify>", vcode, 4);
        if ( qq->process == P_VERIFYING )
        {
            qqclient_verify(qq, vcode);
            OPS("### @ API : [Input] Verify Code : %s", vcode);
            strcat(response, myqq_resume_login());
        }
        else
        {
            strcat(response, "DONT NEED");
        }
        //printf("Done of %s.\n", api);
        DEL(vcode);
    }
    else if ( stricmp(api, "login.check")==0 )
    {
        strcat(response, myqq_check_login(qq));
        //printf("Done of %s.\n", api);
    }
    // ##### API of buddy #####
    else if ( stricmp(api, "buddy.name")==0 )
    {
        char *uid;
        NEW(uid, 10);
        mid_value(buffer, "<uid>", "</uid>", uid, 10);
        strcat(response, myqq_get_buddy_name(qq, atoi(uid)));
        //printf("Done of %s.\n", api);
        DEL(uid);
    }
    else if ( stricmp(api, "buddy.send")==0 )
    {
        char *uid, *message;
        NEW(uid, 10);
        NEW(message, SEND_MESSAGE_SIZE);
        mid_value(buffer, "<uid>", "</uid>", uid, 10);
        mid_value(buffer, "<message>", "</message>", message, SEND_MESSAGE_SIZE);
        if ( myqq_send_im_to_buddy(qq, atoi(uid), message, 0) )
        {
            strcat(response, "SEND FAILED");
        }
        else
        {
            strcat(response, "SEND SUCCESS");
        }
        //printf("Done of %s.\n", api);
        DEL(uid);
        DEL(message);
    }
    else if ( stricmp(api, "buddy.info")==0 )
    {
        char *uid;
        NEW(uid, 10);
        mid_value(buffer, "<uid>", "</uid>", uid, 10);
        char *info;
        NEW(info, KB(4));
        myqq_get_buddy_info(qq, atoi(uid), info, KB(4));
        strcat(response, info);
        //printf("Done of %s.\n", api);
        DEL(uid);
        DEL(info);
    }
    else if ( stricmp(api, "buddy.list")==0 )
    {
        char *online;
        NEW(online, 3);
        mid_value(buffer, "<online>", "</online>", online, 3);
        char *info;
        NEW(info, BUDDY_BUF_SIZE);
        if ( stricmp(online, "yes")==0 )
        {
            myqq_get_buddy_list(qq, info, BUDDY_BUF_SIZE, 1);
        }
        else
        {
            myqq_get_buddy_list(qq, info, BUDDY_BUF_SIZE, 0);
        }
        strcat(response, info);
        //printf("Done of %s.\n", api);
        DEL(online);
        DEL(info);
    }
    // ##### API of qun #####
    else if ( stricmp(api, "qun.name")==0 )
    {
        char *gid;
        NEW(gid, 10);
        mid_value(buffer, "<gid>", "</gid>", gid, 10);
        strcat(response, myqq_get_qun_name(qq, atoi(gid)));
        //printf("Done of %s.\n", api);
        DEL(gid);
    }
    else if ( stricmp(api, "qun.send")==0 )
    {
        char *gid, *message;
        NEW(gid, 10);
        NEW(message, SEND_MESSAGE_SIZE);
        mid_value(buffer, "<gid>", "</gid>", gid, 10);
        mid_value(buffer, "<message>", "</message>", message, SEND_MESSAGE_SIZE);
        if ( myqq_send_im_to_qun(qq, atoi(gid), message, 0) )
        {
            strcat(response, "SEND FAILED");
        }
        else
        {
            strcat(response, "SEND SUCCESS");
        }
        //printf("Done of %s.\n", api);
        DEL(gid);
        DEL(message);
    }
    else if ( stricmp(api, "qun.buddy.name")==0 )
    {
        char *gid, *uid;
        NEW(gid, 10);
        NEW(uid, 10);
        mid_value(buffer, "<gid>", "</gid>", gid, 10);
        mid_value(buffer, "<uid>", "</uid>", uid, 10);
        strcat(response, myqq_get_qun_member_name(qq, atoi(gid), atoi(uid)));
        //printf("Done of %s.\n", api);
        DEL(gid);
        DEL(uid);
    }
    else if ( stricmp(api, "qun.info")==0 )
    {
        char *gid;
        NEW(gid, 10);
        mid_value(buffer, "<gid>", "</gid>", gid, 10);
        char *info;
        NEW(info, KB(4));
        myqq_get_qun_info(qq, atoi(gid), info, KB(4));
        strcat(response, info);
        //printf("Done of %s.\n", api);
        DEL(gid);
        DEL(info);
    }
    else if ( stricmp(api, "qun.list")==0 )
    {
        char *info;
        NEW(info, QUN_BUF_SIZE);
        myqq_get_qun_list(qq, info, QUN_BUF_SIZE);
        strcat(response, info);
        //printf("Done of %s.\n", api);
        DEL(info);
    }
    else if ( stricmp(api, "qun.buddy.list")==0 )
    {
        char *gid, *online;
        NEW(gid, 10);
        NEW(online, 3);
        mid_value(buffer, "<gid>", "</gid>", gid, 10);
        mid_value(buffer, "<online>", "</online>", online, 3);
        char *info;
        NEW(info, BUDDY_BUF_SIZE);
        if ( stricmp(online, "yes")==0 )
        {
            myqq_get_qun_member_list(qq, atoi(gid), info, BUDDY_BUF_SIZE, 1);
        }
        else
        {
            myqq_get_qun_member_list(qq, atoi(gid), info, BUDDY_BUF_SIZE, 0);
        }
        strcat(response, info);
        //printf("Done of %s.\n", api);
        DEL(gid);
        DEL(online);
        DEL(info);
    }
    // ##### API of me #####
    else if ( stricmp(api, "me.sleep")==0 )
    {
        char *power;
        NEW(power, 3);
        mid_value(buffer, "<power>", "</power>", power, 3);
        if ( strcmp(power, "on")==0 )
        {
            apis->APISleep = 1;
        }
        else
        {
            apis->APISleep = 0;
        }
        strcat(response, "SWITCH SUCCESS");
        //printf("Done of %s.\n", api);
        DEL(power);
    }
    else if ( stricmp(api, "me.list.update")==0 )
    {
        qun_update_all(qq);
        buddy_update_list(qq);
        group_update_list(qq);
        if ( qqclient_wait(qq, 10)<0 )
        {
            strcat(response, "UPDATE FAILED");
        }
        else
        {
            strcat(response, "UPDATE SUCCESS");
        }
        //printf("Done of %s.\n", api);
    }
    else if ( stricmp(api, "me.autoreply")==0 )
    {
        char *power;
        NEW(power, 3);
        mid_value(buffer, "<power>", "</power>", power, 3);
        if ( strcmp(power, "on")==0 )
        {
            myqq_auto_reply(1);
        }
        else
        {
            myqq_auto_reply(0);
        }
        strcat(response, "SWITCH SUCCESS");
        //printf("Done of %s.\n", api);
        DEL(power);
    }
    else if ( stricmp(api, "me.status.update")==0 )
    {
        char *status;
        NEW(status, 6);
        mid_value(buffer, "<status>", "</status>", status, 6);
        if( strcmp( status, "away") == 0 )
            qqclient_change_status( qq, QQ_AWAY );
        else if( strcmp( status, "online") == 0 )
            qqclient_change_status( qq, QQ_ONLINE );
        else if( strcmp( status, "hidden") == 0 )
            qqclient_change_status( qq, QQ_HIDDEN );
        else if( strcmp( status, "killme") == 0 )
            qqclient_change_status( qq, QQ_KILLME );
        else if( strcmp( status, "busy") == 0 )
            qqclient_change_status( qq, QQ_BUSY );
        strcat(response, "UPDATE SUCCESS");
        //printf("Done of %s.\n", api);
        DEL(status);
    }
    else if ( stricmp(api, "me.buddy.add")==0 )
    {
        char *uid, *message;
        NEW(uid, 10);
        NEW(message, 50);
        mid_value(buffer, "<uid>", "</uid>", uid, 10);
        mid_value(buffer, "<message>", "</message>", message, 50);
        qqclient_add(qq, atoi(uid), message);
        strcat(response, "ADD SUCCESS");
        //printf("Done of %s.\n", api);
        DEL(uid);
        DEL(message);
    }
    else if ( stricmp(api, "me.buddy.del")==0 )
    {
        char *uid;
        NEW(uid, 10);
        mid_value(buffer, "<uid>", "</uid>", uid, 10);
        qqclient_del(qq, atoi(uid));
        strcat(response, "DELETE SUCCESS");
        //printf("Done of %s.\n", api);
        DEL(uid);
    }
    else
    {
        strcat(response, "UNKNOWN API COMMAND");
        OPS("### @ API : Command Unknown : %s", api);
    }
API_RESPONSE:
    // Send
    send(client, response, strlen(response), 0);

    // print
    OPS("### @ API : Command of : %s ...Done.", api);

    // Response Test
    DBG("##### Response Begin #####\n%s\n##### Response End #####\n", response);
    // Close
    shutdown(client, SD_BOTH);
    closesocket(client);
    // Clear
    DEL(response);
    //DEL(client);
    pthread_detach(pthread_self());
}

char* api_check_licence(apiclient* apis)
{
    // verify begins
    char *server;
    NEW(server, 128);
    strcpy(server, LICENCE_SERVER);
    char *data;
    NEW(data, HTTP_POST_SIZE);
    // init time stamp from server
    api_init_licence_time_now(apis);
    char *hash = api_create_transfer_hash(apis, "uri");
    sprintf( data,
        "url=%s&"
        "time=%d&"
        "key=%s"
        , url_encode(apis->APICallback), apis->licence_time_now, hash
    );
    DEL(hash);
    char *http = http_post(server, data);
    DEL(server);
    DEL(data);
    char *next;
    char *status;
    // check status
    NEW(status, 6);
    next = mid_value(http, "<status>", "</status>", status, 6);
    if (stricmp(status, "") == 0)
    {
        apis->Licenced = 0;
        return "Licence Server Error";
    }
    else if (stricmp(status, "false") == 0)
    {
        char *msg;
        NEW(msg, 256);
        next = mid_value(next, "<msg>", "</msg>", msg, 256);
        apis->Licenced = 0;
        return msg;
    }
    else if (stricmp(status, "true") != 0)
    {
        apis->Licenced = 0;
        return status;
    }
    // remove old and apply new
    DEL(apis->licence_status);
    NEW(apis->licence_status, 6);
    strcpy(apis->licence_status, status);
    DEL(status);
    // check transfer key
    char *timer, *key;
    NEW(timer, 12);
    NEW(key, 33);
    next = mid_value(next, "<time>", "</time>", timer, 12);
    next = mid_value(next, "<key>", "</key>", key, 33);
    int nowRecv = apis->licence_time_now;
    int timeMovs = abs(nowRecv - atoi(timer));
    if (timeMovs > 12)
    {
        apis->Licenced = 0;
        return "Local Times out";
    }
    char *local_key  = api_create_transfer_hash(apis, "sts");
    if ( stricmp(key, local_key) != 0 )
    {
        apis->Licenced = 0;
        return "Transfer Key Error";
    }
    DEL(timer);
    DEL(key);
    DEL(local_key);
    DEL(http);
    char *msg;
    NEW(msg, 256);
    next = mid_value(next, "<msg>", "</msg>", msg, 256);
    apis->Licenced = 1;
    return msg;
}

void api_init_licence_time_now(apiclient* apis)
{
    char *server;
    NEW(server, 128);
    strcpy(server, LICENCE_SERVER);
    char *data;
    NEW(data, HTTP_POST_SIZE);
    apis->licence_time_now = time(NULL);
    sprintf( data,
        "process=api_init_server_time&"
        "local=%d"
        , apis->licence_time_now
    );
    char *http = http_post(server, data);
    DEL(server);
    DEL(data);
    char *next;
    char *timer, *key;
    NEW(timer, 12);
    NEW(key, 33);
    next = mid_value(http, "<time>", "</time>", timer, 12);
    next = mid_value(next, "<key>", "</key>", key, 33);
    apis->licence_time_now = atoi(timer);
    apis->licence_seckey = LICENCE_SECKEY;
    char *local_key = api_create_transfer_hash(apis, "sck");
    if (stricmp(key, local_key) != 0)
    {
        apis->licence_time_now = 0;
    }
    // 真JJ奇怪，这两个变量用DEL就会崩溃
    //DEL(timer);
    //DEL(key);
    // 没办法，先用下面的代码释放内存了
    free(*timer);
    free(*key);
    DEL(local_key);
    DEL(http);
}

char* api_create_transfer_hash(apiclient* apis, char* type)
{
    char *keys = api_create_keys(apis, type);

    char *result;
    NEW(result, 33);

	md5_state_t state;
	md5_byte_t digest[16];

	md5_init(&state);
	md5_append(&state, (const md5_byte_t *)keys, strlen(keys));
	md5_finish(&state, digest);

    char one[2];
	int di;
	for (di = 0; di < 16; ++di)
	{
	    sprintf(one, "%02x", digest[di]);
	    strcat(result, one);
	}

	DEL(keys);
	// finish
    return result;
}

char* api_create_keys(apiclient* apis, char* type)
{
    char *keys, *flag;
    NEW(keys, 512);
    NEW(flag, 384);
    if (stricmp(type, "uri") == 0)
    {
        sprintf(flag, "@URI~%s", apis->APICallback);
    }
    else if (stricmp(type, "sts") == 0)
    {
        sprintf(flag, "@STS~%s", apis->licence_status);
    }
    else if (stricmp(type, "sck") == 0)
    {
        sprintf(flag, "@SCK~%s", apis->licence_seckey);
    }
    sprintf(keys, LICENCE_HASHED, apis->licence_time_now, flag);
    DEL(flag);
    return keys;
}

void api_update_licence(apiclient* apis)
{
    // update licence
    char *licence = api_check_licence(apis);
    if ( apis->Licenced )
    {

    }
    else
    {
        printf("Licence status changed.\n");
        printf("\n!!! @ Not Allowed Client @ !!!\n\n");
        printf("Please goto %s/\n\n", LICENCE_CONSOLE_OL);
        printf("Exited.");
    }
    DEL(licence);
}

#define INTERVAL 1
void api_licence_daemon( void* data )
{
	apiclient* apis = (apiclient*) data;
	int sleepintv = 120*60*1000/INTERVAL;
	int counter = 1;
	while( 1 ){
		counter ++;
		if( counter % INTERVAL == 0 ){
			api_update_licence(apis);
		}
		USLEEP( sleepintv );
	}
	return;
}
