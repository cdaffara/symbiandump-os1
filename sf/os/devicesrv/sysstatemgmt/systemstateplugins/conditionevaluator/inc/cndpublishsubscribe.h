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
* Name        : conditiononpubsub.h
* Declaration of CConditionOnPubSub and CConditionOnPubSubFlag
* classes
*/




/**
 @file
 @internalComponent
 @released
*/

#ifndef __CNDPUBLISHSUBSCRIBE_H__
#define __CNDPUBLISHSUBSCRIBE_H__

#include "conditionevaluatebase.h"

class CCndPublishSubscribe : public CConditionEvaluateBase
	{
public:
	~CCndPublishSubscribe();
	static CCndPublishSubscribe* NewL(const TConditionCheckType aConditionCheckType,
									const TInt aCategory, const TUint aKey, const TInt aCndValue);
	virtual TBool EvaluateL() const;

private:

	CCndPublishSubscribe(const TConditionCheckType aConditionCheckType, const TInt aCategory,
	        			const TUint aKey, const TInt aCndValue);

private:
	TConditionCheckType iConditionCheckType;
	TInt iCategory;
    TUint iKey;
    TInt iCndValue;
	}; // class CCndPublishSubscribe

#endif // __CNDPUBLISHSUBSCRIBE_H__
