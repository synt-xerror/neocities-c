# Neocities C API

Minimal, secure C wrapper for the Neocities API using API key authentication.

No login flow.
No credential storage.
Stateless by design.

Users must generate their own API key via the Neocities dashboard.

---

## Features

* Site info retrieval
* File listing
* Upload files
* Delete files
* TLS verification enabled by default

Authentication is performed exclusively via:

```
Authorization: Bearer <API_KEY>
```

---

## Requirements

* libcurl

Linux (example):

```
sudo apt install libcurl4-openssl-dev
```

---

## Authentication

Generate your API key at:

Neocities > Settings > Manage Site Settings > API
(or https://neocities.org/settings/YOUR-USERNAME#api_key)

If you're coding a CLI or application, you must provide the key at runtime (env var, config file, etc).

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

For security reasons, you should get the API key via Neocities settings. This function is not avaiable. 

---

## Example Usage

See example.c

---

## Security Notes

* TLS verification is enforced
* Redirects are disabled intentionally
* API key is never persisted internally

Recommended:

* Store key in env vars
* Avoid hardcoding

---

## Design Philosophy

* No login/password support
* API key only
* Explicit memory ownership
* Minimal abstraction

---

## License

[MIT](LICENSE)

