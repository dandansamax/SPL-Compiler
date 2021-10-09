#!/bin/bash

cd ..
make preprocess
cd bin
./preprocess ../test/test_2_include01.spl
