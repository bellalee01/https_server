/************************************
 * client.c
 * HTTPS客户端，对HTTPS服务器进行测试
 * 2019-11-15
 * 作者：张枫、李雪菲、赵挽涛
************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <event2/event.h>
#include <event2/util.h>
#include <event2/listener.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/thread.h>
#include "client.h"

enum MsgType {GET, POST, UPLOAD, DOWNLOAD}; // 请求报文类型
int i = 0;
/**
 *  TODO:HTTP请求报文
 *  TODO:HTTP持久连接，保持连接
 *  TODO:HTTP多次交互
 *  TODO:浏览器测试
 */
int main()
{
    // 创建socket
    int port = 8080;
    struct sockaddr_in my_address;
    memset(&my_address, 0, sizeof(my_address));
    my_address.sin_family = AF_INET;
    my_address.sin_addr.s_addr = htonl(0x7f000001); // 127.0.0.1
    my_address.sin_port = htons(port);

    struct timeval tv;
    evutil_timerclear(&tv);
    tv.tv_sec = 2;

    // 创建事件循环和写入事件
    struct event_base* base = event_base_new();
    evutil_socket_t fd;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    struct bufferevent* conn = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(conn, server_msg_cb, NULL, event_cb, NULL);
    bufferevent_enable(conn, EV_READ);
    // bufferevent_set_timeouts(conn, &tv, NULL);
    if(bufferevent_socket_connect(conn,(struct sockaddr*)&my_address,sizeof(my_address)) == 0)
        printf("connect success\n");
 
    // 将数据写入缓冲区
    char *mesg = CreateMsg(GET, NULL);
    bufferevent_write(conn, mesg, strlen(mesg));

    // 检测写入缓冲区数据
    // struct evbuffer* output = bufferevent_get_output(conn);
    // int len = 0;
    // len = evbuffer_get_length(output);
    // printf("output buffer has %d bytes left\n", len);
 
    // 定时器
    struct event *timer = event_new(base, -1, EV_TIMEOUT|EV_PERSIST, timer_cb, conn);
    struct timeval tv2;
    evutil_timerclear(&tv2);
    tv2.tv_usec = 1000*100;    
    evtimer_add(timer, &tv2);

    // 开始执行
    event_base_dispatch(base);
 
    // 释放资源
    bufferevent_free(conn);
    event_base_free(base);
    printf("Client program is over\n");
    
    return 0;
}

char *CreateMsg(int msgType, char *uri) {
    char *msg = NULL;
    msg = "ABCDTest";
    switch(msgType) {
        case GET: ; break;
        case POST: ; break;
        case UPLOAD: ; break;
        case DOWNLOAD: ; break;
        default: ; break;
    }
    return msg;
}

void event_cb(struct bufferevent *bev, short events, void *arg) {
    if (events & BEV_EVENT_CONNECTED) {
        printf("Client has successfully cliented.\n");
        return;
    }
    if (events & BEV_EVENT_EOF) {
        printf("Connection closed.\n");
    }
    else if (events & BEV_EVENT_ERROR) {
        printf("Some other error.\n");
    }
    else if (events & BEV_EVENT_TIMEOUT) {
        printf("Connection timeout.\n");
    }
    struct event_base *base = bufferevent_get_base(bev);
    event_base_loopexit(base, NULL);
}

void server_msg_cb(struct bufferevent *bev, void *arg) {
    char msg[1024];
    int len = bufferevent_read(bev, msg, sizeof(msg));
    msg[len] = '\0';
    printf("Recv %d bytes messsage from server:\n%s\n", len, msg);
    
    // bufferevent_free(bev);
    // struct event_base *base = bufferevent_get_base(bev);
    // event_base_loopexit(base, NULL);
}

static void timer_cb(evutil_socket_t fd, short events, void *arg) {
    struct bufferevent *conn = (struct bufferevent *)arg;
    char mesg[1024];
    memset(mesg, 0, sizeof(mesg));
    sprintf(mesg, "%d", i++);

    struct evbuffer* output = bufferevent_get_output(conn);
    int len = 0;
    len = evbuffer_get_length(output);
    printf("output buffer has %d bytes left\n", len);

    bufferevent_write(conn, mesg, strlen(mesg));
}
