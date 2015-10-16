#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>

MYSQL *connectDB(){
    
  char *sql_serv  = "localhost";
  char *user      = "root";
  char *passwd    = "";
  char *db_name   = "keijiban";
  MYSQL *conn     = NULL;
  // mysql接続
  conn = mysql_init(NULL);
  if( !mysql_real_connect(conn,sql_serv,user,passwd,db_name,0,NULL,0) ){
    // error
    exit(-1);
  }
  return conn;
}

MYSQL *excuteSQL(MYSQL *conn, char *sql){
  char sql_str[255];
  memset( &sql_str[0] , 0x00 , sizeof(sql_str) );
  snprintf( &sql_str[0] , sizeof(sql_str)-1 , sql );
  if( mysql_query( conn , &sql_str[0] ) ){
    // error
    mysql_close(conn);
    exit(-1);
  }
  return conn;
}

void simpleExcuteSQL(char *sql){
  MYSQL_RES *resp = NULL;

  // mysql接続
  MYSQL *conn = connectDB();

  // クエリ実行
  excuteSQL(conn, sql);

  // 後片づけ
  mysql_free_result(resp);
  mysql_close(conn);
}

typedef struct{
  int id;
  char *name;
} user_t;

typedef struct{
  int id;
  char *title;
  char *body;
  char *userName;
} article_t;



#define USER

void userFindAll(user_t *users){
  MYSQL_RES *resp = NULL;
  MYSQL_ROW row;

  // mysql接続
  MYSQL *conn = connectDB();

  // クエリ実行
  excuteSQL(conn, "select * from users");

  // レスポンス
  resp = mysql_use_result(conn);
  int i;

  for (i=0; (row = mysql_fetch_row(resp)) != NULL; ) {
    //printf( "%d : %s\n" , atoi(row[0]) , row[1] );
    users[i].id = atoi(row[0]);
    users[i].name = row[1];
    printf( "%d : %s\n" , users[i].id , users[i].name );
  }
  
  // 後片づけ
  mysql_free_result(resp);
  mysql_close(conn);
}

void userInsert(char *name, char *passwd){

    char sql[200];
    sprintf(sql, "INSERT INTO users (name, passwd) VALUES ('%s', '%s')", name, passwd);
    simpleExcuteSQL(sql);
}

#define Article

void articleFindAll(article_t *articles){
  MYSQL_RES *resp = NULL;
  MYSQL_ROW row;

  // mysql接続
  MYSQL *conn = connectDB();

  // クエリ実行
  excuteSQL(conn, "select * from articles as a join users as u on a.user_id = u.id");

  // レスポンス
  resp = mysql_use_result(conn);
  int i;

  for (i=0; (row = mysql_fetch_row(resp)) != NULL; ) {
    //printf( "%d : %s\n" , atoi(row[0]) , row[1] );
    articles[i].id = atoi(row[0]);
    articles[i].title = row[1];
    articles[i].body = row[2];
    articles[i].userName = row[5];
    printf( "id: %d,  title: %s, body: %s, userName: %s\n" ,
            articles[i].id , articles[i].title, articles[i].body, articles[i].userName);
  }
  
  // 後片づけ
  mysql_free_result(resp);
  mysql_close(conn);
}

int main(void){
  user_t users[100];
  article_t articles[100];
  userFindAll(users);
  articleFindAll(articles);
  userInsert("shimotai", "passworddesu");
  //MYSQL_RES *resp = NULL;
  //MYSQL_ROW row;

  //// mysql接続
  //MYSQL *conn = connectDB();

  //// クエリ実行
  //excuteSQL(conn, "select * from users");

  //// レスポンス
  //resp = mysql_use_result(conn);
  //while((row = mysql_fetch_row(resp)) != NULL ){
  //  printf( "%d : %s\n" , atoi(row[0]) , row[1] );
  //}

  //// 後片づけ
  //mysql_free_result(resp);
  //mysql_close(conn);
  return 0;
}
