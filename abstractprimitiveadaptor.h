#ifndef OSMPBF_ABSTRACTPRIMITIVEADAPTOR_H
#define OSMPBF_ABSTRACTPRIMITIVEADAPTOR_H

#include <cstdint>
#include <string>

#include "refcountobject.h"

class PrimitiveGroup;

namespace osmpbf {
	class PrimitiveBlockInputAdaptor;

	class AbstractPrimitiveInputAdaptor : public RefCountObject {
	public:
		AbstractPrimitiveInputAdaptor()
			: RefCountObject(), m_Controller(NULL) {}
		AbstractPrimitiveInputAdaptor(PrimitiveBlockInputAdaptor * controller)
			: RefCountObject(), m_Controller(controller) {}

		virtual bool isNull() const { return !m_Controller; }

		virtual int64_t id() = 0;

		virtual int tagsSize() const = 0;

		virtual int keyId(int index) const = 0;
		virtual int valueId(int index) const = 0;

		virtual const std::string & key(int index) const = 0;
		virtual const std::string & value(int index) const = 0;

	protected:
		PrimitiveBlockInputAdaptor * m_Controller;
	};

	class PrimitiveBlockOutputAdaptor;

	class AbstractPrimitiveOutputAdaptor : public RefCountObject {
	public:
		AbstractPrimitiveOutputAdaptor()
			: RefCountObject(), m_Controller(NULL) {}
		AbstractPrimitiveOutputAdaptor(PrimitiveBlockOutputAdaptor * controller)
			: RefCountObject(), m_Controller(controller) {}

		virtual bool isNULL() const { return !m_Controller; }

		virtual int64_t id() const = 0;
		virtual void setId(int64_t value) = 0;

		virtual int tagsSize() const = 0;

		virtual const std::string & key(int index) const = 0;
		virtual const std::string & value(int index) const = 0;

		virtual void setKey(int index, const std::string & key) = 0;
		virtual void setValue(int index, const std::string & value) = 0;
		virtual void setValue(const std::string & key, const std::string & value) = 0;

		virtual void addTag(const std::string & key, const std::string & value) = 0;
		virtual void removeTagLater(int index) = 0;

		virtual void clearTags() = 0;

	protected:
		PrimitiveBlockOutputAdaptor * m_Controller;
	};
}
#endif // OSMPBF_ABSTRACTPRIMITIVEADAPTOR_H
