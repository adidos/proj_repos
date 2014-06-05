#include "index.h"

uint32_t Index::_current_idx = 1;
CMutex Index::_idx_mutex;

list<uint32_t> Index::_idle_list;
CMutex Index::_list_mutex;