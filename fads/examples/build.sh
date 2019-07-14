#! /bin/bash

rm -rf build && mkdir -p build && cd build && cmake ../../compiler/ &&  make &&  mv fadsgen .. && cd .. && rm -rf build && ./fadsgen sample.fads && mkdir build && cd build && cmake .. && make
