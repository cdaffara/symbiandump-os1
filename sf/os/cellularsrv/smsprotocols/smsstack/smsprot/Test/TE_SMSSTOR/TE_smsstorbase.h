// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef _TE_SMSSTORBASE_H_
#define _TE_SMSSTORBASE_H_

#include <testconfigfileparser.h>
#include <test/testexecuteserverbase.h>
#include <logwrap.h>
#include <es_sock.h>

#include "gsmubuf.h"
#include "Gsmumsg.h"
#include "gsmuset.h"
#include "smsuset.h"
#include "testconfigfileparser.h"
#include "smsuaddr.H"
#include "smsustrm.h"
#include "smsprot.h"

const TInt KMaxNumberOfCharsFor7BitSms = 160;

class CSmsStorTestStep : public CTestStep
	{
public:
	void AddReferenceL( TInt aLogId, TInt aRef );
	CSmsMessage* CreateSmsMessageL( const TDesC& aDes, TSmsDataCodingScheme::TSmsAlphabet aAlphabet, CSmsPDU::TSmsPDUType aPDUType = CSmsPDU::ESmsSubmit) ;
	void AddStatusReportL( TInt aLogId, TInt aRef, TSmsStatus::TSmsStatusValue aValue ) ;
	void SetGlobalsL(TSmsServiceCenterAddress aTel, TSmsServiceCenterAddress aSC, TInt aMsgSize ) ;
    void AddSubmitL( TInt aLogId );
    void TestSmsSegmentationStoreMessageTestL(TInt aMsgLen, TBool aDupMessageRef = EFalse, TInt aAddCount = 20) ;
	void TestAddStatusReportL(TInt aMsgLen, TInt& aStartLogId, TInt& aStartMsgRef, TSmsStatus::TSmsStatusValue aStatus, TInt aAddCount, TBool aAddSubmit, TBool aDupRef) ;
	void TestSmsSegmentionStorePurge2L(TInt aMsgLen, TInt aAddCount, TInt aDelivered) ;
 	void AddDeliverL( TInt aIndex ) ;
	void DoEncodePDUsL(TInt aSize, TSmsDataCodingScheme::TSmsAlphabet aAlphabet, CSmsPDU::TSmsPDUType aPDUType) ;
	void DoInternalizeExternalizeSMSL(TInt aSize, TSmsDataCodingScheme::TSmsAlphabet aAlphabet, CSmsPDU::TSmsPDUType aPDUType) ;
	void FillDes(TDes& aDes,TInt aLength) ;
	void Ascii2Hex( TPtr8 aPDU );
	
	TInt ReadMaxNumMessageValueL();
	void CreateNewSegmentationStoreL();
	void DeleteSegmetationStore();
	void CheckSegmentationStoreForMessagesL(RArray<TInt>& aLogIdsToFind);
	void AddMessageAndCheckPurgeL(TInt aAddNumMessages, TInt aExpectedNumMessages);

public:
 	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

	TSmsServiceCenterAddress iTelephoneNumber ;
	TSmsServiceCenterAddress iServiceCenterNumber ;

	RFs iFs;
	CSmsSegmentationStore* iSegmentationStore;
	CSmsReassemblyStore* iReassemblyStore;
	TSmsSettings iSmsSettings;

	TInt iLogId ;
	TInt iMsgRef ;
	HBufC* iMsgBuf ;
	};

#endif
