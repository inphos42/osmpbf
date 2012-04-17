#ifndef OSMPBF_ABSTRACTOSMPRIMITIVE_H
#define OSMPBF_ABSTRACTOSMPRIMITIVE_H

#include <sys/types.h>
#include <string>
#include "refcountobject.h"

class PrimitiveGroup;

namespace osmpbf {
	class OSMPrimitiveBlockController;

	class AbstractOSMPrimitive : public RefCountObject {
	public:
		AbstractOSMPrimitive()
			: RefCountObject(), m_Controller(NULL), m_Group(NULL), m_Position(-1) {}
		AbstractOSMPrimitive(OSMPrimitiveBlockController * controller, const PrimitiveGroup * group, int position)
			: RefCountObject(), m_Controller(controller), m_Group(group), m_Position(position) {}

		inline bool isNull() const { return !m_Controller || !m_Group || (m_Position < 0); };

		virtual int64_t id() = 0;

		virtual int keysSize() = 0;

		virtual std::string key(int index) = 0;
		virtual std::string value(int index) = 0;
		virtual std::string value(std::string key) = 0;
	protected:
		OSMPrimitiveBlockController * m_Controller;
		const PrimitiveGroup * m_Group;
		int m_Position;
	};

	class AbstractOSMNode : public AbstractOSMPrimitive {
	public:
		AbstractOSMNode()
			: AbstractOSMPrimitive() {}
		AbstractOSMNode(OSMPrimitiveBlockController * controller, const PrimitiveGroup * group, int position)
			: AbstractOSMPrimitive(controller, group, position) {}

		virtual int64_t lat() = 0;
		virtual int64_t lon() = 0;
	};

	class AbstractOSMWay : public AbstractOSMPrimitive {
	public:
		AbstractOSMWay()
			: AbstractOSMPrimitive() {}
		AbstractOSMWay(OSMPrimitiveBlockController * controller, const PrimitiveGroup * group, int position)
			: AbstractOSMPrimitive(controller, group, position) {}

		virtual int64_t ref(int index) = 0;
		virtual int refsSize() const = 0;
	};

	class AbstractOSMRelation : public AbstractOSMPrimitive {
	public:
		AbstractOSMRelation()
			: AbstractOSMPrimitive() {}
		AbstractOSMRelation(OSMPrimitiveBlockController * controller, const PrimitiveGroup * group, int position)
			: AbstractOSMPrimitive(controller, group, position) {}

		enum MemberType {NODE = 0, WAY = 1, RELATION = 2};

		virtual int32_t roles_sid(int index) const = 0;
		virtual int64_t memberId(int index) = 0;
		virtual MemberType memberType(int index) const = 0;
	};
}
#endif // OSMPBF_ABSTRACTOSMPRIMITIVE_H
