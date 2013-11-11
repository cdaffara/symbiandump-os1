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
// EtelPacketMbmsService.h
// This is the header file for 
// 
//

/**@file
@internalComponent */
#ifndef TE_ETELPACKETMBMSSERVICE_H_
#define TE_ETELPACKETMBMSSERVICE_H_
#include "Te_EtelPacketTestStepBase.h"
#include "TE_Pcktcanceller.h"
#include "pcktretrieve.h"

_LIT(KMbmsService,"MBMS_SERVICE");
_LIT(KMbmsSession,"MBMS_SESSION");
class CTestMbmsService : public CTestStepEtelPacket
	{
public:
	CTestMbmsService();
	~CTestMbmsService();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual enum TVerdict doTestStepL( void );
protected:
	void TestMbmsUpdateServiceL();
	void TestMbmsRetrieveServiceListL();
	void TestMbmsServicesEnumerationL();
private:
	RPhone iPhone;
	RPacketService iPcktService;
	CActiveScheduler* iTestScheduler;
public:
	class CTestRetrieveEtelPacketMonitorServices : public CActive
	 	{
	public:
	  	static 	CTestRetrieveEtelPacketMonitorServices* NewLC(RPacketService& aService,CTestMbmsService* aTestService);
	  	~CTestRetrieveEtelPacketMonitorServices();	  	
	  	void TestRetrieve(TBool aStartReqd);
	  	void TestCancel();
	protected:
	CTestRetrieveEtelPacketMonitorServices(RPacketService& aService,CTestMbmsService* aTestService);
	  	void ConstructL();
	  	void RunL();
	  	void DoCancel();
	  	void Start();
	  	CTestCanceller* RetrieveCancelPtr();
	  	TInt CheckMonitorServicesResultsL();
	  	TInt CheckMonitorServicesErrorResultsL();
	protected:
		CRetrievePcktMbmsMonitoredServices* iRetrieve;
	  	CTestCanceller* iCanceller;
	  	RPacketService& iService;
	  	CTestMbmsService* iTestService;
	   	};
	friend class CTestRetrieveEtelPacketMonitorServices;
	};
	
#endif /*TE_ETELPACKETMBMSSERVICE_H_*/
