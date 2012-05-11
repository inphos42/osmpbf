#ifndef OSMPBF_PRIMITIVEBLOCKINPUTADAPTOR_H
#define OSMPBF_PRIMITIVEBLOCKINPUTADAPTOR_H

#include <cstdint>
#include <string>

#include "abstractprimitiveadaptor.h"

#include "inode.h"
#include "iway.h"

class PrimitiveBlock;
class PrimitiveGroup;

namespace osmpbf {
	class PrimitiveBlockInputAdaptor {
	public:
		PrimitiveBlockInputAdaptor(char * rawData, uint32_t length, bool unpackDense = false);
		virtual ~PrimitiveBlockInputAdaptor();

		const std::string & queryStringTable(int id) const;
		int stringTableSize() const;

		INode getNodeAt(int position) const;
		int nodesSize() const;

		IWay getWayAt(int position) const;
		int waysSize() const;

		INodeStream getNodeStream();
		IWayStream getWayStream();

		bool isNull() const {
			return !(m_PrimitiveBlock && (
				m_NodesGroup ||
				m_WaysGroup ||
				m_DenseNodesGroup ||
				m_RelationsGroup));
		}

		int32_t granularity() const;

		int64_t latOffset() const;
		int64_t lonOffset() const;

		inline double toWGS84Lati(int64_t rawValue) const {
			return (latOffset() + (granularity() * rawValue));
		}

		inline double toWGS84Loni(int64_t rawValue) const {
			return (lonOffset() + (granularity() * rawValue));
		}

		inline int64_t toWGS84Latd(int64_t rawValue) const {
			return toWGS84Lati(rawValue) * .000000001;
		}

		inline int64_t toWGS84Lond(int64_t rawValue) const {
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

		PrimitiveBlock * m_PrimitiveBlock;

		PrimitiveGroup * m_NodesGroup;
		PrimitiveGroup * m_DenseNodesGroup;
		PrimitiveGroup * m_WaysGroup;
		PrimitiveGroup * m_RelationsGroup;

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
