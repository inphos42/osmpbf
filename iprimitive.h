#ifndef OSMPBF_IPRIMITIVE
#define OSMPBF_IPRIMITIVE

#include "refcountobject.h"
#include "abstractprimitiveinputadaptor.h"

namespace osmpbf {
	class IPrimitive : public RCWrapper< AbstractPrimitiveInputAdaptor > {
	public:
		IPrimitive(const IPrimitive & other) : RCWrapper< AbstractPrimitiveInputAdaptor >(other) {}

		inline IPrimitive & operator=(const IPrimitive & other) { RCWrapper< AbstractPrimitiveInputAdaptor >::operator=(other); return *this; }

		inline int64_t id() const { return m_Private->id(); }

		inline int tagsSize() const { return m_Private->tagsSize(); }

		inline uint32_t keyId(int index) const { return m_Private->keyId(index); }
		inline uint32_t valueId(int index) const { return m_Private->valueId(index); }

		inline const std::string & key(int index) const { return m_Private->key(index); }
		inline const std::string & value(int index) const { return m_Private->value(index); }

		inline const std::string & value(uint32_t key) const { return m_Private->value(key); }
		inline const std::string & value(const std::string key) const { return m_Private->value(key); }

	protected:
		IPrimitive() : RCWrapper< AbstractPrimitiveInputAdaptor >() {}
		IPrimitive (AbstractPrimitiveInputAdaptor * data) : RCWrapper< AbstractPrimitiveInputAdaptor >(data) {}
	};
}
#endif
