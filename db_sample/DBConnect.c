#include <stdio.h>
#include <stdlib.h>
#include <mysql/mysql.h>

#define DBHOST "localhost"
#define DBUSER "myuser"
#define DBPASS "mypass"
#define DBNAME "mydb"

int main()
{

       MYSQL *conn;
       MYSQL_RES *res;
       MYSQL_ROW row;

       // 接続
       conn = mysql_init(NULL);
       if (!mysql_real_connect(conn, DBHOST, DBUSER, DBPASS, DBNAME, 3306, NULL, 0)) {
          fprintf(stderr, "%s\n", mysql_error(conn));
          exit(1);
       }

       // クエリ発行
            if (mysql_query(conn, "show tables")) {
                fprintf(stderr, "%s\n", mysql_error(conn));
                exit(1);
            }
            res = mysql_use_result(conn);

       // 結果表示
        while ((row = mysql_fetch_row(res)) != NULL) {
            printf("%s\n", row[0]);
        }
       mysql_free_result(res);

       // 切断
       mysql_close(conn);

       return 0;
}
