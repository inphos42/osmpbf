#ifndef OSMPBF_REFCOUNTOBJECT_H
#define OSMPBF_REFCOUNTOBJECT_H

#include <cstdint>

namespace osmpbf {
	class RefCountObject {
	public:
		RefCountObject() : m_ref(0) {}
		virtual ~RefCountObject() {}
		
		inline void refInc() {
			m_ref++;
		}
		inline void refDec() {
			m_ref--;
			
			if (m_ref < 1)
				delete this;
		}
		
		inline int refCounter() const { return m_ref; }
	private:
		uint32_t m_ref;
	};
}

#endif
