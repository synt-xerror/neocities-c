#!/bin/bash

BASHRC="$HOME/.bashrc"

read -p "Username: " NEOCITIES_USER
read -p "Password: " NEOCITIES_PASS

# Remove linhas existentes se houver
sed -i '/^NEOCITIES_USER=/d' "$BASHRC"
sed -i '/^NEOCITIES_PASS=/d' "$BASHRC"

# Adiciona no final
echo "NEOCITIES_USER=\"$NEOCITIES_USER\"" >> "$BASHRC"
echo "NEOCITIES_PASS=\"$NEOCITIES_PASS\"" >> "$BASHRC"

echo "Successfully logged in. Saved on $HOME/.bashrc"
