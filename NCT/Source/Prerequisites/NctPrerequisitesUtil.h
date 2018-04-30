
#pragma once

#ifdef NCT_EXPORTS
	#define NCT_EXPORT __declspec(dllexport)
#else
    #define NCT_EXPORT 
#endif

#include <cstdint>
#include <cassert>
#include <vector>
#include <queue>
#include <set>
#include <string>
#include <sstream>
#include <memory>
#include <ctime>
#include <cmath>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <functional>
#include <limits>
#include <new>   
#include <cstdint>
#include <utility>
#include <stack>
#include <assert.h>
#include <map>