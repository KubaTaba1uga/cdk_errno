#!/bin/bash

set -xeu

rm -rf build

meson setup build

meson compile -C build

