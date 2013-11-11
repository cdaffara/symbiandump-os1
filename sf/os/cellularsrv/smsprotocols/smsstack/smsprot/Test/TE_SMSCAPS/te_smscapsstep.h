// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TS_SMSCAPSSTEP.H
// 
//

#ifndef _TE_SMSCAPSSTEP_H_
#define _TE_SMSCAPSSTEP_H_

#include <es_sock.h>
#include <gsmumsg.h>
#include <testconfigfileparser.h>
#include <test/testexecuteserverbase.h>

class CSmsCapsTestStep : public CTestStep
	{
public:
	CSmsCapsTestStep(RSocketServ& aSockServer,RFs& aFs);
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

	CSmsMessage* CreateSmsMessageL(const TDesC& aDes, TSmsDataCodingScheme::TSmsAlphabet aAlphabet, CSmsPDU::TSmsPDUType aType= CSmsPDU::ESmsSubmit);
protected:
	RSocketServ& iSocketServer;
	RFs iFs;
	};

class CSmsCapsBase : public CSmsCapsTestStep
{
public:
    CSmsCapsBase(RSocketServ& aSockServer,RFs& aFs);

	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
};

class CSmsCapsSmsIoctlDeleteSms : public CSmsCapsTestStep
	{
public:
	CSmsCapsSmsIoctlDeleteSms(RSocketServ& aSockServer,RFs& aFs) ;

	virtual TVerdict doTestStepL();
	};

class CSmsCapsSmsIoctlEnumerateSms : public CSmsCapsTestStep
	{
public:
	CSmsCapsSmsIoctlEnumerateSms(RSocketServ& aSockServer,RFs& aFs) ;

	virtual TVerdict doTestStepL();
	};

class CSmsCapsSmsIoctlReadSucceededSms : public CSmsCapsBase    //TestStep
	{
public:
	CSmsCapsSmsIoctlReadSucceededSms(RSocketServ& aSockServer,RFs& aFs);
	virtual TVerdict doTestStepL();

	};

class CSmsCapsSmsIoctlReadFailedSms : public CSmsCapsBase
	{
public:
	CSmsCapsSmsIoctlReadFailedSms(RSocketServ& aSockServer,RFs& aFs) ;

	virtual TVerdict doTestStepL();
	};


class CSmsCapsSmsIoctlSendSms : public CSmsCapsTestStep
	{
public:
	CSmsCapsSmsIoctlSendSms(RSocketServ& aSockServer,RFs& aFs) ;

	virtual TVerdict doTestStepL();
	};

class CSmsCapsSmsIoctlWriteSms : public CSmsCapsBase
	{
public:
	CSmsCapsSmsIoctlWriteSms(RSocketServ& aSockServer,RFs& aFs) ;

	virtual TVerdict doTestStepL();

	};


class CSmsCapsSmsIoctlReadParamsSms : public CSmsCapsBase
	{
public:
	CSmsCapsSmsIoctlReadParamsSms(RSocketServ& aSockServer,RFs& aFs) ;

	virtual TVerdict doTestStepL();
	};


class CSmsCapsSmsIoctlCompleteReadParamsSms : public CSmsCapsBase
	{
public:
	CSmsCapsSmsIoctlCompleteReadParamsSms(RSocketServ& aSockServer,RFs& aFs) ;

	virtual TVerdict doTestStepL();
	};


class CSmsCapsSmsIoctlWriteParamsSms : public CSmsCapsTestStep
	{
public:
	CSmsCapsSmsIoctlWriteParamsSms(RSocketServ& aSockServer,RFs& aFs) ;

	virtual TVerdict doTestStepL();
	};

class CSmsCapsSmsSetLocalName : public CSmsCapsTestStep
	{
public:
	CSmsCapsSmsSetLocalName(RSocketServ& aSockServer,RFs& aFs) ;

	virtual TVerdict doTestStepL();
	};

class CSmsCapsSmsWrite : public CSmsCapsTestStep
	{
public:
	CSmsCapsSmsWrite(RSocketServ& aSockServer,RFs& aFs) ;

	virtual TVerdict doTestStepL();
	};

// WAP

class CSmsCapsWapSetLocalName : public CSmsCapsTestStep
	{
public:
	CSmsCapsWapSetLocalName(RSocketServ& aSockServer,RFs& aFs) ;
	TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
	};

class CSmsCapsWapWrite : public CSmsCapsTestStep
	{
public:
	CSmsCapsWapWrite(RSocketServ& aSockServer,RFs& aFs) ;
	TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
	};


class CSmsCapsWapIoctl : public CSmsCapsTestStep
	{
public:
	CSmsCapsWapIoctl(RSocketServ& aSockServer,RFs& aFs) ;
	TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
	};

class CSmsCapsWapSetOption : public CSmsCapsTestStep
	{
public:
	CSmsCapsWapSetOption(RSocketServ& aSockServer,RFs& aFs) ;
	TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
	};

#endif
