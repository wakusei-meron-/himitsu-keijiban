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
  snprintf( &sql_str[0] , sizeof(sql_str)-1 , "select * from users" );
  if( mysql_query( conn , &sql_str[0] ) ){
    // error
    mysql_close(conn);
    exit(-1);
  }
  return conn;
}

typedef struct{
  int id;
  char *name;
} user_t;

void userFindAll(user_t users[]){
  MYSQL_RES *resp = NULL;
  MYSQL_ROW row;

  // mysql接続
  MYSQL *conn = connectDB();

  // クエリ実行
  excuteSQL(conn, "select * from users");

  // レスポンス
  //struct User users[100]; // 100人以上はしらない...
  resp = mysql_use_result(conn);
  int i;

  for (i=0; (row = mysql_fetch_row(resp)) != NULL; ) {
    //printf( "%d : %s\n" , atoi(row[0]) , row[1] );
    users[i].id = atoi(row[0]);
    users[i].name = row[1];
  }
  

  // 後片づけ
  mysql_free_result(resp);
  mysql_close(conn);
}

int main(void){
  user_t users[100];
  userFindAll(users);
  int count = sizeof(users);
  int i;
  for (i=0; i<count; i++){
    printf( "%d : %s\n" , users[i].id , users[i].name );
  }
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
