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

#ifndef COFF_SYMBOL_H
#define COFF_SYMBOL_H

#include "coff.h"


namespace coff
{
    class Symbol
    {
    public:
        Symbol(const char*, const char*);
    public:
        static unsigned get_entrysize();

        unsigned get_name() const;
        unsigned get_section() const;
        unsigned get_auxcount() const;
    private:
        const coff_symentry_t* m_data;
    };
}


#endif

