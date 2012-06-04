#include "abstractprimitiveinputadaptor.h"

#include "primitiveblockinputadaptor.h"

namespace osmpbf {
	const std::string & AbstractPrimitiveInputAdaptor::key(int index) const {
		return m_Controller->queryStringTable(keyId(index));
	}

	const std::string & AbstractPrimitiveInputAdaptor::value(int index) const {
		return m_Controller->queryStringTable(valueId(index));
	}

	const std::string & AbstractPrimitiveInputAdaptor::value(uint32_t key) const {
		for (int i = 0; i < tagsSize(); ++i) {
			if (keyId(i) == key)
				return m_Controller->queryStringTable(valueId(i));
		}

		return m_Controller->queryStringTable(0); // return "null" string
	}

	const std::string & AbstractPrimitiveInputAdaptor::value(const std::string & key) const {
		for (int i = 0; i < tagsSize(); ++i) {
			if (m_Controller->queryStringTable(keyId(i)) == key)
				return m_Controller->queryStringTable(valueId(i));
		}

		return m_Controller->queryStringTable(0); // return "null" string
	}
}
