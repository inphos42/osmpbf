#include <cstdint>
#include <sys/types.h>
#include <string>

namespace osmpbf {
	class OSMPrimitiveBlockController;
	
	class AbstractOSMPrimitive {
	public:
		AbstractOSMPrimitive() : m_Controller(NULL), m_Position(-1) {};
		AbstractOSMPrimitive(OSMPrimitiveBlockController * controller);
		virtual ~AbstractOSMPrimitive();
		
		inline bool isNull() const { return !m_Controller || (m_Position == -1); };
		
		virtual int64_t id() const = 0;
		
		virtual std::string key(int index) = 0;
		virtual std::string value(int index) = 0;
	protected:
		OSMPrimitiveBlockController * m_Controller;
		int m_Position;
	};
	
	class AbstractOSMNode : AbstractOSMPrimitive {
	public:
		virtual int64_t lat() const = 0;
		virtual int64_t lon() const = 0;
		
		virtual std::string value(std::string key) = 0;
	};
	
	class AbstractOSMWay : public AbstractOSMPrimitive {
	public:
		virtual int64_t ref(int index) const = 0;
		virtual int refCount() const = 0;
		
		virtual std::string value(std::string key) = 0;
	};
	
	class OSMNode {
	public:
		OSMNode() : m_Private(NULL) {};
		OSMNode(AbstractOSMNode * data) : m_Private(data) {};
		~OSMNode() { if (m_Private) delete m_Private; }
		
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
		OSMWay(AbstractOSMWay * data) : m_Private(data) {}
		~OSMWay() { if (m_Private) delete m_Private; }
		
		bool isNull() const { return !m_Private || m_Private->isNull(); }
		
		inline int64_t id() const { return m_Private->id(); }
		
		inline int64_t ref(int index) const { return m_Private->ref(index); }
		inline int refCount() const { return m_Private->refCount(); }
	private:
		AbstractOSMWay * m_Private;
	};
	
	class OSMPrimitiveBlockController {
		friend class AbstractOSMPrimitive;
	public:
		OSMPrimitiveBlockController(char * rawData, uint32_t length);
		
		std::string queryStringTable(int id);
		
		OSMNode getNode(int64_t id);
		OSMNode getNode(int position);
		int nodeCount();
		
		OSMWay getWay(int64_t id);
		OSMWay getWay(int position);
		int wayCount();
		
		int32_t granularity();
		
		int64_t latOffset();
		int64_t lonOffset();
	private:
		class OSMProtoBufWay : public AbstractOSMWay {
		public:
			virtual int64_t id() const;
			
			virtual int64_t lat() const;
			virtual int64_t lon() const;
		
		protected:
			virtual ~OSMProtoBufWay();
		};
		
		class OSMProtoBufNode : public AbstractOSMNode {
		public:
			virtual int64_t id() const;
			
			virtual int64_t lat() const;
			virtual int64_t lon() const;
		
		protected:
			virtual ~OSMProtoBufNode();
		};
		
		class OSMProtoBufDenseNode : public AbstractOSMNode {
		public:
			virtual int64_t id() const;
			
			virtual int64_t lat() const;
			virtual int64_t lon() const;
		protected:
			virtual ~OSMProtoBufDenseNode();
		};
	};
	
	class OSMDataController {
	public:
		OSMNode getNode(int64_t id);
		OSMNode getNode(int position);
		inline uint32_t nodeCount() const { return m_NodeCount; }
		
		OSMWay getWay(int64_t id);
		OSMWay getWay(int position);
		inline uint32_t wayCount() const { return m_WayCount; }
	private:
		uint32_t m_NodeCount;
		uint32_t m_WayCount;
	};
}
