#include <openssl/sha.h>
#include <string.h>

/** 暗号化
 * @param src 元の文字列
 * @param src_len 元の文字列の長さ
 * @param 返還後の文字列
 */
int compute_sha256(char * src, int src_len, char * buffer) {
  const char salt[4] = "salt";
  //sprintf(src, "%s%s", src, salt);
  //strcat(src, salt);
  SHA256_CTX c;
  SHA256_Init(&c);
  SHA256_Update(&c, src, src_len);
  SHA256_Final(buffer, &c);
  return 0;
}

