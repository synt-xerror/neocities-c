#ifndef NEOCITIES_H
#define NEOCITIES_H

#include <stddef.h>

// -------------------------
// API functions
// -------------------------
int neocities_info(const char *sitename, char **out);
int neocities_list(const char *api_key, const char *path, char **out);
int neocities_delete(const char *api_key, const char **filenames, size_t count, char **response);
int neocities_upload(const char *api_key, const char **local_files, const char **remote_names, size_t count, char **response);
int neocities_apikey(const char *user, const char *pass, char **out);

#endif // NEOCITIES_H
