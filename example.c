#include <stdio.h>
#include "neocities.h"
#include <stdlib.h>

int main() {
	static const char* username = "YOUR_USER_NAME";
	static const char* api_key = "YOUR_API_KEY";
	
	char* out = NULL;

	// GET /api/info
	if (neocities_info(username, &out) == 0) printf("%s", out);
	free(out);

	// GET /api/list
	if (neocities_list(api_key, "/", &out) == 0) printf("%s", out);
	free(out);

	static const char* files[] = { "test.txt" };
	static const int qnt_files = sizeof(files) / sizeof(files[0]);

	// GET /api/upload
	if (neocities_upload(api_key, files, files, qnt_files, &out) == 0) printf("%s", out);
	free(out);

	// GET /api/delete
	if (neocities_delete(api_key, files, qnt_files, &out) == 0) printf("%s", out);
	free(out);

	return 0;
}
