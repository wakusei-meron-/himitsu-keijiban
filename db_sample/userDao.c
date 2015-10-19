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
  char email[200];
  char password[200];
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

/** emailからユーザー取得
 *
 * @param email
 *
 * @return 見つかれば1, 見つからないと0
 */
int userFindByemail(user_t *user, char *email){

  // 変数の初期化
  MYSQL_RES *resp = NULL;
  MYSQL_ROW row;
  char sql[200];

  // MySQLと接続
  MYSQL *conn = connectDB();

  // クエリ生成
  sprintf(sql, "SELECT * FROM users WHERE email = '%s' LIMIT 1", email);
  //printf(sql);

  // クエリ実行
  excuteSQL(conn, sql);

  // レスポンスを取り出す
  resp = mysql_use_result(conn);

  // 見つからなかったらreturn 0
  if ((row = mysql_fetch_row(resp)) == NULL) {
    printf("couldn't find user\n");
    return 0;
  }

  // 取り出したレスポンスを構造体Userにマッピング
  user->id = atoi(row[0]);
  strncpy(user->name, row[1], sizeof(user->name));
  strncpy(user->password, row[2], sizeof(user->password));
  strncpy(user->email, row[3], sizeof(user->email));
  //printf( "%d : %s, pass: %s\n" , user.id , user.name, user.password );
  
  // 後片づけ
  mysql_free_result(resp);
  mysql_close(conn);
  return 1;
}

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

/** ユーザーログイン
 * パスワードの長さが短いとパスワードが合っててもだめかも
 * 
 * @param email
 * @param password
 *
 * @param 成功=>1, 失敗=>0
 */
int userLogin(user_t *user, char *email, char *password){
  // パスワード用の変数の初期化
  unsigned char password_cript[200];

  if (userFindByemail(user, email)){
    compute_sha256(password, strlen(password), password_cript);
    //printf("input:%s, db:%s, comp: %d\n", 
    //        password_cript, user->password, strcmp(password_cript, user.password));
    if (!strncmp(password_cript,  user->password, 15)){
      return 1;
    }else{
      return 0;
    }
  }
  return 0;
}
