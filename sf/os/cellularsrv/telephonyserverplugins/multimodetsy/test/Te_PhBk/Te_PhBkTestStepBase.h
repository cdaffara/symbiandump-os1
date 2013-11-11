// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TE_PHBK_TESTSTEP_BASE__
#define __TE_PHBK_TESTSTEP_BASE__

#include <test/testexecutestepbase.h>

#include <e32base.h>
#include <etel.h>
#include <etelmm.h>
#include <e32test.h>
#include <e32keys.h>
#include <c32comm.h>
#include <f32file.h>
#include <mmretrieve.h>
#include <mpbutil.h>

_LIT(KMmtsyName,"MM");
_LIT(KPhoneName,"GsmPhone1");

const TInt KPBDataClientBuf=800; // this buffer should be able to fit approx 50 entires
const TInt KPBDataClientSmallBuf=45; // this buffer should be able to fit approx 3 entires
const TInt KPPBEntriesNum=7;
const TInt KBufSize=30;

const TUint8 KTypeOfNumber=0x81;

_LIT16(KSymbolText,"(:,)=(),");
_LIT16(KAnnText,"Ann");
_LIT16(KMattSText,"MattS");
_LIT16(KAndyText,"Andy Cloke");		// has an OK in it!
_LIT16(KMattMText,"MattM");
_LIT16(KPaulText,"Paul");
_LIT16(KMumAndDadText,"Mum and Dad");
_LIT16(KTextTooLong,"ABCDEFGHIJKLMNOPQRSTUVWXYZABCD");
_LIT16(KHenrikText,"Henrik");
_LIT16(KHenrikSVText,"HenrikSV");
_LIT16(KMikaelText,"Mikael");
_LIT16(KIngeText,"Inge");
_LIT16(KPatrickText,"Patrick");
_LIT16(KPontusText,"Pontus");
_LIT16(KAndersText,"Anders");
_LIT16(KHajText,"Haj");

_LIT16(KSymbolPhoneNum,"01632960000");
_LIT16(KAnnPhoneNum,"01632960000");
_LIT16(KMattSPhoneNum,"01632960000");
_LIT16(KAndyPhoneNum,"01632960000");
_LIT16(KMattMPhoneNum,"01632960000");
_LIT16(KPaulPhoneNum,"01632960000");
_LIT16(KMumAndDadPhoneNum,"01632960000");
_LIT16(KNumberTooLong,"0171123456789012345678901234567331111114011111111150111111116011111111701111111180111");// needs to be over 80 since the Ericsson 320 can take that many digits
_LIT16(KHenrikPhoneNum,"1712081801");
_LIT16(KHenrikSVPhoneNum,"1712081802");
_LIT16(KMikaelPhoneNum,"1712081803");
_LIT16(KIngePhoneNum,"1712081804");
_LIT16(KPatrickPhoneNum,"1712081805");
_LIT16(KPontusPhoneNum,"1712081806");
_LIT16(KAndersPhoneNum,"1712081807");
_LIT16(KHajPhoneNum,"1712081808");

_LIT16(KSearch1,"Ann");
_LIT16(KSearch2,"Matt");	// Used in a search, phone book contains 'MattS' and 'MattM'
_LIT16(KSearch3,"Unknown");
_LIT16(KSearch4,"Mum");

const TUint KAddressType=129;

class CPhBkTestStep : public CTestStep
	{
public:
 	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

protected:
	void WaitWithTimeout(TRequestStatus& aStatus, TInt aNumberOfMicroSeconds) ;
	TInt clearPhoneBookL(const TDesC& aPhoneBookName) ;
	void checkPhoneBookInfoL(RMobilePhoneStore::TMobilePhoneStoreInfoV1& aInfo) ;
	void WriteBatchOfEntriesToThePhoneL() ;
	TInt CheckPhoneStoreInfo(RMobilePhone& aPhone,	const TDesC& aPhoneStoreName,
													const TDesC& aMode,
													const TDesC& aPhoneStoreDescription);

protected:
	RTelServer iTelServer;
	RMobilePhone iPhone;
	TRequestStatus iStatus;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 iDummyInfo;
	TBool iMESupported;
	TBool iMTSupported;
	};

#endif // __TE_PHBK_TESTSTEP_BASE__
