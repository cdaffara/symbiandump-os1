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
* Declaration of CConditionOnFeature class
*/




/**
 @file
 @internalComponent
 @released
*/

#ifndef __CNDFEATURE_H__
#define __CNDFEATURE_H__

#include "conditionevaluatebase.h"

class CCndFeature : public CConditionEvaluateBase
	{
public:
	~CCndFeature();
	static CCndFeature* NewL(const TConditionCheckType aConditionCheckType, const TInt aFeatureId, const TUint32 aCndValue);
	static CCndFeature* NewL(const TConditionCheckType aConditionCheckType, const TInt aFeatureId);
	virtual TBool EvaluateL() const;

private:
	CCndFeature(const TConditionCheckType aConditionCheckType, const TInt aFeatureId, const TUint32 aCndValue);

private:
    TConditionCheckType iConditionCheckType;
    TInt iFeatureId;
    TUint32 iCndValue;
	}; // class CCndFeature

#endif // __CNDFEATURE_H__
