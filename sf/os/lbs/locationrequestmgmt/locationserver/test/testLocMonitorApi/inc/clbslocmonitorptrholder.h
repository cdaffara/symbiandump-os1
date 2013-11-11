/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


#ifndef CLBSLOCMONITORPTRHOLDER_H
#define CLBSLOCMONITORPTRHOLDER_H

#include <e32base.h>

class CLbsLocMonitorPtrHolder : public CBase
/**
Holds the TPtr8 and TPtrC8 arrays for all RPositionServer requests.
@internalComponent
 */
	{
public:
	
	enum TParameter
		{
		EPosForGetLastKnownPosition = 0,
		EPosForGetLastKnownPositionArea,
		EAreaForGetLastKnownPositionArea,
		EParamsForGetLastKnownPositionArea,
		ELastParam, // Kee this the last
		};
	
public:
	IMPORT_C static CLbsLocMonitorPtrHolder* NewL(TInt aNumberOfPtrs,
	                                            TInt aNumberOfPtrCs);
	IMPORT_C ~CLbsLocMonitorPtrHolder();
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

#endif // CLBSLOCMONITORPTRHOLDER_H
