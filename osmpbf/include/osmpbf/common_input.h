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

#ifndef OSMPBF_COMMON_INPUT_H
#define OSMPBF_COMMON_INPUT_H

#include <osmpbf/common.h>

#include <generics/fielditerator.h>

namespace osmpbf
{

class IPrimitive;
class INode;
class IWay;
class IRelation;
class IInfo;

class INodeStream;
class IWayStream;
class IRelationStream;

class PrimitiveBlockInputAdaptor;
class AbstractPrimitiveInputAdaptor;
class AbstractNodeInputAdaptor;
class WayInputAdaptor;
class RelationInputAdaptor;

typedef generics::DeltaFieldConstForwardIterator<int64_t> RefIterator;

} // namespace osmpbf

#endif // OSMPBF_COMMON_INPUT_H
