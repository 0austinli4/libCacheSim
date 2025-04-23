# CMake generated Testfile for 
# Source directory: /home/al2926/libCacheSim/test
# Build directory: /home/al2926/libCacheSim/cache_interface/test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(testReader "/home/al2926/libCacheSim/cache_interface/bin/testReader")
set_tests_properties(testReader PROPERTIES  WORKING_DIRECTORY "." _BACKTRACE_TRIPLES "/home/al2926/libCacheSim/test/CMakeLists.txt;23;add_test;/home/al2926/libCacheSim/test/CMakeLists.txt;0;")
add_test(testDistUtils "/home/al2926/libCacheSim/cache_interface/bin/testDistUtils")
set_tests_properties(testDistUtils PROPERTIES  WORKING_DIRECTORY "." _BACKTRACE_TRIPLES "/home/al2926/libCacheSim/test/CMakeLists.txt;24;add_test;/home/al2926/libCacheSim/test/CMakeLists.txt;0;")
add_test(testProfilerLRU "/home/al2926/libCacheSim/cache_interface/bin/testProfilerLRU")
set_tests_properties(testProfilerLRU PROPERTIES  WORKING_DIRECTORY "." _BACKTRACE_TRIPLES "/home/al2926/libCacheSim/test/CMakeLists.txt;25;add_test;/home/al2926/libCacheSim/test/CMakeLists.txt;0;")
add_test(testSimulator "/home/al2926/libCacheSim/cache_interface/bin/testSimulator")
set_tests_properties(testSimulator PROPERTIES  WORKING_DIRECTORY "." _BACKTRACE_TRIPLES "/home/al2926/libCacheSim/test/CMakeLists.txt;26;add_test;/home/al2926/libCacheSim/test/CMakeLists.txt;0;")
add_test(testEvictionAlgo "/home/al2926/libCacheSim/cache_interface/bin/testEvictionAlgo")
set_tests_properties(testEvictionAlgo PROPERTIES  WORKING_DIRECTORY "." _BACKTRACE_TRIPLES "/home/al2926/libCacheSim/test/CMakeLists.txt;27;add_test;/home/al2926/libCacheSim/test/CMakeLists.txt;0;")
add_test(testPrefetchAlgo "/home/al2926/libCacheSim/cache_interface/bin/testPrefetchAlgo")
set_tests_properties(testPrefetchAlgo PROPERTIES  WORKING_DIRECTORY "." _BACKTRACE_TRIPLES "/home/al2926/libCacheSim/test/CMakeLists.txt;28;add_test;/home/al2926/libCacheSim/test/CMakeLists.txt;0;")
