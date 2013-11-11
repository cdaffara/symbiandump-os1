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
// Some utility functions for converting between big and little endian.
// 
//

#ifndef ENDIAN_H
#define ENDIAN_H

#include <stdint.h> // For uint16_t and uint32_t.

namespace misc
{
    // Big endian <-> little endian.
    uint32_t reverse_bytes(uint32_t);
    uint16_t reverse_bytes(uint16_t);
}


#endif

