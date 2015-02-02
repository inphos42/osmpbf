/*
    This file is part of the osmpbf library.

    Copyright(c) 2012-2014 Oliver Groß.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 3 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, see
    <http://www.gnu.org/licenses/>.
 */

#include <osmpbf/xmlconverter.h>
#include <osmpbf/inode.h>
#include <osmpbf/iway.h>
#include <osmpbf/irelation.h>

namespace osmpbf
{

void printTags(std::ostream & out, const IPrimitive & prim, const std::string & prefix)
{
	for(uint32_t i = 0, s = prim.tagsSize();  i < s; ++i) {
		out << prefix << "<tag k=" << prim.key(i) << " v=" << prim.value(i) << ">\n";
	}
}

std::string primitiveTypeToString(osmpbf::PrimitiveType t)
{
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

std::ostream & XmlConverter::print(std::ostream & out, const INode & node)
{
	out << "<node id=" << node.id() << " lat=" << node.latd() << " lon=" << node.lond()  << ">\n";
	printTags(out, node, "\t");
	out << "</node>";
	return out;
}

std::ostream & XmlConverter::print(std::ostream & out, const IWay & way)
{
	out << "<way id=" << way.id() << ">\n";
	for(osmpbf::RefIterator refIt(way.refBegin()), refEnd(way.refEnd()); refIt != refEnd; ++refIt) {
		out << "\t<nd ref=" << *refIt << "/>" << std::endl;
	}
	printTags(out, way, "\t");
	out << "</way>";
	return out;
}

std::ostream & XmlConverter::print(std::ostream & out, const IRelation & relation)
{
	out << "<relation id=" << relation.id() << ">\n";
	for(osmpbf::IMemberStream mem(relation.getMemberStream()); !mem.isNull(); mem.next()) {
		out << "\t<member type=" << primitiveTypeToString( mem.type() ) << " ref=" << mem.id() << " role=" << mem.role() << "/>\n";
	}
	printTags(out, relation, "\t");
	out << "</relation>";
	return out;
}

} // namespace osmpbf
