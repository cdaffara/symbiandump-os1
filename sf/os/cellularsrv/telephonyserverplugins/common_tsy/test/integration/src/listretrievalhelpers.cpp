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
// Implementation of list retrieval helpers classes
// 
//

/**
 @file 
*/
#include "listretrievalhelpers.h"
#include <mmretrieve.h>
#include <etelmm.h>

//Canceller class definition
CTestCanceller::CTestCanceller(CActive* aRetrieve) :
CAsyncOneShot(EPriorityNormal), iRetrieve(aRetrieve)
	{
	}
	
CTestCanceller::~CTestCanceller()
	{
	}
	
void CTestCanceller::ConstructL()
	{
	}
	
void CTestCanceller::RunL()
	{
	iRetrieve->Cancel();
	CActiveScheduler::Stop();
	};
	
//CRetrieveListHelperBase class definition
template<class TRetriever, class TList, class TSubSess>
CRetrieveListHelperBase<TRetriever, TList, TSubSess>::~CRetrieveListHelperBase()
	{
	if(iRetrieve)
		{
		iRetrieve->Cancel();
		}
	delete iList;
	delete iRetrieve;
	delete iCanceller;
	delete iScheduler;
	}
	
template<class TRetriever, class TList, class TSubSess>
CRetrieveListHelperBase<TRetriever, TList, TSubSess>::CRetrieveListHelperBase(TSubSess& aSubSess) :
	CActive(EPriorityNormal), iSubSess(aSubSess)
	{
	}

template<class TRetriever, class TList, class TSubSess>
void CRetrieveListHelperBase<TRetriever, TList, TSubSess>::ConstructL()
	{
	//create scheduler if not already present
	if(!CActiveScheduler::Current())
		{
		iScheduler = new(ELeave) CActiveScheduler;
    	CActiveScheduler::Install(iScheduler);
		}
	CreateEtelRetrieverL();
	iCanceller = new (ELeave) CTestCanceller(this);
	CActiveScheduler::Add(this);
	}



template<class TRetriever, class TList, class TSubSess>
void CRetrieveListHelperBase<TRetriever, TList, TSubSess>::RunL()
	{
    CActiveScheduler::Stop();
	}

template<class TRetriever, class TList, class TSubSess>
void CRetrieveListHelperBase<TRetriever, TList, TSubSess>::DoCancel()
	{
	iRetrieve->Cancel();
	}
	

	
//
//Definitions for concrete implementations of CRetrieveListHelper//

//CRetrieveMobilePhoneDetectedNetworksHelper
CRetrieveListHelperBase<CRetrieveMobilePhoneDetectedNetworks, CMobilePhoneNetworkList, RMobilePhone>* CRetrieveMobilePhoneDetectedNetworksHelper::NewL(RMobilePhone& aPhone,TListVersion aListNumber)
	{
	CRetrieveMobilePhoneDetectedNetworksHelper* self = new (ELeave) CRetrieveMobilePhoneDetectedNetworksHelper(aPhone,aListNumber);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
CRetrieveMobilePhoneDetectedNetworksHelper::CRetrieveMobilePhoneDetectedNetworksHelper(RMobilePhone& aSess,TListVersion aListNumber)
: CRetrieveListHelperBase<CRetrieveMobilePhoneDetectedNetworks, CMobilePhoneNetworkList, RMobilePhone>(aSess), iListVersionToRetrieve(aListNumber)
	{	
	}

void CRetrieveMobilePhoneDetectedNetworksHelper::CreateEtelRetrieverL()
	{
	//create etelmm list retrieval object
	iRetrieve = CRetrieveMobilePhoneDetectedNetworks::NewL(iSubSess);
	}
	
void CRetrieveMobilePhoneDetectedNetworksHelper::StartListRetrieval()
	{
		switch(iListVersionToRetrieve)
		{
			case ERetrieveListV1:
			{
				iRetrieve->Start(iStatus);
				break;		
			}
			case ERetrieveListV2:
			{
				iRetrieve->StartV2(iStatus);
				break;	
			}
			case ERetrieveListV5:
			{
				iRetrieve->StartV5(iStatus);
				break;	
			}
		}
		
	}
	
TAny* CRetrieveMobilePhoneDetectedNetworksHelper::GetListL()
	{
		switch(iListVersionToRetrieve)
			{
			case ERetrieveListV1:
				{
				return iRetrieve->RetrieveListL();
				}
			case ERetrieveListV2:
				{
				return iRetrieve->RetrieveListV2L();
				}
			case ERetrieveListV5:
				{
				return iRetrieve->RetrieveListV5L();
				}
			default:
				{
				_LIT(KErrorLitText,"Wrong argument was entered by user");
				User::Panic(KErrorLitText, KErrArgument); // Attempting to retrieve a non-existant version of the list.
				break;
				}
			}
		return NULL;

	}	
//

//CRetrieveMobilePhoneSmsListHelper	
CRetrieveMobilePhoneSmsListExec* CRetrieveMobilePhoneSmsListHelper::NewL(RMobilePhoneStore& aSubSess, TInt aStartIndex, TInt aBatchSize)
	{
	CRetrieveMobilePhoneSmsListHelper* self = new (ELeave) CRetrieveMobilePhoneSmsListHelper(aSubSess, aStartIndex, aBatchSize);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
CRetrieveMobilePhoneSmsListHelper::CRetrieveMobilePhoneSmsListHelper(RMobilePhoneStore& aSubSess, TInt aStartIndex, TInt aBatchSize)
: CRetrieveListHelperBase<CRetrieveMobilePhoneSmsList, CMobilePhoneGsmSmsList, RMobilePhoneStore>(aSubSess), iStartIndex(aStartIndex), iBatchSize(aBatchSize)
	{	
	}

void CRetrieveMobilePhoneSmsListHelper::CreateEtelRetrieverL()
	{
	//create etelmm list retrieval object
	iRetrieve = CRetrieveMobilePhoneSmsList::NewL(iSubSess, RMobileSmsStore::KETelMobileGsmSmsEntryV1);
	}
	
void CRetrieveMobilePhoneSmsListHelper::StartListRetrieval()
	{
	if(iStartIndex == -1 || iBatchSize == -1)
		iRetrieve->Start(iStatus);
	else
		iRetrieve->StartBatch(iStatus, iStartIndex,iBatchSize);
	}
	
TAny* CRetrieveMobilePhoneSmsListHelper::GetListL()
	{
	return iRetrieve->RetrieveGsmListL();
	}

//
//CRetrieveMobilePhoneCFListHelper
CRetrieveListHelperBase<CRetrieveMobilePhoneCFList, CMobilePhoneCFList, RMobilePhone>* CRetrieveMobilePhoneCFListHelper::NewL(RMobilePhone& aPhone, RMobilePhone::TMobilePhoneCFCondition aCondition, RMobilePhone::TMobileService aServiceGroup, RMobilePhone::TMobileInfoLocation aLocation)
	{
	CRetrieveMobilePhoneCFListHelper* self = new (ELeave) CRetrieveMobilePhoneCFListHelper(aPhone, aCondition, aServiceGroup, aLocation);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
CRetrieveMobilePhoneCFListHelper::CRetrieveMobilePhoneCFListHelper(RMobilePhone& aSess, RMobilePhone::TMobilePhoneCFCondition aCondition, RMobilePhone::TMobileService aServiceGroup,RMobilePhone::TMobileInfoLocation aLocation)
: CRetrieveListHelperBase<CRetrieveMobilePhoneCFList, CMobilePhoneCFList, RMobilePhone>(aSess), iCondition(aCondition), iServiceGroup(aServiceGroup), iLocation(aLocation)
	{	
	}

void CRetrieveMobilePhoneCFListHelper::CreateEtelRetrieverL()
	{
	//create etelmm list retrieval object
	iRetrieve = CRetrieveMobilePhoneCFList::NewL(iSubSess);
	}
	
void CRetrieveMobilePhoneCFListHelper::StartListRetrieval()
	{
	iRetrieve->Start(iStatus, iCondition, iServiceGroup, iLocation);
	}
	
TAny* CRetrieveMobilePhoneCFListHelper::GetListL()
	{
	return iRetrieve->RetrieveListL();
	}
//
//CRetrieveMobilePhoneCBListHelper
CRetrieveListHelperBase<CRetrieveMobilePhoneCBList, CMobilePhoneCBList, RMobilePhone>* CRetrieveMobilePhoneCBListHelper::NewL(RMobilePhone& aPhone, RMobilePhone::TMobilePhoneCBCondition aCondition,RMobilePhone::TMobileInfoLocation aLocation)
	{
	CRetrieveMobilePhoneCBListHelper* self = new (ELeave) CRetrieveMobilePhoneCBListHelper(aPhone, aCondition,aLocation);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
CRetrieveMobilePhoneCBListHelper::CRetrieveMobilePhoneCBListHelper(RMobilePhone& aSess, RMobilePhone::TMobilePhoneCBCondition aCondition, RMobilePhone::TMobileInfoLocation aLocation)
: CRetrieveListHelperBase<CRetrieveMobilePhoneCBList, CMobilePhoneCBList, RMobilePhone>(aSess), iCondition(aCondition),iLocation(aLocation)
	{	
	}

void CRetrieveMobilePhoneCBListHelper::CreateEtelRetrieverL()
	{
	//create etelmm list retrieval object
	iRetrieve = CRetrieveMobilePhoneCBList::NewL(iSubSess);
	}
	
void CRetrieveMobilePhoneCBListHelper::StartListRetrieval()
	{
	iRetrieve->Start(iStatus, iCondition, iLocation);
	}
	
TAny* CRetrieveMobilePhoneCBListHelper::GetListL()
	{
	return iRetrieve->RetrieveListL();
	}

//
//Definitions for concrete implementations of CRetrieveListHelper//

//CRetrieveMobilePhoneCWListHelper
CRetrieveListHelperBase<CRetrieveMobilePhoneCWList, CMobilePhoneCWList, RMobilePhone>* CRetrieveMobilePhoneCWListHelper::NewL(RMobilePhone& aPhone)
	{
	CRetrieveMobilePhoneCWListHelper* self = new (ELeave) CRetrieveMobilePhoneCWListHelper(aPhone);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
CRetrieveMobilePhoneCWListHelper::CRetrieveMobilePhoneCWListHelper(RMobilePhone& aSess)
: CRetrieveListHelperBase<CRetrieveMobilePhoneCWList, CMobilePhoneCWList, RMobilePhone>(aSess)
	{	
	}

void CRetrieveMobilePhoneCWListHelper::CreateEtelRetrieverL()
	{
	//create etelmm list retrieval object
	iRetrieve = CRetrieveMobilePhoneCWList::NewL(iSubSess);
	}
	
void CRetrieveMobilePhoneCWListHelper::StartListRetrieval()
	{
	iRetrieve->Start(iStatus);
	}
	
TAny* CRetrieveMobilePhoneCWListHelper::GetListL()
	{
	return iRetrieve->RetrieveListL();
	}	

//
//Definitions for concrete implementations of CRetrieveListHelper//

//CRetrieveMobilePhoneCcbsListHelper
CRetrieveListHelperBase<CRetrieveMobilePhoneCcbsList, CMobilePhoneCcbsList, RMobilePhone>* CRetrieveMobilePhoneCcbsListHelper::NewL(RMobilePhone& aPhone)
	{
	CRetrieveMobilePhoneCcbsListHelper* self = new (ELeave) CRetrieveMobilePhoneCcbsListHelper(aPhone);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
CRetrieveMobilePhoneCcbsListHelper::CRetrieveMobilePhoneCcbsListHelper(RMobilePhone& aSess)
: CRetrieveListHelperBase<CRetrieveMobilePhoneCcbsList, CMobilePhoneCcbsList, RMobilePhone>(aSess)
	{	
	}

void CRetrieveMobilePhoneCcbsListHelper::CreateEtelRetrieverL()
	{
	//create etelmm list retrieval object
	iRetrieve = CRetrieveMobilePhoneCcbsList::NewL(iSubSess);
	}
	
void CRetrieveMobilePhoneCcbsListHelper::StartListRetrieval()
	{
	iRetrieve->Start(iStatus);
	}
	
TAny* CRetrieveMobilePhoneCcbsListHelper::GetListL()
	{
	return iRetrieve->RetrieveListL();
	}	


//
//Definitions for concrete implementations of CRetrieveListHelper//

//CRetrieveMobilePhoneSmspListHelper
CRetrieveListHelperBase<CRetrieveMobilePhoneSmspList, CMobilePhoneSmspList, RMobileSmsMessaging>* CRetrieveMobilePhoneSmspListHelper::NewL(RMobileSmsMessaging& aPhone)
	{
	CRetrieveMobilePhoneSmspListHelper* self = new (ELeave) CRetrieveMobilePhoneSmspListHelper(aPhone);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
CRetrieveMobilePhoneSmspListHelper::CRetrieveMobilePhoneSmspListHelper(RMobileSmsMessaging& aSess)
: CRetrieveListHelperBase<CRetrieveMobilePhoneSmspList, CMobilePhoneSmspList, RMobileSmsMessaging>(aSess)
	{	
	}

void CRetrieveMobilePhoneSmspListHelper::CreateEtelRetrieverL()
	{
	//create etelmm list retrieval object
	iRetrieve = CRetrieveMobilePhoneSmspList::NewL(iSubSess);
	}
	
void CRetrieveMobilePhoneSmspListHelper::StartListRetrieval()
	{
	iRetrieve->Start(iStatus);
	}
	
TAny* CRetrieveMobilePhoneSmspListHelper::GetListL()
	{
	return iRetrieve->RetrieveListL();
	}	
//

//
//Definitions for concrete implementations of CRetrieveListHelper//

//CRetrieveMobilePhoneBroadcastIdListHelper
CRetrieveListHelperBase<CRetrieveMobilePhoneBroadcastIdList, CMobilePhoneBroadcastIdList, RMobileBroadcastMessaging>* CRetrieveMobilePhoneBroadcastIdListHelper::NewL(RMobileBroadcastMessaging& aPhone,RMobileBroadcastMessaging::TMobileBroadcastIdType aIdType)
	{
	CRetrieveMobilePhoneBroadcastIdListHelper* self = new (ELeave) CRetrieveMobilePhoneBroadcastIdListHelper(aPhone, aIdType);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
CRetrieveMobilePhoneBroadcastIdListHelper::CRetrieveMobilePhoneBroadcastIdListHelper(RMobileBroadcastMessaging& aSess, RMobileBroadcastMessaging::TMobileBroadcastIdType aIdType)
: CRetrieveListHelperBase<CRetrieveMobilePhoneBroadcastIdList, CMobilePhoneBroadcastIdList, RMobileBroadcastMessaging>(aSess), iIdType(aIdType)
	{	
	}

void CRetrieveMobilePhoneBroadcastIdListHelper::CreateEtelRetrieverL()
	{
	//create etelmm list retrieval object
	iRetrieve = CRetrieveMobilePhoneBroadcastIdList::NewL(iSubSess);
	}
	
void CRetrieveMobilePhoneBroadcastIdListHelper::StartListRetrieval()
	{
	iRetrieve->Start(iStatus,iIdType);
	}
	
TAny* CRetrieveMobilePhoneBroadcastIdListHelper::GetListL()
	{
	return iRetrieve->RetrieveListL();
	}	
//


//
//Definitions for concrete implementations of CRetrieveListHelper//

//CRetrieveMobilePhoneNamListHelper
CRetrieveListHelperBase<CRetrieveMobilePhoneNamList, CMobilePhoneNamList, RMobilePhoneStore>* CRetrieveMobilePhoneNamListHelper::NewL(RMobilePhoneStore& aPhone,TInt aNamId, TListVersion aNamList)
	{
	CRetrieveMobilePhoneNamListHelper* self = new (ELeave) CRetrieveMobilePhoneNamListHelper(aPhone, aNamId, aNamList);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
CRetrieveMobilePhoneNamListHelper::CRetrieveMobilePhoneNamListHelper(RMobilePhoneStore& aSess, TInt aNamId,TListVersion aNamList)
: CRetrieveListHelperBase<CRetrieveMobilePhoneNamList, CMobilePhoneNamList, RMobilePhoneStore>(aSess), iNamId(aNamId), iListVersionToRetrieve(aNamList)
	{	
	}

void CRetrieveMobilePhoneNamListHelper::CreateEtelRetrieverL()
	{
	//create etelmm list retrieval object
	iRetrieve = CRetrieveMobilePhoneNamList::NewL(iSubSess);
	}
	
	
void CRetrieveMobilePhoneNamListHelper::StartListRetrieval()
	{
		switch(iListVersionToRetrieve)
			{
			case ERetrieveListV1:
				{
				iRetrieve->Start(iStatus,iNamId);
				break;		
				}
			case ERetrieveListV4:
				{
				iRetrieve->StartV4(iStatus,iNamId);
				break;	
				}
			
			}
	}
	
TAny* CRetrieveMobilePhoneNamListHelper::GetListL()
	{
	switch(iListVersionToRetrieve)
		{
			case ERetrieveListV1:
				{
				return iRetrieve->RetrieveListL();
				}
			case ERetrieveListV4:
				{
				return iRetrieve->RetrieveListV4L();
				}
			default:
				{
				_LIT(KErrorLitText,"Wrong argument was entered by user");
				User::Panic(KErrorLitText, KErrArgument); // Attempting to retrieve a non-existant version of the list.
				break;
				}
		}
	return NULL;
	
	}	
//


//
//Definitions for concrete implementations of CRetrieveListHelper//

//CRetrieveMobilePhoneONListHelper
CRetrieveListHelperBase<CRetrieveMobilePhoneONList, CMobilePhoneONList, RMobilePhoneStore>* CRetrieveMobilePhoneONListHelper::NewL(RMobilePhoneStore& aPhone)
	{
	CRetrieveMobilePhoneONListHelper* self = new (ELeave) CRetrieveMobilePhoneONListHelper(aPhone);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
CRetrieveMobilePhoneONListHelper::CRetrieveMobilePhoneONListHelper(RMobilePhoneStore& aSess)
: CRetrieveListHelperBase<CRetrieveMobilePhoneONList, CMobilePhoneONList, RMobilePhoneStore>(aSess)
	{	
	}

void CRetrieveMobilePhoneONListHelper::CreateEtelRetrieverL()
	{
	//create etelmm list retrieval object
	iRetrieve = CRetrieveMobilePhoneONList::NewL(iSubSess);
	}
	
void CRetrieveMobilePhoneONListHelper::StartListRetrieval()
	{
	iRetrieve->Start(iStatus);
	}
	
TAny* CRetrieveMobilePhoneONListHelper::GetListL()
	{
	return iRetrieve->RetrieveListL();
	}	
//


//
//Definitions for concrete implementations of CRetrieveListHelper//

//CRetrieveMobilePhoneENListHelper
CRetrieveListHelperBase<CRetrieveMobilePhoneENList, CMobilePhoneENList, RMobilePhoneStore>* CRetrieveMobilePhoneENListHelper::NewL(RMobilePhoneStore& aPhone)
	{
	CRetrieveMobilePhoneENListHelper* self = new (ELeave) CRetrieveMobilePhoneENListHelper(aPhone);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
CRetrieveMobilePhoneENListHelper::CRetrieveMobilePhoneENListHelper(RMobilePhoneStore& aSess)
: CRetrieveListHelperBase<CRetrieveMobilePhoneENList, CMobilePhoneENList, RMobilePhoneStore>(aSess)
	{	
	}

void CRetrieveMobilePhoneENListHelper::CreateEtelRetrieverL()
	{
	//create etelmm list retrieval object
	iRetrieve = CRetrieveMobilePhoneENList::NewL(iSubSess);
	}
	
void CRetrieveMobilePhoneENListHelper::StartListRetrieval()
	{
	iRetrieve->Start(iStatus);
	}
	
TAny* CRetrieveMobilePhoneENListHelper::GetListL()
	{
	return iRetrieve->RetrieveListL();
	}	
//

//Definitions for concrete implementations of CRetrieveListHelper//

//CRetrieveMobilePhonePreferredNetworksHelper
CRetrieveListHelperBase<CRetrieveMobilePhonePreferredNetworks, CMobilePhoneStoredNetworkList, RMobilePhone>* CRetrieveMobilePhonePreferredNetworksHelper::NewL(RMobilePhone& aPhone)
	{
	CRetrieveMobilePhonePreferredNetworksHelper* self = new (ELeave) CRetrieveMobilePhonePreferredNetworksHelper(aPhone);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
CRetrieveMobilePhonePreferredNetworksHelper::CRetrieveMobilePhonePreferredNetworksHelper(RMobilePhone& aSess)
: CRetrieveListHelperBase<CRetrieveMobilePhonePreferredNetworks, CMobilePhoneStoredNetworkList, RMobilePhone>(aSess)
	{	
	}

void CRetrieveMobilePhonePreferredNetworksHelper::CreateEtelRetrieverL()
	{
	//create etelmm list retrieval object
	iRetrieve = CRetrieveMobilePhonePreferredNetworks::NewL(iSubSess);
	}
	
void CRetrieveMobilePhonePreferredNetworksHelper::StartListRetrieval()
	{
	iRetrieve->Start(iStatus);
	}
	
TAny* CRetrieveMobilePhonePreferredNetworksHelper::GetListL()
	{
	return iRetrieve->RetrieveListL();
	}	
//
