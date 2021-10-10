#!/bin/bash

cd ..
make preprocess
cd bin
echo ">>test_2_include01.spl"
./preprocess ../test-ex/test_2_include01.spl
echo ">>test_2_macro02.spl"
./preprocess ../test-ex/test_2_macro02.spl
echo ">>test_2_macro03.spl"
./preprocess ../test-ex/test_2_macro03.spl
echo ">>test_2_macro04.spl"
./preprocess ../test-ex/test_2_macro04.spl
echo ">>test_2_macro05.spl"
./preprocess ../test-ex/test_2_macro05.spl
