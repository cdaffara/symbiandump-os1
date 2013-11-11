// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file testltsysmsstore.h
// 

#ifndef TESTLTSYSMSSTORE_H
#define TESTLTSYSMSSTORE_H

//system include 
#include <e32std.h>
#include <e32base.h>

//user include
#include "testltsysmsbase.h"
#include "testltsymodel.h"
// CLASS DECLARATION

/**
 * CTestLtsySmsStore
 *
 */
class CTestLtsySmsStore : public CTestLtsySmsBase
	{
public:

	~CTestLtsySmsStore();
	static CTestLtsySmsStore* NewL(CTestLtsyModel& aTestModel);
	static CTestLtsySmsStore* NewLC(CTestLtsyModel& aTestModel);
	CTestLtsySmsStore(CTestLtsyModel& aTestModel);
	void ConstructL();
protected:	
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	void StoreSmsL();
	void ReceiveSmsL();
	void WriteSmsL();
	void AckSmsStoredL();
	void NackSmsStoredL();
private:
	void FillWriteSmsAttibutesL(TBuf8<400> aTpdu,RMobilePhone::TMobileAddress  aMsgSca);
	/**
	*  Receiving message's attributs
	*/ 
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 iReceiveAttr;
	/**
	*  Receiving message's TPDU
	*/
	TBuf8<KMessageLen> iRecvMsg;
	
	}; // class CTestLtsySmsStore
_LIT(KCTestLtsySmsStore,"CTestLtsySmsStore");

#endif // TESTLTSYSMSSTORE_H
