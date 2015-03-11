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

#include "osmpbf/iway.h"

namespace osmpbf
{

// IWay

IWay::IWay() : IPrimitive() {}
IWay::IWay(const IWay & other) : IPrimitive(other) {}

IWay &IWay::operator=(const IWay & other) { IPrimitive::operator=(other); return *this; }

IWay::IWay(WayInputAdaptor * data) : IPrimitive(data) {}

// IWayStream

IWayStream::IWayStream() : IWay() {}
IWayStream::IWayStream(const IWayStream & other) : IWay(other) {}

IWayStream &IWayStream::operator=(IWayStream & other) { IWay::operator=(other); return *this; }

IWayStream::IWayStream(PrimitiveBlockInputAdaptor * controller) : IWay(new WayStreamInputAdaptor(controller)) {}

} // namespace osmpbf
