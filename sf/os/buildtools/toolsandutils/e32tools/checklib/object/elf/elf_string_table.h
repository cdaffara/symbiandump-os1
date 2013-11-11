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

#ifndef ELF_STRING_TABLE_H
#define ELF_STRING_TABLE_H

namespace elf
{
    class String_table
    {
    public:
        String_table(const char*, unsigned);

        const char* get_string(unsigned) const;
    private:
        const char* m_first_p;
        const char* m_last_p;
    };
}

#endif

