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
			: RefCountObject(), m_Controller(NULL), m_Group(NULL), m_Index(-1) {}
		AbstractPrimitiveInputAdaptor(PrimitiveBlockInputAdaptor * controller, PrimitiveGroup * group, int index)
			: RefCountObject(), m_Controller(controller), m_Group(group), m_Index(index) {}

		virtual bool isNull() const { return !m_Controller || !m_Group || (m_Index < 0); }

		virtual int64_t id() = 0;

		virtual int keysSize() const = 0;

		virtual int keyId(int index) const = 0;
		virtual int valueId(int index) const = 0;

		virtual const std::string & key(int index) const = 0;
		virtual const std::string & value(int index) const = 0;

	protected:
		PrimitiveBlockInputAdaptor * m_Controller;
		PrimitiveGroup * m_Group;
		int m_Index;
	};

	class PrimitiveBlockOutputAdaptor;

	class AbstractPrimitiveOutputAdaptor : public RefCountObject {
	public:
		AbstractPrimitiveOutputAdaptor()
			: RefCountObject(), m_Controller(NULL), m_Group(NULL), m_Index(-1) {}
		AbstractPrimitiveOutputAdaptor(PrimitiveBlockInputAdaptor * controller, PrimitiveGroup * group, int index)
			: RefCountObject(), m_Controller(controller), m_Group(group), m_Index(index) {}

		virtual bool isNULL() const { return !m_Controller || !m_Group || (m_Index < 0); }

		virtual int64_t id() const = 0;
		virtual void setId(int64_t value) = 0;

		virtual int tagsSize() const = 0;

		virtual std::string & key(int index) = 0;
		virtual std::string & value(int index) = 0;

		virtual void addTag(std::string key, std::string value) = 0;
		virtual void removeTag(int index) = 0;

		virtual void clearTags() = 0;
	protected:
		PrimitiveBlockInputAdaptor * m_Controller;
		PrimitiveGroup * m_Group;
		int m_Index;
	};
}
#endif // OSMPBF_ABSTRACTPRIMITIVEADAPTOR_H
