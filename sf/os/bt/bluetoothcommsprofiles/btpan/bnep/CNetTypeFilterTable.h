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

#ifndef __CNETTYPEFILTERTABLE_H
#define __CNETTYPEFILTERTABLE_H 

#include <e32base.h>
#include <e32def.h>
#include "BnepTypes.h"

NONSHARABLE_CLASS(CNetTypeFilterTable) : public CBase  
    {
public:
    CNetTypeFilterTable();
    TBool FilteredOut (const TUint16 aProtocol);
    TInt Insert (const TUint16 aStart, const TUint16 aEnd);
    void Reset ();

private: 
    TUint iCtr;
    TFixedArray< TUint16 , KNetTypeFilterTableBoundary  > iTable;
    };
#endif
