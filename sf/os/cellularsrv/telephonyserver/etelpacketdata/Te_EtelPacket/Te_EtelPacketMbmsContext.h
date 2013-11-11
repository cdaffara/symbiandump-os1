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
// EtelPacketMbmsContext.h
// This is the header file for 
// 
//

/**@file
@internalComponent */
#ifndef TE_ETELPACKETMBMSCONTEXT_H_
#define TE_ETELPACKETMBMSCONTEXT_H_
#include "Te_EtelPacketTestStepBase.h"
#include "TE_Pcktcanceller.h"
#include "pcktretrieve.h"

_LIT(KMbmsContext,"MBMS_CONTEXT");
class CTestMbmsContext : public CTestStepEtelPacket
	{
public:
	CTestMbmsContext();
	~CTestMbmsContext();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual enum TVerdict doTestStepL( void );
protected:
	void TestOpenMbmsContextL();
	void TestMbmsConfigurationL();
	void TestActivateMbmsContextL();
	void TestDeactivateMbmsContextL();
	void TestMbmsServiceStatusL();
	void TestMbmsUpdateSessionL();
	void TestInitialiseMbmsContextL();
	void TestMbmsGetSessionListL();
private:
	RPhone iPhone;
	RPacketService iPcktService;
	RPacketMbmsContext iMbmsContext;
	CActiveScheduler* iTestScheduler;
	};

NONSHARABLE_CLASS(CTestMbmsSession) : public CActive
		{
public:
	static CTestMbmsSession* NewLC(RPacketMbmsContext& aPcktMbmsContext, RPacketMbmsContext::CMbmsSession& aSessionIdList, CTestMbmsContext* aTestMbmsContext);
	~CTestMbmsSession();
	void Start();		
	CRetrievePcktMbmsSessionList* iRetrieve;
protected:
	CTestMbmsSession(RPacketMbmsContext& aPcktMbmsContext, RPacketMbmsContext::CMbmsSession& aSessionIdList, CTestMbmsContext* aTestMbmsContext);
	void ConstructL();
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
protected:
	RPacketMbmsContext::CMbmsSession& iSessionIdList;	
public:
	CTestCanceller* iCanceller;
protected:
	RPacketMbmsContext& iPcktMbmsContext;
	CTestMbmsContext* iTestMbmsContext;
		};
	
#endif /*TE_ETELPACKETMBMSCONTEXT_H_*/
