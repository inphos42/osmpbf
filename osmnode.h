#ifndef OSMPBF_OSMNODE_H
#define OSMPBF_OSMNODE_H

#include <cstdint>
#include <string>

#include "abstractosmprimitive.h"

namespace osmpbf {
	class OSMPrimitiveBlockController;

	class OSMNode {
	public:
		OSMNode() : m_Private(NULL) {}
		OSMNode(const OSMNode & other) : m_Private(other.m_Private) { if (m_Private) m_Private->refInc(); }
		virtual ~OSMNode() { if (m_Private) m_Private->refDec(); }

		OSMNode & operator=(const OSMNode & other) {
			if (m_Private)
				m_Private->refDec();
			m_Private = other.m_Private;
			if (m_Private)
				m_Private->refInc();
			return *this;
		}

		bool operator==(const OSMNode & other) { return m_Private == other.m_Private; }

		inline bool isNull() const { return !m_Private || m_Private->isNull(); }

		inline int64_t id() const { return m_Private->id(); }

		inline int64_t lat() const { return m_Private->lat(); }
		inline int64_t lon() const { return m_Private->lon(); }

		inline int keysSize() const { return m_Private->keysSize(); }
		inline std::string key(int index) const { return m_Private->key(index); }
		inline std::string value(int index) const { return m_Private->value(index); }
// 		inline std::string value(std::string key) const { return m_Private->value(key); }
	private:
		friend class OSMPrimitiveBlockController;

		class AbstractOSMNodeAdaptor : public AbstractOSMPrimitiveAdaptor {
		public:
			AbstractOSMNodeAdaptor() : AbstractOSMPrimitiveAdaptor() {}
			AbstractOSMNodeAdaptor(OSMPrimitiveBlockController * controller, PrimitiveGroup * group, int position)
				: AbstractOSMPrimitiveAdaptor(controller, group, position) {}

			virtual int64_t lat() = 0;
			virtual int64_t lon() = 0;
		};

		class OSMPlainNodeAdaptor : public AbstractOSMNodeAdaptor {
		public:
			OSMPlainNodeAdaptor();
			OSMPlainNodeAdaptor(OSMPrimitiveBlockController * controller, PrimitiveGroup * group, int position);

			virtual int64_t id();

			virtual int64_t lat();
			virtual int64_t lon();

			virtual int keysSize();

			virtual std::string key(int index);
			virtual std::string value(int index);

// 			virtual std::string value(std::string key);
		};

		class OSMDenseNodeAdaptor : public AbstractOSMNodeAdaptor {
		public:
			OSMDenseNodeAdaptor();
			OSMDenseNodeAdaptor(OSMPrimitiveBlockController * controller, PrimitiveGroup * group, int position);

			virtual int64_t id();

			virtual int64_t lat();
			virtual int64_t lon();

			virtual int keysSize();

			virtual std::string key(int index);
			virtual std::string value(int index);

// 			virtual std::string value(std::string key);
		private:
			bool m_HasCachedId;
			bool m_HasCachedLat;
			bool m_HasCachedLon;

			int64_t m_CachedId;
			int64_t m_CachedLat;
			int64_t m_CachedLon;
		};

		OSMNode(AbstractOSMNodeAdaptor * data) : m_Private(data) { if (m_Private) m_Private->refInc(); }

		AbstractOSMNodeAdaptor * m_Private;
	};
}
#endif // OSMPBF_OSMNODE_H
