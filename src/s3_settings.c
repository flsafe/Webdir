#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STR 128

static char SETTINGS_FILE[STR] = ".webdir-settings";

static char ACCESS_KEY[STR] = "ACCESS_KEY";
static char access_key[STR] = "";

static char SECRET_KEY[STR] = "SECRET_KEY";
static char secret_key[STR] = "";

static char HOST[STR] = "HOST";
static char host[STR] = "";

static char BUCKET[STR] = "BUCKET"; 
static char bucket[STR] = "";


static int get_settings_file_path(char *out_path_buff){
  char *home_dir_path; 

  home_dir_path = getenv("HOME");
  if (NULL == home_dir_path)
    return 0;

  return sprintf(out_path_buff, "%128s/%128s", home_dir_path, SETTINGS_FILE);
}

static void set_key_val(char *key, char *val){
  if (0 == strncmp(ACCESS_KEY, key, STR))
    strncpy(access_key, val, STR);
  else if (0 == strncmp(SECRET_KEY, key, STR))
    strncpy(secret_key, val, STR);
  else if (0 == strncmp(HOST, key, STR))
    strncpy(host, val, STR);
  else if (0 == strncmp(BUCKET, key, STR))
    strncpy(bucket, val, STR);
}

static void read_settings_file(){
  FILE *file;
  char settings_path[STR] = "", setting_key[STR] = "", setting_val[STR] = "";

  get_settings_file_path(settings_path);
  file = fopen(settings_path, "r");
  if (!file) return;
   
  while (EOF != fscanf(file, "%128s %128s", setting_key, setting_val))
    set_key_val(setting_key, setting_val);
}

int load_settings(){
  int is_loaded_ok();

  read_settings_file();
  return is_loaded_ok();
}

int is_loaded_ok(){
  return (access_key[0] && secret_key[0] && host[0]);
}

char *get_access_key(){
  return access_key;
}

char *get_secret_key(){
  return secret_key;
}

char *get_host(){
  return host;
}

char *get_bucket(){
  return bucket;
}

#ifdef TESTING

int main(){
  int load_settings_file(); 
 
  printf("stat %d\n", load_settings());
  printf("ACCESS_KEY %s\n", get_access_key());
  printf("SECRET_KEY %s\n", get_secret_key());
  printf("HOST %s\n", get_host());
  printf("BUCKET %s\n", get_bucket());

  return 1;
}

#endif
