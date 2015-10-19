#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "DBConnect.c"
#include "userDao.c"


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
  
  //int i;
  //user_t users[100];
  //article_t articles[100];

  // ユーザーの追加
  userInsert("shimotai", "passworddayo", "s@example.com");
  userLogin("s@example.com", "passworddayo");

  // ユーザー一覧の取得
  //userFindAll(users);
  //for(i=0; users[i].id == 0; i++){
  //  printf( "%d : %s\n" , users[i].id , users[i].name );
  //}
  
  // 記事一覧の取得
  //articleFindAll(articles);
  //for(i=0; articles[i].id == 0; i++){
  //  printf( "id: %d,  title: %s, body: %s, userName: %s\n" ,
  //      articles[i].id , articles[i].title, articles[i].body, articles[i].userName);
  //}
  return 0;
}
