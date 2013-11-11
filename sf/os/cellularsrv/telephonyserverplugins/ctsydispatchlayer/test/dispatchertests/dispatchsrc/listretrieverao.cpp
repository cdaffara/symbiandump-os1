// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements AO to retrieve lists
//

#include "listretrieverao.h"

#include "cctsyactiveretriever.h"

#include <test/rmockltsy.h>
#include <test/tmockltsydata.h>

#include <pcktretrieve.h>

#include <ctsy/ltsy/mltsydispatchphoneinterface.h>


CTestCanceller::CTestCanceller(CActive& aRetrieve)
: CAsyncOneShot(EPriorityNormal),iRetrieve(aRetrieve)
	{
	
	}

CTestCanceller::~CTestCanceller()
	{
	Cancel();
	}

void CTestCanceller::RunL()
	{
	iRetrieve.Cancel();
	CActiveScheduler::Stop();
	}

CGetDetectedNetworksAO* CGetDetectedNetworksAO::NewLC(RMobilePhone& aPhone, RMockLtsy& aMockLtsy)
	{
	CGetDetectedNetworksAO* self = new(ELeave) CGetDetectedNetworksAO(aPhone,aMockLtsy);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }
      
CGetDetectedNetworksAO::CGetDetectedNetworksAO(RMobilePhone& aPhone, RMockLtsy& aMockLtsy)
    : 	CActive(EPriorityNormal), 
    	iPhone(aPhone),
    	iMockLtsy(aMockLtsy)
    {
    }
      

void CGetDetectedNetworksAO::ConstructL()
	{
	iActiveScheduler = new (ELeave) CActiveScheduler();
	CActiveScheduler::Install(iActiveScheduler);

	iDetectedNetworksRetrieve = CRetrieveMobilePhoneDetectedNetworks::NewL(iPhone);
    iCanceller = new (ELeave) CTestCanceller(*this);
    
	CActiveScheduler::Add(this);
	}
         
CGetDetectedNetworksAO::~CGetDetectedNetworksAO()
    {
    Cancel();
    
    delete iDetectedNetworksRetrieve;
    iDetectedNetworksRetrieve = NULL;
    
	delete iCanceller;
	iCanceller = NULL;
	
	delete iActiveScheduler;
	iActiveScheduler = NULL;
	
	iNetworkList = NULL;
	}

void CGetDetectedNetworksAO::TestRetrieveL(CMobilePhoneNetworkListV2& aNetworkList)
    {
    iNetworkList = &aNetworkList;
    
    SetActive();
	iDetectedNetworksRetrieve->Cancel();
    iDetectedNetworksRetrieve->StartV2(iStatus);
    
    TRequestStatus requestStatus;
    iMockLtsy.NotifyTerminated(requestStatus);
    
    RBuf8 data;
    CleanupClosePushL(data);
    TMockLtsyData1<CMobilePhoneNetworkListV2*> listData(iNetworkList);
    listData.SerialiseL(data);
    iMockLtsy.CompleteL(MLtsyDispatchPhoneGetDetectedNetworks::KLtsyDispatchPhoneGetDetectedNetworksApiId, KErrNone, data);
    CleanupStack::PopAndDestroy(&data);
    
    User::WaitForRequest(requestStatus);
    
    CActiveScheduler::Start();
	} 

void CGetDetectedNetworksAO::TestRetrieveNegativeL()
    {
    SetActive();
	iDetectedNetworksRetrieve->Cancel();
    iDetectedNetworksRetrieve->StartV2(iStatus);
       
    CActiveScheduler::Start();
	} 


void CGetDetectedNetworksAO::TestCancel()
	{
    SetActive();
	iDetectedNetworksRetrieve->Cancel();
    iDetectedNetworksRetrieve->StartV2(iStatus);

    iCanceller->Cancel();
    iCanceller->Call();
    
    CActiveScheduler::Start();
	}      

void CGetDetectedNetworksAO::RunL()
    {
    CActiveScheduler::Stop();
	}
      

void CGetDetectedNetworksAO::DoCancel()
    {
    iDetectedNetworksRetrieve->Cancel();
    }       

TInt CGetDetectedNetworksAO::CheckResultsL()
    {
	TInt ret = KErrNone;

    CMobilePhoneNetworkListV2* retrievedNetworkList = iDetectedNetworksRetrieve->RetrieveListV2L();
	
	if(iNetworkList && retrievedNetworkList)
		{
		ret |= (retrievedNetworkList->Enumerate() != iNetworkList->Enumerate());
		
		const TInt KCount = retrievedNetworkList->Enumerate();
		for(TInt i = 0; i < KCount && ret == KErrNone; ++i)
			{
			const RMobilePhone::TMobilePhoneNetworkInfoV2& info1 = retrievedNetworkList->GetEntryL(i);
			const RMobilePhone::TMobilePhoneNetworkInfoV2& info2 = iNetworkList->GetEntryL(i);

			ret |= 	(info1.iAccess 			!= info2.iAccess);
			ret |=	(info1.iMode			!= info2.iMode);
			ret |=	(info1.iStatus			!= info2.iStatus);
			ret |=	(info1.iBandInfo 		!= info2.iBandInfo);
			ret |=	(info1.iCountryCode		!= info2.iCountryCode);
			ret |=	(info1.iCdmaSID			!= info2.iCdmaSID);
			ret |=	(info1.iAnalogSID		!= info2.iAnalogSID);
			ret |=	(info1.iNetworkId		!= info2.iNetworkId);
			ret |=	(info1.iDisplayTag		!= info2.iDisplayTag);
			ret |=	(info1.iShortName		!= info2.iShortName);
			ret |=	(info1.iLongName		!= info2.iLongName);
			}
		}
	
	return ret;
   }

//implementation of CGetENListAO

CGetENListAO* CGetENListAO::NewLC(RMobileENStore& aEnStore)
	{
	CGetENListAO* self = new(ELeave) CGetENListAO(aEnStore);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }
      
CGetENListAO::CGetENListAO(RMobileENStore& aEnStore)
    : 	CActive(EPriorityNormal), 
    	iEnStore(aEnStore)
    {
    }
      

void CGetENListAO::ConstructL()
	{
	iActiveScheduler = new (ELeave) CActiveScheduler();
	CActiveScheduler::Install(iActiveScheduler);

	iRetrieveMobilePhoneENList = CRetrieveMobilePhoneENList::NewL(iEnStore);
    iCanceller = new (ELeave) CTestCanceller(*this);
    
	CActiveScheduler::Add(this);
	}
         
CGetENListAO::~CGetENListAO()
    {
    Cancel();
    
    delete iRetrieveMobilePhoneENList;
    iRetrieveMobilePhoneENList = NULL;
    
	delete iCanceller;
	iCanceller = NULL;
	
	delete iActiveScheduler;
	iActiveScheduler = NULL;
	}

void CGetENListAO::TestRetrieveL()
    {
    
    SetActive();
    iRetrieveMobilePhoneENList->Cancel();
    iRetrieveMobilePhoneENList->Start(iStatus);
    CActiveScheduler::Start();
	} 

void CGetENListAO::TestCancel()
	{
    SetActive();
    iRetrieveMobilePhoneENList->Cancel();
    iRetrieveMobilePhoneENList->Start(iStatus);

    iCanceller->Cancel();
    iCanceller->Call();
    
    CActiveScheduler::Start();
	}      

void CGetENListAO::RunL()
    {
    CActiveScheduler::Stop();
	}
      

void CGetENListAO::DoCancel()
    {
    iRetrieveMobilePhoneENList->Cancel();
    }       

CMobilePhoneENList* CGetENListAO::RetrieveListL()
	{
	return iRetrieveMobilePhoneENList->RetrieveListL();
	}

//implementation of CGetONListAO

CGetONListAO* CGetONListAO::NewLC(RMobileONStore& aOnStore)
	{
	CGetONListAO* self = new(ELeave) CGetONListAO(aOnStore);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }
      
CGetONListAO::CGetONListAO(RMobileONStore& aOnStore)
    : 	CActive(EPriorityNormal), 
    	iOnStore(aOnStore)
    {
    }
      

void CGetONListAO::ConstructL()
	{
	iActiveScheduler = new (ELeave) CActiveScheduler();
	CActiveScheduler::Install(iActiveScheduler);

	iRetrieveMobilePhoneONList = CRetrieveMobilePhoneONList::NewL(iOnStore);
    iCanceller = new (ELeave) CTestCanceller(*this);
    
	CActiveScheduler::Add(this);
	}
         
CGetONListAO::~CGetONListAO()
    {
    Cancel();
    
    delete iRetrieveMobilePhoneONList;
    iRetrieveMobilePhoneONList = NULL;
    
	delete iCanceller;
	iCanceller = NULL;
	
	delete iActiveScheduler;
	iActiveScheduler = NULL;
	}

void CGetONListAO::TestRetrieveL()
    {
    SetActive();
    iRetrieveMobilePhoneONList->Cancel();
    iRetrieveMobilePhoneONList->Start(iStatus);
    CActiveScheduler::Start();
	} 

void CGetONListAO::TestCancel()
	{
    SetActive();
    iRetrieveMobilePhoneONList->Cancel();
    iRetrieveMobilePhoneONList->Start(iStatus);

    iCanceller->Cancel();
    iCanceller->Call();
    
    CActiveScheduler::Start();
	}      

void CGetONListAO::RunL()
    {
    CActiveScheduler::Stop();
	}
      

void CGetONListAO::DoCancel()
    {
    iRetrieveMobilePhoneONList->Cancel();
    }       

CMobilePhoneONList* CGetONListAO::RetrieveListL()
	{
	return iRetrieveMobilePhoneONList->RetrieveListL();
	}


//implementation of CGetSMSListAO

CGetSMSListAO* CGetSMSListAO::NewLC(RMobileSmsStore& aSmsStore)
	{
	CGetSMSListAO* self = new(ELeave) CGetSMSListAO(aSmsStore);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }
      
CGetSMSListAO::CGetSMSListAO(RMobileSmsStore& aSmsStore)
    : 	CActive(EPriorityNormal), 
    	iSmsStore(aSmsStore)
    {
    }
      

void CGetSMSListAO::ConstructL()
	{
	iActiveScheduler = new (ELeave) CActiveScheduler();
	CActiveScheduler::Install(iActiveScheduler);

	RMobileSmsStore::TMobileSmsEntryExtensionId type = RMobileSmsStore::KETelMobileGsmSmsEntryV1;
	iRetrieveMobilePhoneSmsList = CRetrieveMobilePhoneSmsList::NewL(iSmsStore, type);
    iCanceller = new (ELeave) CTestCanceller(*this);
    
	CActiveScheduler::Add(this);
	}
         
CGetSMSListAO::~CGetSMSListAO()
    {
    Cancel();
    
    delete iRetrieveMobilePhoneSmsList;
    iRetrieveMobilePhoneSmsList = NULL;
    
	delete iCanceller;
	iCanceller = NULL;
	
	delete iActiveScheduler;
	iActiveScheduler = NULL;
	}

void CGetSMSListAO::TestRetrieveL()
    {
    SetActive();
    iRetrieveMobilePhoneSmsList->Cancel();
    iRetrieveMobilePhoneSmsList->Start(iStatus);
    CActiveScheduler::Start();
	} 

void CGetSMSListAO::TestCancel()
	{
    SetActive();
    iRetrieveMobilePhoneSmsList->Cancel();
    iRetrieveMobilePhoneSmsList->Start(iStatus);

    iCanceller->Cancel();
    iCanceller->Call();
    
    CActiveScheduler::Start();
	}      

void CGetSMSListAO::RunL()
    {
    CActiveScheduler::Stop();
	}
      

void CGetSMSListAO::DoCancel()
    {
    iRetrieveMobilePhoneSmsList->Cancel();
    }       

CMobilePhoneGsmSmsList* CGetSMSListAO::RetrieveListL()
	{
	return iRetrieveMobilePhoneSmsList->RetrieveGsmListL();
	}


//implementation of CGetMbmsMonitorServiceListAO

CGetMbmsMonitorServiceListAO* CGetMbmsMonitorServiceListAO::NewLC(RPacketService& aPacketService)
	{
	CGetMbmsMonitorServiceListAO* self = new (ELeave) CGetMbmsMonitorServiceListAO(aPacketService);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }
      
CGetMbmsMonitorServiceListAO::CGetMbmsMonitorServiceListAO(RPacketService& aPacketService)
    : 	CActive(EPriorityNormal), 
    	iPacketService(aPacketService)
    {
    }
      

void CGetMbmsMonitorServiceListAO::ConstructL()
	{
	iActiveScheduler = new (ELeave) CActiveScheduler();
	CActiveScheduler::Install(iActiveScheduler);

	iRetrievePcktMbmsMonitoredServices = CRetrievePcktMbmsMonitoredServices::NewL(iPacketService);
    iCanceller = new (ELeave) CTestCanceller(*this);
    
	CActiveScheduler::Add(this);
	}
         
CGetMbmsMonitorServiceListAO::~CGetMbmsMonitorServiceListAO()
    {
    Cancel();
    
    delete iRetrievePcktMbmsMonitoredServices;
    iRetrievePcktMbmsMonitoredServices = NULL;
    
	delete iCanceller;
	iCanceller = NULL;
	
	delete iActiveScheduler;
	iActiveScheduler = NULL;
	}

void CGetMbmsMonitorServiceListAO::TestRetrieveL()
    {
    SetActive();
    iRetrievePcktMbmsMonitoredServices->Cancel();
    iRetrievePcktMbmsMonitoredServices->Start(iStatus);
    CActiveScheduler::Start();
	} 

void CGetMbmsMonitorServiceListAO::TestCancel()
	{
    SetActive();
    iRetrievePcktMbmsMonitoredServices->Cancel();
    iRetrievePcktMbmsMonitoredServices->Start(iStatus);

    iCanceller->Cancel();
    iCanceller->Call();
    
    CActiveScheduler::Start();
	}      

void CGetMbmsMonitorServiceListAO::RunL()
    {
    CActiveScheduler::Stop();
	}
      

void CGetMbmsMonitorServiceListAO::DoCancel()
    {
    iRetrievePcktMbmsMonitoredServices->Cancel();
    }       

CPcktMbmsMonitoredServiceList* CGetMbmsMonitorServiceListAO::RetrieveListL()
	{
	return iRetrievePcktMbmsMonitoredServices->RetrieveListL();
	}

TBool CGetMbmsMonitorServiceListAO::CheckResultsL(CPcktMbmsMonitoredServiceList& aMbmsMonitoredServiceList)
	{
	CPcktMbmsMonitoredServiceList* mbmsMonitoredServiceRetrieveList = iRetrievePcktMbmsMonitoredServices->RetrieveListL();
	
	if(mbmsMonitoredServiceRetrieveList)
		{
		
		if(mbmsMonitoredServiceRetrieveList->Enumerate() != aMbmsMonitoredServiceList.Enumerate())
			{
			return EFalse;
			}
		
		const TInt listCount = mbmsMonitoredServiceRetrieveList->Enumerate();
		for(TInt i = 0; i < listCount; ++i)
			{
			const RPacketService::TMbmsServiceAvailabilityV1& entry1 = mbmsMonitoredServiceRetrieveList->GetEntryL(i);
			const RPacketService::TMbmsServiceAvailabilityV1& entry2 = aMbmsMonitoredServiceList.GetEntryL(i);
			
			TBool equal = 	(entry1.ExtensionId() == entry2.ExtensionId()) &&
							(entry1.iTmgi.GetMCC() == entry2.iTmgi.GetMCC()) &&
							(entry1.iTmgi.GetMNC() == entry2.iTmgi.GetMNC()) &&
							(entry1.iTmgi.GetServiceId() == entry2.iTmgi.GetServiceId()) &&
							(entry1.iMbmsServiceMode == entry2.iMbmsServiceMode) &&
							(entry1.iMbmsAvailabilityStatus == entry2.iMbmsAvailabilityStatus) &&
							(entry1.iMbmsAccessBearer == entry2.iMbmsAccessBearer);
			
			if (!equal)
				{
				return EFalse;
				}
			}
		
		}

	return ETrue;
	}


//implementation of CGetMbmsSessionListAO

CGetMbmsSessionListAO* CGetMbmsSessionListAO::NewLC(RPacketMbmsContext& aPacketMbmsContext)
	{
	CGetMbmsSessionListAO* self = new (ELeave) CGetMbmsSessionListAO(aPacketMbmsContext);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }
      
CGetMbmsSessionListAO::CGetMbmsSessionListAO(RPacketMbmsContext& aPacketMbmsContext)
    : 	CActive(EPriorityNormal), 
    	iPacketMbmsContext(aPacketMbmsContext)
    {
    }
      

void CGetMbmsSessionListAO::ConstructL()
	{
	iActiveScheduler = new (ELeave) CActiveScheduler();
	CActiveScheduler::Install(iActiveScheduler);

	iSessionIdList =  RPacketMbmsContext::CMbmsSession::NewL();
	
	iRetrievePcktMbmsSessionList = CRetrievePcktMbmsSessionList::NewL(iPacketMbmsContext, *iSessionIdList);
    iCanceller = new (ELeave) CTestCanceller(*this);
    
	CActiveScheduler::Add(this);
	}
         
CGetMbmsSessionListAO::~CGetMbmsSessionListAO()
    {
    Cancel();
    
    delete iRetrievePcktMbmsSessionList;
    iRetrievePcktMbmsSessionList = NULL;
    
    delete iSessionIdList;
    iSessionIdList = NULL;
    
	delete iCanceller;
	iCanceller = NULL;
	
	delete iActiveScheduler;
	iActiveScheduler = NULL;
	}

void CGetMbmsSessionListAO::TestRetrieveL()
    {
    SetActive();
    iRetrievePcktMbmsSessionList->Cancel();
    iRetrievePcktMbmsSessionList->Start(iStatus);
    CActiveScheduler::Start();
	} 

void CGetMbmsSessionListAO::TestCancel()
	{
    SetActive();
    iRetrievePcktMbmsSessionList->Cancel();
    iRetrievePcktMbmsSessionList->Start(iStatus);

    iCanceller->Cancel();
    iCanceller->Call();
    
    CActiveScheduler::Start();
	}      

void CGetMbmsSessionListAO::RunL()
    {
    CActiveScheduler::Stop();
	}
      

void CGetMbmsSessionListAO::DoCancel()
    {
    iRetrievePcktMbmsSessionList->Cancel();
    }       

RPacketMbmsContext::CMbmsSession& CGetMbmsSessionListAO::RetrieveListL()
	{
	return *iSessionIdList;
	}

TBool CGetMbmsSessionListAO::CheckResultsL(RPacketMbmsContext::CMbmsSession& aMbmsSessionList)
	{
	if(aMbmsSessionList.iSessionIdList.Count() != iSessionIdList->iSessionIdList.Count())
		{
		return EFalse;
		}
	
	const TInt count = aMbmsSessionList.iSessionIdList.Count();
	
	for(TInt i = 0; i < count; ++i)
		{
		if(aMbmsSessionList.iSessionIdList[i] != iSessionIdList->iSessionIdList[i])
			{
			return EFalse;
			}
		}

	return ETrue;
	}
