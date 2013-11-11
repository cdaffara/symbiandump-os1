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

#include "elf_string_table.h"
#include <cassert>

namespace elf
{
    const char* String_table::get_string(unsigned n) const
    {
        return (m_first_p + n);
    }

    String_table::String_table(const char* p, unsigned size)
        : m_first_p(p), m_last_p(p + size - 1)
    {
        assert(m_first_p <= m_last_p);

        if (*m_first_p != '\0')
        {
            //throw String_table_error("the first character is non-NUL");
            assert(0);
        }

        if (*m_last_p != '\0')
        {
            //throw String_table_error("the last character is non-NUL");
            assert(0);
        }
    }
}

