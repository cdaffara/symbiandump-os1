// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include "coff_file_header.h"
#include <stdexcept>
#include <string>


namespace coff
{
    File_header::File_header(const char* p1, const char* p2, bool a_pedantic)
        : m_data( reinterpret_cast<const coff_file_header_t*>(p1) )
    {
        if ( unsigned(p2 - p1 + 1) < sizeof(coff_file_header_t) )
        {
            throw std::runtime_error("not a COFF object: the file is too small");
        }

        if (m_data->magic != 0x014c)
        {
            throw std::runtime_error("not a COFF object: bad magic");
        }

        if (a_pedantic) // Do some extra checks for correctness.
        {
            // ...
        }
    }

    unsigned File_header::get_symtab_offset() const
    {
        return m_data->symtab;
    }

    unsigned File_header::get_symcount() const
    {
        return m_data->nsyms;
    }
}

