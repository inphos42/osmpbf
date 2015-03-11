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

#include "osmpbf/abstractprimitiveinputadaptor.h"

#include <osmpbf/primitiveblockinputadaptor.h>

namespace osmpbf
{

const std::string & AbstractPrimitiveInputAdaptor::key(int index) const
{
	return m_Controller->queryStringTable(keyId(index));
}

const std::string & AbstractPrimitiveInputAdaptor::value(int index) const
{
	return m_Controller->queryStringTable(valueId(index));
}

const std::string & AbstractPrimitiveInputAdaptor::valueByKeyId(uint32_t key) const
{
	for (int i = 0; i < tagsSize(); ++i)
	{
		if (keyId(i) == key)
			return m_Controller->queryStringTable(valueId(i));
	}

	return m_Controller->queryStringTable(NULL_STRING_ID); // return "null" string
}

const std::string & AbstractPrimitiveInputAdaptor::valueByKey(const std::string & key) const
{
	for (int i = 0; i < tagsSize(); ++i)
	{
		if (m_Controller->queryStringTable(keyId(i)) == key)
			return m_Controller->queryStringTable(valueId(i));
	}

	return m_Controller->queryStringTable(NULL_STRING_ID); // return "null" string
}

} // namespace osmpbf
