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

#include "coff_string_table.h"

static const char NULL_STRING[] = "";

namespace coff
{
    String_table::String_table(const char* p) : m_first_p(p)
    {
        m_size = *reinterpret_cast<const int*>(m_first_p);
    }

    const char* String_table::get_string(unsigned n) const
    {
        if (n < 4)
        {
            return NULL_STRING;
        }

        return (m_first_p + n);
    }
}

