git ls-files -- '*.c' '*.h' | xargs clang-format -i -style=file
git diff --exit-code --color
