// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#ifndef _TE_MMBROADCAST_H_
#define _TE_MMBROADCAST_H_

#include "TE_EtelMMTestStepBase.h"
#include <etelmm.h>
#include <e32base.h>
#include <mmretrieve.h>
#include "TE_mmcanceller.h"

class CTestBroadcastMessaging : public CEtelMMTestStepBase
	{
public:
	CTestBroadcastMessaging() ;
	~CTestBroadcastMessaging(){} ;
	virtual enum TVerdict doTestStepL();
	TInt iTestCount;

/*private:*/

	class CTestGetCbmiList : public CActive 
		{
	public:
		static CTestGetCbmiList* NewLC(RMobileBroadcastMessaging &aMessaging, CTestBroadcastMessaging* aTestBroadcastMessaging);
		~CTestGetCbmiList();
		void Start();
		CTestCanceller* RetrieveCancelPtr();
		
	protected:
		void RunL();
		void DoCancel();
		CTestGetCbmiList(RMobileBroadcastMessaging &aMessaging, CTestBroadcastMessaging* aTestBroadcastMessaging);
		void ConstructL();
		TInt CheckResults();
	protected:
		CRetrieveMobilePhoneBroadcastIdList* iRetrieve;
		CTestCanceller* iCanceller;
		RMobileBroadcastMessaging &iMessaging;
		RMobileBroadcastMessaging::TMobileBroadcastIdType iIdType;
		CTestBroadcastMessaging* iTestBroadcastMessaging;
		};

private:

	class CTestStoreCbmiList : public CActive
		{
	public:
		static CTestStoreCbmiList* NewLC(RMobileBroadcastMessaging &aMessaging, CTestBroadcastMessaging* aTestBroadcastMessaging);
		~CTestStoreCbmiList();
		void StartL();
		CTestCanceller* RetrieveCancelPtr();
	private:
		CTestStoreCbmiList(RMobileBroadcastMessaging &aMessaging, CTestBroadcastMessaging* aTestBroadcastMessaging);
		void ConstructL();
		void RunL();
		void DoCancel();
	private:
		CMobilePhoneBroadcastIdList *iList;
		CTestCanceller* iCanceller;
		RMobileBroadcastMessaging &iMessaging;
		RMobileBroadcastMessaging::TMobileBroadcastIdType iIdType;
		CTestBroadcastMessaging* iTestBroadcastMessaging;
		};
friend class CTestGetCbmiList;
friend class CTestStoreCbmiList;

	};

#endif
