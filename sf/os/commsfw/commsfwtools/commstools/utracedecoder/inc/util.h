// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef UTIL_H
#define UTIL_H

#include <iostream>

unsigned int HexToVal(const char* aHex);
void DumpBytes(std::ostream& aStream, const unsigned char* aData, unsigned int aLength);
void DumpBytes(std::ostream& aStream, const unsigned char* aData, unsigned int aLength, unsigned int* aZones, unsigned int aZoneCount);

#endif
// UTIL_H

