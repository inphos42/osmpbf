#ifndef OSMPBF_ABSTRACTPRIMITIVEINPUTADAPTOR_H
#define OSMPBF_ABSTRACTPRIMITIVEINPUTADAPTOR_H

#include <cstdint>
#include <string>

#include <generics/refcountobject.h>

namespace osmpbf {
	class PrimitiveBlockInputAdaptor;

	class AbstractPrimitiveInputAdaptor : public generics::RefCountObject {
	public:
		AbstractPrimitiveInputAdaptor()
			: RefCountObject(), m_Controller(NULL) {}
		AbstractPrimitiveInputAdaptor(PrimitiveBlockInputAdaptor * controller)
			: RefCountObject(), m_Controller(controller) {}

		virtual bool isNull() const { return !m_Controller; }

		virtual int64_t id() = 0;

		virtual int tagsSize() const = 0;

		virtual uint32_t keyId(int index) const = 0;
		virtual uint32_t valueId(int index) const = 0;

		virtual const std::string & key(int index) const;
		virtual const std::string & value(int index) const;

		// convenience functions (very slow)
		virtual const std::string & valueByKeyId(uint32_t key) const;
		virtual const std::string & valueByKey(const std::string & key) const;

	protected:
		PrimitiveBlockInputAdaptor * m_Controller;
	};
}

#endif // OSMPBF_ABSTRACTPRIMITIVEADAPTOR_H
