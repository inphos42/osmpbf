#ifndef OSMPBF_FIELDITERATOR_H
#define OSMPBF_FIELDITERATOR_H

namespace osmpbf {
	template<typename Element>
	class FieldIterator {
	public:
		FieldIterator() : m_Data(NULL) {}
		FieldIterator(Element * data) : m_Data(data) {}
		FieldIterator(const FieldIterator<Element> & other) : m_Data(other.m_Data) {}

		inline FieldIterator<Element> & operator=(const FieldIterator<Element> & other) {
			m_Data = other.m_Data;

			return *this;
		}

		inline bool operator==(const FieldIterator<Element> & other) { return (m_Data == other.m_Data); }
		inline bool operator!=(const FieldIterator<Element> & other) { return (m_Data != other.m_Data); }

		inline Element & operator*() { return *m_Data; }

		inline FieldIterator<Element> & operator++() { ++m_Data; return *this; }
		inline FieldIterator<Element> operator++(int) { return FieldIterator<Element>(m_Data++); }
		inline FieldIterator<Element> & operator--() { --m_Data; return *this; }
		inline FieldIterator<Element> operator--(int) { return FieldIterator<Element>(m_Data--); }

		inline bool isNull() const { return !m_Data; }

	protected:
		Element * m_Data;
	};

	template<typename Element>
	class FieldConstIterator {
	public:
		FieldConstIterator() : m_Data(NULL) {}
		FieldConstIterator(const Element * data) : m_Data(data) {}
		FieldConstIterator(const FieldConstIterator<Element> & other) : m_Data(other.m_Data) {}

		inline FieldConstIterator<Element> & operator=(const FieldConstIterator<Element> & other) {
			m_Data = other.m_Data;

			return *this;
		}

		inline bool operator==(const FieldConstIterator<Element> & other) { return (m_Data == other.m_Data); }
		inline bool operator!=(const FieldConstIterator<Element> & other) { return (m_Data != other.m_Data); }

		inline const Element & operator*() const { return *m_Data; }

		inline FieldConstIterator<Element> & operator++() { ++m_Data; return *this; }
		inline FieldConstIterator<Element> operator++(int) { return FieldConstIterator<Element>(m_Data++); }
		inline FieldConstIterator<Element> & operator--() { --m_Data; return *this; }
		inline FieldConstIterator<Element> operator--(int) { return FieldConstIterator<Element>(m_Data--); }

		inline bool isNull() const { return !m_Data; }

	protected:
		const Element * m_Data;
	};

	template<typename Element>
	class DeltaFieldConstForwardIterator {
	public:
		DeltaFieldConstForwardIterator() : m_Data(NULL), m_PreviousSum(0) {}
		DeltaFieldConstForwardIterator(const Element * data) : m_Data(data), m_PreviousSum(0) {}
		DeltaFieldConstForwardIterator(const DeltaFieldConstForwardIterator<Element> & other) :
			m_Data(other.m_Data), m_PreviousSum(other.m_PreviousSum) {}

		inline DeltaFieldConstForwardIterator<Element> & operator=(const DeltaFieldConstForwardIterator<Element> & other) {
			m_Data = other.m_Data;
			m_PreviousSum = other.m_PreviousSum;

			return *this;
		}

		inline bool operator==(const DeltaFieldConstForwardIterator<Element> & other) {
			return (m_Data == other.m_Data) && (m_PreviousSum == other.m_PreviousSum);
		}
		inline bool operator!=(const DeltaFieldConstForwardIterator<Element> & other) {
			return (m_Data != other.m_Data) || (m_PreviousSum != other.m_PreviousSum);
		}

		inline const Element operator*() const { return *m_Data + m_PreviousSum; }

		inline DeltaFieldConstForwardIterator<Element> & operator++() {
			m_PreviousSum += *m_Data;
			m_Data++;

			return *this;
		}
		inline DeltaFieldConstForwardIterator<Element> operator++(int) {
			DeltaFieldConstForwardIterator<Element> oldSelf = *this;
			this->operator++();
			return oldSelf;
		}

		inline bool isNull() const { return !m_Data; }

	protected:
		const Element * m_Data;
		Element m_PreviousSum;
	};
}

#endif // OSMPBF_FIELDITERATOR_H
