#include "neocities.h"
#include <curl/curl.h>
#include <jansson.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

 // -------------------------
// HTTP responde buffer
// -------------------------
struct response {
    	char *data;
    	size_t len;
}; 

static size_t write_cb(
	void *ptr, 
	size_t size, 
	size_t nmemb, 
	void *userdata
)
{
    	struct response *r = (struct response*)userdata;
    	size_t chunk = size * nmemb;
    	char *tmp = realloc(r->data, r->len + chunk + 1);
    	if (!tmp) return 0;
    	r->data = tmp;
    	memcpy(r->data + r->len, ptr, chunk);
    	r->len += chunk;
    	r->data[r->len] = '\0';
    	return chunk;
}

// -------------------------
// HTTP helpers
// -------------------------
static int perform_request(
    	const char *url,
    	const char *api_key,
    	const char *post_fields,
    	struct response *resp,
    	curl_mime *mime
)
{
    	CURL *curl = curl_easy_init();
    	if (!curl) return 1;

    	resp->data = malloc(1);
    	resp->len = 0;
	resp->data[0] = '\0';

    	struct curl_slist *headers = NULL;
	
	if (api_key && strlen(api_key) > 0) {
		char auth_header[512];
	    	snprintf(auth_header, sizeof(auth_header),
				"Authorization: Bearer %s", api_key);
	    	headers = curl_slist_append(headers, auth_header);
	}

    	curl_easy_setopt(curl, CURLOPT_URL, url);
    	
	if (headers)
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    	
	/* SSL hardening */
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
    	
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 0L); // política
	
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    	curl_easy_setopt(curl, CURLOPT_WRITEDATA, resp);

	if (post_fields) {
    	    	curl_easy_setopt(curl, CURLOPT_POST, 1L);
        	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields);
    	}

    	if (mime)
    	    	curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);

    	CURLcode res = curl_easy_perform(curl);

    	curl_slist_free_all(headers);
    	curl_easy_cleanup(curl);

    	long http_code = 0;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

	if (res != CURLE_OK || http_code >= 400) {
	    	curl_slist_free_all(headers);
	    	curl_easy_cleanup(curl);
	    	free(resp->data);
	    	return 2;
	}

    	return 0;
}

// -------------------------
// JSON parsers
// -------------------------
void neocities_free_info(
	neocities_info_t *info
)
{
    	if (!info) return;
    	free(info->sitename);
    	free(info->created_at);
    	free(info->last_updated);
    	free(info->domain);
    	for (size_t i=0; i<info->tag_count; i++) free(info->tags[i]);
    	free(info->tags);
}

void neocities_free_filelist(
	neocities_filelist_t *list
)
{
    	if (!list) return;
    	for (size_t i=0; i<list->count; i++) free(list->paths[i]);
    	free(list->paths);
}

// -------------------------
// info — GET /api/info
// -------------------------
int neocities_info(
	const char *sitename, neocities_info_t *out
)
{
    	char url[512];
    	if (sitename)
    	    snprintf(url, sizeof(url), "https://neocities.org/api/info?sitename=%s", sitename);
    	else
    	    snprintf(url, sizeof(url), "https://neocities.org/api/info");

    	struct response resp;
    	int ret = perform_request(url, NULL, NULL, &resp, NULL);
    	if (ret) return ret;

    	json_error_t error;
    	json_t *root = json_loads(resp.data, 0, &error);
    	free(resp.data);
    	if (!root) return 3;

    	const char *result = json_string_value(json_object_get(root,"result"));
    	if (!result || strcmp(result,"success")!=0) { json_decref(root); return 4; }

	json_t *info = json_object_get(root,"info");
	if (!json_is_object(info)) {
	    json_decref(root);
	    return 4;
	}
		
	json_t *j_sitename = json_object_get(info,"sitename");
	json_t *j_created  = json_object_get(info,"created_at");
	json_t *j_updated  = json_object_get(info,"last_updated");
	json_t *j_domain   = json_object_get(info,"domain");
	json_t *j_hits     = json_object_get(info,"hits");
	
	out->sitename     = json_is_string(j_sitename) ? strdup(json_string_value(j_sitename)) : NULL;
	out->created_at   = json_is_string(j_created)  ? strdup(json_string_value(j_created))  : NULL;
	out->last_updated = json_is_string(j_updated)  ? strdup(json_string_value(j_updated))  : NULL;
	out->domain       = json_is_string(j_domain)   ? strdup(json_string_value(j_domain))   : NULL;
	out->hits         = json_is_integer(j_hits)    ? json_integer_value(j_hits) : 0;
	
    	json_t *tags = json_object_get(info,"tags");
	if (!json_is_array(tags)) {
	    out->tags = NULL;
	    out->tag_count = 0;
	} else {
	    size_t tc = json_array_size(tags);
	    out->tags = malloc(sizeof(char*) * tc);
	    out->tag_count = tc;
	
	    for (size_t i=0; i<tc; i++) {
	        json_t *tag = json_array_get(tags,i);
	        out->tags[i] = json_is_string(tag) ? strdup(json_string_value(tag)) : NULL;
	    }
	}

    	json_decref(root);
    	return 0;
}

// -------------------------
// list — GET /api/list
// -------------------------
int neocities_list(
	const char *api_key,
	const char *path, neocities_filelist_t *out
)
{
	char url[512];
    	if (path) snprintf(url, sizeof(url), "https://neocities.org/api/list?path=%s", path);
    	else snprintf(url, sizeof(url), "https://neocities.org/api/list");

    	struct response resp;
    	int ret = perform_request(url, api_key, NULL, &resp, NULL);
	if (ret) return ret;

    	json_error_t error;
    	json_t *root = json_loads(resp.data, 0, &error);
    	free(resp.data);
    	if (!root) return 3;

    	json_t *arr = json_object_get(root, "files");
    	if (!arr) { json_decref(root); return 4; }

    	size_t count = json_array_size(arr);
    	out->paths = malloc(sizeof(char*) * count);
    	out->count = count;
    	for (size_t i=0; i<count; i++) {
    	    json_t *f = json_array_get(arr, i);
    	    out->paths[i] = strdup(json_string_value(json_object_get(f,"path")));
    	}
    	json_decref(root);
    	return 0;
}

// -------------------------
// delete — POST /api/delete
// -------------------------
int neocities_delete(
	const char *api_key,
        const char **filenames,
        size_t count,
        char **response
)
{
	char body[1024] = "";
    	for (size_t i=0; i<count; i++) {
    	    char tmp[256];
    	    snprintf(tmp, sizeof(tmp), "filenames[]=%s&", filenames[i]);
    	    strncat(body, tmp, sizeof(body)-strlen(body)-1);
    	} 
    	struct response resp;
    	int ret = perform_request(
	    "https://neocities.org/api/delete",
	    api_key,
	    body,
	    &resp,
	    NULL
	);
	if (ret) return ret;
    	*response = resp.data;
    	return 0;
}

// -------------------------
// upload — POST /api/upload
// (multipart/form-data)
// -------------------------
int neocities_upload(
	const char *api_key,
    	const char **local_files,
   	const char **remote_names,
    	size_t count,
    	char **response
)
{
	if (!api_key || strlen(api_key) == 0)
		return 3;

	CURL *curl = curl_easy_init();
    	if (!curl) return 1;

    	struct response resp;
    	resp.data = malloc(1);
    	resp.len = 0;
	resp.data[0] = '\0';

    	curl_easy_setopt(curl, CURLOPT_URL, "https://neocities.org/api/upload");
    	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);

    	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 0L);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);


	curl_easy_setopt(curl, CURLOPT_USERPWD, NULL);
    	
	struct curl_slist *headers = NULL;
	char auth_header[512];
	snprintf(auth_header, sizeof(auth_header),
	    "Authorization: Bearer %s", api_key);
	
	headers = curl_slist_append(headers, auth_header);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    	curl_mime *form = curl_mime_init(curl);
    	for (size_t i=0; i<count; i++) {
    	    curl_mimepart *part = curl_mime_addpart(form);
    	    curl_mime_name(part, remote_names[i]);
    	    curl_mime_filedata(part, local_files[i]);
    	}
    	curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);

    	CURLcode res = curl_easy_perform(curl);
    	curl_mime_free(form);
    	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);

    	if (res != CURLE_OK) { free(resp.data); return 2; }

    	*response = resp.data;
    	return 0;
}

// -------------------------
// get_api_key — GET /api/key
// -------------------------
// no. for security reasons of course.
