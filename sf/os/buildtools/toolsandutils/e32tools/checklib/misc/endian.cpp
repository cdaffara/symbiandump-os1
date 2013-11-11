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

#include "endian.h"


uint32_t misc::reverse_bytes(uint32_t n)
{
    unsigned char b0 = (n >> 0*8) & 0xff;
    unsigned char b1 = (n >> 1*8) & 0xff;
    unsigned char b2 = (n >> 2*8) & 0xff;
    unsigned char b3 = (n >> 3*8) & 0xff;

    return (b0 << 3*8) | (b1 << 2*8) | (b2 << 1*8) | (b3 << 0*8);
}

uint16_t misc::reverse_bytes(uint16_t n)
{
    unsigned char b0 = (n >> 0*8) & 0xff;
    unsigned char b1 = (n >> 1*8) & 0xff;

    return (b0 << 1*8) | (b1 << 0*8);
}

