#ifndef INDEX_H
#define INDEX_H

#include <list>
#incude <stdint.h>

#include "thread_sync.h"

using namespace std;

class Index
{
public:
	static uint32_t get()
	{
		{
			CScopeGuard gaurd(_list_mutex);
			if(0 != _idle_list.size())
			{
				uint32_t i = _idle_list.front();
				_idle_list.pop_front();
				return i;
			}
		}
	
		{
			CScopeGuard gaurd(_idx_mutex);
			uint32_t temp = _current_idx;
			_current_idx = (_current_idx + 1) % 0x7FFFFFFE;
			return temp;
		}
	}

	static void free(uint32_t index)
	{
		CScopeGuard gaurd(_list_mutex);
		_idle_list->push_back(Index);
	}

private:
	static uint32_t _current_idx;
	static CMutex _idx_mutex;

	static list<uint32_t> _idle_list;
	static CMutex _list_mutex;
}

#endif //INDEX_H