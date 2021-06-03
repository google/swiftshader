#!/bin/bash

# This script is called by the SwiftShader into Android AutoRoller:
# https://autoroll.skia.org/r/swiftshader-android

# Remove git submodules
# Android does not allow remote submodules (b/189557997)
git config --file .gitmodules --get-regexp path | awk '{ system("git rm " $2) }'
rm .gitmodules