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
// @file
// 
//

#ifndef _TE_WAPDGRMBASE_H_
#define _TE_WAPDGRMBASE_H_

#include <e32def.h>
#include <e32std.h>
#include <f32file.h>

#ifdef SYMBIAN_ZERO_COPY_NETWORKING
#include <comms-infras/commsbufpondop.h>
#else
#include <es_mbman.h>
#endif // SYMBIAN_ZERO_COPY_NETWORKING

#include <cflog.h>

#include "wappstor.h"
#include "smsstacklog.h"
#include "WAPDGRM.H"

#include <testconfigfileparser.h>
#include <test/testexecuteserverbase.h>

_LIT8(KWapDgrm1,    "");
_LIT8(KWapDgrm2,    "Auto on kuin ikääntynyt jalosukuinen englantilainen herrasmies konsanaan. Kun istuu sen juuri oikealla tavalla hieman kuluneella nahkaistuimella ja katselee pähkinänpuun juuresta tehtyä kojelautaa, tuntee olevansa itseänsä paremmassa seurassa. Ajassa, jolloin hienostuneisuudella oli vielä merkitystä. Korin värinä on hillitysti hohteleva Opaslecent Dark Green ja hieman punertavan ruskea nahkasisustus on soinnutettu sävy sävyyn pähkinänjuuresta tehdyn kojelaudan kanssa aina etuistuimen selkänojasta aukeavaa pöytää myöten. 2,4 litraisessa moottorissa on hevosvoimia 120");
_LIT8(KWapDgrm3,    "1234567890");
_LIT8(KWapDgrm4,    "A123456789B123456789C123456789D123456789E123456789F123456789G123456789H123456789I123456789J123456789K123456789L123456789M123456789N123456789O123456789P123456789Q123456789R123456789S123456789T123456789U123456789V123456789W123456789X123456789Y123456789Z123456789A123456789B123456789C123456789D123456789E123456789F123456789G123456789H123456789I123456789J123456789K123456789L123456789M123456789N123456789O123456789P123456789Q123456789R123456789S123456789T123456789U123456789V123456789W123456789X123456789Y123456789Z123456789A123456789B123456789C123456789D123456789E123456789F123456789G123456789H123456789I123456789J123456789K123456789L123456789M123456789N123456789O123456789P123456789Q123456789R123456789S123456789T123456789U123456789V123456789W123456789X123456789Y123456789Z123456789");
_LIT8(KWapDgrm5,    "A123456789B123456789C123456789D123456789E123456789F123456789G123456789H123456789I123456789J123456789K123456789L123456789M123456789N123456789O123456789P123456789Q123456789R123456789S123456789T123456789U123456789V123456789W123456789X123456789Y123456789Z123456789A123456789B123456789C123456789D123456789E123456789F123456789G123456789H123456789I123456789J123456789K123456789L123456789M123456789N123456789O123456789P123456789Q123456789R123456789S123456789T123456789U123456789V123456789W123456789X123456789Y123456789Z123456789A123456789B123456789C123456789D123456789E123456789F123456789G123456789H123456789I123456789J123456789K123456789L123456789M123456789N123456789O123456789P123456789Q123456789R123456789S123456789T123456789U123456789V123456789W123456789X123456789Y123456789Z123456789A123456789B123456789C123456789D123456789E123456789F123456789G123456789H123456789I123");
_LIT8(KWapDgrm6,    "A123456789B123456789C123456789D123456789E123456789F123456789G123456789H123456789I123456789J123456789K123456789L123456789M123456789N123456789O123456789P1");

class CWapDgrmTestStep : public CTestStep
{

public:
	// definition of helper functions
	void _Print(TInt aNum);
	void _Print(const TPtrC8 aStr);
	void _Print(const TDesC& aStr);
	void _PrintBool(TBool aTruth);
	void _Print(CWapDatagram& aWap);
	void _Print(CSmsMessage& aSms);
	void _PrintL(CSmsBufferBase& aSmsBuffer);
	void _PrintL(CArrayPtr<CSmsMessage>& aSmsArray);
	void _FlushReserveArrayL(CWapReassemblyStore* aWapStore);
	void _Parse2L(CArrayPtrFlat<CSmsMessage>& aSmsArray,
				CWapReassemblyStore* aWapStore);
	void ConstructWap(CWapDatagram& aWap, TDesC& aFromAddress,
				TDesC& aToAddress, TInt aFromPort, TInt aToPort,
				TSmsDataCodingScheme::TSmsAlphabet aAlphabet,
				TBool aIs16BitPorts);
	void DoTestWapProtDatagramL(const TDesC8& aBuffer, TDesC& aToAddress,
				TInt aToPort, TDesC& aFromAddress, TInt aFromPort,
				TSmsDataCodingScheme::TSmsAlphabet aAlphabet,
				TBool aIs16BitPorts);

public:
	const TDesC8* iWapDgrmTable[7];
	TBool iIsFlushReserveArray;

public:
 	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

private:
	TInt iInsertSms;
	CArrayPtrFlat<CWapDatagram>* iReserveArray;
	RFs 					iFs;
    CActiveScheduler* 		iScheduler;
#ifdef SYMBIAN_ZERO_COPY_NETWORKING	
	RCommsBufPondOp	iCommsBufPond;
#else
    CMBufManager* 			iBufMgr;
#endif // SYMBIAN_ZERO_COPY_NETWORKING	


	TBool iIsRunParsing;		//controls whether CSmsMessages are pushed for decoding
	TBool iIsPrintEncoded;	//controls whether CSmsMessages are printed out

	__CFLOG_DECLARATION_MEMBER;
};


#endif
