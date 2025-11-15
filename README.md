# Neocities C CLI

A simple command-line tool written in C that interacts with the [Neocities API](https://neocities.org/api).  
Currently, only the `--info` command is implemented — it fetches and displays basic information about your Neocities site.

---

## Features
- Fetch site information using Neocities API  
- Authenticates via environment variables  
- JSON parsing with [jansson](https://digip.org/jansson/)  
- Works on Linux, Windows (MSYS2/MinGW), and Android (via Termux)  
- Compilable with a Makefile for easy builds  

---

## Requirements

You’ll need:
- A C compiler (`gcc` or `clang`)
- [libcurl](https://curl.se/libcurl/)
- [jansson](https://digip.org/jansson/)
- `make`

### Linux (Debian/Ubuntu-based)
```bash
sudo apt update
sudo apt install build-essential libcurl4-openssl-dev libjansson-dev make
```

### Windows (MSYS2 / MinGW)
```bash
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-curl mingw-w64-x86_64-jansson make
```

### Android (Termux)

```bash
pkg install clang curl-dev jansson make
```

## Building

Run in cloned repository:
```bash
make
```

This will generate the executable neocities (or neocities.exe on Windows).

To clean build files:

```bash
make clean
```

###  Environment Variables

Before running, export your Neocities credentials:

#### Linux / macOS / Termux

```bash
export NEOCITIES_USER="your_username"
export NEOCITIES_PASS="your_password"
```

#### Windows (PowerShell)

```batch
setx NEOCITIES_USER "your_username"
setx NEOCITIES_PASS "your_password"
```

## Usage

```bash
./neocities --info
```

Example output:

```
Neocities C CLI

Sitename: example
Hits: 42
Created at: 2024-01-10
Last updated: 2025-11-01
Domain: domain.com
Tags: art, blog, personal.
```

## Optional: Install Globally

You can move the compiled executable to a directory in your PATH to run it from anywhere:

#### Linux / Termux

```bash
sudo mv neocities /usr/local/bin/
```

#### Windows (PowerShell)
Move neocities.exe to a folder in your PATH, e.g., C:\Windows\System32\.

After that, you can run:

```batch
neocities --info
```

from any folder without specifying the path.

## Notes

Only the --info command is implemented for now.

If you get curl_easy_perform errors, check your network and SSL setup.

On Termux, use clang for the best compatibility.