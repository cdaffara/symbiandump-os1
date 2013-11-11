// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Provides the implementation to evaluate the condition on SwP
// 
//

/**
 @internalComponent
*/

#include "ssmdebug.h"
#include "cndsystemwideproperty.h"
#include <ssm/ssmsystemwideproperty.h>

CCndSwp::CCndSwp(const TConditionCheckType aConditionCheckType,
        			const TUint aKey, const TInt aCndValue): iConditionCheckType(aConditionCheckType),
        		    iKey(aKey),
        		    iCndValue(aCndValue)
	{

	}

CCndSwp::~CCndSwp()
	{
	
	}

CCndSwp* CCndSwp::NewL(const TConditionCheckType aConditionCheckType,
								const TUint aKey, const TInt aCndValue)
	{
	CCndSwp* self = new (ELeave)CCndSwp(aConditionCheckType,aKey, aCndValue);
	return self;
	}

TBool CCndSwp::EvaluateL() const
    {
    TInt value = 0;
  
    RSsmSystemWideProperty property;
	User::LeaveIfError(property.Connect(iKey));
	CleanupClosePushL(property);

	SSMLOGLEAVEIFERROR(property.GetValue(value));
	
    TBool retVal = EFalse;
    if (iConditionCheckType == ECompareValue)
        {
        retVal = (value == iCndValue);
        }
    else
        {
        retVal = ((value & iCndValue) == iCndValue);
        }

	CleanupStack::PopAndDestroy(&property);
    return retVal;
    }
