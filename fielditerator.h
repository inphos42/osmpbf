#ifndef OSMPBF_FIELDITERATOR_H
#define OSMPBF_FIELDITERATOR_H

namespace osmpbf {
	template<class Element>
	class DeltaFieldConstForwardIterator {
	public:
		DeltaFieldConstForwardIterator() : m_Data(NULL), m_Previous(0) {}
		DeltaFieldConstForwardIterator(const Element * data) : m_Data(const_cast<Element *>(data)), m_Previous(0) {}
		DeltaFieldConstForwardIterator(const DeltaFieldConstForwardIterator<Element> & other) :
			m_Data(other.m_Data), m_Previous(other.m_Previous) {}

		inline DeltaFieldConstForwardIterator<Element> & operator=(const DeltaFieldConstForwardIterator<Element> & other) {
			m_Data = other.m_Data;
			m_Previous = other.m_Previous;

			return *this;
		}

		inline bool operator==(const DeltaFieldConstForwardIterator<Element> & other) { return (m_Data == other.m_Data); }
		inline bool operator!=(const DeltaFieldConstForwardIterator<Element> & other) { return (m_Data != other.m_Data); }

		inline bool isNull() const { return !m_Data; }

		inline const Element operator*() const { return *m_Data + m_Previous; }

		inline DeltaFieldConstForwardIterator<Element> & operator++() {
			m_Previous += *m_Data;
			m_Data++;

			return *this;
		}
	private:
		Element * m_Data;
		Element m_Previous;
	};
}

#endif // OSMPBF_FIELDITERATOR_H
