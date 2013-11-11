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
// Set of helpers to retireve etelmm lists
// Contains the CTestCanceller class declaration.
// 
//

/**
 @file 
 @internalTechnology
*/
 

#ifndef LISTRETRIEVALHELPERS_H
#define LISTRETRIEVALHELPERS_H

#include <e32base.h>
#include "etelmm.h"
#include <mmretrieve.h>

//canceller class that is used to cancel an outstanding list retrieval operation
class CTestCanceller : public CAsyncOneShot
	{
public:
	CTestCanceller(CActive*);
	~CTestCanceller();
private:
	
	void ConstructL();
	void RunL();
private:
	CActive* iRetrieve;// not owned by this
	};
	
const TInt ECancelAfterStart = 1;
//Base helper class for retrieving etelmm lists. 
//NOTE: CRetrieveListHelperBase owns the list and therefore it should not be explicitly deleted.
//Usage: The helper class encaspulates all the fucntionality needed to retrieve a list.  It will create & install
//an active scheuler if required and will delete it on cleaning up the object. 
/*
Example instantiation, usage & deletion

CRetrieveMobilePhoneSmsListExec* gsmsmslist = CRetrieveMobilePhoneSmsListHelper::NewL(store);	
CleanupStack::PushL(gsmsmslist);
CMobilePhoneGsmSmsList* list = gsmsmslist->DoGetList(err, ECancelAfterStart);
//list owned by CRetrieveMobilePhoneSmsListExec an therefore does not need pushing onto cleanup stack or explicitly deleting.
CleanupStack::PopAndDestroy(); //gsmsmslist
*/

template<class TRetriever, class TList, class TSubSess>
class CRetrieveListHelperBase : public CActive
	{
public:	
	~CRetrieveListHelperBase();
		
public: 
	inline void CancelListRetrieval();
	inline TList* DoGetList(TInt& aCompletionErr, TInt aCancelAfterStart = 0);
		
protected: //pure virtuals
	virtual void CreateEtelRetrieverL() = 0; //implement this to create the etelmm list retrieval object
	virtual void StartListRetrieval() = 0; //implement this to start list retrieval
	virtual TAny* GetListL() = 0; //implement this to get the list returned from the retrieval
	
protected:
	CRetrieveListHelperBase(TSubSess& aPhone);
	inline TList* RetrieveListAndCompletionError(TInt& aCompletionError);
	void ConstructL();
	virtual void RunL();
	virtual void DoCancel();

protected:
	TRetriever* iRetrieve; //owned
	TSubSess& iSubSess; //not owned
private:		
	CActiveScheduler* iScheduler; //owned
	CTestCanceller* iCanceller; //owned
	TList* iList; //owned
	};
	
template<class TRetriever, class TList, class TSubSess>
inline TList* CRetrieveListHelperBase<TRetriever, TList, TSubSess>::DoGetList(TInt& aCompletionErr, TInt aCancelAfterStart)
	{
	delete iList; //delete old list to allow multiple list retrievals off same instance.
	iList = NULL;
	StartListRetrieval();	
	SetActive();
	if(aCancelAfterStart)
		{
		CancelListRetrieval();
		}
	CActiveScheduler::Start();	//start scheduler
	//get results
	return	RetrieveListAndCompletionError(aCompletionErr);
	}
	
template<class TRetriever, class TList, class TSubSess>
inline void CRetrieveListHelperBase<TRetriever, TList, TSubSess>::CancelListRetrieval()
	{
	iCanceller->Call();
	}
	
template<class TRetriever, class TList, class TSubSess>
inline TList* CRetrieveListHelperBase<TRetriever, TList, TSubSess>::RetrieveListAndCompletionError(TInt& aCompletionError)
	{
	aCompletionError = iStatus.Int();
	//delete any old lists
	delete iList;
	TRAP_IGNORE(iList = static_cast<TList*>(GetListL()));
	return iList;
	}
	
//
// enum will be used for CRetrieveMobilePhoneDetectedNetworksHelper class
enum TListVersion
/**
 *  This List is used by DetecetedNetworks and NAM store list retriever objects.
 */
		{
		ERetrieveListV1,
		ERetrieveListV2,
		ERetrieveListV4,
		ERetrieveListV5,
		ERetrieveListV8,
		};
//Declarations for concrete implementations of CRetrieveListHelper
//One for each CRetrieveMobilePhoneXXX
typedef CRetrieveListHelperBase<CRetrieveMobilePhoneDetectedNetworks, CMobilePhoneNetworkList, RMobilePhone> CRetrieveMobilePhoneDetectedNetworksExec;	
class CRetrieveMobilePhoneDetectedNetworksHelper : public CRetrieveListHelperBase<CRetrieveMobilePhoneDetectedNetworks, CMobilePhoneNetworkList, RMobilePhone>
	{
public:
	static CRetrieveMobilePhoneDetectedNetworksExec* NewL(RMobilePhone& aSubSess, TListVersion aListNumber);
protected:
	CRetrieveMobilePhoneDetectedNetworksHelper(RMobilePhone& aPhone,TListVersion aListNumber);
protected: //from CRetrieveListHelper
	virtual void CreateEtelRetrieverL();
	virtual void StartListRetrieval();
	virtual TAny* GetListL();
private:
	TListVersion iListVersionToRetrieve;
	};
//	
typedef CRetrieveListHelperBase<CRetrieveMobilePhoneSmsList, CMobilePhoneGsmSmsList, RMobilePhoneStore> CRetrieveMobilePhoneSmsListExec;
class CRetrieveMobilePhoneSmsListHelper : public CRetrieveListHelperBase<CRetrieveMobilePhoneSmsList, CMobilePhoneGsmSmsList, RMobilePhoneStore>
	{
public:
	static CRetrieveMobilePhoneSmsListExec* NewL(RMobilePhoneStore& aSubSess, TInt aStartIndex=-1, TInt aBatchSize=-1);
protected:
	CRetrieveMobilePhoneSmsListHelper(RMobilePhoneStore& aSubSess, TInt aStartIndex, TInt aBatchSize);
protected: //from CRetrieveListHelper
	virtual void CreateEtelRetrieverL();
	virtual void StartListRetrieval();
	virtual TAny* GetListL();
private:
	TInt iStartIndex;
	TInt iBatchSize;
	};
//	
typedef CRetrieveListHelperBase<CRetrieveMobilePhoneCFList, CMobilePhoneCFList, RMobilePhone> CRetrieveMobilePhoneCFListExec;	
class CRetrieveMobilePhoneCFListHelper : public CRetrieveListHelperBase<CRetrieveMobilePhoneCFList, CMobilePhoneCFList, RMobilePhone>
	{
public:
	static CRetrieveMobilePhoneCFListExec* NewL(RMobilePhone& aSubSess, RMobilePhone::TMobilePhoneCFCondition aCondition, RMobilePhone::TMobileService aServiceGroup,RMobilePhone::TMobileInfoLocation aLocation = RMobilePhone::EInfoLocationCachePreferred );
protected:
	CRetrieveMobilePhoneCFListHelper(RMobilePhone& aPhone, RMobilePhone::TMobilePhoneCFCondition aCondition, RMobilePhone::TMobileService aServiceGroup,RMobilePhone::TMobileInfoLocation aLocation = RMobilePhone::EInfoLocationCachePreferred);
protected: //from CRetrieveListHelper
	virtual void CreateEtelRetrieverL();
	virtual void StartListRetrieval();
	virtual TAny* GetListL();
private:
	RMobilePhone::TMobilePhoneCFCondition iCondition;
	RMobilePhone::TMobileService iServiceGroup;
	RMobilePhone::TMobileInfoLocation iLocation; 
	};
//	
typedef CRetrieveListHelperBase<CRetrieveMobilePhoneCBList, CMobilePhoneCBList, RMobilePhone> CRetrieveMobilePhoneCBListExec;	
class CRetrieveMobilePhoneCBListHelper : public CRetrieveListHelperBase<CRetrieveMobilePhoneCBList, CMobilePhoneCBList, RMobilePhone>
	{
public:
	static CRetrieveMobilePhoneCBListExec* NewL(RMobilePhone& aSubSess, RMobilePhone::TMobilePhoneCBCondition aCondition, RMobilePhone::TMobileInfoLocation aLocation);
protected:
	CRetrieveMobilePhoneCBListHelper(RMobilePhone& aPhone, RMobilePhone::TMobilePhoneCBCondition aCondition,RMobilePhone::TMobileInfoLocation aLocation);
protected: //from CRetrieveListHelper
	virtual void CreateEtelRetrieverL();
	virtual void StartListRetrieval();
	virtual TAny* GetListL();
private:
	RMobilePhone::TMobilePhoneCBCondition iCondition;
	RMobilePhone::TMobileInfoLocation iLocation;
	};

//	
typedef CRetrieveListHelperBase<CRetrieveMobilePhoneCWList, CMobilePhoneCWList, RMobilePhone> CRetrieveMobilePhoneCWListExec;	
class CRetrieveMobilePhoneCWListHelper : public CRetrieveListHelperBase<CRetrieveMobilePhoneCWList, CMobilePhoneCWList, RMobilePhone>
	{
public:
	static CRetrieveMobilePhoneCWListExec* NewL(RMobilePhone& aSubSess);
protected:
	CRetrieveMobilePhoneCWListHelper(RMobilePhone& aPhone);
protected: //from CRetrieveListHelper
	virtual void CreateEtelRetrieverL();
	virtual void StartListRetrieval();
	virtual TAny* GetListL();
	};


//	
typedef CRetrieveListHelperBase<CRetrieveMobilePhoneCcbsList, CMobilePhoneCcbsList, RMobilePhone> CRetrieveMobilePhoneCcbsListExec;	
class CRetrieveMobilePhoneCcbsListHelper : public CRetrieveListHelperBase<CRetrieveMobilePhoneCcbsList, CMobilePhoneCcbsList, RMobilePhone>
	{
public:
	static CRetrieveMobilePhoneCcbsListExec* NewL(RMobilePhone& aSubSess);
protected:
	CRetrieveMobilePhoneCcbsListHelper(RMobilePhone& aPhone);
protected: //from CRetrieveListHelper
	virtual void CreateEtelRetrieverL();
	virtual void StartListRetrieval();
	virtual TAny* GetListL();
	};


//	
typedef CRetrieveListHelperBase<CRetrieveMobilePhoneSmspList, CMobilePhoneSmspList, RMobileSmsMessaging> CRetrieveMobilePhoneSmspListExec;	
class CRetrieveMobilePhoneSmspListHelper : public CRetrieveListHelperBase<CRetrieveMobilePhoneSmspList, CMobilePhoneSmspList, RMobileSmsMessaging>
	{
public:
	static CRetrieveMobilePhoneSmspListExec* NewL(RMobileSmsMessaging& aSubSess);
protected:
	CRetrieveMobilePhoneSmspListHelper(RMobileSmsMessaging& aPhone);
protected: //from CRetrieveListHelper
	virtual void CreateEtelRetrieverL();
	virtual void StartListRetrieval();
	virtual TAny* GetListL();
	};


//	
typedef CRetrieveListHelperBase<CRetrieveMobilePhoneBroadcastIdList, CMobilePhoneBroadcastIdList, RMobileBroadcastMessaging> CRetrieveMobilePhoneBroadcastIdListExec;	
class CRetrieveMobilePhoneBroadcastIdListHelper : public CRetrieveListHelperBase<CRetrieveMobilePhoneBroadcastIdList, CMobilePhoneBroadcastIdList, RMobileBroadcastMessaging>
	{
public:
	static CRetrieveMobilePhoneBroadcastIdListExec* NewL(RMobileBroadcastMessaging& aSubSess,RMobileBroadcastMessaging::TMobileBroadcastIdType aIdType);
protected:
	CRetrieveMobilePhoneBroadcastIdListHelper(RMobileBroadcastMessaging& aPhone, RMobileBroadcastMessaging::TMobileBroadcastIdType aIdType);
protected: //from CRetrieveListHelper
	virtual void CreateEtelRetrieverL();
	virtual void StartListRetrieval();
	virtual TAny* GetListL();
private:
	RMobileBroadcastMessaging::TMobileBroadcastIdType iIdType;
	};



//

//	
typedef CRetrieveListHelperBase<CRetrieveMobilePhoneNamList, CMobilePhoneNamList, RMobilePhoneStore> CRetrieveMobilePhoneNamListExec;	
class CRetrieveMobilePhoneNamListHelper : public CRetrieveListHelperBase<CRetrieveMobilePhoneNamList, CMobilePhoneNamList, RMobilePhoneStore>
	{
public:
	static CRetrieveMobilePhoneNamListExec* NewL(RMobilePhoneStore& aSubSess, TInt aNamId, TListVersion aNamList);
protected:
	CRetrieveMobilePhoneNamListHelper(RMobilePhoneStore& aPhone,TInt iNamId, TListVersion iNamList);
protected: //from CRetrieveListHelper
	virtual void CreateEtelRetrieverL();
	virtual void StartListRetrieval();
	virtual TAny* GetListL();
private:
	TInt iNamId;
	TListVersion iListVersionToRetrieve;
	};


//	
typedef CRetrieveListHelperBase<CRetrieveMobilePhoneONList, CMobilePhoneONList, RMobilePhoneStore> CRetrieveMobilePhoneONListExec;	
class CRetrieveMobilePhoneONListHelper : public CRetrieveListHelperBase<CRetrieveMobilePhoneONList, CMobilePhoneONList, RMobilePhoneStore>
	{
public:
	static CRetrieveMobilePhoneONListExec* NewL(RMobilePhoneStore& aSubSess);
protected:
	CRetrieveMobilePhoneONListHelper(RMobilePhoneStore& aPhone);
protected: //from CRetrieveListHelper
	virtual void CreateEtelRetrieverL();
	virtual void StartListRetrieval();
	virtual TAny* GetListL();
	};


//

//	
typedef CRetrieveListHelperBase<CRetrieveMobilePhoneENList, CMobilePhoneENList, RMobilePhoneStore> CRetrieveMobilePhoneENListExec;	
class CRetrieveMobilePhoneENListHelper : public CRetrieveListHelperBase<CRetrieveMobilePhoneENList, CMobilePhoneENList, RMobilePhoneStore>
	{
public:
	static CRetrieveMobilePhoneENListExec* NewL(RMobilePhoneStore& aSubSess);
protected:
	CRetrieveMobilePhoneENListHelper(RMobilePhoneStore& aPhone);
protected: //from CRetrieveListHelper
	virtual void CreateEtelRetrieverL();
	virtual void StartListRetrieval();
	virtual TAny* GetListL();
	};


//

//	
typedef CRetrieveListHelperBase<CRetrieveMobilePhonePreferredNetworks, CMobilePhoneStoredNetworkList, RMobilePhone> CRetrieveMobilePhonePreferredNetworksExec;	
class CRetrieveMobilePhonePreferredNetworksHelper : public CRetrieveListHelperBase<CRetrieveMobilePhonePreferredNetworks, CMobilePhoneStoredNetworkList, RMobilePhone>
	{
public:
	static CRetrieveMobilePhonePreferredNetworksExec* NewL(RMobilePhone& aSubSess);
protected:
	CRetrieveMobilePhonePreferredNetworksHelper(RMobilePhone& aPhone);
protected: //from CRetrieveListHelper
	virtual void CreateEtelRetrieverL();
	virtual void StartListRetrieval();
	virtual TAny* GetListL();
	};


//

#endif //LISTRETRIEVALHELPERS_H
