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

//int login(char *email, char *password){
//
//  userFindByPass()
//}
