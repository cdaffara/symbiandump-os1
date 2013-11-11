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
// Implementation of CConditionOnFeature class
//




#include "cndfeature.h"
#include <featmgr/featurecontrol.h>
#include <featmgr/featmgr.h> 

CCndFeature::CCndFeature(const TConditionCheckType aConditionCheckType, const TInt aFeatureId, const TUint32 aCndValue)
						: iConditionCheckType( aConditionCheckType ), iFeatureId ( aFeatureId ),
	        		    iCndValue( aCndValue )
	{
	}

CCndFeature* CCndFeature::NewL(const TConditionCheckType aConditionCheckType, const TInt aFeatureId)
	{
	const TUint nullFeatureData = 0;

	//Passing the condition value as 0 as it is only required while checking the feature data.
	return NewL(aConditionCheckType, aFeatureId, nullFeatureData);
	}

CCndFeature* CCndFeature::NewL(const TConditionCheckType aConditionCheckType, const TInt aFeatureId, const TUint32 aCndValue)
	{
	CCndFeature* self = new (ELeave)CCndFeature(aConditionCheckType, aFeatureId, aCndValue);
	return self;
	}

TBool CCndFeature::EvaluateL() const
    {
    RFeatureControl featureControl;
    User::LeaveIfError(featureControl.Open());

    TUid featureUid = {iFeatureId};
    TBool retVal = EFalse;
	TFeatureEntry featureEntry(featureUid);
	TInt result = featureControl.FeatureSupported(featureUid);

	if (KFeatureSupported  == result)
		{
		if (ECompareFeatureData == iConditionCheckType)
	    	{
	    	if(iCndValue == featureEntry. FeatureData())
	    		{
	    		retVal = ETrue;
	    		}
	    	}
		else
			{
			retVal = ETrue;
			}
		}

    featureControl.Close();
    return retVal;
    }

CCndFeature::~CCndFeature()
	{
	}

