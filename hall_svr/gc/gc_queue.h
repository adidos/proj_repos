
/* ======================================================
* 
* file:		GCQueue.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-19 19:38:52
* 
* ======================================================*/
#ifndef GC_QUEUE_H
#define GC_QUEUE_H

#include <string>

template <class T> 
class GCQueue 
{
	T*  m_buffer;

	int m_capacity;
	int m_start;
	int m_size;
	int m_max_size;

	pthread_mutex_t m_lock;
	pthread_cond_t	m_cond_not_full;
	pthread_cond_t	m_cond_not_empty;

private:
	bool Lock()
	{
		pthread_mutex_lock(&m_lock);
		return true;
	}

	bool Unlock()
	{
		pthread_mutex_unlock(&m_lock);
		return true;
	}

	bool wait_not_empty()
	{
		pthread_cond_wait(&m_cond_not_empty, &m_lock);
		return true;
	}

	bool wait_not_full()
	{
		pthread_cond_wait(&m_cond_not_full, &m_lock);
		return true;
	}

	bool wait_not_empty(int waitMs)
	{
		struct timespec timespot;
		getAbsTimeout(waitMs, timespot);					
		pthread_cond_timedwait(&m_cond_not_empty, &m_lock, &timespot);
		return true;
	}

	bool wait_not_full(int waitMs)
	{
		struct timespec timespot;		
		getAbsTimeout(waitMs, timespot);		
		pthread_cond_timedwait(&m_cond_not_full, &m_lock, &timespot);

		return true;
	}

	bool notify_not_empty()
	{
		pthread_cond_signal(&m_cond_not_empty);

		return true;
	}

	bool notify_not_full()
	{
		pthread_cond_signal(&m_cond_not_full);

		return true;
	}

public:

	GCQueue(int capacity)
	{
		m_buffer = NULL;
		m_start = 0;
		m_size = 0;
		m_capacity = capacity;

		pthread_mutex_init(&m_lock, NULL);
		pthread_cond_init(&m_cond_not_full, NULL);
		pthread_cond_init(&m_cond_not_empty, NULL);
	}

	virtual ~GCQueue()
	{
		pthread_mutex_destroy(&m_lock);
		pthread_cond_destroy(&m_cond_not_full);
		pthread_cond_destroy(&m_cond_not_empty);
	}

	void init(int capacity)
	{
		try
		{
			m_buffer = allocBuffer(capacity);
			m_capacity = capacity;
		}
		catch(std::exception&){}
	}

	void uninit()
	{
		freeBuffer(m_buffer);
	}

	bool pop(T& retVal, int waitMs)
	{
		if(!checkBuffer())
			return false;

		bool ret_val = false;
		Lock();

		try
		{
			if(m_size == 0)
			{//empty
				if(waitMs == 0)
				{
					Unlock();
					return false;
				}
				else if(waitMs < 0)
				{//wait infinitely
					while(m_size == 0)
					{
						wait_not_empty();	
					}
				}
				else
				{//wait for a given timeout
					wait_not_empty(waitMs);
				}
			}

			if(m_size > 0)
			{
				retVal = m_buffer[m_start];
				ret_val = true;

				if(++m_start >= m_capacity)
					m_start -= m_capacity;

				m_size --;
				notify_not_full();
			}
		}
		catch (std::exception& ) {
			//exception
			ret_val = false;
		}

		Unlock();
		return ret_val;
	}

	bool push(const T& element, int waitMs)
	{
		if(!checkBuffer())
			return false;

		bool retVal = false;
		Lock();

		try	
		{
			if(m_size >= m_capacity)
			{//full
				if(waitMs == 0)
				{					
					Unlock();
					return false;
				}
				else if(waitMs < 0)
				{//wait infinitely
					while(m_size >= m_capacity)
					{
						wait_not_full();
					}
				}
				else
				{//wait for a given timeout
					wait_not_full(waitMs);
				}
			}

			if(m_size < m_capacity)
			{
				int newpos = m_start + m_size;
				if(newpos >= m_capacity)
					newpos -= m_capacity;

				m_buffer[newpos] = element;
				m_size ++;
				if(m_max_size < m_size)
					m_max_size = m_size;
				notify_not_empty();
				retVal = true;
			}
		}
		catch (std::exception&) {
			//exception
			retVal = false;
		}

		Unlock();
		return retVal;
	}

	bool pop(T& retVal)
	{
		return pop(retVal, -1);
	}

	bool push(T element)
	{
		return push(element, -1);
	}

	int getCapacity()
	{
		return m_capacity;
	}

	int getSize()
	{
		Lock();
		int size = m_size;
		Unlock();
		return size;
	}

	bool	empty()
	{
		Lock();
		bool ret = (0 == m_size);
		Unlock();
		return ret;
	}

	int getMaxSize(bool resetMax = true)
	{
		Lock();
		int maxSize = m_max_size;
		
		if(resetMax)
		{
			m_max_size = m_size;			
		}
		Unlock();
		return maxSize;		
	}

protected:
	virtual T* allocBuffer(int capacity)
	{
		return new T[capacity];
	}
	virtual void freeBuffer(T* pBuffer)
	{	
		if(pBuffer)
			delete []pBuffer;
	}

private:
	bool checkBuffer()
	{
		if(m_buffer == NULL)
		{
			m_buffer = allocBuffer(m_capacity);
		}
		
		return m_buffer != NULL;
	}

	void getAbsTimeout(int timeout, struct timespec& abstimeout)
	{
		struct timeval now;
		gettimeofday(&now, NULL);
		timeout += (now.tv_usec / 1000);

		abstimeout.tv_sec = now.tv_sec + timeout / 1000;
		abstimeout.tv_nsec = (timeout % 1000) * 1000 * 1000;
	}

};

#endif
