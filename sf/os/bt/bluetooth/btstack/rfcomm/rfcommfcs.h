// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The FCS calculation functions
// 
//

#ifndef RFCOMMFCS_H
#define RFCOMMFCS_H

#include <e32def.h>

TUint8 CalcFCS(const TUint8* aData, TInt aLen);
TBool CheckFCS(const TUint8* aData, TInt aLen, TUint8 aFCS);

#endif
