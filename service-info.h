#ifndef _SERVICE_INFO_H
#define _SERVICE_INFO_H

typedef enum {
  AUTH_INVALID = 0,
  AUTH_USERNAME,
  AUTH_USERNAME_PASSWORD,
  AUTH_OAUTH
} ServiceAuthType;

typedef struct {
  char *name;
  char *display_name;
  char *description;
  char *link;
  ServiceAuthType auth;
  char *icon;
  union {
    struct {
      char *consumer_key;
      char *consumer_secret;
      char *base_url;
      char *request_token_function;
      char *authorize_function;
      char *access_token_function;
      char *callback;
    } oauth;
  };
} ServiceInfo;

ServiceInfo * get_info_for_service (const char *name);

#endif /* _SERVICE_INFO_H */
