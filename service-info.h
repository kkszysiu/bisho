typedef enum {
  AUTH_INVALID = 0,
  AUTH_USERNAME,
  AUTH_USERNAME_PASSWORD
} ServiceAuthType;

typedef struct {
  char *name;
  char *display_name;
  char *description;
  char *link;
  ServiceAuthType auth;
  char *icon;
} ServiceInfo;

ServiceInfo * get_info_for_service (const char *name);
