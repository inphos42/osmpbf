#ifndef OSMPBF_OSMNODE_H
#define OSMPBF_OSMNODE_H

#include <cstdint>
#include <string>

#include "abstractosmprimitive.h"

namespace osmpbf {
	class OSMPrimitiveBlockInputAdaptor;

	class AbstractOSMNodeAdaptor : public AbstractOSMPrimitiveAdaptor {
	public:
		AbstractOSMNodeAdaptor() : AbstractOSMPrimitiveAdaptor() {}
		AbstractOSMNodeAdaptor(OSMPrimitiveBlockInputAdaptor * controller, PrimitiveGroup * group, int position)
			: AbstractOSMPrimitiveAdaptor(controller, group, position) {}

		virtual int64_t lat() = 0;
		virtual int64_t lon() = 0;

		virtual int64_t rawLat() const = 0;
		virtual int64_t rawLon() const = 0;
	};

	class OSMNode {
		friend class OSMPrimitiveBlockInputAdaptor;
	public:
		OSMNode();
		OSMNode(const OSMNode & other);
		virtual ~OSMNode();

		OSMNode & operator=(const OSMNode & other);

		inline bool operator==(const OSMNode & other) { return m_Private == other.m_Private; }

		inline bool isNull() const { return !m_Private || m_Private->isNull(); }

		inline int64_t id() const { return m_Private->id(); }

		inline int64_t lat() const { return m_Private->lat(); }
		inline int64_t lon() const { return m_Private->lon(); }

		inline int64_t rawLat() const { return m_Private->rawLat(); }
		inline int64_t rawLon() const { return m_Private->rawLon(); }

		inline int keysSize() const { return m_Private->keysSize(); }

		inline int keyId(int index) const { return m_Private->keyId(index); }
		inline int valueId(int index) const { return m_Private->valueId(index); }

		inline std::string key(int index) const { return m_Private->key(index); }
		inline std::string value(int index) const { return m_Private->value(index); }

	protected:
		OSMNode(AbstractOSMNodeAdaptor * data);

		AbstractOSMNodeAdaptor * m_Private;
	};

	class OSMPlainNodeAdaptor : public AbstractOSMNodeAdaptor {
	public:
		OSMPlainNodeAdaptor();
		OSMPlainNodeAdaptor(OSMPrimitiveBlockInputAdaptor * controller, PrimitiveGroup * group, int position);

		virtual int64_t id();

		virtual int64_t lat();
		virtual int64_t lon();

		virtual int64_t rawLat() const;
		virtual int64_t rawLon() const;

		virtual int keysSize() const;

		virtual int keyId(int index) const;
		virtual int valueId(int index) const;

		virtual std::string key(int index) const;
		virtual std::string value(int index) const;
	};

	class OSMDenseNodeAdaptor : public AbstractOSMNodeAdaptor {
	public:
		OSMDenseNodeAdaptor();
		OSMDenseNodeAdaptor(OSMPrimitiveBlockInputAdaptor * controller, PrimitiveGroup * group, int position);

		virtual int64_t id();

		virtual int64_t lat();
		virtual int64_t lon();

		virtual int64_t rawLat() const;
		virtual int64_t rawLon() const;

		virtual int keysSize() const;

		virtual int keyId(int index) const;
		virtual int valueId(int index) const;

		virtual std::string key(int index) const;
		virtual std::string value(int index) const;

	public:
		bool m_HasCachedId;
		bool m_HasCachedLat;
		bool m_HasCachedLon;

		int64_t m_CachedId;
		int64_t m_CachedLat;
		int64_t m_CachedLon;
	};

	class OSMStreamNodeAdaptor : public AbstractOSMNodeAdaptor {
	public:
		OSMStreamNodeAdaptor();
		OSMStreamNodeAdaptor(OSMPrimitiveBlockInputAdaptor * controller);

		void next();
		void previous();

		virtual bool isNull() const { return !m_Controller || !(m_Group || m_DenseGroup) || (m_Index < 0) || m_Index > m_NodesSize + m_DenseNodesSize - 1; }

		virtual int64_t id() { return m_Id; }

		virtual int64_t lat() { return m_WGS84Lat; }
		virtual int64_t lon() { return m_WGS84Lon; }

		virtual int64_t rawLat() const { return m_Lat; }
		virtual int64_t rawLon() const { return m_Lon; }

		virtual int keysSize() const;

		virtual int keyId(int index) const;
		virtual int valueId(int index) const;

		virtual std::string key(int index) const;
		virtual std::string value(int index) const;

	private:
		PrimitiveGroup * m_DenseGroup;

		int m_DenseIndex;
		const int m_NodesSize;
		const int m_DenseNodesSize;

		int64_t m_Id;
		int64_t m_Lat;
		int64_t m_Lon;
		int64_t m_WGS84Lat;
		int64_t m_WGS84Lon;
	};

	class OSMStreamNode : public OSMNode {
	public:
		OSMStreamNode(OSMPrimitiveBlockInputAdaptor * controller);
		OSMStreamNode(const OSMStreamNode & other);

		inline void next() { static_cast<OSMStreamNodeAdaptor *>(m_Private)->next(); }
		inline void previous() { static_cast<OSMStreamNodeAdaptor *>(m_Private)->previous(); }
	};
}
#endif // OSMPBF_OSMNODE_H
