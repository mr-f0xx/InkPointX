#pragma once
// The fuzzer runs thousands of iterations on deliberately corrupt input, so
// every parser diagnostic is expected noise. Drop them.
#define LOG_ERR(...) ((void)0)
#define LOG_INF(...) ((void)0)
#define LOG_DBG(...) ((void)0)
