#ifndef OSMPBF_REFCOUNTOBJECT_H
#define OSMPBF_REFCOUNTOBJECT_H

#include <cstdint>
#include <cassert>

namespace osmpbf {
	class RefCountObject {
	public:
		RefCountObject() : m_rc(0) {}
		virtual ~RefCountObject() {}

		inline void rcInc() { m_rc++; }
		inline void rcDec() { assert(m_rc); m_rc--; if (m_rc < 1) delete this; }

		inline int rc() const { return m_rc; }
	private:
		RefCountObject(const RefCountObject & other);
		RefCountObject & operator=(const RefCountObject & other);

		uint32_t m_rc;
	};

	template<class RCObj>
	class RCWrapper {
	public:
		RCWrapper() : m_Private(NULL) {};
		RCWrapper(RCObj * data) : m_Private(data) { if (m_Private) m_Private->rcInc(); }
		RCWrapper(const RCWrapper & other) : m_Private(other.m_Private) { if (m_Private) m_Private->rcInc(); }
		virtual ~RCWrapper() { if (m_Private) m_Private->rcDec(); }

		RCWrapper & operator=(const RCWrapper & other) {
			if (m_Private) m_Private->rcDec();
			m_Private = other.m_Private;
			if (m_Private) m_Private->rcInc();
			return *this;
		}

		bool operator==(const RCWrapper & other) { return m_Private == other.m_Private; }

		inline bool isNull() const { return !m_Private || m_Private->isNull(); }

	protected:
		RCObj * m_Private;
	};
}

#endif
