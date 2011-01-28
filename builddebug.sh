#!/bin/bash

mkdir -p build
(cd build; ../configure --prefix="" --enable-debug && make $1 ) && ln -s build/src/dunelegacy dunelegacy
