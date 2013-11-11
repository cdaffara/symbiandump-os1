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

#ifndef COFF_STRING_TABLE_H
#define COFF_STRING_TABLE_H


namespace coff
{
    class String_table
    {
    public:
        String_table(const char*);

        const char* get_string(unsigned) const;
    private:
        const char* m_first_p;
        unsigned m_size;
    };
}


#endif

