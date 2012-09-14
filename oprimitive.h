#ifndef OSMPBF_OPRIMITIVE_H
#define OSMPBF_OPRIMITIVE_H

#include <generics/refcountobject.h>

namespace osmpbf {

	template < class OutputAdaptor >
	class OPrimitive : public generics::RCWrapper< OutputAdaptor > {
	public:
		OPrimitive(const OPrimitive & other) : generics::RCWrapper< OutputAdaptor >(other) {}

		inline virtual bool isNull() const { return generics::RCWrapper< OutputAdaptor >::isNull() || generics::RCWrapper< OutputAdaptor >::m_Private->isNull(); }

		inline OPrimitive & operator=(const OPrimitive & other) { generics::RCWrapper< OutputAdaptor >::operator=(other); return *this; }

		inline int64_t id() const { return generics::RCWrapper< OutputAdaptor >::m_Private->id(); }
		inline void setId(int64_t value) { generics::RCWrapper< OutputAdaptor >::m_Private->setId(value); }

		inline int tagsSize() const { return generics::RCWrapper< OutputAdaptor >::m_Private->tagsSize(); }

		inline const std::string & key(int index) { return generics::RCWrapper< OutputAdaptor >::m_Private->key(index); }
		inline const std::string & value(int index) { return generics::RCWrapper< OutputAdaptor >::m_Private->value(index); }

		inline void addTag(const std::string & key, const std::string & value) { generics::RCWrapper< OutputAdaptor >::m_Private->addTag(key, value); }
		inline void removeTagLater(int index) { generics::RCWrapper< OutputAdaptor >::m_Private->removeTagLater(index); }

		inline void clearTags() { generics::RCWrapper< OutputAdaptor >::m_Private->clearTags(); }

	protected:
		OPrimitive() : generics::RCWrapper< OutputAdaptor >() {}
		OPrimitive(OutputAdaptor * data) : generics::RCWrapper< OutputAdaptor >(data) {}
	};

}

#endif
