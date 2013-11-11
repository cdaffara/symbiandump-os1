// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @internalComponent
 @prototype
*/

#include "dvbhhaitestutil.h"

const TUid KDvbhPropertyCategory = 
	{
	0x101f45ef //TestFramework's UID3
	};

struct TDvbhProperty
	{
	TInt iKey;
	TInt iType;
	};
		
const TUint KNumDvbhProperties = 8;
const TDvbhProperty KDvbhProperties[] =
	{
	{EDvbhPropertyKeyState, RProperty::EInt},
	{EDvbhPropertyKeyPlatform, RProperty::EByteArray},
	{EDvbhPropertyKeyNetworkTime, RProperty::EByteArray},
	{EDvbhPropertyKeyPerformanceData, RProperty::EByteArray},
	{EDvbhPropertyKeyFrequency, RProperty::EInt},
	{EDvbhPropertyKeyNetworkId, RProperty::EInt},
	{EDvbhPropertyKeySignalQuality, RProperty::EInt}, 
	{EDvbhPropertyKeyCellId, RProperty::EInt}
	};
	
TInt CDvbhTestUtil::GetKey(TDvbhPropertyKey aKey)
	{
	return KDvbhProperties[aKey].iKey;
	}
	
TInt CDvbhTestUtil::GetType(TDvbhPropertyKey aKey)
	{
	return KDvbhProperties[aKey].iType;
	}
	
CDvbhTestUtil::CDvbhTestUtil(MDvbhHaiTestLoggerInterface& aTestStepLogger)
: iTestStepLogger(aTestStepLogger)
	{
	}
	
CDvbhTestUtil* CDvbhTestUtil::NewL(MDvbhHaiTestLoggerInterface& aTestStepLogger)
	{
	CDvbhTestUtil* self = new (ELeave) CDvbhTestUtil(aTestStepLogger);
	return self;
	}
	
TInt CDvbhTestUtil::DefineDvbhProperty(TInt aPropertyKey, TInt aType) const
	{
	TInt err = RProperty::Define(KDvbhPropertyCategory, aPropertyKey, aType);
	if (err != KErrAlreadyExists && err != KErrNone)
	    {
	    iTestStepLogger.InfoPrintf3(_L("Error %d defining property %d"), err, aPropertyKey);
	    return err;
	    }
	else
		{
		err = KErrNone;
		}
    return err;	
	}

TInt CDvbhTestUtil::DefineDvbhProperty(TInt aPropertyKey, TInt aType, const TDesC8& aInitialValue) const
	{
	TInt err = DefineDvbhProperty(aPropertyKey, aType);
	if (err != KErrNone)
		{
		return err;
		}
	return SetByteArrayProperty(aPropertyKey, aInitialValue);
	}
	
TInt CDvbhTestUtil::DeleteDvbhProperty(TInt aPropertyKey)
	{
	TInt err = RProperty::Delete(KDvbhPropertyCategory, aPropertyKey);
	if (err != KErrNone && err != KErrNotFound)
	    {
	    iTestStepLogger.InfoPrintf3(_L("Error %d deleting property %d"), err, aPropertyKey);
	    }
	else
		{
		err = KErrNone;
		}
	return err;
	}

TInt CDvbhTestUtil::SetByteArrayProperty(TInt aPropertyKey, const TDesC8& aValue) const
	{
	TInt result = RProperty::Set(KDvbhPropertyCategory, aPropertyKey, aValue);
	if (result != KErrNone)
		{
		iTestStepLogger.InfoPrintf3(_L("Error %d setting the byte array property with key %d."), result, aPropertyKey);
		}
	return result;
	}
	
TInt CDvbhTestUtil::SetIntegerProperty(TInt aPropertyKey, TInt aValue) const
	{
	TInt result = RProperty::Set(KDvbhPropertyCategory, aPropertyKey, aValue);
	if (result != KErrNone)
		{
		iTestStepLogger.InfoPrintf4(_L("Error %d setting the integer property with key %d to %d"), result, aPropertyKey, aValue);
		}
	return result;		
	}

TInt CDvbhTestUtil::DefineDvbhStateProperty(TInt aInitialState)
	{
	TInt result = DefineDvbhProperty(EDvbhPropertyKeyState, KDvbhProperties[EDvbhPropertyKeyState].iType);
	if (result == KErrNone && aInitialState != ENoInitialState)
		{
		result = SetStateProperty(aInitialState);	
		}
	return result;
	}
	
TInt CDvbhTestUtil::DefineDvbhIntegerProperty(TInt aPropertyKey, TInt aInitialValue) const
	{
	TInt result = DefineDvbhProperty(aPropertyKey, KDvbhProperties[aPropertyKey].iType);
	if (result == KErrNone)
		{
		result = SetIntegerProperty(aPropertyKey, aInitialValue);	
		}
	return result;	
	}
	
TInt CDvbhTestUtil::SetStateProperty(TInt aValue)
	{
	TInt result = SetIntegerProperty(EDvbhPropertyKeyState, aValue);
	if (result != KErrNone)
		{
		iTestStepLogger.InfoPrintf3(_L("Error %d setting state property to %d"), result, aValue);
		}
	return result;
	}
	
TInt CDvbhTestUtil::DefineAllDvbhProperties()
	{
	for (TUint i=0;i<KNumDvbhProperties;++i)
		{
		const TDvbhProperty property = KDvbhProperties[i];
		TInt defineResult = DefineDvbhProperty(property.iKey, property.iType);
		if (defineResult != KErrNone)
			{
		    iTestStepLogger.InfoPrintf3(_L("Error %d defining property number %d"), defineResult, i);
			DeleteAllDvbhProperties();
			return defineResult;
			}
		}
	return KErrNone;
	}
	
TInt CDvbhTestUtil::DeleteAllDvbhProperties()
	{
	TInt result = KErrNone;
	for (TUint i=0;i<KNumDvbhProperties;++i)
		{
		const TDvbhProperty property = KDvbhProperties[i];
		TInt deleteResult = DeleteDvbhProperty(property.iKey);
		if (deleteResult != KErrNone)
			{
		    result = deleteResult;
			}
		}
	return result;	
	}
	
TBool CDvbhTestUtil::CompareVersions(const TVersion& aVersion1, const TVersion& aVersion2)
	{
	return (aVersion1.iMinor == aVersion2.iMinor &&
			aVersion1.iMajor == aVersion2.iMajor &&
			aVersion1.iBuild == aVersion2.iBuild);
	}

TBool CDvbhTestUtil::ComparePlatforms(const TDvbhPlatform& aPlatform1, const TDvbhPlatform& aPlatform2)
	{
	return (aPlatform1.iId == aPlatform2.iId &&
			aPlatform1.iName.Compare(aPlatform2.iName) == 0); // Compare returns 0 if they are the same
	}

TBool CDvbhTestUtil::ComparePerformanceDatas(const TDvbhPerformanceData& aData1, const TDvbhPerformanceData& aData2)
	{
	return (aData1.iFilterCount == aData2.iFilterCount &&
			aData1.iMaxFilters == aData2.iMaxFilters &&
			aData1.iDataCount == aData2.iDataCount &&
			aData1.iWriteCount == aData2.iWriteCount &&
			aData1.iWriteCommandCount == aData2.iWriteCommandCount &&
			aData1.iReadCount == aData2.iReadCount &&
			aData1.iReadCommandCount == aData2.iReadCommandCount);
	}
		
TBool CDvbhTestUtil::CompareScanConfigurations(const TDvbhScanConfiguration& aConfig1, const TDvbhScanConfiguration& aConfig2)
	{
	return (aConfig1.iScanStartFrequency == aConfig2.iScanStartFrequency &&
			aConfig1.iScanEndFrequency == aConfig2.iScanEndFrequency &&
			aConfig1.iSignalBandwidth == aConfig2.iSignalBandwidth &&
			aConfig1.iScanOptions == aConfig2.iScanOptions );
	}



