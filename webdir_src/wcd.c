#include <stdio.h>
#include "libs3.h"
#include "s3_settings.h"
#include "unistd.h"

#define STR 64
#define TXT 4096

#define SLEEP_UNITS_PER_SECOND 1

int RequestStatus = 0;
int RequestRetries = 5;
char RequestErrDetails[TXT] = "";

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

static void printError()
{
    if (RequestStatus < S3StatusErrorAccessDenied) {
        fprintf(stderr, "\nERROR: %s\n", S3_get_status_name(RequestStatus));
    }
    else {
        fprintf(stderr, "\nERROR: %s\n", S3_get_status_name(RequestStatus));
        fprintf(stderr, "%s\n", RequestErrDetails);
    }
}

static void res_complete(S3Status status,
                         const S3ErrorDetails *error, 
                         void *callbackData){
    (void) callbackData;

    RequestStatus = status;
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

static S3Status res_properties(const S3ResponseProperties *properties, 
                                    void *callbackData)
{
  return S3StatusOK;
}

static void test_bucket(char *bname){

    S3_init();
    load_settings();

    char skey[STR];
    get_secret_key(skey);
    char akey[STR];
    get_access_key(akey);

    S3ResponseHandler res_handler = {
        &res_properties, &res_complete/* Can properties be null?*/
    };

    char loc_constraint[64];
    do {
        S3_test_bucket(S3ProtocolHTTPS, 
                       S3UriStyle, 
                       akey, 
                       skey,
                       0, 
                       bname, 
                       sizeof(loc_constraint),
                       loc_constraint, 
                       0, 
                       &res_handler, 
                       0);
    } while (S3_status_is_retryable(RequestStatus) && should_retry());

    const char *result = 0;

    switch (RequestStatus) {
    case S3StatusOK:
        // bucket exists
        result = loc_constraint[0] ? loc_constraint: "USA";
        break;
    case S3StatusErrorNoSuchBucket:
        result = "Does Not Exist";
        break;
    case S3StatusErrorAccessDenied:
        result = "Access Denied";
        break;
    default:
        result = 0;
        break;
    }

    if (result) {
        printf("%-56s  %-20s\n", "                         Bucket",
               "       Status");
        printf("--------------------------------------------------------  "
               "--------------------\n");
        printf("%-56s  %-20s\n", bname, result);
    }
    else {
        printError();
    }

    S3_deinitialize();
}

int main(int argc, char **argv){
  if (argc == 2){
    test_bucket(argv[1]);
  }
}
