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

#ifndef COFF_H
#define COFF_H


struct coff_file_header_t
{
    unsigned short magic;

    unsigned short _junk1;
    unsigned long  _junk2;

    unsigned long  symtab;
    unsigned long  nsyms;

    unsigned short _junk3;
    unsigned short _junk4;
};

struct coff_symentry_t
{
    unsigned long is_inline __attribute__((packed));
    unsigned long offset    __attribute__((packed));
    unsigned long _junk1    __attribute__((packed));
    
    short section;

    unsigned short _junk2;

    unsigned char  _junk3;
    unsigned char  aux_count;
};


#endif


