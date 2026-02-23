# Neocities API C library

Minimal, secure C wrapper for the Neocities API.

---

## Features

* Site info retrieval
* File listing
* Upload files
* Delete files
* Get API key
* TLS verification enabled by default

---

## Requirements

* libcurl

Linux (example):

```
sudo apt install libcurl4-openssl-dev
```

---

## API Overview

### Info (public)

```c
int neocities_info(const char *sitename, char **out);
```

Fetch metadata about a site.

---

### List files

```c
int neocities_list(const char *api_key, const char *path, char** out);
```

List files at a path.

---

### Upload

```c
int neocities_upload(const char *api_key, const char **local_files, const char **remote_names, size_t count, char **response);
```

Uploads multiple files.

* `local_files[i]` → local path
* `remote_names[i]` → remote filename

---

### Delete

```c
int neocities_delete(const char *api_key, const char **filenames, size_t count, char **response);
```

Deletes files from the site.

---

### API Key

```c
int neocities_apikey(const char *user, const char *pass, char **out);
```

Get your API key.

---

## Example Usage

See example.c

---

## License

[MIT](LICENSE)

