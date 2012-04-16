#ifndef OSMPBF_REFCOUNTOBJECT_H
#define OSMPBF_REFCOUNTOBJECT_H

namespace osmpbf {
	class RefCountObject {
	public:
		RefCountObject() : m_ref(0) {}
		
		inline void refInc() { m_ref++; }
		inline void refDec() { m_ref--; if (m_ref < 1) delete this; }
	private:
		int m_ref;
	};
}

#endif
