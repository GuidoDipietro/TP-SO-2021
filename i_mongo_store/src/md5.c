#include "../include/md5.h"

static char* md5gen(char* path) {
  char* cmd = malloc(6 + 2 + strlen(path));
  strcpy(cmd, "md5sum ");
  strcpy(cmd + 7, path);

  FILE* f = popen(cmd, "r");
  char* str = malloc(34);
  fgets(str, 33, f);
  str[33] = '\0';

  pclose(f);
  free(cmd);

  return str;
}

char* md5sum(void* content, uint64_t len) {
  FILE* f = fopen("temp.md5sum", "w");
  fwrite(content, len, 1, f);
  fclose(f);
  char* md5 = md5gen("temp.md5sum");
  remove("temp.md5sum");
  return md5;
}
