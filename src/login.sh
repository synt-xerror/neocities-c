#!/bin/bash
set -e

CONFIG_DIR="$HOME/.config/neocities"
CONFIG_FILE="$CONFIG_DIR/credentials"

mkdir -p "$CONFIG_DIR"
chmod 700 "$CONFIG_DIR"

read -p "Username: " USER
read -s -p "Password: " PASS
echo

read -p "Would you like to see the credentials on your screen to confirm? [Y/n]: " CHOICE
case $CHOICE in
    y|Y) echo -e "Username: $USER\nPassword: $PASS" ;;
    *) echo "" ;;
esac

cat > "$CONFIG_FILE" <<EOF
user = $USER
pass = $PASS
EOF

chmod 600 "$CONFIG_FILE"

echo "Credentials saved in $CONFIG_FILE"
