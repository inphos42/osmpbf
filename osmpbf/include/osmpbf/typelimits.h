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

#ifndef OSMPBF_TYPELIMITS_H
#define OSMPBF_TYPELIMITS_H

#include <cstdint>

namespace osmpbf
{

#ifdef __LP64__
typedef uint64_t SizeType;
typedef int64_t SignedSizeType;
typedef int64_t OffsetType;
#else
typedef uint32_t SizeType;
typedef int32_t SignedSizeType;
typedef uint32_t OffsetType;
#endif

} // namespace osmpbf

#endif // OSMPBF_TYPELIMITS_H
