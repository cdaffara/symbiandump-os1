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

#include "object.h"
#include <cassert>


std::auto_ptr<Object> Object_factory::create(objkind_t a_kind, const char* p1, const char* p2)
{
    switch(a_kind)
    {
    case ELF:
        return std::auto_ptr<Object>( new Elf_object(p1, p2) );
        break;
    case COFF:
        return std::auto_ptr<Object>( new Coff_object(p1, p2) );
        break;
    default:
        break;
    }

    assert(0);

    // Dead code, just to get rid of the warning.
    return std::auto_ptr<Object>(0);
}

Object::~Object() {}

