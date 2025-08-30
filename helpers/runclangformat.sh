#!/usr/bin/bash
# Run clang-format on all C++ source and header files in the repo

# Fail on error
set -euo pipefail

# Root of repo (where this script lives)
ROOT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Find all C++ files (adjust extensions as needed)
find "$ROOT_DIR/.." \
    -type f \
    \( -name '*.cpp' -o -name '*.cc' -o -name '*.cxx' -o -name '*.c' \
       -o -name '*.h' -o -name '*.hpp' -o -name '*.hh' \) \
    -print0 \
  | xargs -0 clang-format -i