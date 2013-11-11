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

#include "WapFullySpecCLPushSteps.h"
#include "WapStackSuiteDefs.h"
#include <simtsy.h>
#include <wapmsgerr.h>
#include <ecom/ecom.h>


CFullySpecCLPushBase::CFullySpecCLPushBase()
/**
* Constructor
*/
	{
	}

CFullySpecCLPushBase::~CFullySpecCLPushBase()
/**
* Destructor
*/
	{
	if(iImp) 
		{
		UnloadInterface();
		}
	}

CWapFullySpecCLPushService* CFullySpecCLPushBase::LoadInterface()
	{
	TInt trapValue;
	CWapFullySpecCLPushService* implementation = NULL;
	// UID {0x101FBB3A} - SWS implementation
	TUid KInterfaceUid = {0x101FBB3A}; 	
	INFO_PRINTF2(_L("Calling new NewL(0x%08X) method"), KInterfaceUid);
	TRAP(trapValue, implementation = CWapFullySpecCLPushService::NewL(KInterfaceUid));
	if(!trapValue)
		{
		INFO_PRINTF2(_L("Interface is loaded: implementation.addr = 0x%08x"), implementation);
		return implementation;
		}
	else return NULL;
	}

void CFullySpecCLPushBase::UnloadInterface()
	{
	INFO_PRINTF2(_L("Interface at implementation.addr = 0x%08x is deleted"), iImp);
	delete iImp;
	iImp = NULL;
	REComSession::FinalClose();	
	}

CFullySpecCLPushStep_1::CFullySpecCLPushStep_1()
/**
* Constructor
*/
	{
	}

CFullySpecCLPushStep_1::~CFullySpecCLPushStep_1()
/**
* Destructor
*/
	{
	}


TVerdict CFullySpecCLPushStep_1::doTestStepL()
/**
* @return - TVerdict code
* Override of base class pure virtual
* Our implementation only gets called if the base class doTestStepPreambleL() did
* not leave. That being the case, the current test result value will be EPass.
*/
	{
	//
	// CWapFullySpecCLPushService
	// 
	INFO_PRINTF1(_L("CreateImplementation - CWapFullySpecCLPushService"));
	INFO_PRINTF1(_L("Calling old NewL() method"));
	
	TInt trapValue; 
	
	SetTestStepResult(EFail);
	TRAP(trapValue, iImp = CWapFullySpecCLPushService::NewL());
	if(!trapValue)
		{
		// interface is loaded
		INFO_PRINTF2(_L("Interface is loaded: implementation.addr = 0x%08x"), iImp);
		UnloadInterface();
		iImp = LoadInterface();
		if(iImp)
			{
			INFO_PRINTF1(_L("CreateImplementation - OK"));
			SetTestStepResult(EPass);
			UnloadInterface();
			}
		}
	return TestStepResult();
	}

CFullySpecCLPushStep_2::CFullySpecCLPushStep_2()
/**
* Constructor
*/
	{
	}

CFullySpecCLPushStep_2::~CFullySpecCLPushStep_2() 
/**
* Destructor
*/
	{
	}

TVerdict CFullySpecCLPushStep_2::doTestStepL()
/** 
* Test Connect methoths 
* @return - TVerdict code
* 
*/
	{
	INFO_PRINTF1(_L("FULLYSPEC-PUSH: Testing Connect"));
	if(TestStepResult() == EPass )
		{
		SetTestStepResult(EFail);
		iImp = LoadInterface();
		if(iImp)
			{
			TInt bearer;
			TInt error	= KErrNone;
			TBool secure= EFalse;
			TBool reconnect = EFalse;
			SetLastError(KErrNone);
			
			if(GetIntFromConfig(ConfigSection(),KWapBearer, bearer)) iBearer = (Wap::TBearer)bearer;
			if(GetIntFromConfig(ConfigSection(), KWapExError, error)) SetExpectedError(error);
			if(GetBoolFromConfig(ConfigSection(), KWapSecure, secure)) iSecure = secure;
			if(GetBoolFromConfig(ConfigSection(),_L("Reconnect"), reconnect)) iReconnect = reconnect;
			
			if(UseSimTsy())
				{
				GetIntFromConfig(ConfigSection(), KWapSIMTSYState, iSimTsyState);
				User::LeaveIfError(RProperty::Set(KUidPSSimTsyCategory, KPSSimTsyTestNumber, iSimTsyState));
				}
			User::LeaveIfError(RProperty::Set(KUidSystemCategory, KUidPhonePwr.iUid, ESAPhoneOn));
			
			SetState(EConnect);
			iControl->ReStart();
			ShowConnectionInfo();

			StartScheduler();
			UnloadInterface();
			}
		}
	return TestStepResult();
	}

TInt CFullySpecCLPushStep_2::CallStateMachine()
	{
	TInt err = KErrNone;
	INFO_PRINTF2(_L("CallStateMachine: step [%d]"), State());
	switch(iState)
		{
		case EConnect:
			INFO_PRINTF1(_L("EConnect"));
			err = iImp->Connect(iRemoteHost, iRemotePort, iBearer, iSecure);
			INFO_PRINTF2(_L("Connection completed with code = %d"), err);
			if( err == KErrNone)
				{
				if(iReconnect)
					{ 
					INFO_PRINTF1(_L("Reconnection"));
					iControl->ReStart();
					break;
					}
				}
			SetState(EFinish);
			SetLastError(err);
			iControl->ReStart();
			break;
			
		case EFinish:
			INFO_PRINTF1(_L("EFinish"));
			if(GetLastError() == GetExpectedError()) SetTestStepResult(EPass);
			else SetTestStepResult(EFail);
			SetState(EIdle);
			StopScheduler();
			break;
			
		case EIdle:
		default:
			INFO_PRINTF2(_L("Error: EIdle or unknown state %d"), State());
			SetTestStepResult(EFail);
			StopScheduler();
			break;
		}
	return (0);
	}

CFullySpecCLPushStep_3::CFullySpecCLPushStep_3()
/**
* Constructor
*/
	{
	}

CFullySpecCLPushStep_3::~CFullySpecCLPushStep_3() 
/**
* Destructor
*/
	{
	}

TVerdict CFullySpecCLPushStep_3::doTestStepL()
/** 
* Test Connect methoths 
* @return - TVerdict code
* 
*/
	{
	INFO_PRINTF1(_L("FullySpecWSPPush. Requests without connection"));
	// check preamble result
	TVerdict verdict = TestStepResult();
	if (verdict==EPass)
		{
		TInt err, ret;
		iImp = LoadInterface();
		if (!iImp)
			{
			ERR_PRINTF1(_L("Unable to load FullySpecWSPPush Interface"));
			verdict = EFail;
			}
		else
			{
			GetIntFromConfig(ConfigSection(),KWapExError,(TInt&)iExError);
			ShowConnectionInfo();
			TRequestStatus reqStatus;
			iImp->AwaitPush(iRecvHeadersBuf, iRecvBodyBuf, iTransactionIdPckg, reqStatus);
			TRAP(ret, TEST_CHECKL(reqStatus.Int(), iExError, _L("AwaitPush Fails not as expected")))
				if (ret) 
					verdict = EFail;
				else
					{
					Wap::TPort port;
					err=iImp->GetLocalPort(port);
					TRAP(ret, TEST_CHECKL(err, iExError, _L("GetLocalPort Fails not as expected")))
						if (ret)
							verdict = EFail;
						else
							{
							HBufC8* localHost;					
							err=iImp->GetLocalAddress(localHost);
							TRAP(ret, TEST_CHECKL(err, iExError, _L("GetLocalAddress Fails not as expected")))
								if (ret) 
									verdict = EFail;
							}
					}
				UnloadInterface();
			}
		}
	if (verdict==EPass)
		{
		INFO_PRINTF1(_L("FullySpecCLPush. Requests Fail as expected"));
		}
	SetTestStepResult(verdict);
	return TestStepResult();
	}

TInt CFullySpecCLPushStep_3::CallStateMachine()
	{
	return (0);
	}

CFullySpecCLPushStep_4::CFullySpecCLPushStep_4()
/**
* Constructor
*/
	{
	}

CFullySpecCLPushStep_4::~CFullySpecCLPushStep_4() 
/**
* Destructor
*/
	{
	}

TVerdict CFullySpecCLPushStep_4::doTestStepL()
/** 
* Test Connect methoths 
* @return - TVerdict code
* 
*/
	{
	INFO_PRINTF1(_L("FULLYSPEC-CLPUSH: Connect-AwaitPush"));
	if(TestStepResult() == EPass )
		{
		SetTestStepResult(EFail);
		iImp = LoadInterface();
		if(iImp)
			{
			SetLastError(KErrNone);
			
			TInt bearer, port;
			TInt error = KErrNone;
			TPtrC16 remote_host;
			
			if(GetIntFromConfig(ConfigSection(),KWapBearer, bearer))
				{
				iBearer = static_cast<Wap::TBearer>(bearer);
				}
			if(GetStringFromConfig(ConfigSection(), KWapRemoteHost, remote_host))
				{
				iRemoteHost.Copy(remote_host);
				}
			if(GetIntFromConfig(ConfigSection(),KWapLocalPort, port)) 
				{
				iLocalPort = static_cast<Wap::TPort>(port);
				}
			if(GetIntFromConfig(ConfigSection(),KWapRemotePort, port)) 
				{
				iRemotePort = static_cast<Wap::TPort>(port);
				}
			if(GetIntFromConfig(ConfigSection(),KWapExError, error))
				{
				SetExpectedError(error);
				}
			GetBoolFromConfig(ConfigSection(), KMiddleCancel, iMiddleCancel);
			GetBoolFromConfig(ConfigSection(), KMultiReadCancel, iMultiReadCancel);
			TInt headerLength, bufLength; 
			if(GetIntFromConfig(ConfigSection(),KWapWSPRecvHeaderLength, headerLength))
				{
				iHeaderRecvBufLength=static_cast<TUint16>(headerLength);
				}
			if(GetIntFromConfig(ConfigSection(),KWapWSPRecvBodyLength, bufLength))
				{
				iBodyRecvBufLength=static_cast<TUint16>(bufLength);
				}
			
			iRecvHeaders = HBufC8::NewL(iHeaderRecvBufLength);
			iRecvBody = HBufC8::NewL(iBodyRecvBufLength);
			
			User::LeaveIfError(RProperty::Set(KUidSystemCategory, KUidPhonePwr.iUid, ESAPhoneOn));
			
			if(UseSimTsy())
				{
				GetIntFromConfig(ConfigSection(), KWapSIMTSYState, iSimTsyState);
				User::LeaveIfError(RProperty::Set(KUidPSSimTsyCategory, KPSSimTsyTestNumber, iSimTsyState));
				}
				
				
			SetState(EConnect);
			iControl->ReStart();
			ShowConnectionInfo();
			//	Start state machine
			StartScheduler();
			delete iRecvBody;
			iRecvBody=NULL;
			delete iRecvHeaders;
			iRecvHeaders=NULL;
			UnloadInterface();
			} 
		
		}
	return TestStepResult();
	}

TInt CFullySpecCLPushStep_4::CallStateMachine()
	{
	TVerdict verdict = EFail;
	TInt err = KErrNone;
	TInt activate = 0;

	INFO_PRINTF2(_L("CallStateMachine: state [%d]"), State());	
	switch(iState)
		{
		case EConnect:
			INFO_PRINTF1(_L("EConnect"));
			err = iImp->Connect(iRemoteHost, iRemotePort, iBearer, EFalse);
			INFO_PRINTF2(_L("Connection completed with code = %d"), err);
			if( err == KErrNone)
				{
				TRAP(err, WaitForInitializeL());
				iRecvHeadersBuf.Set(iRecvHeaders->Des());
				iRecvBodyBuf.Set(iRecvBody->Des());
				iImp->AwaitPush(iRecvHeadersBuf, iRecvBodyBuf, iTransactionIdPckg, *iStatus);
				INFO_PRINTF1(_L("Start AwaitingPush....."));
				activate = 1;
				if (!iMiddleCancel)
					{
					SetState(EAwaitPush);
					}
				else
					{
					INFO_PRINTF1(_L("Cancel AwaitPush"));
					iImp->CancelAwaitPush();
					SetState(EFinish);
					}
				}
			else 
				{
				SetState(EFinish);
				iControl->ReStart();
				}
			SetLastError(err);
			break;
			
		case EAwaitPush:
			INFO_PRINTF1(_L("EAwaitPush"));
			if(iStatus->Int() != KErrNone && iStatus->Int()!=Wap::EMoreData) 
				// AwaitPush failed
				{
				INFO_PRINTF1(_L("AwaitPush failed"));
				SetLastError(iStatus->Int());
				SetState(EFinish);
				iControl->ReStart();
				}
			else
				{
				iMaxWspHeaderRecvBuffer.Append(iRecvHeadersBuf);
				iMaxWspDataRecvBuffer.Append(iRecvBodyBuf);
				if (iStatus->Int()==Wap::EMoreData)
					{
					iImp->AwaitPush(iRecvHeadersBuf, iRecvBodyBuf, iTransactionIdPckg, *iStatus);
					activate = 1;
					if (!iMultiReadCancel)
						{
						SetState(EAwaitPush);
						}
					else
						{
						INFO_PRINTF1(_L("Cancel Receive in EMutipleReceive state"));
						iImp->CancelAwaitPush();
						SetState(EFinish);
						}					
					
					}
				else
					{
					SetLastError(iStatus->Int());
					iControl->ReStart();
					SetState(EFinish);
					}
				}
			break;
			
		case EFinish:
			INFO_PRINTF1(_L("EFinish"));
			if(iStatus->Int() != KErrNone) 
				{
				SetLastError(iStatus->Int());
				}
			verdict = (GetLastError() == GetExpectedError())?EPass:EFail;
			if (verdict==EPass && !iMiddleCancel && !iMultiReadCancel)
				{
				iImp->CancelAwaitPush();
				INFO_PRINTF1(_L("Headers"));
				DumpBuf(iMaxWspHeaderRecvBuffer);
				INFO_PRINTF1(_L("Body"));
				DumpBuf(iMaxWspDataRecvBuffer);
				if (verdict==EPass)
					{
					Wap::TPort port;
					iImp->GetLocalPort(port);
					verdict = (port == iLocalPort)?EPass:EFail;
					}
				if (verdict==EPass)
					{
					//This test can not perform.
					HBufC8* buf;
					iImp->GetLocalAddress(buf);
					delete buf;
					}
				}
			SetTestStepResult(verdict);
			SetState(EIdle);
			StopScheduler();
			break;
			
		case EIdle:
		default:
			INFO_PRINTF2(_L("Error: EIdle or unknown state %d"), State());
			SetTestStepResult(verdict);
			StopScheduler();
			break;
		}
		return (activate);
}

CFullySpecCLPushStep_5::CFullySpecCLPushStep_5()
/**
* Constructor
*/
	{
	}

CFullySpecCLPushStep_5::~CFullySpecCLPushStep_5() 
/**
* Destructor
*/
	{
	}

TVerdict CFullySpecCLPushStep_5::doTestStepL()
/** 
* Test Cancel await push 
* @return - TVerdict code
* 
*/
	{
	INFO_PRINTF1(_L("FullySpecified-PUSH: Testing Cancel"));
	if(TestStepResult() == EPass )
		{
		SetTestStepResult(EFail);
		iImp = LoadInterface();
		if(iImp)
			{
			TInt bearer;
			TInt error	= KErrNone;
			TBool secure= EFalse;
			TBool reconnect = EFalse;
			SetLastError(KErrNone);
			
			if(GetIntFromConfig(ConfigSection(),KWapBearer, bearer)) iBearer = (Wap::TBearer)bearer;
			if(GetIntFromConfig(ConfigSection(), KWapExError, error)) SetExpectedError(error);
			if(GetBoolFromConfig(ConfigSection(), KWapSecure, secure)) iSecure = secure;
			if(GetBoolFromConfig(ConfigSection(),_L("Reconnect"), reconnect)) iReconnect = reconnect;
			
			ShowConnectionInfo();
			error = iImp->Connect(iRemoteHost, iRemotePort, iBearer, iSecure);
			INFO_PRINTF2(_L("Connection completed with code = %d"), error);
			if( error == KErrNone)
				{
				TBuf8<100> header, body;
				TPckgBuf<TUint8> pushIdPckg;
				TRequestStatus reqStatus;
				iImp->AwaitPush(header, body, pushIdPckg, reqStatus);
				SetState(EAwaitPush);
				StartScheduler();
				iControl->ReStart();
				if(reqStatus==KErrCancel)
					{
					SetTestStepResult(EPass);
					}
				}
			
			}
		}
	return TestStepResult();
	}

TInt CFullySpecCLPushStep_5::CallStateMachine()
	{
	INFO_PRINTF2(_L("CallStateMachine: state [%d]"), State());	
	switch(iState)
		{
		case EAwaitPush:
			INFO_PRINTF1(_L("EAwaitPush"));
			iImp->CancelAwaitPush();
			SetState(EFinish);
			iControl->ReStart();
			break;
		case EFinish:
			INFO_PRINTF1(_L("EFinish"));
			SetState(EIdle);
			StopScheduler();
			break;
		case EIdle:
		default:
			INFO_PRINTF2(_L("Error: EIdle or unknown state %d"), State());
			SetTestStepResult(EFail);
			StopScheduler();
			break;
		}
	return (0);
	}

CFullySpecCLPushStep_6::CFullySpecCLPushStep_6() 
	{
	}
CFullySpecCLPushStep_6::~CFullySpecCLPushStep_6() 
	{
	}
TVerdict CFullySpecCLPushStep_6::doTestStepL()
	{
	//Not in use
	return TestStepResult();
	}

TInt CFullySpecCLPushStep_6::CallStateMachine()
	{
	return (0);
	}

CFullySpecCLPushStep_7::CFullySpecCLPushStep_7() 
	{
	}
CFullySpecCLPushStep_7::~CFullySpecCLPushStep_7() 
	{
	}
TVerdict CFullySpecCLPushStep_7::doTestStepL()
	{
	INFO_PRINTF1(_L("FULLYSPEC-CLPUSH: Connect-AwaitPush-AwaitPush"));
	if(TestStepResult() == EPass )
		{
		SetTestStepResult(EFail);
		iImp = LoadInterface();
		if(iImp)
			{
			SetLastError(KErrNone);
			
			TInt bearer;
			TInt error = KErrNone;
			
			if(GetIntFromConfig(ConfigSection(),KWapBearer, bearer)) 
				{
				iBearer = static_cast<Wap::TBearer>(bearer);
				}
			if(GetIntFromConfig(ConfigSection(),KWapExError, error)) 
				{
				SetExpectedError(error);
				}
			TInt headerLength, bufLength; 
			if(GetIntFromConfig(ConfigSection(),KWapWSPRecvHeaderLength, headerLength))
				{
				iHeaderRecvBufLength=static_cast<TUint16>(headerLength);
				}
			if(GetIntFromConfig(ConfigSection(),KWapWSPRecvBodyLength, bufLength))
				{
				iBodyRecvBufLength=static_cast<TUint16>(bufLength);
				}
			iRecvHeaders = HBufC8::NewL(iHeaderRecvBufLength);
			iRecvBody = HBufC8::NewL(iBodyRecvBufLength);
			SetState(EConnect);
			iControl->ReStart();
			ShowConnectionInfo();
			// Start state machine
			StartScheduler();
			delete iRecvBody;
			iRecvBody=NULL;
			delete iRecvHeaders;
			iRecvHeaders=NULL;
			UnloadInterface();
			}
		}
	return TestStepResult();
	}

TInt CFullySpecCLPushStep_7::CallStateMachine()
	{
	TVerdict verdict = EFail;
	TInt err = KErrNone;
	TInt activate = 0;
	
	INFO_PRINTF2(_L("CallStateMachine: state [%d]"), State());	
	switch(iState)
		{
		case EConnect:
			INFO_PRINTF1(_L("EConnect"));
			err = iImp->Connect(iRemoteHost, iRemotePort, iBearer, EFalse);
			INFO_PRINTF2(_L("Connection completed with code = %d"), err);
			if( err == KErrNone)
				{
//				WaitForInitializeL();
				TRequestStatus reqStatus;
				iRecvHeadersBuf.Set(iRecvHeaders->Des());
				iRecvBodyBuf.Set(iRecvBody->Des());
				iImp->AwaitPush(iRecvHeadersBuf, iRecvBodyBuf, iTransactionIdPckg, reqStatus);
				iImp->AwaitPush(iRecvHeadersBuf, iRecvBodyBuf, iTransactionIdPckg, *iStatus);
				INFO_PRINTF1(_L("AwaitPush"));
				activate = 1;
				}
			else
				{
				iControl->ReStart();
				}
			SetLastError(err);
			SetState(EFinish);
			break;
			
		case EFinish:
			
			INFO_PRINTF1(_L("EFinish"));
			iImp->CancelAwaitPush();
			if(iStatus->Int() != KErrNone) 
				{
				SetLastError(iStatus->Int());
				}
			verdict = (GetLastError() == GetExpectedError())?EPass:EFail;
			SetTestStepResult(verdict);
			SetState(EIdle);
			StopScheduler();
			break;
			
		case EIdle:
		default:
			INFO_PRINTF2(_L("Error: EIdle or unknown state %d"), State());
			SetTestStepResult(verdict);
			StopScheduler();
			
			break;
		}
	return (activate);
	}

CFullySpecCLPushStep_8::CFullySpecCLPushStep_8() 
	{
	}
CFullySpecCLPushStep_8::~CFullySpecCLPushStep_8() 
	{
	}
TVerdict CFullySpecCLPushStep_8::doTestStepL()
/**
*	@return - TVerdict code
*	Wrong parameter values for FullySpecified CL Push API:
*	- Wrong TUid on construction
*	- Connect to wrong bearer (already implemented in test 2)
*/
	{
	INFO_PRINTF1(_L("CreateImplementation with wrong Uid- CWapFullySpecCLPushService"));
	if(TestStepResult() == EPass )
		{
		TInt error;
		if(GetIntFromConfig(ConfigSection(),KWapExError2, error))
			{
			iExError2=error;
			}
		TInt trapValue;
		TUid invalidInterfaceUid = {0x00000000}; 
		INFO_PRINTF2(_L("Calling new NewL(0x%08X) method"), invalidInterfaceUid);
		TRAP(trapValue, iImp = CWapFullySpecCLPushService::NewL(invalidInterfaceUid));
		TVerdict verdict = (trapValue == iExError2)?EPass:EFail;
		if (verdict==EPass)
			{
			INFO_PRINTF2(_L("CreateImplementation with wrong Uid- Failed as expected with error %d"),trapValue);
			}
		SetTestStepResult(verdict);
		}
	return TestStepResult();
	}

TInt CFullySpecCLPushStep_8::CallStateMachine()
	{
	return (0);
	}

CFullySpecCLPushStep_9::CFullySpecCLPushStep_9() {}
CFullySpecCLPushStep_9::~CFullySpecCLPushStep_9() {}
TVerdict CFullySpecCLPushStep_9::doTestStepL()
	{
	return TestStepResult();
	}

TInt CFullySpecCLPushStep_9::CallStateMachine()
	{
	return (0);
	}

