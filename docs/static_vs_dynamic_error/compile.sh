#!/bin/bash

set -xeu

rm -rf build

# meson setup build -Dbuildtype=release
meson setup build -Dbuildtype=debug -Db_pie=false

meson compile -v -C build

