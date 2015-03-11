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

#ifndef OSMPBF_XML_CONVERTER_H
#define OSMPBF_XML_CONVERTER_H

#include <osmpbf/common_input.h>

#include <ostream>

namespace osmpbf
{

struct XmlConverter
{
	static std::ostream & print(std::ostream & out, const INode & node);
	static std::ostream & print(std::ostream & out, const IWay & way);
	static std::ostream & print(std::ostream & out, const IRelation & relation);
};

} // namespace osmpbf

#endif // OSMPBF_XML_CONVERTER_H
