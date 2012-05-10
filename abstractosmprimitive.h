#ifndef OSMPBF_ABSTRACTOSMPRIMITIVE_H
#define OSMPBF_ABSTRACTOSMPRIMITIVE_H

#include <cstdint>
#include <string>
#include "refcountobject.h"

class PrimitiveGroup;

namespace osmpbf {
	class OSMPrimitiveBlockInputAdaptor;

	class AbstractOSMPrimitiveAdaptor : public RefCountObject {
	public:
		AbstractOSMPrimitiveAdaptor()
			: RefCountObject(), m_Controller(NULL), m_Group(NULL), m_Index(-1) {}
		AbstractOSMPrimitiveAdaptor(OSMPrimitiveBlockInputAdaptor * controller, PrimitiveGroup * group, int index)
			: RefCountObject(), m_Controller(controller), m_Group(group), m_Index(index) {}

		virtual bool isNull() const { return !m_Controller || !m_Group || (m_Index < 0); }

		virtual int64_t id() = 0;

		virtual int keysSize() const = 0;

		virtual int keyId(int index) const = 0;
		virtual int valueId(int index) const = 0;

		virtual std::string key(int index) const = 0;
		virtual std::string value(int index) const = 0;

	protected:
		OSMPrimitiveBlockInputAdaptor * m_Controller;
		PrimitiveGroup * m_Group;
		int m_Index;
	};
}
#endif // OSMPBF_ABSTRACTOSMPRIMITIVE_H
