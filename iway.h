#ifndef OSMPBF_OSMWAY_H
#define OSMPBF_OSMWAY_H

#include <cstdint>
#include <string>

#include "abstractprimitiveadaptor.h"
#include "fielditerator.h"

class Way;

namespace osmpbf {
	class PrimitiveBlockInputAdaptor;
	class PrimitiveBlockOutputAdaptor;

	class WayInputAdaptor : public AbstractPrimitiveInputAdaptor {
	public:
		WayInputAdaptor();
		WayInputAdaptor(PrimitiveBlockInputAdaptor * controller, const Way * data);

		virtual bool isNull() const { return AbstractPrimitiveInputAdaptor::isNull() || !m_Data; }

		virtual int64_t id();

		virtual int refsSize() const;
		virtual int64_t rawRef(int index) const;

		// warning: This methods complexity is O(n). It's here for convenience. You shouldn't
		//          call this method very often or with a high index parameter.
		virtual int64_t ref(int index) const;

		DeltaFieldConstForwardIterator<int64_t> refBegin() const;
		DeltaFieldConstForwardIterator<int64_t> refEnd() const;

		virtual int tagsSize() const;

		virtual int keyId(int index) const;
		virtual int valueId(int index) const;

		virtual const std::string & key(int index) const;
		virtual const std::string & value(int index) const;

	protected:
		const Way * m_Data;
	};

	class IWay : public RCWrapper<osmpbf::WayInputAdaptor> {
		friend class PrimitiveBlockInputAdaptor;
	public:
		IWay(const IWay & other);

		IWay & operator=(const IWay & other);

		inline int64_t id() const { return m_Private->id(); }

		inline int64_t ref(int index) const { return m_Private->ref(index); }
		inline int64_t rawRef(int index) const { return m_Private->rawRef(index); }
		inline int refsSize() const { return m_Private->refsSize(); }

		inline DeltaFieldConstForwardIterator<int64_t> refBegin() const { return m_Private->refBegin(); }
		inline DeltaFieldConstForwardIterator<int64_t> refEnd() const { return m_Private->refEnd(); }

		inline int tagsSize() const { return m_Private->tagsSize(); }

		inline int keyId(int index) const { return m_Private->keyId(index); }
		inline int valueId(int index) const { return m_Private->valueId(index); }

		inline const std::string & key(int index) const { return m_Private->key(index); }
		inline const std::string & value(int index) const { return m_Private->value(index); }

	protected:
		IWay();
		IWay(WayInputAdaptor * data);
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
		int m_Index;
	};

	class IWayStream : public IWay {
	public:
		IWayStream(PrimitiveBlockInputAdaptor * controller);
		IWayStream(const IWayStream & other);

		IWayStream & operator=(IWayStream & other);

		inline void next() { static_cast<WayStreamInputAdaptor *>(m_Private)->next(); }
		inline void previous() { static_cast<WayStreamInputAdaptor *>(m_Private)->previous(); }

	private:
		IWayStream();
		IWayStream(WayInputAdaptor * data);
	};
}
#endif // OSMPBF_OSMWAY_H
