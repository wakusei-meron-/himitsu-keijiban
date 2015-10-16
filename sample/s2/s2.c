#include <sys/fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define HTTP_TCP_PORT 80

// プロトタイプ
void http(int sockfd);
int send_msg(int fd, char *msg);


int main() {

    printf("Waiting for request...\n");

    // ソケット用
    int sockfd;

    // accept時に新規生成されるソケット用
    int new_sockfd;

    // `writer_addr`構造体の長さを受け取る
    int writer_len;

    // 待ち受け用アドレス構造体
    struct sockaddr_in reader_addr;

    // 送信用アドレス構造体
    struct sockaddr_in writer_addr;

    // 構造体をゼロクリア
    memset(&reader_addr, 0, sizeof(reader_addr));
    memset(&writer_addr, 0, sizeof(writer_addr));

    // 読み込み用の構造体を設定
    reader_addr.sin_family      = AF_INET;
    reader_addr.sin_addr.s_addr = htonl(INADDR_ANY); // INADDR_ANY = 0.0.0.0 = 誰からでも待ち受け
    reader_addr.sin_port        = htons(HTTP_TCP_PORT);

    // 待ち受け用にソケットを開く
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "error: socket()\n");
        return 1;
    }

    // 待ち受け用としてソケットをバインド
    if (bind(sockfd, (struct sockaddr *)&reader_addr, sizeof(reader_addr)) < 0) {
        fprintf(stderr, "error: bind()\n");
        fprintf(stderr, "%s", strerror(errno));
        close(sockfd);
        return 1;
    }

    // 待ち受け用ソケットで待ち受け（接続待ち・リッスン）
    if (listen(sockfd, 5) < 0) {
        fprintf(stderr, "error: listen()\n");
        close(sockfd);
        return 1;
    }

    // 接続を待ち続ける
    while(1) {
        // 開いたソケットへの接続要求が来たら`accept()`により、その接続の新規ソケットディスクリプタが生成され返される。
        // ※ `listen()`しているソケットはキューが作られ、そのキューの先頭から`accept()`は接続相手の新規ソケットディスクリプタを生成する。
        if ((new_sockfd = accept(sockfd, (struct sockaddr *)&writer_addr, (socklen_t *)&writer_len)) < 0) {
            fprintf(stderr, "error: accepting a socket.\n");
            break;
        }
        else {
            // acceptから得られたソケットディスクリプタを使ってHTTP通信開始
            http(new_sockfd);

            // 通信が終わったらソケットを閉じる。
            // ※ 新規で作られたソケットなのでlistenしているソケットには影響しない。
            close(new_sockfd);
        }
    }

    close(sockfd);
}


/**
 * HTTP通信用関数
 *
 * @param sockfd 通信用に確立されたソケットディスクリプタ
 */
void http(int sockfd) {

    // ファイル読み込み時の読み取りバイトサイズ格納用
    int len;

    // 要求されたファイルを開くためのファイルディスクリプタ
    int read_fd;

    // ソケットからデータを読み取るバッファ
    char buf[1024];

    // メソッド名を格納する
    char method[16];

    // 接続相手のアドレスを格納する
    char uri_addr[256];

    // HTTP Versionを格納する
    char http_ver[64];

    // 接続要求のあったファイル名を格納する
    char *uri_file;

    // HTTPリクエストを読み込み開始
    if (read(sockfd, buf, 1024) <= 0) {
        fprintf(stderr, "error: reading a request.\n");
    }
    else {
        // Method, Path, HTTP-versionに分解
        printf("\n\n--------\n%s\n--------\n\n", buf);
        sscanf(buf, "%s %s %s", method, uri_addr, http_ver);
        memset(&buf[0], 0, sizeof(buf));

        // `GET`メソッドのみ受け付ける
        if (strcmp(method, "GET") != 0) {
            send_msg(sockfd, "501 Not implemented.");
            close(read_fd);
        }

        // uri_addrで得られたパスから1文字進める（つまり先頭の'/'を無視する
        uri_file = uri_addr + 1;

        // HTTPヘッダを送信
        send_msg(sockfd, "HTTP/1.0 200 OK\r\n");
        send_msg(sockfd, "text/html\r\n");
        send_msg(sockfd, "\r\n");

        send_msg(sockfd, "<html><title>keiji-ban</title><body>");
        send_msg(sockfd, "<h1>welcome to keiji-ban!</h1>");
        send_msg(sockfd, "<form action=\"http://www.google.com\">");
        send_msg(sockfd, "<input type=\"text\">");
        send_msg(sockfd, "<button type=\"submit\">submit</button>");
        send_msg(sockfd, "</form>");
        send_msg(sockfd, "</body></html>");

/*
<form action="http://www.futomi.com/lecture/form/exec/gettest.cgi" method="get">
    <p>お名前：<input type="text" name="namae" value="太郎" size="20" /></p>
    <p>OS：
    <input type="radio" name="OS" value="win" checked="checked" /> Windows 
    <input type="radio" name="OS" value="mac" /> Machintosh 
    <input type="radio" name="OS" value="unix" /> Unix
    </p>
    <p><input type="submit" name="submit" value="送信" /></p>
    </form>
*/
        
        close(read_fd);
    }
}


/**
 * メッセージ送信用関数
 *
 * @param fd   ソケットディスクリプタ
 * @param *msg 送信するメッセージ
 */
int send_msg(int fd, char *msg) {
    int len;
    len = strlen(msg);

    // 指定されたメッセージ`msg`をソケットに送信
    if (write(fd, msg, len) != len) {
        fprintf(stderr, "error: writing.");
    }

    return len;
}
