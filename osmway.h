#ifndef OSMPBF_OSMWAY_H
#define OSMPBF_OSMWAY_H

#include <cstdint>
#include <string>

#include "abstractosmprimitive.h"
#include "fielditerator.h"

namespace osmpbf {
	class OSMPrimitiveBlockController;

	class OSMWay {
	public:
		OSMWay() : m_Private(NULL) {};
		OSMWay(const OSMWay & other) : m_Private(other.m_Private) { if (m_Private) m_Private->refInc(); }
		virtual ~OSMWay() { if (m_Private) m_Private->refDec(); }

		OSMWay & operator=(const OSMWay & other) {
			if (m_Private)
				m_Private->refDec();
			m_Private = other.m_Private;
			if (m_Private)
				m_Private->refInc();
			return *this;
		}

		bool operator==(const OSMWay & other) { return m_Private == other.m_Private; }

		inline bool isNull() const { return !m_Private || m_Private->isNull(); }

		inline int64_t id() const { return m_Private->id(); }

		inline int64_t ref(int index) const { return m_Private->ref(index); }
		inline int64_t rawRef(int index) const { return m_Private->rawRef(index); }
		inline int refsSize() const { return m_Private->refsSize(); }
		inline DeltaFieldConstForwardIterator<int64_t> refBegin() const { return m_Private->refBegin(); }
		inline DeltaFieldConstForwardIterator<int64_t> refEnd() const { return m_Private->refEnd(); }

		inline int keysSize() const { return m_Private->keysSize(); }
		inline std::string key(int index) const { return m_Private->key(index); }
		inline std::string value(int index) const { return m_Private->value(index); }
// 		inline std::string value(std::string key) const { return m_Private->value(key); }

	private:
		friend class OSMPrimitiveBlockController;

		class OSMWayAdaptor : public AbstractOSMPrimitiveAdaptor {
		public:
			OSMWayAdaptor();
			OSMWayAdaptor(OSMPrimitiveBlockController * controller, PrimitiveGroup * group, int position);

			virtual int64_t id();

			virtual int keysSize();

			virtual int64_t ref(int index);
			virtual int64_t rawRef(int index);
			virtual int refsSize() const;

			virtual std::string key(int index);
			virtual std::string value(int index);
// 			virtual std::string value(std::string key);

			DeltaFieldConstForwardIterator<int64_t> refBegin() const;
			DeltaFieldConstForwardIterator<int64_t> refEnd() const;
		};

		OSMWay(OSMWayAdaptor * data) : m_Private(data) { if (m_Private) m_Private->refInc(); }

		OSMWayAdaptor * m_Private;
	};
}
#endif // OSMPBF_OSMWAY_H
