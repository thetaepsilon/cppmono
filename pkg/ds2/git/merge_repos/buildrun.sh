#!/bin/sh
t="./merge_repos.bin";
c++17strict -I ../../../ -o $t merge_repos.cpp || exit $?;
exec $t "$@";

