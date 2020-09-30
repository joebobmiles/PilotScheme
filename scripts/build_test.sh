#!/bin/bash

mkdir -p ./bin

pushd ./bin > /dev/null

clang -g \
	-std=c11 \
	-I ../includes \
	-o ./test \
	../test/test.c

popd > /dev/null
