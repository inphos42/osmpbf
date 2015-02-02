#ifndef OSMPBF_XML_CONVERTER_H
#define OSMPBF_XML_CONVERTER_H
#include <ostream>

namespace osmpbf
{

class INode;
class IWay;
class IRelation;

struct XmlConverter
{
	static std::ostream & print(std::ostream & out, const INode & node);
	static std::ostream & print(std::ostream & out, const IWay & way);
	static std::ostream & print(std::ostream & out, const IRelation & relation);
};

} // namespace osmpbf

#endif // OSMPBF_XML_CONVERTER_H
