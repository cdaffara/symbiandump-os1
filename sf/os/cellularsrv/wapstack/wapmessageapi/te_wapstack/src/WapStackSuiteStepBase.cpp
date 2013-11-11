// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <c32comm.h>
#include <etel.h>
#include <wapmessage.h>
#include <simtsy.h>
#include "WapStackSuiteDefs.h"
#include "WapStackSuiteStepBase.h"

CWapStackSuiteStepBase::CWapStackSuiteStepBase() : iLenBuffer(0), iLength(iLenBuffer), iRecvBuf(0,0), iRecvHeadersBuf(0,0), iRecvBodyBuf(0,0)
	{
	SetTestStepName(KWapWDPStackSuiteLogFile);
	iSendBuffPtr = NULL;
	iRecvBuffPtr = NULL;
	iRecvHeaders = NULL; 
	iRecvBody = NULL;
	iURI = NULL;
	iSendHeaders = NULL;
	iSendBody = NULL;
	}

CWapStackSuiteStepBase::~CWapStackSuiteStepBase()
	{
	if(iSendBuffPtr) delete iSendBuffPtr;
	if(iRecvBuffPtr) delete iRecvBuffPtr;
	if(iRecvHeaders) delete iRecvHeaders;
	if(iRecvBody) delete iRecvBody;
	if(iURI) delete iURI;
	if(iSendHeaders) delete iSendHeaders;
	if(iSendBody) delete iSendBody;
	}

TVerdict CWapStackSuiteStepBase::doTestStepPreambleL()
/**
* @return - TVerdict
* Implementation of CTestStep base class virtual
* Load serial drivers
* Do all initialisation common to derived classes in here.
*/
	{
	__UHEAP_MARK;
	TInt err = KErrNone;
	CActiveScheduler* sheduler;
	sheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sheduler);
	SetScheduler(sheduler);

	iControl = CActiveControl::NewL(this);
	sheduler->Add(iControl);
	iStatus = iControl->Status();


	err=User::LoadPhysicalDevice(PDD_NAME);
    if (err!=KErrNone && err!=KErrAlreadyExists) User::Leave(err);

    err=User::LoadLogicalDevice(LDD_NAME );
    if (err!=KErrNone && err!=KErrAlreadyExists) User::Leave(err);

	TBool startC32 = ETrue;
	GetBoolFromConfig(ConfigSection(),KWapStartC32, startC32);
	if(startC32)
	{
		_LIT(KPhbkSyncCMI, "phbsync.cmi");
		err = StartC32WithCMISuppressions(KPhbkSyncCMI);
		ERR_PRINTF2(TRefByValue<const TDesC>(_L("Start Comms Process Status = %d\n")), err);
		if(err!=KErrNone && err!=KErrAlreadyExists)
		    {
		    User::Leave(err);
			}
	    }

	LoadConfig();

    SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CWapStackSuiteStepBase::doTestStepPostambleL()
	{

	delete iControl;
	iControl = NULL;
	delete Scheduler();
	if(iSendBuffPtr) delete iSendBuffPtr;
	iSendBuffPtr = NULL;
	if(iRecvBuffPtr) delete iRecvBuffPtr;
	iRecvBuffPtr = NULL;
	__UHEAP_MARKEND;
	return TestStepResult();
	}

void CWapStackSuiteStepBase::LoadConfig()
	{
	TPtrC16 remote_host, local_host;

	GetIntFromConfig(KWapCommonSec, KWapBearer, (TInt&)iBearer);
	if(GetStringFromConfig(KWapCommonSec, KWapRemoteHost, remote_host))
		{
		iRemoteHost.Copy(remote_host);
		}
	if(GetStringFromConfig(KWapCommonSec, KWapLocalHost, local_host))
		{
		iLocalHost.Copy(local_host);
		}
	TInt remote, local, timeOut;
	if(GetIntFromConfig(KWapCommonSec, KWapRemotePort, remote))
		{
		iRemotePort = static_cast<Wap::TPort>(remote);
		}
	if(GetIntFromConfig(KWapCommonSec, KWapLocalPort, local))
		{
		iLocalPort = static_cast<Wap::TPort>(local);
		}
	if(GetIntFromConfig(KWapCommonSec,KWapTimeOut, timeOut))
		{
		iTimeout=timeOut;
		}
	TInt headerLength, bufLength, bufLen, method; 
	if(GetIntFromConfig(KWapCommonSec,KWapWSPRecvHeaderLength, headerLength))
		{
		iHeaderRecvBufLength=static_cast<TUint16>(headerLength);
		}
	if(GetIntFromConfig(KWapCommonSec,KWapWSPRecvBodyLength, bufLength))
		{
		iBodyRecvBufLength=static_cast<TUint16>(bufLength);
		}
	if(GetIntFromConfig(KWapCommonSec,KWapWdpBufLength, bufLen))
		{
		iWdpRecvBufLength=static_cast<TUint16>(bufLen);
		}
	if(GetIntFromConfig(KWapCommonSec,KWapWSPMethod, method))
		{
		iMethod = static_cast<TUint16>(method);
		}
	else
		{
		iMethod = 64; // default method GET
		}
	GetIntFromConfig(KWapCommonSec, KWapExError, iExError);
	if(!GetIntFromConfig(KWapCommonSec, KWapRepeat, iRepeat))
		{
		iRepeat = 1;
		};

	GetIntFromConfig(KWapCommonSec, KWapExError2, iExError2);
	GetBoolFromConfig(KWapCommonSec, KWapUseSIMTSY, iUseSIMTSY);
	GetBoolFromConfig(KWapCommonSec, KWapDetailedLogging, iDetailedLogging);
	GetBoolFromConfig(KWapCommonSec, KAwaitLengthCancel, iAwaitLengthCancel);
	GetBoolFromConfig(KWapCommonSec, KMiddleCancel, iMiddleCancel);
	GetBoolFromConfig(KWapCommonSec, KMultiReadCancel, iMultiReadCancel);
	GetBoolFromConfig(KWapCommonSec, KAwaitLengthTwice, iAwaitLengthTwice);
	GetBoolFromConfig(KWapCommonSec, KWapDelayPortClosure, iDelayPortClosure);
	GetIntFromConfig(KWapCommonSec,KWapSIMTSYState, iSimTsyState);
	
	TInt result;
	// if using SIMTSY, we need to define the test number property
	// since we set the test number before loading SIMTSY
	if (iUseSIMTSY)
		{
		result = RProperty::Define(KUidPSSimTsyCategory, KPSSimTsyTestNumber, RProperty::EInt);
		__ASSERT_ALWAYS(result == KErrNone || result == KErrAlreadyExists,User::Panic(KWapStackSuitePanic, EDataCorrupt));
		}

	// Phone power property, normally defined by UIKON, is not defined by any
	// startup code in console builds - we're on our own with this one.
	result = RProperty::Define(KUidSystemCategory, KUidPhonePwr.iUid, RProperty::EInt);
	__ASSERT_ALWAYS(result == KErrNone || result == KErrAlreadyExists,User::Panic(KWapStackSuitePanic, EDataCorrupt));

	}

TInt CWapStackSuiteStepBase::CallStateMachine()
	{
	return (0);
	}

void CWapStackSuiteStepBase::StartScheduler()
	{
	INFO_PRINTF1(_L("Star Scheduler"));
	CActiveScheduler::Start();
	}

void CWapStackSuiteStepBase::StopScheduler()
	{
	INFO_PRINTF1(_L("Stop Scheduler"));
	CActiveScheduler::Stop();
	}

void CWapStackSuiteStepBase::WaitForInitializeL()
	{
	_LIT(KMMTSY,"MM");
	_LIT(KSIMTSY,"SIM");

	TName tsy;
	
	if(UseSimTsy())
		{
		tsy=KSIMTSY;
		}
	else
		{
		tsy = KMMTSY;
		}

	RTelServer serverT;
	User::LeaveIfError(serverT.Connect());
	CleanupClosePushL(serverT);
	User::LeaveIfError(serverT.LoadPhoneModule(tsy));
	INFO_PRINTF2(_L("%S TSY loaded"), &tsy);

	// Find the phone corresponding to this TSY and open a number of handles on it
	TInt numPhones;
	User::LeaveIfError(serverT.EnumeratePhones(numPhones));
	RPhone phone;
	while (numPhones--)
		{
		TName phoneTsy;
		User::LeaveIfError(serverT.GetTsyName(numPhones,phoneTsy));
		if (phoneTsy.CompareF(tsy)==KErrNone)
			{
			RTelServer::TPhoneInfo info;
			User::LeaveIfError(serverT.GetPhoneInfo(numPhones,info));
			User::LeaveIfError(phone.Open(serverT,info.iName));
			CleanupClosePushL(phone);
			const TInt err = phone.Initialise();
			User::LeaveIfError(err);
			CleanupStack::PopAndDestroy(&phone);
			break;
			}
		}
	CleanupStack::PopAndDestroy(&serverT);
	}

void CWapStackSuiteStepBase::DumpBuf(const TDesC8& aBuf)
	{
	if(!DetailedLogging()) return;
	TBuf16<100> dumpbuf;
	TInt totLen = aBuf.Length();

	INFO_PRINTF1(_L("-------------------------------------------------------------------------"));
	INFO_PRINTF2(_L("Buffer: %d bytes"), totLen);
	INFO_PRINTF1(_L("-------------------------------------------------------------------------"));

	for (TInt i = 0; i < totLen; i += 16)
		{
		dumpbuf.FillZ(100);
		dumpbuf.Zero();
		dumpbuf.AppendFormat(_L("%04x  "),i);
		TInt j = 0;
		TInt thisRow = Min(16,totLen - j - i);
		for (j = 0; j < 16; j++)
			{
			if (j < thisRow)
				{
				TText8 chr = aBuf[i + j];
				dumpbuf.AppendFormat(_L("%02x "),chr);
				}
			else
				dumpbuf.AppendFormat(_L("   "));
			}
		dumpbuf.AppendFormat(_L("   "));
		for (j = 0; j < thisRow; j++)
			{
			TText8 chr = aBuf[i + j];
			TChar c(chr);
			if (!c.IsPrint())
				{
					chr='.';
				}
			// NOTE: "<" & ">" are HTML tags (change to "[" & "]")
			if((chr == '<') || (chr == '>'))
				{ 
				chr+=31;
				}
				dumpbuf.AppendFormat(_L("%c"),chr);
			}
		INFO_PRINTF2(_L("%s"), dumpbuf.Ptr());
		}
	INFO_PRINTF1(_L("-------------------------------------------------------------------------"));
	}

void CWapStackSuiteStepBase::ShowConnectionInfo()
{
	if(!DetailedLogging()) return;

	TBuf16<WAP_MAX_HOST_NAME> host;
	TBuf16<100> infostr;

	INFO_PRINTF1(_L("-------------------------------------------------------------------------"));
	INFO_PRINTF1(_L("Connection Info:"));
	INFO_PRINTF1(_L("-------------------------------------------------------------------------"));

	infostr.FillZ(100);
	switch(iBearer)
	{
	case Wap::EAll: infostr.Copy(_L("EAll"));
		break;
	case Wap::EIP: infostr.Copy(_L("EIP"));
		break;
	case Wap::ESMS7: infostr.Copy(_L("ESMS7"));
		break;
	case Wap::ESMS: infostr.Copy(_L("ESMS"));
		break;
	case Wap::EWAPSMS7: infostr.Copy(_L("EWAPSMS7"));
		break;
	case Wap::EWAPSMS: infostr.Copy(_L("EWAPSMS"));
		break;
	default:
		break;
	}

	INFO_PRINTF2(_L("Bearer      : %s"), infostr.Ptr());
	host.FillZ(WAP_MAX_HOST_NAME);
	host.Copy(iRemoteHost);
	INFO_PRINTF2(_L("Remote Host : %s"), host.Ptr());
	INFO_PRINTF2(_L("Remote Port : %d"),iRemotePort);
	host.FillZ(100);
	host.Copy(iLocalHost);
	INFO_PRINTF2(_L("Local Host  : %s"), host.Ptr());
	INFO_PRINTF2(_L("Local Port  : %d"), iLocalPort);
	infostr.FillZ(100);
	switch(iSecure)
	{
	case ETrue: infostr.Copy(_L("TRUE"));
		break;
	case EFalse: infostr.Copy(_L("FALSE"));
		break;
	default:
		break;
	}
	INFO_PRINTF2(_L("Secure      : %s"), infostr.Ptr());
	infostr.FillZ(100);
	switch(iReconnect)
	{
	case ETrue: infostr.Copy(_L("TRUE"));
		break;
	case EFalse: infostr.Copy(_L("FALSE"));
		break;
	default:
		break;
	}
	INFO_PRINTF2(_L("Timeout     : %d"), iTimeout);
	INFO_PRINTF2(_L("Reconnect   : %s"), infostr.Ptr());
	INFO_PRINTF2(_L("ExitCode    : %d"), iExError);
	INFO_PRINTF2(_L("Repeat      : %d"), iRepeat);

	INFO_PRINTF1(_L("-------------------------------------------------------------------------"));
}

/**
	Active controller
*/
CActiveControl* CActiveControl::NewL(MControlNotify* aControl)
	{
	CActiveControl* self = new(ELeave) CActiveControl(aControl);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CActiveControl::ConstructL()
	{
	}

CActiveControl::CActiveControl(MControlNotify* aControl)
: CActive(EPriorityStandard)
	{
	iControl = aControl;
	}

CActiveControl::~CActiveControl()
	{
	}

void CActiveControl::ReStart()
	{
	TRequestStatus* status = &iStatus;
	SetActive();
	User::RequestComplete(status, KErrNone);
	}

void CActiveControl::RunL()
	{
	if(iControl->CallStateMachine())
		{
			SetActive();
		}
	}

void CActiveControl::DoCancel()
	{
	}
