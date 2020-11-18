#!/usr/bin/env bash

set -ve

cd "$(dirname "$0")"
cd rd
find . | cpio -o -H newc | pigz -9c > ../rd-new.cpio
cd ..
