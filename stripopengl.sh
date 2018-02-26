#!/bin/bash

find include -name "*.h" |  while read file; do
  unifdef -D_DIRECTX -U_OPENGL $file > "$file.stripped"
  cat "$file.stripped" > $file
  rm "$file.stripped"
done

find src -name "*.cpp" |  while read file; do
  unifdef -D_DIRECTX -U_OPENGL $file > "$file.stripped"
  cat "$file.stripped" > $file
  rm "$file.stripped"
done
