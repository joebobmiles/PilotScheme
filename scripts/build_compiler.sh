#!/bin/bash

mkdir -p ./bin

pushd ./bin > /dev/null

clang -g \
	--std=c99 \
	-I ../includes \
	-o ./pilotc \
	../test/examples/compiler/pilotc.c

popd > /dev/null
