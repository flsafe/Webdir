#ifndef S3_SETTINGS_H
#define S3_SETTINGS_H

void load_settings();

char *get_access_key();
char *get_secret_key();
char *get_host();
char *get_bucket();

#endif
