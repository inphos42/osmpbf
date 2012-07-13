#ifndef OSMPBF_PRIMITIVEBLOCKINPUTADAPTOR_H
#define OSMPBF_PRIMITIVEBLOCKINPUTADAPTOR_H

#include <cstdint>
#include <string>

namespace crosby {
namespace binary {
	class PrimitiveBlock;
	class PrimitiveGroup;
}
}

namespace osmpbf {
	class INode;
	class IWay;
	class IRelation;

	class INodeStream;
	class IWayStream;
	class IRelationStream;

	class PrimitiveBlockInputAdaptor {
	public:
		PrimitiveBlockInputAdaptor();
		PrimitiveBlockInputAdaptor(char * rawData, uint32_t length, bool unpackDense = false);
		virtual ~PrimitiveBlockInputAdaptor();

		void parseData(char * rawData, uint32_t length, bool unpackDense = false);

		const std::string & queryStringTable(int id) const;
		int stringTableSize() const;

		INode getNodeAt(int position) const;
		int nodesSize() const;

		IWay getWayAt(int position) const;
		int waysSize() const;

		IRelation getRelationAt(int position) const;
		int relationsSize() const;

		INodeStream getNodeStream();
		IWayStream getWayStream();
		IRelationStream getRelationStream();

		bool isNull() const {
			return !(m_PrimitiveBlock && (
				m_PlainNodesGroup ||
				m_WaysGroup ||
				m_DenseNodesGroup ||
				m_RelationsGroup));
		}

		int32_t granularity() const;

		int64_t latOffset() const;
		int64_t lonOffset() const;

		inline int64_t toWGS84Lati(int64_t rawValue) const {
			return (latOffset() + (granularity() * rawValue));
		}

		inline int64_t toWGS84Loni(int64_t rawValue) const {
			return (lonOffset() + (granularity() * rawValue));
		}

		inline double toWGS84Latd(int64_t rawValue) const {
			return toWGS84Lati(rawValue) * .000000001;
		}

		inline double toWGS84Lond(int64_t rawValue) const {
			return toWGS84Loni(rawValue) * .000000001;
		}

		void unpackDenseNodes();
		inline bool denseNodesUnpacked() const { return m_DenseNodesUnpacked; }
	private:
		friend class PlainNodeInputAdaptor;
		friend class DenseNodeInputAdaptor;
		friend class NodeStreamInputAdaptor;

		friend class WayInputAdaptor;
		friend class WayStreamInputAdaptor;

		friend class RelationInputAdaptor;
		friend class RelationStreamInputAdaptor;

		crosby::binary::PrimitiveBlock * m_PrimitiveBlock;

		crosby::binary::PrimitiveGroup * m_PlainNodesGroup;
		crosby::binary::PrimitiveGroup * m_DenseNodesGroup;
		crosby::binary::PrimitiveGroup * m_WaysGroup;
		crosby::binary::PrimitiveGroup * m_RelationsGroup;

		bool m_DenseNodesUnpacked;
		int * m_DenseNodeKeyValIndex;

		inline int queryDenseNodeKeyValIndex(int index) {
			if (!m_DenseNodeKeyValIndex) buildDenseNodeKeyValIndex();
			return m_DenseNodeKeyValIndex[index];
		}

		void buildDenseNodeKeyValIndex();
	};
}

#endif // OSMPBF_PRIMITIVEBLOCKINPUTADAPTOR_H
