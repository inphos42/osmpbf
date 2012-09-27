#ifndef OSMPBF_IPRIMITIVE
#define OSMPBF_IPRIMITIVE

#include <generics/refcountobject.h>

#include "abstractprimitiveinputadaptor.h"

namespace osmpbf {
	class IPrimitive : public generics::RCWrapper< AbstractPrimitiveInputAdaptor > {
	public:
		IPrimitive(const IPrimitive & other) : RCWrapper< AbstractPrimitiveInputAdaptor >(other) {}

		inline virtual bool isNull() const { return generics::RCWrapper< AbstractPrimitiveInputAdaptor >::isNull() || m_Private->isNull(); }

		inline IPrimitive & operator=(const IPrimitive & other) { RCWrapper< AbstractPrimitiveInputAdaptor >::operator=(other); return *this; }

		inline int64_t id() const { return m_Private->id(); }

		inline int tagsSize() const { return m_Private->tagsSize(); }

		inline uint32_t keyId(int index) const { return m_Private->keyId(index); }
		inline uint32_t valueId(int index) const { return m_Private->valueId(index); }

		inline const std::string & key(int index) const { return m_Private->key(index); }
		inline const std::string & value(int index) const { return m_Private->value(index); }

		inline const std::string & valueByKeyId(uint32_t key) const { return m_Private->valueByKeyId(key); }
		inline const std::string & valueByKey(const std::string key) const { return m_Private->valueByKey(key); }

	protected:
		IPrimitive() : RCWrapper< AbstractPrimitiveInputAdaptor >() {}
		IPrimitive (AbstractPrimitiveInputAdaptor * data) : RCWrapper< AbstractPrimitiveInputAdaptor >(data) {}
	};
}
#endif
