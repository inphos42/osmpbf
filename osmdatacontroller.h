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
	class OSMPrimitiveBlockController;

	class OSMNodeStream {
	public:
		OSMNodeStream(OSMPrimitiveBlockController * controller);
		OSMNodeStream(const OSMNodeStream & other);

		void next();
		void previous();

		inline bool isNull() const { return m_Position < 0 || !m_Controller; }

		inline int64_t id() const { return m_Id; }

		inline int64_t lat() const { return m_Lat; }
		inline int64_t lon() const { return m_Lon; }

		int keysSize() const;
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
	};

	class OSMPrimitiveBlockController {
	public:
		OSMPrimitiveBlockController(char * rawData, uint32_t length, bool unpackDense = false);
		virtual ~OSMPrimitiveBlockController();

		std::string queryStringTable(int id) const;

		OSMNode getNodeAt(int position) const;
		int nodesSize() const;

		OSMWay getWayAt(int position) const;
		int waysSize() const;

		OSMNodeStream getNodeStream();

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

		void unpackDenseNodes();
	private:
		friend class OSMNodeStream;

		friend class OSMNode::OSMPlainNodeAdaptor;
		friend class OSMNode::OSMDenseNodeAdaptor;
		friend class OSMWay::OSMWayAdaptor;

		PrimitiveBlock * m_PBFPrimitiveBlock;

		PrimitiveGroup * m_NodesGroup;
		PrimitiveGroup * m_DenseNodesGroup;
		PrimitiveGroup * m_WaysGroup;
		PrimitiveGroup * m_RelationsGroup;

		bool m_DenseNodesUnpacked;
		int * m_DenseNodeKeyValIndex;

		void buildDenseNodeKeyValIndex();
	};
}

#endif
