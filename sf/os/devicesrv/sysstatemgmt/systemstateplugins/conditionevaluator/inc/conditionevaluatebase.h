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

#ifndef __CONDITIONEVALUATEBASE_H__
#define __CONDITIONEVALUATEBASE_H__

#include <e32std.h>
#include <e32base.h>

enum TConditionCheckType
    {
    ECompareNone,
    ECompareValue = 1,		// value comparison
    ECompareBitwiseAnd,		// Check if a bit is set
    ECompareFeatureData		// compare feature data value
    };

class CConditionEvaluateBase : public CBase
	{
public:
	virtual TBool EvaluateL() const = 0;

	};

#endif // __CONDITIONEVALUATEBASE_H__
