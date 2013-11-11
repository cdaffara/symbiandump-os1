// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of the Header file for CheckSum implementation of the elf2e32 tool
// @internalComponent
// @released
// 
//

#ifndef CHECKSUM_H
#define CHECKSUM_H

#include <stddef.h>
unsigned long checkSum(const void *aPtr);
void Crc(unsigned short & aCrc,const void * aPtr,size_t aLength);
void Crc32(unsigned long & aCrc, const void * aPtr, size_t aLength);

#endif // CHECKSUM_H



