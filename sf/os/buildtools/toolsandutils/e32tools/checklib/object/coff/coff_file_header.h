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

#ifndef COFF_FILE_HEADER_H
#define COFF_FILE_HEADER_H

#include "coff.h"


namespace coff
{
    class File_header
    {
    public:
        File_header(const char*, const char*, bool = 0);
    public:
        unsigned get_symtab_offset() const;
        unsigned get_symcount() const;
    private:
        const coff_file_header_t* m_data;
    };
}

#endif

