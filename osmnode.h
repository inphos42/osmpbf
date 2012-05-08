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

		inline int keyId(int index) const { return m_Private->keyId(index); }
		inline int valueId(int index) const { return m_Private->valueId(index); }

		inline std::string key(int index) const { return m_Private->key(index); }
		inline std::string value(int index) const { return m_Private->value(index); }
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

			virtual int keysSize() const;

			virtual int keyId(int index) const;
			virtual int valueId(int index) const;

			virtual std::string key(int index) const;
			virtual std::string value(int index) const;
		};

		class OSMDenseNodeAdaptor : public AbstractOSMNodeAdaptor {
		public:
			OSMDenseNodeAdaptor();
			OSMDenseNodeAdaptor(OSMPrimitiveBlockController * controller, PrimitiveGroup * group, int position);

			virtual int64_t id();

			virtual int64_t lat();
			virtual int64_t lon();

			virtual int keysSize() const;

			virtual int keyId(int index) const;
			virtual int valueId(int index) const;

			virtual std::string key(int index) const;
			virtual std::string value(int index) const;
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

	class OSMNodeStream {
	public:
		OSMNodeStream(OSMPrimitiveBlockController * controller);
		OSMNodeStream(const OSMNodeStream & other);

		void next();
		void previous();

		inline bool isNull() const { return m_Position < 0 || m_Position > m_NodesSize + m_DenseNodesSize - 1 || !m_Controller; }

		inline int64_t id() const { return m_Id; }

		inline int64_t lat() const { return m_Lat; }
		inline int64_t lon() const { return m_Lon; }

		// return wgs84 coordinates in nanodegrees
		inline int64_t wgs84Lati() const { return m_WGS84Lat; }
		inline int64_t wgs84Loni() const { return m_WGS84Lon; }

		// return wgs84 coordinates in degrees
		inline double wgs84Latd() const { return m_WGS84Lat * .000000001; }
		inline double wgs84Lond() const { return m_WGS84Lon * .000000001; }

		int keysSize() const;

		int keyId(int index) const;
		int valueId(int index) const;

		std::string key(int index) const;
		std::string value(int index) const;
	private:
		OSMNodeStream() : m_NodesSize(0), m_DenseNodesSize(0) {};

		OSMPrimitiveBlockController * m_Controller;

		int m_Position; // [-1, 0 .. m_NodesSize]
		int m_DensePosition;
		const int m_NodesSize;
		const int m_DenseNodesSize;

		int64_t m_Id;
		int64_t m_Lat;
		int64_t m_Lon;
		int64_t m_WGS84Lat;
		int64_t m_WGS84Lon;
	};
}
#endif // OSMPBF_OSMNODE_H
