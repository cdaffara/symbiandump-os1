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
//

/**
 @file
 @internalAll 
*/

#ifndef LISTRETRIEVERAO_H
#define LISTRETRIEVERAO_H

#include <etelmm.h>
#include <etelpckt.h>
#include <e32base.h>
#include <mmretrieve.h>

class RMockLtsy;

class CRetrievePcktMbmsMonitoredServices;
class CRetrievePcktMbmsSessionList;

class CTestCanceller : public CAsyncOneShot
	{
public:
	CTestCanceller(CActive& aRetrieve);
	~CTestCanceller();
	
private:
	void ConstructL();
	void RunL();
private:
	CActive& iRetrieve;// not owned by this
	};

/**************************************************************/


class CGetDetectedNetworksAO : public CActive
	{
public:
	
	static CGetDetectedNetworksAO* NewLC(RMobilePhone& aPhone, RMockLtsy& aMockLtsy);
	~CGetDetectedNetworksAO();
	void TestRetrieveL(CMobilePhoneNetworkListV2& aNetworkList);
	void TestRetrieveNegativeL();
	TInt CheckResultsL();
	void TestCancel();
	inline TInt RetrieveLastError() const {return iStatus.Int();}

protected:
	CGetDetectedNetworksAO(RMobilePhone& aPhone, RMockLtsy& aMockLtsy);
	void ConstructL();
	void RunL();
	void DoCancel();

protected:
	RMobilePhone& iPhone;
	RMockLtsy& iMockLtsy;

	//owned
	CRetrieveMobilePhoneDetectedNetworks* iDetectedNetworksRetrieve;
	CTestCanceller* iCanceller;
	CActiveScheduler* iActiveScheduler; 
	
	//not owned
	CMobilePhoneNetworkListV2* iNetworkList; 
	};

/**************************************************************/

class CGetENListAO : public CActive
	{
public:
	
	static CGetENListAO* NewLC(RMobileENStore& aEnStore);
	~CGetENListAO();
	void TestRetrieveL();
	void TestCancel();
	CMobilePhoneENList* RetrieveListL();
	
	inline TInt RetrieveLastError() const {return iStatus.Int();}

protected:
	CGetENListAO(RMobileENStore& aEnStore);
	void ConstructL();
	void RunL();
	void DoCancel();

protected:
	RMobileENStore& iEnStore;

	//owned
	CTestCanceller* iCanceller;
	CActiveScheduler* iActiveScheduler; 
	CRetrieveMobilePhoneENList* iRetrieveMobilePhoneENList;
	};

/**************************************************************/


class CGetONListAO : public CActive
	{
public:
	
	static CGetONListAO* NewLC(RMobileONStore& aOnStore);
	~CGetONListAO();
	void TestRetrieveL();
	void TestCancel();
	CMobilePhoneONList* RetrieveListL();
	
	inline TInt RetrieveLastError() const {return iStatus.Int();}

protected:
	CGetONListAO(RMobileONStore& aOnStore);
	void ConstructL();
	void RunL();
	void DoCancel();

protected:
	RMobileONStore& iOnStore;

	//owned
	CTestCanceller* iCanceller;
	CActiveScheduler* iActiveScheduler; 
	CRetrieveMobilePhoneONList* iRetrieveMobilePhoneONList;
	};

/**************************************************************/


class CGetSMSListAO : public CActive
	{
public:
	
	static CGetSMSListAO* NewLC(RMobileSmsStore& aSmsStore);
	~CGetSMSListAO();
	void TestRetrieveL();
	void TestCancel();
	CMobilePhoneGsmSmsList* RetrieveListL();
	
	inline TInt RetrieveLastError() const {return iStatus.Int();}

protected:
	CGetSMSListAO(RMobileSmsStore& aSmsStore);
	void ConstructL();
	void RunL();
	void DoCancel();

protected:
	RMobileSmsStore& iSmsStore;

	//owned
	CTestCanceller* iCanceller;
	CActiveScheduler* iActiveScheduler; 	
	CRetrieveMobilePhoneSmsList* iRetrieveMobilePhoneSmsList;
	};

/**************************************************************/


class CGetMbmsMonitorServiceListAO : public CActive
	{
public:
	
	static CGetMbmsMonitorServiceListAO* NewLC(RPacketService& aPacketService);
	~CGetMbmsMonitorServiceListAO();
	void TestRetrieveL();
	void TestCancel();
	TBool CheckResultsL(CPcktMbmsMonitoredServiceList& aMbmsMonitoredServiceList);
	CPcktMbmsMonitoredServiceList* RetrieveListL();
	
	inline TInt RetrieveLastError() const {return iStatus.Int();}

protected:
	CGetMbmsMonitorServiceListAO(RPacketService& aPacketService);
	void ConstructL();
	void RunL();
	void DoCancel();

protected:
	RPacketService& iPacketService;

	//owned
	CTestCanceller* iCanceller;
	CActiveScheduler* iActiveScheduler; 
	CRetrievePcktMbmsMonitoredServices* iRetrievePcktMbmsMonitoredServices;
	};

/**************************************************************/

class CGetMbmsSessionListAO : public CActive
	{
public:
	
	static CGetMbmsSessionListAO* NewLC(RPacketMbmsContext& aPacketMbmsContext);
	~CGetMbmsSessionListAO();
	void TestRetrieveL();
	void TestCancel();
	RPacketMbmsContext::CMbmsSession& RetrieveListL();
	TBool CheckResultsL(RPacketMbmsContext::CMbmsSession& aMbmsSessionList);
	
	inline TInt RetrieveLastError() const {return iStatus.Int();}

protected:
	CGetMbmsSessionListAO(RPacketMbmsContext& aPacketMbmsContext);
	void ConstructL();
	void RunL();
	void DoCancel();

protected:
	RPacketMbmsContext& iPacketMbmsContext;

	//owned
	CTestCanceller* iCanceller;
	CActiveScheduler* iActiveScheduler; 
	RPacketMbmsContext::CMbmsSession* iSessionIdList;
	CRetrievePcktMbmsSessionList* iRetrievePcktMbmsSessionList;
	};

/**************************************************************/

#endif
