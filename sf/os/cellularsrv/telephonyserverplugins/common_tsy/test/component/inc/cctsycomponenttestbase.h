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
// base class for CTSY component tests
// 
//

/**
 @file 
*/

#ifndef CCTSYCOMPONENTTESTBASE_H
#define CCTSYCOMPONENTTESTBASE_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>
#include <etelpckt.h>

#include "rmockltsy.h"
#include <ctsy/serviceapi/cmmutility.h>
#include "CMmCommonStaticUtility.h"

// for making iCurrentICCType == EICCTypeSim3G  
enum TICCType
    {
    EICCTypeSim2GGsm,
    EICCTypeSim3G,
    EICCTypeSimUnknown
    };

class CStorageInfoData;

class CCtsyComponentTestBase : public CTestFixture
	{

protected:
	enum TExtendedErrorClient
		{
		EUseBasicError,
		EUseExtendedError
		};
	void OpenEtelServerL(TExtendedErrorClient aSetExtendedErrorClient = EUseBasicError);
	void OpenPhoneL();
	void DialL(RCall& aCall, const TInt aCallId, const RMobilePhone::TMobileService aService);
	void DialL(RCall& aCall,
				const TInt aCallId,
				const RMobilePhone::TMobileService aService,
				const TDesC &aTelNumber);
	void OpenPacketServiceL(RPacketService& aRPacketService,	          
	          RPacketService::TStatus aStartAttachStatus = RPacketService::EStatusUnattached,
	          RPacketService::TAttachMode aStartAttachMode = RPacketService::EAttachWhenNeeded,
	          TInt aMaxMonitorServices=10,
	          TInt aMaxServices=10
	          );
	          	          
    TInt InitContextL(RPacketContext& aPacketContext,
                      TInfoName& aContextName,
                      TInfoName aHostCidName = KNullDesC());
	TInt SetContextActiveL(TInfoName& aContextName);
	TInt SetContextStatusL(TInfoName& aContextName, RPacketContext::TContextStatus aStatus);
	TInt CreateQoSAndSetProfileParamLC(RPacketQoS& aPacketQoS, 
									   RPacketContext& aPacketContext,
									   TInfoName& aContextName,
									   TPacketDataConfigBase& aRequested,
									   TInfoName aPrimaryContextName = KNullDesC());
									   
    TInt InitMbmsContextL(RPacketMbmsContext& aMbmsContext,
                          TInfoName& aContextName);	
    TInt SetMbmsActiveL(TInfoName& aContextName);    
    TInt SetMbmsConfigL(RPacketMbmsContext& aPacketContext, 
                        TInfoName& aContextName, 
                        RPacketMbmsContext::TContextConfigMbmsV1 aStatus);   
    TInt SetMbmsActiveL(RPacketMbmsContext& aPacketContext, 
                        RPacketMbmsContext::TContextConfigMbmsV1& aConfig,
                        TInfoName& aContextName);                                           								 
    TInt SetMbmsDeactiveL(RPacketMbmsContext& aPacketContext, 
                        TInfoName& aContextName);    									 
    TInt SetMbmsDeleteL(RPacketMbmsContext& aPacketContext, 
                        TInfoName& aContextName); 
                      
	TInt SetPacketServiceStatusL(RPacketService::TStatus& aStatus, TBool aIsResumed);
	
	TInt AddContextPacketFilterL(RPacketContext& aContext,
	                             TInfoName& aContextName, 
								 RPacketContext::TPacketFilterV2& aFilter,
								 TBool aIsFailInCTSY = 0);

	TInt OpenNewCall(RLine& aLine,
						RCall& aCall,
						const TDesC& aLineName);
	
	TInt OpenNewCall(RLine& aLine, RCall& aCall, const TDesC& aLineName, TName& aNewName);
	
	TInt CreateIncomingCallL(RLine& aLine, 
	                        const TInt aCallId, 
	                        const TDesC& aLineName, // deprecated
	                        TName& aIncomingCallName,
	                        RMobilePhone::TMobileService aMobileService,
	                        RMobileCall::TMobileCallStatus aMobileCallStatus);

	void CloseIncomingCallL(RCall& aCall,
	         const TInt aCallId,
	         const RMobilePhone::TMobileService aMobileService = RMobilePhone::EVoiceService);
	
	void CloseCallL(RCall& aCall,
	         const TInt aCallId,
	         const RMobilePhone::TMobileService aMobileService);
	
	void PrepareCallCloseL(const TInt aCallId,
							const RMobilePhone::TMobileService aMobileService);
	
	void CloseLineL(RLine& aLine,
	        TInt aCallId = 0,
	        RMobilePhone::TMobileService aMobileService = RMobilePhone::EServiceUnspecified);

	void AssertMockLtsyStatusL();

	// to push a CleanupItem ont the cleanupstack to close the phone
	static void Cleanup(TAny* self);
	void DoCleanup();
	
	// Fake type used for bad parameter testing.
	// These will have the correct extension ID but the wrong descriptor
	// size compared to the T-class which the API expects.
	class TTestMultimodeType
		{
	public:
		TTestMultimodeType(TInt aExtensionId) { iExtensionId = aExtensionId; }
		TInt ExtensionId() const { return iExtensionId; }

	private:
		TInt iExtensionId;
		};

	typedef TPckg<TTestMultimodeType> TTestMultimodeTypePckg;
		
	void CallGetMobileCallInfoL(TInt aCallId, 
	                           RMobilePhone::TMobileService aService, 
    	                       const TDesC &aTelNumber);
	
	void ChangeCallStatusL(const TInt aCallId,
			const RMobilePhone::TMobileService aMobileService,
			const RMobileCall::TMobileCallStatus aCallStatus,
			const TInt aError = KErrNone);

	void GetNextServiceGroup(RMobilePhone::TMobileService &aServiceGroup);
	
	void OpenLineLC(RLine& aLine, const TDesC& aLineName);

	void OpenNewCallLC(RLine& aLine, RCall& aCall);

	void OpenIncomingCallLC(
			RLine& aLine,
			RCall& aCall,
			const TInt aCallId,
			RMobilePhone::TMobileService aMobileService,
			RMobileCall::TMobileCallStatus aMobileCallStatus);

	void OpenPhoneBookStoreL(RMobilePhoneBookStore &aStore, TName &aName, RMobilePhone &aPhone);

	void ChangeUsedSimCardTypeL(TICCType aType);
	
	void CompleteDialStatusNotificationsL(RCall &aCall, TInt aCallId, RMobilePhone::TMobileService aMobileService);
	
	void CompleteCallDisconnectNotificationsL(TInt aCallId, RMobilePhone::TMobileService aMobileService);
	
static void SetStorageInfoData(CStorageInfoData& aStorage, TInt aVal = 50);

protected:
	RTelServer iTelServer;
	RMobilePhone iPhone;
	RMockLtsy  iMockLTSY;

	}; 

#endif 

