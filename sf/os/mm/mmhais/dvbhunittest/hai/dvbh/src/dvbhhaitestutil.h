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

#ifndef DVBHHAITESTUTIL_H
#define DVBHHAITESTUTIL_H

#include <e32base.h>
#include <e32property.h>
#include <mobiletv/hai/dvbh/dvbhtypes.h>
#include <in_sock.h>


/**
* Constants for expected values
*/
const TInt KDvbhTestFilterId = 977;
const TIp6Addr KDvbhTestIpV6Addr = {{{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,1}}};
const TInt KDvbhTestCustomCommand = 0;
_LIT8(KDvbhTestCustomCommandInput, 	"DvbhTestCustomCommandInput");
_LIT8(KDvbhTestCustomCommandOutput, "DvbhTestCustomCommandOutput");
_LIT8(KDvbhTestHardwareInfo, "Example Hardware Info;Version 1.0");

const TInt KInitialState = EDvbhStateInactive;
const TInt KExpectedState = EDvbhStateReady;

const TInt KInitialSignalQuality = EDvbhSignalQualityFair;
const TInt KExpectedSignalQuality = EDvbhSignalQualityExcellent;

const TInt KInitialFrequency = 1500000;
const TInt KExpectedFrequency = 3600000;

const TInt KInitialCellId = 1006;
const TInt KExpectedCellId = 756;

const TInt KInitialNetworkId = 9993;
const TInt KExpectedNetworkId = 3726;

const TUint32 KDvbhTestTerminalTimeStamp = 987654321;

const TInt KNoVerdict = -1;

const TInt KInitialPlatformId = 123456789;
_LIT8(KInitialPlatformName, "DvbhTestInitialPlatform");

const TInt KExpectedPlatformId = 987654321;
_LIT8(KExpectedPlatformName, "DvbhTestExpectedPlatform");

/**
* @internalComponent
* @prototype
*
* Publish & Subscribe variables used for implementation to API communication.
*
* This must be kept identical to the ones defined in the non-exported header
* file dvbhimplcommon.h
*/	
enum TDvbhPropertyKey
	{
	EDvbhPropertyKeyState = 0,					// integer (TDvbhState)
	EDvbhPropertyKeyPlatform = 1,   	 		// byte array (TDvbhPlatform + TIp6Addr + TInt16)
	EDvbhPropertyKeyNetworkTime = 2, 	 		// byte array (TDvbhNetworkTime)
	EDvbhPropertyKeyPerformanceData = 3, 		// byte array (TDvbhPerformanceData)
	EDvbhPropertyKeyFrequency = 4, 		 		// byte array (TDvbhFrequency)
	EDvbhPropertyKeyNetworkId = 5, 		 		// byte array (TDvbhNetworkId)
	EDvbhPropertyKeySignalQuality = 6, 	 		// byte array (TDvbhSignalQuality)
	EDvbhPropertyKeyCellId = 7	 		 		// byte array (TDvbhCellId)
	};

struct TDvbhPlatformProperty
	{
	TDvbhPlatform iPlatform;
	TIp6Addr iEsgRoot;
	};

/**
* Interface to enable CDvbhTestUtil to log test data & messages.
*/	
class MDvbhHaiTestLoggerInterface
	{
public:
	virtual void InfoPrintf3(const TDesC16& aString, TInt aInt1, TInt aInt2) = 0;
	virtual void InfoPrintf4(const TDesC16& aString, TInt aInt1, TInt aInt2, TInt aInt3) = 0;
	};
	
class RTestStep;

/**
* @internalComponent
* @prototype
*
* A class of utility methods to help the DVB-H HAI test steps.
*/
class CDvbhTestUtil : public CBase
	{
public:
	enum 
		{
		ENoInitialState = -1
		};
		
	enum
		{
		ENumDvbhProperties = 8
		};
public:
	static CDvbhTestUtil* NewL(MDvbhHaiTestLoggerInterface& aTestStepLogger);
		
	TInt DefineDvbhProperty(TInt aPropertyKey, TInt aType = RProperty::EInt) const;
	TInt DefineDvbhProperty(TInt aPropertyKey, TInt aType, const TDesC8& aInitialValue) const;
	TInt DefineDvbhStateProperty(TInt aInitialState = ENoInitialState);
	TInt DefineDvbhIntegerProperty(TInt aPropertyKey, TInt aInitialValue) const;
	TInt DeleteDvbhProperty(TInt aPropertyKey);
	TInt SetStateProperty(TInt aValue);
	TInt SetByteArrayProperty(TInt aPropertyKey, const TDesC8& aValue) const;
	TInt SetIntegerProperty(TInt aPropertyKey, TInt aValue) const;
	TInt DefineAllDvbhProperties();
	TInt DeleteAllDvbhProperties();
public:
	static TInt GetKey(TDvbhPropertyKey aKey);
	static TInt GetType(TDvbhPropertyKey aKey);
	static TBool CompareVersions(const TVersion& aVersion1, const TVersion& aVersion2);
	static TBool ComparePlatforms(const TDvbhPlatform& aPlatform1, const TDvbhPlatform& aPlatform2);
	static TBool ComparePerformanceDatas(const TDvbhPerformanceData& aData1, const TDvbhPerformanceData& aData2);
	static TBool CompareScanConfigurations(const TDvbhScanConfiguration& aConfig1, const TDvbhScanConfiguration& aConfig2);
private:
	CDvbhTestUtil(MDvbhHaiTestLoggerInterface& aTestStepLogger);
private:
	MDvbhHaiTestLoggerInterface& iTestStepLogger;
	};
	
#endif // DVBHHAITESTUTIL_H
