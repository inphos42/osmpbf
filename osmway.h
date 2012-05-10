#ifndef OSMPBF_OSMWAY_H
#define OSMPBF_OSMWAY_H

#include <cstdint>
#include <string>

#include "abstractosmprimitive.h"
#include "fielditerator.h"

namespace osmpbf {
	class OSMPrimitiveBlockInputAdaptor;

	class OSMWayAdaptor : public AbstractOSMPrimitiveAdaptor {
	public:
		OSMWayAdaptor();
		OSMWayAdaptor(OSMPrimitiveBlockInputAdaptor * controller, PrimitiveGroup * group, int position);

		virtual int64_t id();

		virtual int refsSize() const;
		virtual int64_t rawRef(int index) const;

		// warning: This methods complexity is O(n). It's here for convenience. You shouldn't
		//          call this method very often or with a high index parameter.
		virtual int64_t ref(int index) const;

		DeltaFieldConstForwardIterator<int64_t> refBegin() const;
		DeltaFieldConstForwardIterator<int64_t> refEnd() const;

		virtual int keysSize() const;

		virtual int keyId(int index) const;
		virtual int valueId(int index) const;

		virtual std::string key(int index) const;
		virtual std::string value(int index) const;
	};

	class OSMWay {
		friend class OSMPrimitiveBlockInputAdaptor;
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

		inline int keyId(int index) const { return m_Private->keyId(index); }
		inline int valueId(int index) const { return m_Private->valueId(index); }

		inline std::string key(int index) const { return m_Private->key(index); }
		inline std::string value(int index) const { return m_Private->value(index); }

	protected:
		OSMWayAdaptor * m_Private;
		OSMWay(OSMWayAdaptor * data) : m_Private(data) { if (m_Private) m_Private->refInc(); }
	};

	class OSMStreamWayAdaptor : public OSMWayAdaptor {
	public:
		OSMStreamWayAdaptor();
		OSMStreamWayAdaptor(OSMPrimitiveBlockInputAdaptor * controller);

		virtual bool isNull() const;

		void next();
		void previous();

	private:
		int m_WaysSize;
	};

	class OSMStreamWay : public OSMWay {
	public:
		OSMStreamWay(OSMPrimitiveBlockInputAdaptor * controller);
		OSMStreamWay(const OSMStreamWay & other);

		inline void next() { static_cast<OSMStreamWayAdaptor *>(m_Private)->next(); }
		inline void previous() { static_cast<OSMStreamWayAdaptor *>(m_Private)->previous(); }
	};
}
#endif // OSMPBF_OSMWAY_H
