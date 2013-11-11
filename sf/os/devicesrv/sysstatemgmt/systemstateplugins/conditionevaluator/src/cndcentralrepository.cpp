// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of CConditionOnCentRep and
// CConditionOnCentRepFlag classes.
//



#include "cndcentralrepository.h"
#include <centralrepository.h>

CCndCentralRepository::CCndCentralRepository(const TConditionCheckType aConditionCheckType, const TInt aRepositoryId,
        			const TUint aKey, const TInt aCndValue): iRepositoryId(aRepositoryId),
        		    iKey(aKey),
        		    iCndValue(aCndValue),
        		    iConditionCheckType(aConditionCheckType)
	{

	}

CCndCentralRepository* CCndCentralRepository::NewL(const TConditionCheckType aConditionCheckType,
								const TInt aRepositoryId, const TUint aKey, const TInt aCndValue)
	{
	CCndCentralRepository* self = new (ELeave)CCndCentralRepository(aConditionCheckType,
																aRepositoryId, aKey, aCndValue);
	return self;
	}

TBool CCndCentralRepository::EvaluateL() const
    {
    CRepository* repository = CRepository::NewLC(TUid::Uid(iRepositoryId));
    TInt value = 0;
    User::LeaveIfError(repository->Get(iKey, value));
    TBool retVal = EFalse;
    if (iConditionCheckType == ECompareValue)
        {
        retVal = (value == iCndValue);
        }
    else
        {
        retVal = ((value & iCndValue) == iCndValue);
        }

    CleanupStack::PopAndDestroy(repository);
    return retVal;
    }

CCndCentralRepository::~CCndCentralRepository()
	{
	}

