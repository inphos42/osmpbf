#ifndef OSMPBF_ABSTRACTOSMPRIMITIVE_H
#define OSMPBF_ABSTRACTOSMPRIMITIVE_H

#include <cstdint>
#include <string>
#include "refcountobject.h"

class PrimitiveGroup;

namespace osmpbf {
	class OSMPrimitiveBlockController;

	class AbstractOSMPrimitiveAdaptor : public RefCountObject {
	public:
		AbstractOSMPrimitiveAdaptor()
			: RefCountObject(), m_Controller(NULL), m_Group(NULL), m_Position(-1) {}
		AbstractOSMPrimitiveAdaptor(OSMPrimitiveBlockController * controller, PrimitiveGroup * group, int position)
			: RefCountObject(), m_Controller(controller), m_Group(group), m_Position(position) {}

		inline bool isNull() const { return !m_Controller || !m_Group || (m_Position < 0); };

		virtual int64_t id() = 0;

		virtual int keysSize() = 0;

		virtual std::string key(int index) = 0;
		virtual std::string value(int index) = 0;
	protected:
		OSMPrimitiveBlockController * m_Controller;
		PrimitiveGroup * m_Group;
		int m_Position;
	};
}
#endif // OSMPBF_ABSTRACTOSMPRIMITIVE_H
