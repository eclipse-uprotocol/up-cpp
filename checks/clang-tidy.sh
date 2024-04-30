#!/bin/bash

PROJECT_ROOT="$(realpath "$(dirname "$0")/../")"

echo "Running clang-format on all files in '$PROJECT_ROOT'"
#find "$PROJECT_ROOT" -iname '*.h' -o -iname '*.cpp' | xargs run-clang-tidy -j8 -extra-arg='-std=c++17' -extra-arg='-x c++'
find "$PROJECT_ROOT" -iname '*.h' -o -iname '*.cpp' | xargs -I"{}" clang-tidy "{}" -- -std=c++17 -x c++

# clang-tidy include/up-cpp/uri/tools/IpAddress.h -- -std=c++17 -x c++
# run-clang-tidy -j8 -extra-arg='-std=c++17' -extra-arg='-x c++'
