#ifndef OSMPBF_INNODE_H
#define OSMPBF_INNODE_H

#include <cstdint>
#include <string>

#include "abstractprimitiveinputadaptor.h"
#include "common.h"
#include "iprimitive.h"

namespace crosby {
namespace binary {
	class Node;
	class DenseNodes;
	class PrimitiveGroup;
}
}

namespace osmpbf {
	class PrimitiveBlockInputAdaptor;

	class AbstractNodeInputAdaptor : public AbstractPrimitiveInputAdaptor {
	public:
		AbstractNodeInputAdaptor()
			: AbstractPrimitiveInputAdaptor() {}
		AbstractNodeInputAdaptor(PrimitiveBlockInputAdaptor * controller)
			: AbstractPrimitiveInputAdaptor(controller) {}

		virtual int64_t lati() = 0;
		virtual int64_t loni() = 0;

		virtual double latd() = 0;
		virtual double lond() = 0;

		virtual int64_t rawLat() const = 0;
		virtual int64_t rawLon() const = 0;

		virtual NodeType type() const = 0;
	};

	class INode : public IPrimitive {
		friend class PrimitiveBlockInputAdaptor;
	public:
		INode(const INode & other);

		inline INode & operator=(const INode & other) { IPrimitive::operator=(other); return *this; }

		inline int64_t lati() const { return dynamic_cast< AbstractNodeInputAdaptor * >(m_Private)->lati(); }
		inline int64_t loni() const { return dynamic_cast< AbstractNodeInputAdaptor * >(m_Private)->loni(); }

		inline double latd() const { return dynamic_cast< AbstractNodeInputAdaptor * >(m_Private)->latd(); }
		inline double lond() const { return dynamic_cast< AbstractNodeInputAdaptor * >(m_Private)->lond(); }

		inline int64_t rawLat() const { return dynamic_cast< AbstractNodeInputAdaptor * >(m_Private)->rawLat(); }
		inline int64_t rawLon() const { return dynamic_cast< AbstractNodeInputAdaptor * >(m_Private)->rawLon(); }

		inline NodeType internalNodeType() const { return dynamic_cast< AbstractNodeInputAdaptor * >(m_Private)->type(); }

	protected:
		INode();
		INode(AbstractNodeInputAdaptor * data);
	};

	class PlainNodeInputAdaptor : public AbstractNodeInputAdaptor {
	public:
		PlainNodeInputAdaptor();
		PlainNodeInputAdaptor(PrimitiveBlockInputAdaptor * controller, const crosby::binary::Node & data);

		virtual bool isNull() const { return AbstractPrimitiveInputAdaptor::isNull() || !m_Data; }

		virtual int64_t id();

		virtual int64_t lati();
		virtual int64_t loni();

		virtual double latd();
		virtual double lond();

		virtual int64_t rawLat() const;
		virtual int64_t rawLon() const;

		virtual int tagsSize() const;

		virtual uint32_t keyId(int index) const;
		virtual uint32_t valueId(int index) const;

		virtual NodeType type() const { return PlainNode; }

	protected:
		const crosby::binary::Node * m_Data;
	};

	class DenseNodeInputAdaptor : public AbstractNodeInputAdaptor {
	public:
		DenseNodeInputAdaptor();
		DenseNodeInputAdaptor(PrimitiveBlockInputAdaptor * controller, const crosby::binary::DenseNodes & data, int index);

		virtual bool isNull() const;

		virtual int64_t id();

		virtual int64_t lati();
		virtual int64_t loni();

		virtual double latd();
		virtual double lond();

		virtual int64_t rawLat() const;
		virtual int64_t rawLon() const;

		virtual int tagsSize() const;

		virtual uint32_t keyId(int index) const;
		virtual uint32_t valueId(int index) const;

		virtual NodeType type() const { return DenseNode; }

	protected:
		const crosby::binary::DenseNodes * m_Data;
		int m_Index;

		bool m_HasCachedId;
		bool m_HasCachedLat;
		bool m_HasCachedLon;

		int64_t m_CachedId;
		int64_t m_CachedLat;
		int64_t m_CachedLon;
	};

	class NodeStreamInputAdaptor : public AbstractNodeInputAdaptor {
	public:
		NodeStreamInputAdaptor();
		NodeStreamInputAdaptor(PrimitiveBlockInputAdaptor * controller);

		virtual bool isNull() const { return !m_Controller || !(m_PlainNodes || m_DenseNodes) || (m_Index < 0) || m_Index >= m_PlainNodesSize + m_DenseNodesSize; }

		virtual int64_t id() { return m_Id; }

		virtual int tagsSize() const;

		virtual uint32_t keyId(int index) const;
		virtual uint32_t valueId(int index) const;

		virtual int64_t lati() { return m_WGS84Lat; }
		virtual int64_t loni() { return m_WGS84Lon; }

		virtual double latd() { return m_WGS84Lat * .000000001; }
		virtual double lond() { return m_WGS84Lon * .000000001; }

		virtual int64_t rawLat() const { return m_Lat; }
		virtual int64_t rawLon() const { return m_Lon; }

		virtual NodeType type() const { return (m_DenseIndex > -1 ? DenseNode : PlainNode); }

		void next();
		void previous();

	private:
		crosby::binary::PrimitiveGroup * m_PlainNodes;
		crosby::binary::PrimitiveGroup * m_DenseNodes;

		int m_Index;
		int m_DenseIndex;

		const int m_PlainNodesSize;
		const int m_DenseNodesSize;

		int64_t m_Id;
		int64_t m_Lat;
		int64_t m_Lon;
		int64_t m_WGS84Lat;
		int64_t m_WGS84Lon;
	};

	class INodeStream : public INode {
	public:
		INodeStream(PrimitiveBlockInputAdaptor * controller);
		INodeStream(const INodeStream & other);

		inline INodeStream & operator=(const INodeStream & other) { INode::operator=(other); return *this; }

		inline void next() { dynamic_cast< NodeStreamInputAdaptor * >(m_Private)->next(); }
		inline void previous() { dynamic_cast< NodeStreamInputAdaptor * >(m_Private)->previous(); }
	private:
		INodeStream();
		INodeStream(AbstractNodeInputAdaptor * data);
	};
}
#endif // OSMPBF_INNODE_H
