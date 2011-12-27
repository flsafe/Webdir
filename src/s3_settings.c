#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

  return sprintf(out_path_buff, "%s/%s", home_dir_path, SETTINGS_FILE);
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

  fclose(file);
}

static int settings_file_exists(){
  char settings_path[STR] = "";

  get_settings_file_path(settings_path);
  return access(settings_path, F_OK) == 0;
}

static int write_settings_file(char access_v[],
                               char secret_v[],
                               char host_v[],
                               char bucket_v[]){
  FILE *file;
  char path[STR] = "";

  get_settings_file_path(path);
  file = fopen(path, "w");
  if (!file) return 0;

  if (! fprintf(file, "%s %s\n", ACCESS_KEY, access_v))
    return 0;
  if (! fprintf(file, "%s %s\n", SECRET_KEY, secret_v))
    return 0;
  if (! fprintf(file, "%s %s\n", HOST, host_v)) 
    return 0;

  if (! fprintf(file, "%s %s\n", BUCKET, bucket_v))
    return 0;

  fclose(file);

  return 1;
}

static int create_default_settings_file(){
  return write_settings_file("<YourAccessKey>",
                             "<YourSecretKey>",
                             "<YourS3Host>",
                             "<YourDefaultBucket>");
}

int is_loaded_ok(){
  return (access_key[0] && secret_key[0] && host[0]) &&
         (access_key[0] != '<' && secret_key[0] != '<' && host[0] != '<');
}

int load_settings(){
  if (settings_file_exists()){
    read_settings_file();
  }
  else{
    create_default_settings_file();
    read_settings_file();
  }
    
  return is_loaded_ok();
}

void get_access_key(char key[]){
  memset(key, '\0', STR); 
  strncpy(key, access_key, STR); 
}

void get_secret_key(char key[]){
  memset(key, '\0', STR); 
  strncpy(key, secret_key, STR); 
}

void get_host(char key[]){
  memset(key, '\0', STR);
  strncpy(key, host, STR); 
}

void get_bucket(char key[]){
  memset(key, '\0', STR); 
  strncpy(key, bucket, STR); 
}

void set_bucket(char val[]){
  strncpy(bucket, val, STR);
  write_settings_file(access_key,
                      secret_key,
                      host,
                      bucket);
}

#ifdef TESTING

int main(){
  int load_settings_file(); 
  char key[STR];
 
  printf("stat %d\n", load_settings());

  get_access_key(key);
  printf("ACCESS_KEY %s\n", key);
  
  get_secret_key(key);
  printf("SECRET_KEY %s\n", key); 

  get_host(key);
  printf("HOST %s\n", key);

  set_bucket("changed_bucket");

  get_bucket(key);
  printf("BUCKET %s\n", key);

  return 1;
}

#endif
