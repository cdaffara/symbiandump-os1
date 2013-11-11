/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Declaration of CConditionOnCentRep and CConditionOnCentRepFlag
* classes.
*/




/**
 @file
 @internalComponent
 @released
*/

#ifndef __CNDCENTRALREPOSITORY_H__
#define __CNDCENTRALREPOSITORY_H__

#include "conditionevaluatebase.h"

class CCndCentralRepository : public CConditionEvaluateBase
	{
public:
	~CCndCentralRepository();
	static CCndCentralRepository* NewL(const TConditionCheckType aConditionCheckType
			, const TInt aRepositoryId, const TUint aKey, const TInt aCndValue);
	virtual TBool EvaluateL() const;

private:
	CCndCentralRepository(const TConditionCheckType aConditionCheckType, const TInt aCategory,
	        			const TUint aKey, const TInt aCndValue);

private:
    TInt iRepositoryId;
    TUint iKey;
    TInt iCndValue;
    TConditionCheckType iConditionCheckType;
	}; // class CCndCentralRepository

#endif // __CNDCENTRALREPOSITORY_H__
