#ifndef S3_SETTINGS_H
#define S3_SETTINGS_H

void load_settings();

void get_access_key(char[]);
void get_secret_key(char[]);
void get_host(char[]);
void get_bucket(char[]);
int  set_bucket(char[]);

#endif
