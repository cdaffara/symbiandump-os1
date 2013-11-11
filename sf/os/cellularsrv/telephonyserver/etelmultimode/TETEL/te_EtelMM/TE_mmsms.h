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

#ifndef _TE_MMSMS_H_
#define _TE_MMSMS_H_

#include "TE_EtelMMTestStepBase.h"
#include <etelmm.h>
#include <e32base.h>
#include <mmretrieve.h>
#include "TE_mmcanceller.h"

class CTestSmsMessaging : public CEtelMMTestStepBase
	{
public:
	TInt iTestCount;
	CTestSmsMessaging() ;
	~CTestSmsMessaging(){} ;
	virtual enum TVerdict doTestStepL();
	/*static*/ void ValidateGetSmsMessageStoreInfo(const RMobilePhoneStore::TMobilePhoneStoreInfoV1 &aSyncInfo);
	/*static*/ void ValidateSmspEntry(const RMobileSmsMessaging::TMobileSmspEntryV1& aEntry);
	/*static*/ void InitSmspEntry(RMobileSmsMessaging::TMobileSmspEntryV1& aEntry);
	/*static*/ void ValidateReceivedSmsMessage(RMobileSmsMessaging::TMobileSmsGsmTpdu& aMsg, RMobileSmsMessaging::TMobileSmsReceiveAttributesV1& aAtt);


/*private:*/
	class CTestGetSmspList : public CActive
		{
	public:
		static CTestGetSmspList* NewLC(RMobileSmsMessaging& aMessaging, CTestSmsMessaging* aTestSmsMessaging);
		~CTestGetSmspList();
		void Start();
		CTestCanceller* RetrieveCancelPtr();
	protected:
		CTestGetSmspList(RMobileSmsMessaging& aMessaging, CTestSmsMessaging* aTestSmsMessaging);
		void ConstructL();
		void RunL();
		void DoCancel();
		TInt CheckResults();
	protected:
		CRetrieveMobilePhoneSmspList* iRetrieve;
		CTestCanceller* iCanceller;
		RMobileSmsMessaging& iMessaging;
		CTestSmsMessaging* iTestSmsMessaging;
		};

friend class CTestGetSmspList;

	};

#endif
