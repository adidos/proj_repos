#include "index.h"

uint32_t Index::_current_idx = 1;
CMutex Index::_idx_mutex;
