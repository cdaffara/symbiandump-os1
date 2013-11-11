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
* Name        : resourcecondition.h
* Declaration of ResourceCondition class
*/




/**
 @file
 @internalComponent
 @released
*/

#ifndef __CONDITIONEVALUATE_H
#define __CONDITIONEVALUATE_H

// INCLUDES
#include <ssm/conditionevaluatebase.h>
#include <ssm/conditiontypes.hrh>
#include <barsc2.h> 
#include <barsread.h>


// CLASS DECLARATION

/**
 * CConditionEvaluate
 *
 */
class CConditionEvaluate : public CBase
	{
public:

	IMPORT_C ~CConditionEvaluate();
	IMPORT_C static CConditionEvaluate* NewL();
	IMPORT_C static CConditionEvaluate* NewLC();
	IMPORT_C TBool EvaluateL(const CResourceFile& aResourceFile, const TInt aResourceId);
private:
	CConditionEvaluateBase* DoEvaluateLC(const CResourceFile& aResourceFile, const TInt aResourceId, RArray<TUint32>& aUsedResourceIds);
	CConditionEvaluate();
	
	CConditionEvaluateBase* ReadConditionOnFeatureMatchL(TResourceReader& aReader);
	CConditionEvaluateBase* ReadConditionOnFeatureL(TResourceReader& aReader);
	CConditionEvaluateBase* ReadConditionOnCentRepFlagL(TResourceReader& aReader);
	CConditionEvaluateBase* ReadConditionOnCentRepMatchL(TResourceReader& aReader);
	CConditionEvaluateBase* ReadConditionOnPubSubFlagL(TResourceReader& aReader);
	CConditionEvaluateBase* ReadConditionOnPubSubMatchL(TResourceReader& aReader);
	CConditionEvaluateBase* ReadLogicalNotL(const CResourceFile& aResourceFile, TResourceReader& aReader,
										RArray<TUint32>& aUsedResourceIds);
	CConditionEvaluateBase* ReadLogicalAndL(const CResourceFile& aResourceFile, TResourceReader& aReader,
										RArray<TUint32>& aUsedResourceIds);
	CConditionEvaluateBase* ReadLogicalOrL(const CResourceFile& aResourceFile, TResourceReader& aReader,
										RArray<TUint32>& aUsedResourceIds);
	CConditionEvaluateBase* ReadConditionOnSwpMatchL(TResourceReader& aReader);
	CConditionEvaluateBase* ReadConditionOnSwpFlagL(TResourceReader& aReader);
	
	}; // class CConditionEvaluate

#endif // __CONDITIONEVALUATE_H
