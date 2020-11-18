#!/usr/bin/env bash

set -ve

cd "$(dirname "$0")"
cd rd
find . | cpio -o -H newc | gzip -c > ../rd-new.cpio
cd ..
