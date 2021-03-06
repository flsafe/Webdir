#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <libs3.h>
#include "s3_settings.h"

#define STR 128
#define TXT 4096

#define SLEEP_UNITS_PER_SECOND 1

int RequestStatus = 0;
int RequestRetries = 5;
char RequestErrDetails[TXT] = { 0 };

static void s3_init(){
    S3Status status;
		char host[STR] = "";

		get_host(host);
    
    if ((status = S3_initialize("s3", S3_INIT_ALL, host))
        != S3StatusOK) {
        fprintf(stderr, "Failed to initialize libs3: %s\n", 
                S3_get_status_name(status));
        exit(-1);
    }
}

static int should_retry(){
    if (RequestRetries--) {
        static int interval = 1 * SLEEP_UNITS_PER_SECOND;
        sleep(interval);
        // Next sleep 1 second longer
        interval++;
        return 1;
    }
    return 0;
}

static void print_error_details(){
		fprintf(stderr, "\nERROR: %s\n", S3_get_status_name(RequestStatus));
		fprintf(stderr, "%s\n", RequestErrDetails);
}

static void gather_error_details(const S3ErrorDetails *error){
    // Compose the error details message now, although we might not use it.
    // Can't just save a pointer to [error] since it's not guaranteed to last
    // beyond this callback
    int len = 0;
    if (error && error->message) {
        len += snprintf(&(RequestErrDetails[len]), 
                        sizeof(RequestErrDetails) - len,
                        "  Message: %s\n", error->message);
    }
    if (error && error->resource) {
        len += snprintf(&(RequestErrDetails[len]), 
                        sizeof(RequestErrDetails) - len,
                        "  Resource: %s\n", error->resource);
    }
    if (error && error->furtherDetails) {
        len += snprintf(&(RequestErrDetails[len]), 
                        sizeof(RequestErrDetails) - len,
                        "  Further Details: %s\n", error->furtherDetails);
    }
    if (error && error->extraDetailsCount) {
        len += snprintf(&(RequestErrDetails[len]), 
                        sizeof(RequestErrDetails) - len,
                        "%s", "  Extra Details:\n");
        int i;
        for (i = 0; i < error->extraDetailsCount; i++) {
            len += snprintf(&(RequestErrDetails[len]), 
                            sizeof(RequestErrDetails) - len, 
                            "    %s: %s\n", 
                            error->extraDetails[i].name,
                            error->extraDetails[i].value);
        }
    }
}

static void res_complete(S3Status status,
                         const S3ErrorDetails *error, 
                         void *callbackData){
    RequestStatus = status;
		gather_error_details(error);
}

static S3Status res_properties(const S3ResponseProperties *properties, 
                               void *callbackData){
  return S3StatusOK;
}

static void print_error_message(char *bname){
    const char *message = 0;
		int ok = 0;

    switch (RequestStatus) {
    case S3StatusOK:
				ok = 1;
        break;
    case S3StatusErrorNoSuchBucket:
        message = "Does Not Exist";
        break;
    case S3StatusErrorAccessDenied:
        message = "Access Denied";
        break;
    default:
        message = 0;
        break;
    }

    if (!ok){
        printf("%s %s\n", bname, message);
    }
}

static int bucket_exists(char *bname){

    s3_init();

    if (! load_settings()){
			printf("Configure your ~/.webdir-settings file!\n");
			exit(1);
		}

    char skey[STR];
    get_secret_key(skey);
    char akey[STR];
    get_access_key(akey);
		char hostname[STR];
		get_host(hostname);

    S3ResponseHandler res_handler = {
        &res_properties, &res_complete/* Can properties be null?*/
    };

    char loc_constraint[STR];
    do {
        S3_test_bucket(S3ProtocolHTTPS, 
                       S3UriStylePath, 
                       akey, 
                       skey,
                       hostname, 
                       bname, 
                       sizeof(loc_constraint),
                       loc_constraint, 
                       0, 
                       &res_handler, 
                       0);
    } while (S3_status_is_retryable(RequestStatus) && should_retry());


    S3_deinitialize();

		return RequestStatus == S3StatusOK; // Bucket exists
}

int main(int argc, char **argv){
  if (argc != 2) return 1;

	if (!bucket_exists(argv[1])){
		print_error_details();
		print_error_message(argv[1]);
	}

	return 0;
}
