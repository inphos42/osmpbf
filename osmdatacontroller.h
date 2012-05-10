#ifndef OSMPBF_OSMDATACONTROLLER_H
#define OSMPBF_OSMDATACONTROLLER_H

#include <cstdint>
#include <string>

#include "abstractosmprimitive.h"

#include "osmnode.h"
#include "osmway.h"

class PrimitiveBlock;
class PrimitiveGroup;

namespace osmpbf {
	class OSMPrimitiveBlockInputAdaptor {
	public:
		OSMPrimitiveBlockInputAdaptor(char * rawData, uint32_t length, bool unpackDense = false);
		virtual ~OSMPrimitiveBlockInputAdaptor();

		std::string queryStringTable(int id) const;
		int stringTableSize() const;

		OSMNode getNodeAt(int position) const;
		int nodesSize() const;

		OSMWay getWayAt(int position) const;
		int waysSize() const;

		OSMStreamNode getNodeStream();
		OSMStreamWay getWayStream();

		bool isNull() const {
			return !(m_PBFPrimitiveBlock && (
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
		friend class OSMPlainNodeAdaptor;
		friend class OSMDenseNodeAdaptor;
		friend class OSMStreamNodeAdaptor;
		friend class OSMWayAdaptor;
		friend class OSMStreamWayAdaptor;

		PrimitiveBlock * m_PBFPrimitiveBlock;

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

#endif
