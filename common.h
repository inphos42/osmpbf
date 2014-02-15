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

#define NULL_PRIMITIVE_ID -1
#define NULL_STRING_ID 0

namespace osmpbf {
	enum NodeType { PlainNode = 1, DenseNode = 2 };
	enum PrimitiveType { NodePrimitive = 0, WayPrimitive = 1, RelationPrimitive = 2 };
}

#endif // OSMPBF_COMMON_H
