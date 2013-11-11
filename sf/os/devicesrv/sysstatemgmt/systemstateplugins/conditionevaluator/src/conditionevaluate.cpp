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
// Implementation of ResourceCondition class
//



#include "conditionevaluate.h"
#include "cndlogical.h"
#include "cndfeature.h"
#include "cndcentralrepository.h"
#include "cndpublishsubscribe.h"
#include "cndsystemwideproperty.h"

CConditionEvaluate::CConditionEvaluate()
	{
	}

EXPORT_C CConditionEvaluate::~CConditionEvaluate()
	{
	}

EXPORT_C CConditionEvaluate* CConditionEvaluate::NewLC()
	{
	CConditionEvaluate* self = CConditionEvaluate::NewL();
	CleanupStack::PushL(self);
	return self;
	}

EXPORT_C CConditionEvaluate* CConditionEvaluate::NewL()
	{
	CConditionEvaluate* self = new (ELeave)CConditionEvaluate();
	return self;
	}

CConditionEvaluateBase* CConditionEvaluate::ReadConditionOnFeatureMatchL(TResourceReader& aReader)
    {
    const TInt featureId = aReader.ReadInt32();
    return CCndFeature::NewL(ECompareValue, featureId);
    }

CConditionEvaluateBase* CConditionEvaluate::ReadConditionOnFeatureL(TResourceReader& aReader)
    {
    const TInt featureId = aReader.ReadInt32();
    const TUint32 aCndValue = aReader.ReadUint32();
    return CCndFeature::NewL(ECompareFeatureData, featureId, aCndValue);
    }

CConditionEvaluateBase* CConditionEvaluate::ReadConditionOnCentRepFlagL(TResourceReader& aReader)
    {
    const TInt repositoryId = aReader.ReadInt32();
    const TUint32 key = aReader.ReadUint32();
    const TInt mask = aReader.ReadInt32();
    return CCndCentralRepository::NewL(ECompareBitwiseAnd, repositoryId, key, mask);
    }

CConditionEvaluateBase* CConditionEvaluate::ReadConditionOnCentRepMatchL(TResourceReader& aReader)
    {
    const TInt repositoryId = aReader.ReadInt32();
    const TUint32 key = aReader.ReadUint32();
    const TInt value = aReader.ReadInt32();
    return CCndCentralRepository::NewL(ECompareValue, repositoryId, key, value );
    }

CConditionEvaluateBase* CConditionEvaluate::ReadConditionOnPubSubFlagL(TResourceReader& aReader)
    {
    const TInt category = aReader.ReadInt32();
    const TUint32 key = aReader.ReadUint32();
    const TInt mask = aReader.ReadInt32();

    return CCndPublishSubscribe::NewL(ECompareBitwiseAnd, category, key, mask);
    }

CConditionEvaluateBase* CConditionEvaluate::ReadConditionOnPubSubMatchL(TResourceReader& aReader)
    {
    const TInt category = aReader.ReadInt32();
    const TUint32 key = aReader.ReadUint32();
    const TInt value = aReader.ReadInt32();

    return CCndPublishSubscribe::NewL(ECompareValue, category, key, value);
    }

CConditionEvaluateBase* CConditionEvaluate::ReadConditionOnSwpMatchL(TResourceReader& aReader)
    {
    const TUint32 key = aReader.ReadUint32();
    const TInt value = aReader.ReadInt32();

    return CCndSwp::NewL(ECompareValue, key, value);
    }
    
CConditionEvaluateBase* CConditionEvaluate::ReadConditionOnSwpFlagL(TResourceReader& aReader)
    {
    const TUint32 key = aReader.ReadUint32();
    const TInt mask = aReader.ReadInt32();

    return CCndSwp::NewL(ECompareBitwiseAnd, key, mask);
    }
    
CConditionEvaluateBase* CConditionEvaluate::ReadLogicalNotL(const CResourceFile& aResourceFile,TResourceReader& aReader,
		RArray<TUint32>& aUsedResourceIds)
    {
    const TUint32 resourceId = aReader.ReadUint32();
    CConditionEvaluateBase* sub = DoEvaluateLC(aResourceFile, resourceId, aUsedResourceIds);
    CConditionEvaluateBase* retVal = new (ELeave) CLogicalNot(sub);
    CleanupStack::Pop(sub); // CLogicalNot owns it now
    return retVal;
    }

CConditionEvaluateBase* CConditionEvaluate::ReadLogicalAndL(const CResourceFile& aResourceFile,TResourceReader& aReader,
		RArray<TUint32>& aUsedResourceIds)
    {
    const TUint32 lhsId = aReader.ReadUint32();
    const TUint32 rhsId = aReader.ReadUint32();

    CConditionEvaluateBase* lhs = DoEvaluateLC(aResourceFile, lhsId, aUsedResourceIds);
    CConditionEvaluateBase* rhs = DoEvaluateLC(aResourceFile, rhsId, aUsedResourceIds);
    CConditionEvaluateBase* retVal = new (ELeave) CLogicalAnd(lhs, rhs);
    CleanupStack::Pop(rhs); // CLogicalAnd owns it now
    CleanupStack::Pop(lhs); // CLogicalAnd owns it now

    return retVal;
    }

CConditionEvaluateBase* CConditionEvaluate::ReadLogicalOrL(const CResourceFile& aResourceFile,TResourceReader& aReader,
		RArray<TUint32>& aUsedResourceIds)
    {
    const TUint32 lhsId = aReader.ReadUint32();
    const TUint32 rhsId = aReader.ReadUint32();
    CConditionEvaluateBase* lhs = DoEvaluateLC(aResourceFile, lhsId, aUsedResourceIds);
    CConditionEvaluateBase* rhs = DoEvaluateLC(aResourceFile, rhsId, aUsedResourceIds);
    CConditionEvaluateBase* retVal = new (ELeave) CLogicalOr(lhs, rhs);
    CleanupStack::Pop(rhs); // CLogicalOr owns it now
    CleanupStack::Pop(lhs); // CLogicalOr owns it now

    return retVal;
    }

CConditionEvaluateBase* CConditionEvaluate::DoEvaluateLC(const CResourceFile& aResourceFile, const TInt aResourceId,
									RArray<TUint32>& aUsedResourceIds)
	{
    // Check for any recursive conditions
    TInt idx = aUsedResourceIds.FindReverse(aResourceId);
    if (0 <= idx)
        {
        //Leave in case of any recursive condition
        User::Leave(KErrCorrupt);
        }
    aUsedResourceIds.AppendL(aResourceId);

    HBufC8* buf = aResourceFile.AllocReadLC(aResourceId);
    TResourceReader reader;
    reader.SetBuffer(buf);

    TUint16 type = reader.ReadUint16();
    CConditionEvaluateBase* retVal = NULL;

    switch (type)
        {
    case EFeatureFlag:
    	retVal = ReadConditionOnFeatureMatchL(reader);
        break;
    case EFeatureFlagData:
    	retVal = ReadConditionOnFeatureL(reader);
    	break;
    case ECentRepFlag:
        retVal = ReadConditionOnCentRepFlagL(reader);
        break;
    case ECentRepIntVal:
        retVal = ReadConditionOnCentRepMatchL(reader);
        break;
    case EPubSubFlag:
        retVal = ReadConditionOnPubSubFlagL(reader);
        break;
    case EPubSubIntVal:
        retVal = ReadConditionOnPubSubMatchL(reader);
        break;
    case ELogicalNot:
        retVal = ReadLogicalNotL(aResourceFile, reader, aUsedResourceIds);
        break;
    case ELogicalAnd:
        retVal = ReadLogicalAndL(aResourceFile, reader, aUsedResourceIds);
        break;
    case ELogicalOr:
        retVal = ReadLogicalOrL(aResourceFile, reader, aUsedResourceIds);
        break;
    case ESwpIntVal:
        retVal = ReadConditionOnSwpMatchL(reader);
        break;
    case ESwpFlag:
        retVal = ReadConditionOnSwpFlagL(reader);
        break;    
    default:
        User::Leave(KErrCorrupt);
        }

    aUsedResourceIds.Remove(aUsedResourceIds.Count() - 1);
    CleanupStack::PopAndDestroy(buf);
    CleanupStack::PushL(retVal);
    return retVal;
	}

EXPORT_C TBool CConditionEvaluate::EvaluateL(const CResourceFile& aResourceFile, const TInt aResourceId)
    {
    RArray<TUint32> array;
    CleanupClosePushL(array);

    CConditionEvaluateBase* expr = DoEvaluateLC(aResourceFile, aResourceId, array);
    TBool retVal = expr->EvaluateL();

    CleanupStack::PopAndDestroy(expr);
    CleanupStack::PopAndDestroy(&array);
    return retVal;
    }
