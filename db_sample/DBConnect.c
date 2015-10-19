#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>

#define DBHOST "localhost"
#define DBUSER "root"
#define DBPASS ""
#define DBNAME "keijiban"

/** DBの初期化
 *
 * @return conn 
 */
MYSQL *connectDB(){
    
  // DBの設定
  char *sql_serv  = DBHOST;
  char *user      = DBUSER;
  char *passwd    = DBPASS;
  char *db_name   = DBNAME;

  // コネクションの初期化
  MYSQL *conn     = NULL;

  // MySQLと接続
  conn = mysql_init(NULL);
  if( !mysql_real_connect(conn,sql_serv,user,passwd,db_name,0,NULL,0) ){
    // error
    exit(-1);
  }

  return conn;
}

/** SQL文の実行
 * @param conn 接続中のコネクション
 * @param sql 実行するSQL文
 *
 * @return conn
 */
MYSQL *excuteSQL(MYSQL *conn, char *sql){

  // 実行用文字列の初期化
  char sql_str[255];
  memset( &sql_str[0] , 0x00 , sizeof(sql_str) );

  // 実行用文字列へバインディング
  snprintf( &sql_str[0] , sizeof(sql_str)-1 , sql );

  // SQLの実行
  if( mysql_query( conn , &sql_str[0] ) ){
    // error
    mysql_close(conn);
    exit(-1);
  }

  return conn;
}

/** 返り値をハンドリングせず単純に実行
 * @param sql 実行したいsql
 *
 * @return 1
 */
int simpleExcuteSQL(char *sql){

  // mysql接続
  MYSQL *conn = connectDB();

  // クエリ実行
  excuteSQL(conn, sql);

  // 後片づけ
  mysql_close(conn);

  return 1;
}
