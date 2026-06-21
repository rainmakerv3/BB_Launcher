#include "miniz.h"

#ifndef inflateSetDictionary
#define inflateSetDictionary(strm, dictionary, dictLength) Z_STREAM_ERROR
#endif

#ifndef deflateSetDictionary
#define deflateSetDictionary(strm, dictionary, dictLength) Z_STREAM_ERROR
#endif