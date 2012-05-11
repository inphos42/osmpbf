#ifndef OSMPBF_OSMWAY_H
#define OSMPBF_OSMWAY_H

#include <cstdint>
#include <string>

#include "abstractprimitiveadaptor.h"
#include "fielditerator.h"

namespace osmpbf {
	class PrimitiveBlockInputAdaptor;

	class WayInputAdaptor : public AbstractPrimitiveInputAdaptor {
	public:
		WayInputAdaptor();
		WayInputAdaptor(PrimitiveBlockInputAdaptor * controller, PrimitiveGroup * group, int position);

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

		virtual const std::string & key(int index) const;
		virtual const std::string & value(int index) const;
	};

	class IWay {
		friend class PrimitiveBlockInputAdaptor;
	public:
		IWay() : m_Private(NULL) {};
		IWay(const IWay & other) : m_Private(other.m_Private) { if (m_Private) m_Private->refInc(); }
		virtual ~IWay() { if (m_Private) m_Private->refDec(); }

		IWay & operator=(const IWay & other) {
			if (m_Private)
				m_Private->refDec();
			m_Private = other.m_Private;
			if (m_Private)
				m_Private->refInc();
			return *this;
		}

		bool operator==(const IWay & other) { return m_Private == other.m_Private; }

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

		inline const std::string & key(int index) const { return m_Private->key(index); }
		inline const std::string & value(int index) const { return m_Private->value(index); }

	protected:
		WayInputAdaptor * m_Private;

		IWay(WayInputAdaptor * data) : m_Private(data) { if (m_Private) m_Private->refInc(); }
	};

	class WayStreamInputAdaptor : public WayInputAdaptor {
	public:
		WayStreamInputAdaptor();
		WayStreamInputAdaptor(PrimitiveBlockInputAdaptor * controller);

		virtual bool isNull() const;

		void next();
		void previous();

	private:
		int m_WaysSize;
	};

	class IWayStream : public IWay {
	public:
		IWayStream(PrimitiveBlockInputAdaptor * controller);
		IWayStream(const IWayStream & other);

		inline void next() { static_cast<WayStreamInputAdaptor *>(m_Private)->next(); }
		inline void previous() { static_cast<WayStreamInputAdaptor *>(m_Private)->previous(); }
	};
}
#endif // OSMPBF_OSMWAY_H
