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

#include "osmpbf/irelation.h"

namespace osmpbf
{

// IRelation

IRelation::IRelation() : IPrimitive() {}
IRelation::IRelation(RelationInputAdaptor * data) : IPrimitive(data) {}
IRelation::IRelation(const IRelation & other) : IPrimitive(other) {}

IRelation & IRelation::operator=(const IRelation & other) { IPrimitive::operator=(other); return *this; }

// IRelationStream

IRelationStream::IRelationStream(PrimitiveBlockInputAdaptor * controller) : IRelation(new RelationStreamInputAdaptor(controller)) {}
IRelationStream::IRelationStream(const IRelationStream & other) : IRelation(other) {}

IRelationStream & IRelationStream::operator=(const IRelationStream & other) { IRelation::operator=(other); return *this; }

} // namespace osmpbf
