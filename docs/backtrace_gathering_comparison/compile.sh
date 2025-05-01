#!/bin/bash

set -xeu

rm -rf build

# Traces require -rdynamic -no-pie for linker
meson setup build -Dbuildtype=debug -Db_pie=false

meson compile -v -C build
