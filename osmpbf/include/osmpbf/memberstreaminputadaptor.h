#ifndef OSMPBF_MEMBERSTREAMINPUTADAPTOR_H
#define OSMPBF_MEMBERSTREAMINPUTADAPTOR_H

namespace osmpbf
{

class MemberStreamInputAdaptor : public generics::RefCountObject
{
public:
	MemberStreamInputAdaptor();
	explicit MemberStreamInputAdaptor(const crosby::binary::Relation * data);

	bool isNull() const;

	int64_t id() const;
	PrimitiveType type() const;
	uint32_t roleId() const;

	void next();
	void previous();

protected:
	const crosby::binary::Relation * m_Data;

	int m_Index;
	const int m_MaxIndex;

	int64_t m_CachedId;
};

} // namespace osmpbf

#endif // OSMPBF_MEMBERSTREAMINPUTADAPTOR_H
