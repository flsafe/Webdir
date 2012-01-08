#include <stdlib.h>
#include <stdio.h>
#include "s3_settings.h"

#define MAX_STR 128

int main(){
  char bucket[MAX_STR]; 

  if (! load_settings()){
    fprintf(stderr, "Can't load your settings file.\n");
    exit(1);
  }
  
  get_bucket(bucket);
  printf("%s\n", bucket);

  return 0;
}
