#ifndef OSMPBF_OWAY_H
#define OSMPBF_OWAY_H

#include <cstdint>
#include <string>
#include <utility>

#include "common.h"
#include "abstractprimitiveadaptor.h"
#include "fielditerator.h"

class Way;

#define NULL_REF -1

namespace osmpbf {
	class PrimitiveBlockOutputAdaptor;

	class WayOutputAdaptor : public AbstractPrimitiveOutputAdaptor {
	public:
		WayOutputAdaptor();
		WayOutputAdaptor(PrimitiveBlockOutputAdaptor * controller, Way * data);

		virtual int64_t id() const;
		virtual void setId(int64_t value);

		virtual int tagsSize() const;

		virtual std::string & key(int index) const;
		virtual std::string & value(int index) const;

		virtual void addTag(const std::string & key, const std::string & value);
		virtual void removeTagLater(int index);

		virtual void clearTags();

		virtual int refsSize() const;

		virtual int64_t ref(int index) const;
		virtual void setRef(int index, int64_t value);

		virtual void addRef(int64_t ref);

		virtual void setRefs(const DeltaFieldConstForwardIterator<int64_t> & from, const DeltaFieldConstForwardIterator<int64_t> & to);
		virtual void setRefs(const FieldConstIterator<int64_t> & from, const FieldConstIterator<int64_t> & to);

		virtual void clearRefs();

	protected:
		Way * m_Data;
	};

	class OWay : public RCWrapper<WayOutputAdaptor> {
		friend class PrimitiveBlockOutputAdaptor;
	public:
		OWay(const OWay & other);

		OWay & operator=(const OWay & other);

		inline int64_t id() const { return m_Private->id(); }
		inline void setId(int64_t value) { m_Private->setId(value); }

		inline int tagsSize() const { return m_Private->tagsSize(); }

		inline std::string & key(int index) { return m_Private->key(index); }
		inline std::string & value(int index) { return m_Private->value(index); }

		inline void addTag(const std::string & key, const std::string & value) { m_Private->addTag(key, value); }
		inline void removeTagLater(int index) { m_Private->removeTagLater(index); }

		inline void clearTags() { m_Private->clearTags(); }

		inline int refsSize() const { return m_Private->refsSize(); }

		inline int64_t ref(int index) const { return m_Private->ref(index); }
		inline void setRef(int index, int64_t value) { m_Private->setRef(index, value); }

		inline void addRef(int64_t ref) { m_Private->addRef(ref); }
		inline void removeRefLater(int index) { m_Private->setRef(index, NULL_REF); }

		inline void setRefs(const DeltaFieldConstForwardIterator<int64_t> & from, const DeltaFieldConstForwardIterator<int64_t> & to) {
			m_Private->setRefs(from, to); }
		inline void setRefs(const FieldConstIterator<int64_t> & from, const FieldConstIterator<int64_t> & to) {
			m_Private->setRefs(from, to); }

		virtual void clearRefs() { m_Private->clearRefs(); }

	protected:
		OWay();
		OWay(WayOutputAdaptor * data);
	};
}

#endif // OSMPBF_OWAY_H
