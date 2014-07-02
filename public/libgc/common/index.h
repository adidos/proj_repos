#ifndef INDEX_H
#define INDEX_H

#include <list>
#include <stdint.h>

#include "thread_sync.h"

using namespace std;

class Index
{
public:
	static uint32_t get()
	{
		CScopeGuard gaurd(_idx_mutex);

		uint32_t temp = _current_idx;
		_current_idx = (_current_idx + 1) % 0x7FFFFFFF;

		if(0 == _current_idx) ++_current_idx;

		return temp;
	}

private:
	static uint32_t _current_idx;
	static CMutex _idx_mutex;
};

#endif //INDEX_H
