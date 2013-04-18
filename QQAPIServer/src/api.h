#ifndef _API_H
#define _API_H

/*
* Config of xiaoc.uuland.org
*/

#define LICENCE_SERVER "http://licence.xiaoc.uuland.org/"
#define LICENCE_HASHED "f240a58b911fc5911a8619fc2798083b.(A).xiaoc@uuland.org.(C).%d.57b49186e7614a868276b80a4d813f78%s"
// MD5 of 20110122170868123.transfer.key.safe.uuland.org
#define LICENCE_SECKEY "9f357651d400c931cafd1196e80f54b3"

// console link
#define LICENCE_CONSOLE_OL "http://console.xiaoc.uuland.org/"
#define API_VERSION "Xiaoc.v1.b20110122"

/* End line */

// for QQAPIServer.c
void api_callback(char *data);
void api_callback_build(char *event, char *message);

// mine
void APISelect(void *data);
char* api_check_licence(apiclient* apis);
void api_init_licence_time_now(apiclient* apis);
char* api_create_transfer_hash(apiclient* apis, char* type);
char* api_create_keys(apiclient* apis, char* type);

void api_update_licence(apiclient* apis);
void api_licence_daemon( void* data );

// API Server uses
char* myqq_login(uint uid, char* password);
char* myqq_logout();
char* myqq_resume_login();
char* myqq_check_login(qqclient* qq);
char* myqq_get_buddy_name( qqclient* qq, uint uid );
char* myqq_get_qun_name( qqclient* qq, uint uid );
char* myqq_get_qun_member_name( qqclient* qq, uint int_uid, uint uid );
int myqq_send_im_to_qun( qqclient* qq, uint int_uid, char* msg, int wait );
int myqq_send_im_to_buddy( qqclient* qq, uint int_uid, char* msg, int wait );
int myqq_get_buddy_info( qqclient* qq, uint uid, char* buf, int size );
int myqq_get_buddy_list( qqclient* qq, char* buf, int size, char online );
int myqq_get_qun_list( qqclient* qq, char* buf, int size );
int myqq_get_qun_member_list( qqclient* qq, uint int_uid, char* buf, int size, char online );
int myqq_get_qun_info( qqclient* qq, uint int_uid, char* buf, int size );
void myqq_auto_reply(int power);
void myqq_modify_signiture( struct qqclient* qq, char* sign );

// qun.c
void qun_update_all( struct qqclient* qq );
// group.c
void group_update_list( struct qqclient* qq );
// buddy.c
void buddy_update_list( qqclient* qq );

#endif
