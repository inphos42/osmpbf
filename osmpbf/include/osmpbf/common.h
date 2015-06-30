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

#ifndef OSMPBF_COMMON_H
#define OSMPBF_COMMON_H

#include <cstdint>

#define NULL_PRIMITIVE_ID -1
#define NULL_STRING_ID 0

#define COORDINATE_SCALE_FACTOR_LAT 0.000000001
#define COORDINATE_SCALE_FACTOR_LON 0.000000001

namespace osmpbf
{

typedef uint8_t NodeTypeFlags;
enum NodeType : NodeTypeFlags
{
	Undefined = 0,
	PlainNode = 1,
	DenseNode = 2
};

typedef uint16_t PrimitiveTypeFlags;
enum PrimitiveType : PrimitiveTypeFlags
{
	NoPrimitive = 0,
	InvalidPrimitive = 0,

	NodePrimitive = 0x1,
	WayPrimitive = 0x2,
	RelationPrimitive = 0x4
};

} // namespace osmpbf

#endif // OSMPBF_COMMON_H
