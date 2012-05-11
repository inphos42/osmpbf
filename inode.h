#ifndef OSMPBF_INNODE_H
#define OSMPBF_INNODE_H

#include <cstdint>
#include <string>

#include "abstractprimitiveadaptor.h"

namespace osmpbf {
	class PrimitiveBlockInputAdaptor;

	class AbstractNodeInputAdaptor : public AbstractPrimitiveInputAdaptor {
	public:
		AbstractNodeInputAdaptor() : AbstractPrimitiveInputAdaptor() {}
		AbstractNodeInputAdaptor(PrimitiveBlockInputAdaptor * controller, PrimitiveGroup * group, int position)
			: AbstractPrimitiveInputAdaptor(controller, group, position) {}

		virtual int64_t lati() = 0;
		virtual int64_t loni() = 0;

		virtual double latd() = 0;
		virtual double lond() = 0;

		virtual int64_t rawLat() const = 0;
		virtual int64_t rawLon() const = 0;
	};

	class INode {
		friend class PrimitiveBlockInputAdaptor;
	public:
		INode();
		INode(const INode & other);
		virtual ~INode();

		INode & operator=(const INode & other);

		inline bool operator==(const INode & other) { return m_Private == other.m_Private; }

		inline bool isNull() const { return !m_Private || m_Private->isNull(); }

		inline int64_t id() const { return m_Private->id(); }

		inline int64_t lati() const { return m_Private->lati(); }
		inline int64_t loni() const { return m_Private->loni(); }

		inline double latd() const { return m_Private->latd(); }
		inline double lond() const { return m_Private->lond(); }

		inline int64_t rawLat() const { return m_Private->rawLat(); }
		inline int64_t rawLon() const { return m_Private->rawLon(); }

		inline int keysSize() const { return m_Private->keysSize(); }

		inline int keyId(int index) const { return m_Private->keyId(index); }
		inline int valueId(int index) const { return m_Private->valueId(index); }

		inline const std::string & key(int index) const { return m_Private->key(index); }
		inline const std::string & value(int index) const { return m_Private->value(index); }

	protected:
		INode(AbstractNodeInputAdaptor * data);

		AbstractNodeInputAdaptor * m_Private;
	};

	class PlainNodeInputAdaptor : public AbstractNodeInputAdaptor {
	public:
		PlainNodeInputAdaptor();
		PlainNodeInputAdaptor(PrimitiveBlockInputAdaptor * controller, PrimitiveGroup * group, int position);

		virtual int64_t id();

		virtual int64_t lati();
		virtual int64_t loni();

		virtual double latd();
		virtual double lond();

		virtual int64_t rawLat() const;
		virtual int64_t rawLon() const;

		virtual int keysSize() const;

		virtual int keyId(int index) const;
		virtual int valueId(int index) const;

		virtual const std::string & key(int index) const;
		virtual const std::string & value(int index) const;
	};

	class DenseNodeInputAdaptor : public AbstractNodeInputAdaptor {
	public:
		DenseNodeInputAdaptor();
		DenseNodeInputAdaptor(PrimitiveBlockInputAdaptor * controller, PrimitiveGroup * group, int position);

		virtual int64_t id();

		virtual int64_t lati();
		virtual int64_t loni();

		virtual double latd();
		virtual double lond();

		virtual int64_t rawLat() const;
		virtual int64_t rawLon() const;

		virtual int keysSize() const;

		virtual int keyId(int index) const;
		virtual int valueId(int index) const;

		virtual const std::string & key(int index) const;
		virtual const std::string & value(int index) const;

	public:
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

		void next();
		void previous();

		virtual bool isNull() const { return !m_Controller || !(m_Group || m_DenseGroup) || (m_Index < 0) || m_Index > m_NodesSize + m_DenseNodesSize - 1; }

		virtual int64_t id() { return m_Id; }

		virtual int64_t lati() { return m_WGS84Lat; }
		virtual int64_t loni() { return m_WGS84Lon; }

		virtual double latd() { return m_WGS84Lat * .000000001; }
		virtual double lond() { return m_WGS84Lon * .000000001; }

		virtual int64_t rawLat() const { return m_Lat; }
		virtual int64_t rawLon() const { return m_Lon; }

		virtual int keysSize() const;

		virtual int keyId(int index) const;
		virtual int valueId(int index) const;

		virtual const std::string & key(int index) const;
		virtual const std::string & value(int index) const;

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

	class INodeStream : public INode {
	public:
		INodeStream(PrimitiveBlockInputAdaptor * controller);
		INodeStream(const INodeStream & other);

		inline void next() { static_cast<NodeStreamInputAdaptor *>(m_Private)->next(); }
		inline void previous() { static_cast<NodeStreamInputAdaptor *>(m_Private)->previous(); }
	};
}
#endif // OSMPBF_INNODE_H
