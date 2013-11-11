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

#ifndef CMOCKLTSYFACTORY_H
#define CMOCKLTSYFACTORY_H

#include <ctsy/mltsyfactory.h>

// The UID that identifies the property category in Telephony Audio Control tests. 
const TUid KMockLtsyFactoryProperty = {0x10012345};

// The property sub-key which defines different keys used in Call Routing Control tests
enum TMockLtsyFactoryPropertyKeys  { EVersionProperty, ENumberOfCalls, ECallName, EEmergencyCall, ECallDirection, ECallType, EReturnErrorCode, EHangUpReason, EReset, ECallStatus, EComplete };

// The different versions of LTSY used in Call Routing Control tests to test both V1 and V2 versions of LTSY
enum TMockLtsyFactoryVersions  { EVersion1 = 1, EVersion2, EVersion3 };
class CMmMessageRouter;
class CMockTelephonyAudioControl;
class MTelephonyAudioControlCallback;

class CMockLtsyFactoryV1 : public CBase, public MLtsyFactoryV1
	{
public:
	static CMockLtsyFactoryV1* NewL();
	~CMockLtsyFactoryV1();
		
public:	// from MLtsyFactoryBase
	TCtsyInterfaceVersion Version();
	void Release();

public: // from MLtsyFactoryV1
	MMessageRouter* GetMessageRouter(MmMessageManagerCallback &aMessageManager);
#ifdef USING_CTSY_DISPATCHER 
	CTsySatMessagingBase* GetSatMessagingService(MCtsySatService &aSatMessagingService);
#else
	CTsySatMessagingBase* GetSatMessagingService( );
#endif

   // CMmPhoNetSender* GetPhoNetSender();
   // CMmPhoNetReceiver* GetPhoNetReceiver();
	
protected:
	CMockLtsyFactoryV1();
	void ConstructL();
	
private:
	CMmMessageRouter* iMessageRouter;	
	};

class CMockLtsyFactoryV2 : public CBase, public MLtsyFactoryV2
	{
public:
	static CMockLtsyFactoryV2* NewL();
	~CMockLtsyFactoryV2();
		
public:	// from MLtsyFactoryBase
	TCtsyInterfaceVersion Version();
	void Release();

public: // from MLtsyFactoryV1
    MMessageRouter* GetMessageRouter(MmMessageManagerCallback &aMessageManager);
#ifdef USING_CTSY_DISPATCHER 
	CTsySatMessagingBase* GetSatMessagingService(MCtsySatService &aSatMessagingService);
#else
	CTsySatMessagingBase* GetSatMessagingService( );
#endif
public: // from MLtsyFactoryV2
    MTelephonyAudioControl* GetTelephonyAudioControl( MTelephonyAudioControlCallback* aTelephonyAudioControlCallback );

protected:
	CMockLtsyFactoryV2();
	void ConstructL();
	
private:
	CMmMessageRouter* iMessageRouter;
	CMockTelephonyAudioControl* iTelephonyAudioControl;
	};	

class CMockLtsyFactoryV3 : public CBase, public MLtsyFactoryV3
	{
public:
	static CMockLtsyFactoryV3* NewL();
	~CMockLtsyFactoryV3();
		
public:	// from MLtsyFactoryBase
	TCtsyInterfaceVersion Version();
	void Release();

public: // from MLtsyFactoryV1
    MMessageRouter* GetMessageRouter(MmMessageManagerCallback &aMessageManager);
#ifdef USING_CTSY_DISPATCHER 
	CTsySatMessagingBase* GetSatMessagingService(MCtsySatService &aSatMessagingService);
#else
	CTsySatMessagingBase* GetSatMessagingService( );
#endif
public: // from MLtsyFactoryV2
    MTelephonyAudioControl* GetTelephonyAudioControl( MTelephonyAudioControlCallback* aTelephonyAudioControlCallback );

public: // from MLtsyFactoryV3
	TUint32 GetLTSYStaticCapabilities( TUint32 aRequestedCapabilitySet );
	
protected:
	CMockLtsyFactoryV3();
	void ConstructL();
	
private:
	CMmMessageRouter* iMessageRouter;
	CMockTelephonyAudioControl* iTelephonyAudioControl;
	};

// Library entry function to LTSY
IMPORT_C MLtsyFactoryBase* LTsyFactoryL();
	
#endif
