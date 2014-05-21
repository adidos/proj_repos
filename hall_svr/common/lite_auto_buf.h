// lite_auto_buf.h

#ifndef __LITE_AUTO_BUF_H
#define __LITE_AUTO_BUF_H

template <size_t N> class AutoByteBuf{
public:
	AutoByteBuf() { m_dynamic_buf = NULL, m_use_dynamic_buf = false; }
	~AutoByteBuf() {
		free_buf();
	}

	byte* get_buffer(int buffer_len){
		if(buffer_len <= (int)N){
			m_use_dynamic_buf = false;
			return m_init_buf;
		}
		else {
			free_buf();
			m_use_dynamic_buf = true;
			m_dynamic_buf = new byte[buffer_len];
			return m_dynamic_buf;	
		}
	}

	byte* current_buffer() {
		if(m_use_dynamic_buf)
			return m_dynamic_buf;
		else
			return m_init_buf;
	}

private:
	void free_buf(){
		if(m_dynamic_buf != NULL){
			delete []m_dynamic_buf;
			m_dynamic_buf = NULL;
		}
	}

private:
	bool m_use_dynamic_buf;
	byte m_init_buf[N];
	byte* m_dynamic_buf;

};

#endif

