//#include "DBConnect.c"
//#include "dao.h"
#include "userDao.c"
/*------------------------ Article ----------------------------*/

// Articleの構造体
typedef struct{
  int id;
  char title[255];
  char body[255];
  char userName[50];
  char date[255];
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
  excuteSQL(conn, "select * from articles as a join users as u on a.user_id = u.id ORDER BY created_date DESC");

  // レスポンス
  resp = mysql_use_result(conn);

  for (i=0; (row = mysql_fetch_row(resp)) != NULL; i++) {
    //printf( "%d : %s\n" , atoi(row[0]) , row[1] );
    articles[i].id = atoi(row[0]);
    strncpy(articles[i].title ,row[1], sizeof(articles[i].title));
    strncpy(articles[i].body ,row[2], sizeof(articles[i].body));
    strncpy(articles[i].date ,row[4], sizeof(articles[i].date));
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
void articleInsert(char *title, char *body, int userId){

    // 変数の初期化
    char sql[200];

    // トランザクション無いけど...
    // ユーザーの取得

    sprintf(sql, "INSERT INTO articles (title, body, user_id) VALUES ('%s', '%s', %d)", title, body, userId);
    simpleExcuteSQL(sql);
}
