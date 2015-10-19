#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DBConnect.c"
#include "myCrypto.c"

/*------------------------ User ----------------------------*/

// userの構造体
typedef struct{
  int id;
  char name[50];
} user_t;

/** 全てのユーザー取得
 *
 * @param users 取得した値を格納するための配列
 */
void userFindAll(user_t *users){

  // 変数の初期化
  int i;
  MYSQL_RES *resp = NULL;
  MYSQL_ROW row;

  // MySQLと接続
  MYSQL *conn = connectDB();

  // クエリ実行
  excuteSQL(conn, "select * from users");

  // レスポンスを取り出す
  resp = mysql_use_result(conn);

  // 取り出したレスポンスを構造体Userにマッピング
  for (i=0; (row = mysql_fetch_row(resp)) != NULL; i++) {
    users[i].id = atoi(row[0]);
    strncpy(users[i].name, row[1], sizeof(users[i].name));
    printf( "%d : %s\n" , users[i].id , users[i].name );
  }
  
  // 後片づけ
  mysql_free_result(resp);
  mysql_close(conn);
}

/** idからユーザー取得
 *
 * @param id
 */
//user_t userById(int id){
//
//  // 変数の初期化
//  int i;
//  user_t user;
//  MYSQL_RES *resp = NULL;
//  MYSQL_ROW row;
//
//  // MySQLと接続
//  MYSQL *conn = connectDB();
//
//  // クエリ実行
//  excuteSQL(conn, "SELECT * FROM users WHERE id = id");
//
//  // レスポンスを取り出す
//  resp = mysql_use_result(conn);
//
//  // 取り出したレスポンスを構造体Userにマッピング
//  //for (i=0; (row = mysql_fetch_row(resp)) != NULL; i++) {
//  //  users[i].id = atoi(row[0]);
//  //  strncpy(users[i].name, row[1], sizeof(users[i].name));
//  //  printf( "%d : %s\n" , users[i].id , users[i].name );
//  //}
//  
//  // 後片づけ
//  mysql_free_result(resp);
//  mysql_close(conn);
//}

/** ユーザーの新規作成
 *
 * @param name
 * @param passwd
 * @param email
 *
 * @return
 */
void userInsert(char *name, char *passwd, char *email){

  unsigned char passwd_cript[200];
  compute_sha256(passwd, strlen(passwd), passwd_cript);
  char sql[200];
  sprintf(sql, "INSERT INTO users (name, passwd, email) VALUES ('%s', '%s', '%s')", name, passwd_cript, email);
    simpleExcuteSQL(sql);
}

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
int main(void){
  
  int i;
  user_t users[100];
  article_t articles[100];

  // ユーザーの追加
  userInsert("shimotai", "passworddayo", "sample@example.com");

  // ユーザー一覧の取得
  userFindAll(users);
  for(i=0; users[i].id == 0; i++){
    printf( "%d : %s\n" , users[i].id , users[i].name );
  }
  
  // 記事一覧の取得
  articleFindAll(articles);
  for(i=0; articles[i].id == 0; i++){
    printf( "id: %d,  title: %s, body: %s, userName: %s\n" ,
        articles[i].id , articles[i].title, articles[i].body, articles[i].userName);
  }
  return 0;
}
