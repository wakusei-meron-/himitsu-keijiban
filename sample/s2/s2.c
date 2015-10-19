#include <sys/fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "userDao.c"

#define HTTP_TCP_PORT 80

/*------------------------ Article ----------------------------*/

// Articleの構造体
typedef struct{
  int id;
  char title[255];
  char body[255];
  char userName[50];
} article_t;

/** 全ての投稿の取得
 *
 * @param users 取得した値を格納するための配列
 */
void articleFindAll(article_t *articles){

  // 変数の初期化
  int i;
  MYSQL_RES *resp = NULL;
  MYSQL_ROW row;

  // mysql接続
  MYSQL *conn = connectDB();

  // クエリ実行
  excuteSQL(conn, "select * from articles as a join users as u on a.user_id = u.id");

  // レスポンス
  resp = mysql_use_result(conn);

  for (i=0; (row = mysql_fetch_row(resp)) != NULL; i++) {
    //printf( "%d : %s\n" , atoi(row[0]) , row[1] );
    articles[i].id = atoi(row[0]);
    strncpy(articles[i].title ,row[1], sizeof(articles[i].title));
    strncpy(articles[i].body ,row[2], sizeof(articles[i].body));
    strncpy(articles[i].userName ,row[5], sizeof(articles[i].userName));
  }
  
  // 後片づけ
  mysql_free_result(resp);
  mysql_close(conn);
}

/** 投稿の新規作成
 *
 * @param title
 * @param body
 * @param userName
 *
 * @return
 */
void articleInsert(char *title, char *body, char *userName){

    // 変数の初期化
    char sql[200];

    // トランザクション無いけど...
    // ユーザーの取得

    sprintf(sql, "INSERT INTO users (name, passwd) VALUES ('%s', '%s')", title, body);
    simpleExcuteSQL(sql);
}

/*-------------------- テスト用main関数 -------------------------------*/
//int main(void){
//  
//  //int i;
//  //user_t users[100];
//  //article_t articles[100];
//
//  // ユーザーの追加
//  userInsert("shimotai", "passworddayo", "s@example.com");
//  userLogin("s@example.com", "passworddayo");
//
//  // ユーザー一覧の取得
//  //userFindAll(users);
//  //for(i=0; users[i].id == 0; i++){
//  //  printf( "%d : %s\n" , users[i].id , users[i].name );
//  //}
//  
//  // 記事一覧の取得
//  //articleFindAll(articles);
//  //for(i=0; articles[i].id == 0; i++){
//  //  printf( "id: %d,  title: %s, body: %s, userName: %s\n" ,
//  //      articles[i].id , articles[i].title, articles[i].body, articles[i].userName);
//  //}
//  return 0;
//}
// プロトタイプ
void http(int sockfd);
int send_msg(int fd, char *msg);

int find(char *str, int size, char *strref, int sizeref) {
    int i;
    int j;
    int equal_flag;

    for (i = 0; i < size; ++i) {
        equal_flag = 1;
        for (j = 0; j < sizeref; ++j) {
            if (str[i+j] != strref[j]) {
                equal_flag = 0;
                break;
            }
        }
        if (equal_flag) {
            return i;
        }
    }
    return -1;
}

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


        // uri_addrで得られたパスから1文字進める（つまり先頭の'/'を無視する
        uri_file = uri_addr + 1;

        // HTTPヘッダを送信
        send_msg(sockfd, "HTTP/1.0 200 OK\r\n");
        send_msg(sockfd, "text/html\r\n");
        send_msg(sockfd, "\r\n");


        //Referer: 
        int i;
        int ref_index = 0;
        for (i=0; i < sizeof(buf) - 8; ++i) {
            if (buf[i] == 'R' && buf[i+1] == 'e' && buf[i+2] == 'f') {
                ref_index = i;
            }
        }
        int ref_index_end = 0;
        for (i = ref_index + 9; i < sizeof(buf); ++i) {
            if (buf[i] == '\r' && buf[i+1] == '\n') {
                ref_index_end = i;
                break;
            }
        }
        char ref[1024];
        strncpy(ref, buf + ref_index + 9, ref_index_end - ref_index - 9);
        printf("\n\nref = %s\n\n", ref);

        // check name, email, passwd
        // /register?name=&email=&passwd=
        char tmpreg[1024];
        strncpy(tmpreg, ref + 18, 9);
        if (strcmp(tmpreg, "/register") == 0) {
            send_msg(sockfd, "<html><title>keiji-ban</title><body>");
            send_msg(sockfd, "<h1>welcome to register page!!</h1>");
            send_msg(sockfd, "<form action=\"http://10.160.33.5/req-register\">");
            send_msg(sockfd, "<input type=\"text\" name=\"name\">");
            send_msg(sockfd, "<input type=\"text\" name=\"email\">");
            send_msg(sockfd, "<input type=\"text\" name=\"passwd\">");
            send_msg(sockfd, "<button type=\"submit\">submit</button>");
            send_msg(sockfd, "</form>");
            send_msg(sockfd, "</body></html>");
        }

        char tmpr[1024];
        strncpy(tmpr, ref + 18, 13);
        if (find(ref, sizeof(ref), "/req-register", 13) != -1) {
            int i_name = find(ref, sizeof(ref), "name=", 5);
            int i_email = find(ref, sizeof(ref), "email=", 6);
            int i_passwd = find(ref, sizeof(ref), "passwd=", 7);
            char tempr_name[1024];
            char tempr_email[1024];
            char tempr_passwd[1024];
            //printf("%s\n", ref)
            int i_name_end = find(ref, sizeof(ref), "&email=", 7);
            int i_email_end = find(ref, sizeof(ref), "&passwd=", 8);
            strncpy(tempr_name, ref + i_name + 5, i_name_end - (i_name + 5));
            strncpy(tempr_email, ref + i_email + 6, i_email_end - (i_email + 6));
            strncpy(tempr_passwd, ref + i_passwd + 7, find(ref, sizeof(ref), "\0", 1) - i_passwd);

            userInsert(tempr_name, tempr_passwd, tempr_email);
            printf("name = %s, passwd = %s, email = %s\n", tempr_name, tempr_passwd, tempr_email);

            send_msg(sockfd, "<html><title>keiji-ban</title><body>");
            send_msg(sockfd, "<h1>done.</h1>");
            send_msg(sockfd, "</body></html>");
        }

        // /submit?title=&name=&content=
        char tmpsub[1024];
        strncpy(tmpsub, ref + 18, 7);
        if (strcmp(tmpsub, "/submit") == 0) {
            send_msg(sockfd, "<html><title>keiji-ban</title><body>");
            send_msg(sockfd, "<h1>welcome to submit page!!</h1>");
            send_msg(sockfd, "</body></html>");
            send_msg(sockfd, "<form action=\"http://10.160.33.5/req-submit\">");
            send_msg(sockfd, "<input type=\"text\" name=\"name\">");
            send_msg(sockfd, "<input type=\"text\" name=\"content\">");
            send_msg(sockfd, "<button type=\"submit\">submit</button>");
            send_msg(sockfd, "</form>");
        }
       
        // req-submit?name=&content=
        if (find(ref, sizeof(ref), "/req-submit", 11) != -1) {
            int i_name = find(ref, sizeof(ref), "name=", 5);
            int i_content = find(ref, sizeof(ref), "email=", 6);
            char tempr_name[102];
            char tempr_email[1024];
            strncpy(tempr_name, ref + i_name + 5, i_content - i_name - 1);
            strncpy(tempr_email, ref + i_content + 7, find(ref, sizeof(ref), "\0", 1) - i_content);

            send_msg(sockfd, "<html><title>keiji-ban</title><body>");
            send_msg(sockfd, "<h1>done.</h1>");
            send_msg(sockfd, "</body></html>");
        }

        // /list
        char tmpshow[1024];
        strncpy(tmpshow, ref + 18, 5);
        if (strcmp(tmpshow, "/show") == 0) {
            send_msg(sockfd, "<html><title>keiji-ban</title><body>");
            send_msg(sockfd, "contents here<br>");
            send_msg(sockfd, "</body></html>");
        }

        // delete buf
        memset(&buf[0], 0, sizeof(buf));

        // `GET`メソッドのみ受け付ける
        if (strcmp(method, "GET") != 0) {
            send_msg(sockfd, "501 Not implemented.");
            close(read_fd);
        }



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
