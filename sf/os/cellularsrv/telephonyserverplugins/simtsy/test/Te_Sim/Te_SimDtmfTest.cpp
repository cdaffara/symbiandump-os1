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

#include "Te_SimDtmfTest.h"

CSimDtmfTest::CSimDtmfTest() 
	{ 
	SetTestStepName(_L("DtmfTest"));
	}

TVerdict CSimDtmfTest::doTestStepL()
	{
	INFO_PRINTF1(_L("BeginDtmfTest"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(0);

	_LIT(KDtmfString0,"45p6789ABCDp");
	_LIT(KDtmfString1,"01w23");

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	INFO_PRINTF2(_L("Result: %d"),ret); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));

	TESTL(iPhone.Initialise()==KErrNone);
	INFO_PRINTF1(_L("Opened phone object"));

	TESTL(iLine.Open(iPhone,KVoiceLineName)==KErrNone);
	INFO_PRINTF1(_L("Opened Line object"));

	TName callName;
	TESTL(iCall.OpenNewCall(iLine,callName)==KErrNone);
	INFO_PRINTF1(_L("Opened Call object"));

	TRequestStatus reqStatus0, reqStatus1;

	INFO_PRINTF2(_L("Dialling %S "),&KTelephoneNumber);
	iCall.Dial(reqStatus0,KTelephoneNumber);
	User::WaitForRequest(reqStatus0);		// Wait for the Call Status Change notification
	TESTL(reqStatus0==KErrNone);
	INFO_PRINTF1(_L("...Call is Connected"));

	INFO_PRINTF1(_L("Testing an erroneous DTMF String Tx Continue"));
	TESTL(iPhone.ContinueDTMFStringSending(ETrue)==KErrNotReady);

	INFO_PRINTF1(_L("Test DTMF Caps"));
	TUint32 dtmfCaps;
	TESTL(iPhone.GetDTMFCaps(dtmfCaps)==KErrNone);
	TESTL(dtmfCaps==(RMobilePhone::KCapsSendDTMFString
				   | RMobilePhone::KCapsSendDTMFSingleTone));

	INFO_PRINTF1(_L("Test Send DTMF String"));
	iPhone.SendDTMFTones(reqStatus0,KDtmfString0);
	User::After(1000000);
	TChar character('1');
	TESTL(iPhone.StartDTMFTone(character)==KErrInUse);
	User::After(1000000);
	TESTL(iPhone.StopDTMFTone()==KErrInUse);
	User::WaitForRequest(reqStatus0);
	TESTL(reqStatus0==KErrNone);

	INFO_PRINTF1(_L("Test Start & Stop DTMF Tone"));
	ret = iPhone.StartDTMFTone(character);
	CHECKPOINTL(ret,KErrNone,CHP_VOICE_CASE("A.8"));

	User::After(1000000);
	ret = iPhone.StopDTMFTone();
	CHECKPOINTL(ret,KErrNone,CHP_VOICE_CASE("A.9"));

	INFO_PRINTF1(_L("Test Send DTMF String with wait & continue"));
	iPhone.NotifyStopInDTMFString(reqStatus1);
	iPhone.SendDTMFTones(reqStatus0,KDtmfString1);
	User::WaitForRequest(reqStatus1);
	TESTL(reqStatus1==KErrNone);
	
	ret = iPhone.ContinueDTMFStringSending(ETrue);
	CHECKPOINTL(ret,KErrNone,CHP_VOICE_CASE("A.11"));

	User::WaitForRequest(reqStatus0);
	TESTL(reqStatus0==KErrNone);
	CHECKPOINTL(reqStatus0,KErrNone,CHP_VOICE_CASE("A.10"));

	INFO_PRINTF1(_L("Test Send DTMF String with wait & terminate"));
	iPhone.NotifyStopInDTMFString(reqStatus1);
	iPhone.SendDTMFTones(reqStatus0,KDtmfString1);
	User::WaitForRequest(reqStatus1);
	TESTL(reqStatus1==KErrNone);
	ret = iPhone.ContinueDTMFStringSending(EFalse);
	CHECKPOINTL(ret,KErrNone,CHP_VOICE_CASE("A.12"));
	User::WaitForRequest(reqStatus0);
	TESTL(reqStatus0==KErrAccessDenied);

	INFO_PRINTF1(_L("Test Send DTMF String terminated early by a call closure"));
	iPhone.SendDTMFTones(reqStatus0,KDtmfString0);
	User::After(1000000);
	INFO_PRINTF1(_L("Hanging up..."));
	TESTL(iCall.HangUp()==KErrNone);
	User::WaitForRequest(reqStatus0);
	TESTL(reqStatus0==KErrEtelNoCarrier);
	CHECKPOINTL(reqStatus0,KErrEtelNoCarrier,CHP_VOICE_CASE("A.13"));

	INFO_PRINTF1(_L("Test Sending DTMF Strings while there is no active call."));
	ret = iPhone.ContinueDTMFStringSending(ETrue);
	CHECKPOINTL(ret,KErrNotReady,CHP_VOICE_CASE("A.14"));
	
	iPhone.SendDTMFTones(reqStatus0,KDtmfString0);
	User::WaitForRequest(reqStatus0);
	TESTL(reqStatus0==KErrEtelCallNotActive);
	CHECKPOINTL(reqStatus0,KErrEtelCallNotActive,CHP_VOICE_CASE("A.15"));

	iCall.Close();
	iLine.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}

