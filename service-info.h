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
      char *server;
      char *consumer_key;
      char *consumer_secret;
      char *access_token_url;
      char *authorize_url;
      char *request_token_url;
      char *callback;
    } oauth;
  };
} ServiceInfo;

ServiceInfo * get_info_for_service (const char *name);

#endif /* _SERVICE_INFO_H */
