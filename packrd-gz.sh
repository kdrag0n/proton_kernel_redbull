#!/usr/bin/env bash

set -ve

cd "$(dirname "$0")"
cd rd
find . | cpio -o -H newc | pigz -c > ../rd-new.cpio
cd ..
