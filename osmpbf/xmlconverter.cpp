#include <osmpbf/xmlconverter.h>
#include <osmpbf/inode.h>
#include <osmpbf/iway.h>
#include <osmpbf/irelation.h>

namespace osmpbf {

void printTags(std::ostream & out, const IPrimitive & prim, const std::string & prefix) {
	for(uint32_t i = 0, s = prim.tagsSize();  i < s; ++i) {
		out << prefix << "<tag k=" << prim.key(i) << " v=" << prim.value(i) << ">\n";
	}
}

std::string primitiveTypeToString(osmpbf::PrimitiveType t) {
	switch (t) {
	case osmpbf::PrimitiveType::NodePrimitive:
		return "node";
	case osmpbf::PrimitiveType::WayPrimitive:
		return "way";
	case osmpbf::PrimitiveType::RelationPrimitive:
		return "relation";
	default:
		return "invalid";
	};
}

std::ostream & XmlConverter::print(std::ostream & out, const INode & node) {
	out << "<node id=" << node.id() << " lat=" << node.latd() << " lon=" << node.lond()  << ">\n";
	printTags(out, node, "\t");
	out << "</node>";
	return out;
}

std::ostream & XmlConverter::print(std::ostream & out, const IWay & way) {
	out << "<way id=" << way.id() << ">\n";
	for(osmpbf::RefIterator refIt(way.refBegin()), refEnd(way.refEnd()); refIt != refEnd; ++refIt) {
		out << "\t<nd ref=" << *refIt << "/>" << std::endl;
	}
	printTags(out, way, "\t");
	out << "</way>";
	return out;
}

std::ostream & XmlConverter::print(std::ostream & out, const IRelation & relation) {
	out << "<relation id=" << relation.id() << ">\n";
	for(osmpbf::IMemberStream mem(relation.getMemberStream()); !mem.isNull(); mem.next()) {
		out << "\t<member type=" << primitiveTypeToString( mem.type() ) << " ref=" << mem.id() << " role=" << mem.role() << "/>\n";
	}
	printTags(out, relation, "\t");
	out << "</relation>";
	return out;
}

}//end namespace
