#!/usr/bin/env bash
set -euo pipefail

if [ -f ~/.ssh/id_ed25519.pub ]; then
  key_file=~/.ssh/id_ed25519.pub
elif [ -f ~/.ssh/id_rsa.pub ]; then
  key_file=~/.ssh/id_rsa.pub
elif [ -f ~/.ssh/id_ecdsa.pub ]; then
  key_file=~/.ssh/id_ecdsa.pub
else
  echo "You have no keys! Creating one for you"
  key_file=~/.ssh/id_ed25519.pub
  ssh-keygen -t ed25519 -f "${key_file%%.pub}" -N ""
fi

echo "============Public Key($(basename -- "$key_file"))============"
cat "$key_file"