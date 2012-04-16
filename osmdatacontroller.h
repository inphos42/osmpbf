#ifndef OSMPBF_OSMDATACONTROLLER_H
#define OSMPBF_OSMDATACONTROLLER_H

#include <sys/types.h>
#include <cstdint>
#include <string>

#include "abstractosmprimitive.h"
#include "refcountobject.h"

class PrimitiveBlock;
class PrimitiveGroup;
class StringTable;
class Node;
class DenseNodes;
class Way;
class Relation;

namespace osmpbf {
// 	class AbstractOSMDataIndex;
// 	class OSMBlobFile;
// 	
// 	class OSMDataController {
// 	public:
// 		OSMDataController(OSMBlobFile * rawData, AbstractOSMDataIndex * index = NULL);
// 		
// 		OSMNode getNode(int64_t id);
// 		OSMNode getNode(int position);
// 		inline uint32_t nodeCount() const { return m_NodeCount; }
// 		
// 		OSMWay getWay(int64_t id);
// 		OSMWay getWay(int position);
// 		inline uint32_t wayCount() const { return m_WayCount; }
// 	private:
// 		uint32_t m_NodeCount;
// 		uint32_t m_WayCount;
// 		
// 		AbstractOSMDataIndex * m_Index;
// 		OSMBlobFile * m_RawData;
// 	};
	
	class OSMNode {
	public:
		OSMNode() : m_Private(NULL) {};
		OSMNode(AbstractOSMNode * data) : m_Private(data) { if (m_Private) m_Private->refInc(); }
		virtual ~OSMNode() { if (m_Private) m_Private->refDec(); }
		
		bool isNull() const { return !m_Private || m_Private->isNull(); }
		
		inline int64_t id() const { return m_Private->id(); }
		
		inline int64_t lat() const { return m_Private->lat(); }
		inline int64_t lon() const { return m_Private->lon(); }
		
		inline std::string value(std::string key) const { return m_Private->value(key); }
	private:
		AbstractOSMNode * m_Private;
	};
	
	class OSMWay {
	public:
		OSMWay() : m_Private(NULL) {};
		OSMWay(AbstractOSMWay * data) : m_Private(data) { if (m_Private) m_Private->refInc(); }
		virtual ~OSMWay() { if (m_Private) m_Private->refDec(); }
		
		bool isNull() const { return !m_Private || m_Private->isNull(); }
		
		inline int64_t id() const { return m_Private->id(); }
		
		inline int64_t ref(int index) const { return m_Private->ref(index); }
		inline int refsSize() const { return m_Private->refsSize(); }
	private:
		AbstractOSMWay * m_Private;
	};
	
	class OSMPrimitiveBlockController {
		friend class AbstractOSMPrimitive;
	public:
		OSMPrimitiveBlockController(char * rawData, uint32_t length);
		virtual ~OSMPrimitiveBlockController();
		
		std::string queryStringTable(int id);
		
		OSMNode getNodeAt(int position) const;
		OSMNode getNodeById(int64_t id) const;
		int nodesSize() const;
		
		OSMWay getWayAt(int position) const;
		OSMWay getWayById(int64_t id) const;
		int waysSize() const;
		
		inline bool isNull() const {
			return !(m_PBFPrimitiveBlock && (
				m_NodesGroup ||
				m_WaysGroup ||
				m_DenseNodesGroup ||
				m_RelationsGroup));
		}
		
		inline int32_t granularity() const;
		
		inline int64_t latOffset() const;
		inline int64_t lonOffset() const;
	private:
		friend class OSMProtoBufWay;
		friend class OSMProtoBufNode;
		friend class OSMProtoBufDenseNode;
		
		PrimitiveBlock * m_PBFPrimitiveBlock;
		
		const PrimitiveGroup * m_NodesGroup;
		const PrimitiveGroup * m_DenseNodesGroup;
		const PrimitiveGroup * m_WaysGroup;
		const PrimitiveGroup * m_RelationsGroup;
		
		int * m_DenseNodeKeyValIndex;
		
		void buildDenseNodeKeyValIndex();
		
		class OSMProtoBufNode : public AbstractOSMNode {
		public:
			OSMProtoBufNode()
				: AbstractOSMNode() {}
			OSMProtoBufNode(OSMPrimitiveBlockController * controller, const PrimitiveGroup * group, int position)
				: AbstractOSMNode(controller, group, position) {}
			
			virtual int64_t id() const;
			
			virtual int64_t lat() const;
			virtual int64_t lon() const;
			
			virtual std::string key(int index);
			virtual std::string value(int index);
			
			virtual std::string value(std::string key);
		};
		
		class OSMProtoBufDenseNode : public AbstractOSMNode {
		public:
			OSMProtoBufDenseNode()
				: AbstractOSMNode() {}
			OSMProtoBufDenseNode(OSMPrimitiveBlockController * controller, const PrimitiveGroup * group, int position)
				: AbstractOSMNode(controller, group, position) {}
			
			virtual int64_t id() const;
			
			virtual int64_t lat() const;
			virtual int64_t lon() const;
			
			virtual std::string key(int index);
			virtual std::string value(int index);
			
			virtual std::string value(std::string key);
		};
		
		class OSMProtoBufWay : public AbstractOSMWay {
		public:
			OSMProtoBufWay()
				: AbstractOSMWay() {}
			OSMProtoBufWay(OSMPrimitiveBlockController * controller, const PrimitiveGroup * group, int position)
				: AbstractOSMWay(controller, group, position) {}
			
			virtual int64_t id() const;
			
			virtual int64_t ref(int index) const;
			virtual int refsSize() const;
			
			virtual std::string key(int index);
			virtual std::string value(int index);
			
			virtual std::string value(std::string key);
		};
	};
}

#endif
