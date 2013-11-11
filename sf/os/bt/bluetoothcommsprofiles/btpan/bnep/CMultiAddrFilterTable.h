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
//

/**
 @file
 @internalComponent
*/

#ifndef __CMULTIADDRFILTERTABLE_H
#define __CMULTIADDRFILTERTABLE_H 

#include <e32base.h>
#include <e32def.h>
#include <bttypes.h>
#include "BnepTypes.h"
#include "TBnepBTDevAddr.h"

NONSHARABLE_CLASS(CMultiAddrFilterTable) : public CBase  
    {
public:
    CMultiAddrFilterTable();
    TInt Insert (const TBnepBTDevAddr& aStart, const TBnepBTDevAddr& aEnd);
    void Reset ();
    TBool FilteredOut (const TBnepBTDevAddr& aAddr);

private: 
    TUint iCtr;
    TFixedArray< TBnepBTDevAddr , KMultiAddrFilterTableBoundary  > iTable;
    };
#endif
