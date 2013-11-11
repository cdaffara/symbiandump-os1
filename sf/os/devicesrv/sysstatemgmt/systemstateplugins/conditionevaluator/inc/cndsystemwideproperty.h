// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This header file is used to evaluate the condition on SwP
// 
//

#ifndef __CNDSYSTEMWIDEPROPERTY_H__
#define __CNDSYSTEMWIDEPROPERTY_H__

#include "conditionevaluatebase.h"

class CCndSwp : public CConditionEvaluateBase
	{
public:
	~CCndSwp();
	static CCndSwp* NewL(const TConditionCheckType aConditionCheckType,
									const TUint aKey, const TInt aCndValue);
	virtual TBool EvaluateL() const;

private:

	CCndSwp(const TConditionCheckType aConditionCheckType,
	        			const TUint aKey, const TInt aCndValue);

private:
	TConditionCheckType iConditionCheckType;
    TUint iKey;
    TInt iCndValue;
	}; // class CCndSwp

#endif // __CNDSYSTEMWIDEPROPERTY_H__
