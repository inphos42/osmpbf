#ifndef OSMPBF_OWAY_H
#define OSMPBF_OWAY_H

#include <cstdint>
#include <string>
#include <utility>

#include "common.h"
#include "abstractprimitiveadaptor.h"

namespace osmpbf {
	class PrimitiveBlockOutputAdaptor;

	class OWay {
	public:
		OWay();
		OWay(const OWay & other);
		OWay(PrimitiveBlockOutputAdaptor * controller);

		int64_t id() const;
		void setId(int64_t value);

		int64_t ref(int index) const;
		void setRef(int index, int64_t value);

		void addRef(int64_t ref);
		void removeRef(int index);

		int refsSize() const;

		std::pair<std:string, std::string> & tag(int index);

		void addTag(std::pair<std::string, std::string> & tag);
		void addTag(std::string key, std::string value);
		void removeTag(int index);

		int tagsSize() const;
	};
}

#endif // OSMPBF_OWAY_H
