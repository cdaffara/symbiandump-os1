// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __LBSPTRHOLDER_H__
#define __LBSPTRHOLDER_H__

#include <e32base.h>
#include <lbscommon.h>


class CPositioningPtrHolder : public CBase
/**
Holds the TPtr8 and TPtrC8 arrays for all RPositionServer requests.
@internalComponent
 */
	{
public:
	IMPORT_C static CPositioningPtrHolder* NewL(TInt aNumberOfPtrs,
	                                            TInt aNumberOfPtrCs);
	IMPORT_C ~CPositioningPtrHolder();
	IMPORT_C TPtr8& Ptr(TInt aIndex);
	IMPORT_C TPtrC8& PtrC(TInt aIndex);
protected:
	IMPORT_C void ConstructL(TInt aNumberOfPtrs, TInt aNumberOfPtrCs);
private:
	/** modifiable pointer array */ 
	RArray<TPtr8> iPtrArray;
	/** non-modifiable pointer array */ 
	RArray<TPtrC8> iPtrCArray;
	};


class CServerPositioningPtrHolder : public CPositioningPtrHolder
/**
Holds the TPtr8 and TPtrC8 arrays for all RPositioner requests.

@internalComponent
 */
	{
public:
	IMPORT_C static CServerPositioningPtrHolder* NewL(TInt aNumberOfPtrs,
	                                                  TInt aNumberOfPtrCs);
	/** Id of position module */
	TPositionModuleId iPositionModuleId;
	};


#endif // __LBSPTRHOLDER_H__
