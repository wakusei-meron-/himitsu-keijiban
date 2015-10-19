#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "userDao.c"
#include "articleDao.c"

/*-------------------- テスト用main関数 -------------------------------*/

#define SUCCESS 1
#define FAILURE 0

int main(void){
  
  int i;
  user_t users[100];
  article_t articles[100];

  // アカウントの登録
  userInsert("shimotai", "passworddayo", "s@example.com");
  if (userInsert("shimotai", "passworddayo", "sa@example.com") == SUCCESS){ 
    printf("create user\n");
  }else {
    // 重複していた場合
    printf("couldn't create user\n");
  }
  
  // ログイン
  user_t user;
  if (userLogin(&user, "s@example.com", "passworddayo")){
      printf("nice login\n id: %d, name: %s, email: %s\n",
        user.id, user.name, user.email);
  }else{
    printf("Couldn't login\n");
  }


  // ユーザー一覧の取得
  userFindAll(users);
  for(i=0; users[i].id == 0; i++){
    printf( "%d : %s\n" , users[i].id , users[i].name );
  }
  
  // 記事の追加
  //articleInsert("title", "contents", 1);
  // 記事一覧の取得
  articleFindAll(articles);
  for(i=0; articles[i].id != 0; i++){
    printf( "id: %d,  title: %s, body: %s, userName: %s, date: %s\n" ,
      articles[i].id , articles[i].title, articles[i].body, articles[i].userName, articles[i].date);
  }
  return 0;
}
