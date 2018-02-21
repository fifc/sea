//
//  sms.cpp
//  短信http接口的c/c++代码调用示例
//  本DEMO仅作参考
//
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h>
#include <unistd.h>

#define SA struct sockaddr
#define MAXLINE 4096
#define MAXSUB  2000
#define MAXPARAM 2048

extern int h_errno;

int sockfd;
char *hostname = "222.73.117.158";
char *send_sms_uri = "/msg/QueryBalance";
char *query_balance_uri = "/msg/HttpBatchSendSM";

/**
* 发http post请求
*/
ssize_t http_post(char *page, char *poststr)
{
    char sendline[MAXLINE + 1], recvline[MAXLINE + 1];
    ssize_t n;
    snprintf(sendline, MAXSUB,
        "POST %s HTTP/1.0\r\n"
        "Host: %s\r\n"
        "Content-type: application/x-www-form-urlencoded\r\n"
        "Content-length: %lu\r\n\r\n"
        "%s", page, hostname, strlen(poststr), poststr);

    write(sockfd, sendline, strlen(sendline));
    printf("[%s]\n", page);
    while ((n = read(sockfd, recvline, MAXLINE)) > 0) {
        recvline[n] = '\0';
        printf("%s", recvline);
    }
    printf("\n");
    return n;
}

/**
* 查账户余额
*/
ssize_t get_balance(char *account, char *password)
{
    char params[MAXPARAM + 1];
    sprintf(params, "account=%s&pswd=%s", account, password);
    return http_post(query_balance_uri, params);
}

/**
* 发送短信
*/
ssize_t send_sms(char *account, char *password, char *mobile, char *msg)
{
    char params[MAXPARAM + 1];
    char *cp = params;
    sprintf(cp,"account=%s&pswd=%s&mobile=%s&msg=%s&needstatus=false&product=&extno=", account, password, mobile, msg);
    printf("[%s]\n", cp);
    return http_post(send_sms_uri, cp);
}

int main(void)
{
    struct sockaddr_in servaddr;

    //建立socket连接
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    //servaddr.sin_addr = hostname;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(80);
    inet_pton(AF_INET, hostname, &servaddr.sin_addr);

    connect(sockfd, (SA *) & servaddr, sizeof(servaddr));

    //修改为您的创蓝账号
    char *account = "jiekou-clcs-15";

	//修改为您的创蓝密码
    char *password = "Tch987654";

    //修改为您要发送的手机号
    char *mobile = "18603000506";

    //设置您要发送的内容
    char *message = "【创蓝文化】您的验证码是1234";

    /**************** 查询余额 *****************/
    get_balance(account, password);

    /**************** 发送短信 *****************/
    send_sms(account, password, mobile, message);

    close(sockfd);
    exit(0);
}
