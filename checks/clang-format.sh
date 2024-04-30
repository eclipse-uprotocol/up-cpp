#!/bin/bash

PROJECT_ROOT="$(realpath "$(dirname "$0")/../")"

echo "Running clang-format on all files in '$PROJECT_ROOT'"
find "$PROJECT_ROOT" -iname '*.h' -o -iname '*.cpp' | xargs clang-format --dry-run
