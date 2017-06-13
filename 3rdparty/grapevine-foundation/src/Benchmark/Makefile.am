noinst_LTLIBRARIES = libgrapevine-benchmark.la

libgrapevine_benchmark_la_SOURCES = \
   FpsCounter.cpp \
   DualScrollTypeHwcSwapsFpsLogger.cpp

libgrapevine_benchmark_la_CPPFLAGS = -I$(top_srcdir)/include $(GRAPEVINE_CFLAGS)

# Includes
library_includedir=$(includedir)/Grapevine/Benchmark
library_include_HEADERS = \
    ../../include/Grapevine/Benchmark/FpsCounter.hpp \
    ../../include/Grapevine/Benchmark/DualScrollTypeHwcSwapsFpsLogger.hpp
