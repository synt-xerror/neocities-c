#ifndef NEOCITIES_H
#define NEOCITIES_H

#include <stddef.h>

// -------------------------
// Output structs
// -------------------------
typedef struct {
    char *sitename;
    int hits;
    char *created_at;
    char *last_updated;
    char *domain;
    char **tags;
    size_t tag_count;
} neocities_info_t;

typedef struct {
    char **paths;
    size_t count;
} neocities_filelist_t;

// -------------------------
// API functions
// -------------------------
int neocities_info(const char *sitename, neocities_info_t *out);
int neocities_list(const char *user, const char *pass, const char *path, neocities_filelist_t *out);
int neocities_upload(const char *user, const char *pass, const char **local_files, const char **remote_names, size_t count, char **response);
int neocities_delete(const char *user, const char *pass, const char **filenames, size_t count, char **response);
int neocities_get_api_key(const char *user, const char *pass, char **api_key);

// -------------------------
// Free functions
// -------------------------
void neocities_free_info(neocities_info_t *info);
void neocities_free_filelist(neocities_filelist_t *list);

#endif // NEOCITIES_H