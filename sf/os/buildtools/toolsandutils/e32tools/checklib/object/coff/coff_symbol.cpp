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

#include "coff_symbol.h"


namespace coff
{
    Symbol::Symbol(const char* p1, const char* p2)
        : m_data( reinterpret_cast<const coff_symentry_t*>(p1) )
    {
        if ( unsigned(p2 - p1 + 1) < sizeof(coff_symentry_t) )
        {
             //throw
        }
    }

    unsigned Symbol::get_entrysize()
    {
        return sizeof(coff_symentry_t);
    }

    unsigned Symbol::get_name() const
    {
        // At the moment we can't handle inlined names. It shouldn't be necessary
        // right now, though, as the mangled name of all ::operator new functions
        // are more than eight characters long.
        if (m_data->is_inline)
        {
            return 0;
        }

        return m_data->offset;
    }

    unsigned Symbol::get_section() const
    {
        return m_data->section;
    }

    unsigned Symbol::get_auxcount() const
    {
        return m_data->aux_count;
    }

}


