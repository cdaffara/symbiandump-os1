// Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// CBoundCLPushStep_xyz derived implementations
// 
//

/**
 @file
 @internalComponent
*/

#include "WapBoundCLPushSteps.h"
#include "WapStackSuiteDefs.h"
#include <simtsy.h>
#include <wapmsgerr.h>
#include <ecom/ecom.h>


CBoundCLPushStepBase::CBoundCLPushStepBase()
/**
* Constructor
*/
	{
	}

CBoundCLPushStepBase::~CBoundCLPushStepBase()
/**
* Destructor
*/
	{
	if(iImp) 
		{
		UnloadInterface();
		}
	}

// Load CWapBoundCLPushService Interface 
CWapBoundCLPushService* CBoundCLPushStepBase::LoadInterface()
	{
	TInt trapValue;
	CWapBoundCLPushService* implementation = NULL;
	// UID {0x101FBB39} - SWS implementation
	TUid KInterfaceUid = {0x101FBB39}; 	
	INFO_PRINTF2(_L("Calling new NewL(0x%08X) method"), KInterfaceUid);
	TRAP(trapValue, implementation = CWapBoundCLPushService::NewL(KInterfaceUid));
	if(!trapValue)
		{
		INFO_PRINTF2(_L("Interface is loaded: implementation.addr = 0x%08x"), implementation);
		return implementation;
		}
	else return NULL;
	}

void CBoundCLPushStepBase::UnloadInterface()
	{
	INFO_PRINTF2(_L("Interface at implementation.addr = 0x%08x is deleted"), iImp);
	delete iImp;
	iImp = NULL;
	REComSession::FinalClose();
	}

CBoundCLPushStep_1::~CBoundCLPushStep_1()
/**
* Destructor
*/
	{
	}

CBoundCLPushStep_1::CBoundCLPushStep_1()
/**
* Constructor
*/
	{
	}

TVerdict CBoundCLPushStep_1::doTestStepL()
/**
* @return - TVerdict code
* Override of base class pure virtual
* Our implementation only gets called if the base class doTestStepPreambleL() did
* not leave. That being the case, the current test result value will be EPass.
*/
	{
	//
	// CWapBoundCLPushService
	// 
	INFO_PRINTF1(_L("CreateImplementation - CWapBoundCLPushService"));
	INFO_PRINTF1(_L("Calling old NewL() method"));
	
	TInt trapValue; 
	
	SetTestStepResult(EFail);
	TRAP(trapValue, iImp = CWapBoundCLPushService::NewL());
	if(!trapValue)
		{
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

CBoundCLPushStep_2::CBoundCLPushStep_2()
/**
* Constructor
*/
	{
	}

CBoundCLPushStep_2::~CBoundCLPushStep_2() 
/**
* Destructor
*/
	{
	}

TVerdict CBoundCLPushStep_2::doTestStepL()
/** 
* Test Connect methoths 
* @return - TVerdict code
* 
*/
	{
	INFO_PRINTF1(_L("BOUNDCL-PUSH: Testing Connect"));
	if(TestStepResult() == EPass )
		{
		SetTestStepResult(EFail);
		iImp = LoadInterface();
		if(iImp)
			{
			TInt bearer, port;
			TInt error	= KErrNone;
			TBool secure= EFalse;
			TBool reconnect= EFalse;
			SetLastError(KErrNone);
			TBool delay = EFalse;
			
			if(GetIntFromConfig(ConfigSection(), KWapBearer, bearer)) iBearer = (Wap::TBearer)bearer;
			if(GetIntFromConfig(ConfigSection(), KWapLocalPort, port)) iLocalPort = (Wap::TPort)port;
			if(GetIntFromConfig(ConfigSection(), KWapExError, error)) SetExpectedError(error);
			if(GetBoolFromConfig(ConfigSection(), KWapSecure, secure)) iSecure = secure;
			if(GetBoolFromConfig(ConfigSection(),_L("Reconnect"), reconnect)) iReconnect = reconnect;
			if(GetBoolFromConfig(ConfigSection(),KWapDelayPortClosure, delay)) iDelayPortClosure = delay;

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

TInt CBoundCLPushStep_2::CallStateMachine()
	{
	TInt err = KErrNone;
	INFO_PRINTF2(_L("CallStateMachine: state [%d]"), State());
	switch(iState)
		{
		case EConnect:
			INFO_PRINTF1(_L("EConnect"));
			err = iImp->Connect(iBearer, (Wap::TPort)iLocalPort, iSecure);
			INFO_PRINTF2(_L("Connection completed with code = %d"), err);
			if( err == KErrNone)
				{
				if(iReconnect)
					{ 
					INFO_PRINTF1(_L("Second connect"));
					iControl->ReStart();
					break;
					}
				}
			SetState(EFinish);
			SetLastError(err);
			iControl->ReStart();
			break;
			
		case EFinish:
			// it's necessary to delay port closure, let the other concurrent test step
			// can fail opening the same port with expected error EPortAlreadyBound (-5498)
			if (iDelayPortClosure)
				User::After(5000000);

			INFO_PRINTF1(_L("EFinish"));
			if(GetLastError() == GetExpectedError()) 
				SetTestStepResult(EPass);
			else 
				SetTestStepResult(EFail);
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

CBoundCLPushStep_3::CBoundCLPushStep_3()
/**
* Constructor
*/
	{
	}

CBoundCLPushStep_3::~CBoundCLPushStep_3() 
/**
* Destructor
*/
	{
	}

TVerdict CBoundCLPushStep_3::doTestStepL()
/*
* Test the BoundCLPush APIs in unconnected state.
*/
	{
	INFO_PRINTF1(_L("BoundCLPush. Requests without connection"));
	// check preamble result
	TVerdict verdict = TestStepResult();
	if (verdict==EPass)
		{
		TInt err, ret;
		iImp = LoadInterface();
		if (!iImp)
			{
			ERR_PRINTF1(_L("Unable to load BoundCLPush Interface"));
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
								else
									{
									err=iImp->GetBearer(iBearer);
									TRAP(ret, TEST_CHECKL(err, iExError, _L("GetBearer Fails not as expected")))
										if (ret) 
											verdict = EFail;
										else
											{
											HBufC8* host;
											err=iImp->GetServerAddress(host);
											TRAP(ret, TEST_CHECKL(err, iExError, _L("GetServerAddress Fails not as expected")))
												if (ret) 
													verdict = EFail;
											}
									}
							}
					}
				UnloadInterface();
			}
		}
	if (verdict==EPass)
		{
		INFO_PRINTF1(_L("BoundCLPush. Requests Fail as expected"));
		}
	SetTestStepResult(verdict);
	return TestStepResult();
	}

TInt CBoundCLPushStep_3::CallStateMachine()
	{
	return (0);
	}


CBoundCLPushStep_4::CBoundCLPushStep_4()
/**
* Constructor
*/
	{
	}

CBoundCLPushStep_4::~CBoundCLPushStep_4() 
/**
* Destructor
*/
	{
	}

TVerdict CBoundCLPushStep_4::doTestStepL()
/** 
* Test receiving PDU data over UDP
* The test is run concurently with CBoundCLPushStep_9 test. CBoundCLPushStep_4 waits for push message whilst 
* CBoundCLPushStep_9 sends a push message over UDP using loopback. 
* The test pair is used to test two cases (dependent on configuration read for CBoundCLPushStep_9):
* - sending PDU data that is smaller than the allocated buffer on the receiving side (<=1024)
* - sending PDU data that is larger than the allocated buffer on the receiving side and needs reallocation (>1024)
* @return - TVerdict code
* 
*/
	{
	INFO_PRINTF1(_L("BOUND-CLPUSH: Connect-AwaitPush"));
	INFO_PRINTF1(_L("Receiving PDU over UDP from a concurrent test... "));
	if(TestStepResult() == EPass )
		{
		SetTestStepResult(EFail);
		iImp = LoadInterface();
		if(iImp)
			{
			SetLastError(KErrNone);
			
			TInt bearer, port;
			TInt error = KErrNone;
			
			if(GetIntFromConfig(ConfigSection(),KWapBearer, bearer))
				{
				iBearer = static_cast<Wap::TBearer>(bearer);
				}
			if(GetIntFromConfig(ConfigSection(),KWapLocalPort, port)) 
				{
				iLocalPort = static_cast<Wap::TPort>(port);
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
				GetIntFromConfig(ConfigSection(),KWapSIMTSYState, iSimTsyState);
				User::LeaveIfError(RProperty::Set(KUidPSSimTsyCategory, KPSSimTsyTestNumber, iSimTsyState));
				}

						
			SetState(EConnect);
			iControl->ReStart();
			ShowConnectionInfo();

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

TInt CBoundCLPushStep_4::CallStateMachine()
	{
	TVerdict verdict = EFail;
	TInt err = KErrNone;
	TInt activate = 0;
	
	INFO_PRINTF2(_L("CallStateMachine:  state [%d]"), State());
	switch(iState)
		{
		case EConnect:
			INFO_PRINTF1(_L("EConnect"));
			err = iImp->Connect(iBearer, iLocalPort, EFalse);
			INFO_PRINTF2(_L("Connection completed with code = %d"), err);
			if( err == KErrNone)
				{
				if(iBearer != Wap::EIP)
					{
					TRAP(err, WaitForInitializeL());
					}
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
				INFO_PRINTF1(_L("Headers:"));
				DumpBuf(iMaxWspHeaderRecvBuffer);
				INFO_PRINTF1(_L("Body"));
				DumpBuf(iMaxWspDataRecvBuffer);
				if (verdict==EPass)
					{
					HBufC8* buf;
					iImp->GetServerAddress(buf);
					verdict = (buf->Match(iRemoteHost)!=KErrNotFound)?EPass:EFail;
					delete buf;
					}
				if (verdict==EPass)
					{
					Wap::TPort port;
					iImp->GetLocalPort(port);
					verdict = (port == iLocalPort)?EPass:EFail;
					}
				if (verdict==EPass)
					{
					Wap::TBearer bearer;
					iImp->GetBearer(bearer);
					verdict = (bearer == iBearer)?EPass:EFail;
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

CBoundCLPushStep_5::CBoundCLPushStep_5()
/**
* Constructor
*/
	{
	}

CBoundCLPushStep_5::~CBoundCLPushStep_5() 
/**
* Destructor
*/
	{
	}

TVerdict CBoundCLPushStep_5::doTestStepL()
/** 
* Test Cancel 
* @return - TVerdict code
* 
*/
	{
	INFO_PRINTF1(_L("BOUNDCL-PUSH: Testing Cancel"));
	if(TestStepResult() == EPass )
		{
		SetTestStepResult(EFail);
		iImp = LoadInterface();
		if(iImp)
			{
			TInt bearer, port;
			TInt error	= KErrNone;
			TBool secure= EFalse;
			TBool reconnect= EFalse;
			SetLastError(KErrNone);
			
			if(GetIntFromConfig(ConfigSection(),KWapBearer, bearer)) iBearer = (Wap::TBearer)bearer;
			if(GetIntFromConfig(ConfigSection(),KWapLocalPort, port)) iLocalPort = (Wap::TPort)port;
			if(GetIntFromConfig(ConfigSection(), KWapExError, error)) SetExpectedError(error);
			if(GetBoolFromConfig(ConfigSection(), KWapSecure, secure)) iSecure = secure;
			if(GetBoolFromConfig(ConfigSection(),_L("Reconnect"), reconnect)) iReconnect = reconnect;
			
			ShowConnectionInfo();
			error = iImp->Connect(iBearer, iLocalPort, iSecure);
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

TInt CBoundCLPushStep_5::CallStateMachine()
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


CBoundCLPushStep_6::CBoundCLPushStep_6() 
	{
	}
CBoundCLPushStep_6::~CBoundCLPushStep_6() 
	{
	}
TVerdict CBoundCLPushStep_6::doTestStepL()
	{
	return TestStepResult();
	}
TInt CBoundCLPushStep_6::CallStateMachine()
	{
	return (0);
	}

CBoundCLPushStep_7::CBoundCLPushStep_7() 
	{
	}
CBoundCLPushStep_7::~CBoundCLPushStep_7() 
	{
	}
TVerdict CBoundCLPushStep_7::doTestStepL()
	{
	INFO_PRINTF1(_L("BOUND-CLPUSH: Connect-AwaitPush-AwaitPush"));
	if(TestStepResult() == EPass )
		{
		SetTestStepResult(EFail);
		iImp = LoadInterface();
		if(iImp)
			{
			SetLastError(KErrNone);
			
			TInt bearer, port;
			TInt error = KErrNone;
			
			if(GetIntFromConfig(ConfigSection(),KWapBearer, bearer)) 
				{
				iBearer = static_cast<Wap::TBearer>(bearer);
				}
			if(GetIntFromConfig(ConfigSection(),KWapLocalPort, port)) 
				{
				iLocalPort = static_cast<Wap::TPort>(port);
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
TInt CBoundCLPushStep_7::CallStateMachine()
	{
	TVerdict verdict = EFail;
	TInt err = KErrNone;
	TInt activate = 0;
	
	INFO_PRINTF2(_L("CallStateMachine: state [%d]"), State());
	switch(iState)
		{
		case EConnect:
			
			INFO_PRINTF1(_L("EConnect"));
			err = iImp->Connect(iBearer, iLocalPort, EFalse);
			INFO_PRINTF2(_L("Connection completed with code = %d"), err);
			if( err == KErrNone)
				{
				iRecvHeadersBuf.Set(iRecvHeaders->Des());
				iRecvBodyBuf.Set(iRecvBody->Des());
				TRequestStatus reqStatus;
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
			INFO_PRINTF2(_L("Error: EIdle or unknown state [%d]"), State());
			SetTestStepResult(verdict);
			StopScheduler();
			break;
		}
	return (activate);
	}

CBoundCLPushStep_8::CBoundCLPushStep_8() 
	{
	}
CBoundCLPushStep_8::~CBoundCLPushStep_8() 
	{
	}
TVerdict CBoundCLPushStep_8::doTestStepL()
/**
*	@return - TVerdict code
*	Wrong parameter values for Bound CL Push API:
*	- Wrong TUid on construction
*	- Connect to wrong bearer (already implemented in test 2)
*/
	{
	INFO_PRINTF1(_L("CreateImplementation with wrong Uid- CWapBoundCLPushService"));
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
		TRAP(trapValue, iImp = CWapBoundCLPushService::NewL(invalidInterfaceUid));
		TVerdict verdict = (trapValue == iExError2)?EPass:EFail;
		if (verdict==EPass)
			{
			INFO_PRINTF2(_L("CreateImplementation with wrong Uid- Failed as expected with error %d"),trapValue);
			}
		SetTestStepResult(verdict);
		}
	return TestStepResult();
	}
TInt CBoundCLPushStep_8::CallStateMachine()
	{
	return (0);
	}

CBoundCLPushStep_9::CBoundCLPushStep_9() 
	{
	}
CBoundCLPushStep_9::~CBoundCLPushStep_9() 
	{
	}
TVerdict CBoundCLPushStep_9::doTestStepL()
/** 
* Test sending PDU data over UDP
* Test should be run concurently with CBoundCLPushStep_4 test. CBoundCLPushStep_4 waits for push message whilst 
* CBoundCLPushStep_9 sends a push message over UDP using loopback. 
* The test pair is used to test two cases (dependent on configuration read for CBoundCLPushStep_9):
* - sending PDU data that is smaller than the allocated buffer on the receiving side (<=1024)
* - sending PDU data that is larger than the allocated buffer on the receiving side and needs reallocation (>1024)
* @return - TVerdict code
*/
	{
	INFO_PRINTF1(_L("BOUND-CLPUSH: Sending Push Message"));
	
	if(TestStepResult() == EPass )
		{
		User::After(20000000);
		SetTestStepResult(EFail);
		SetLastError(KErrNone);
		TInt error = KErrNone;
		/**
		Getting information from config file
		Local port
		Remote port
		Local address
		Remote address
		Size of data to be sent
		*/
		GetIntFromConfig(ConfigSection(),KLocalPort, iLclPort);
		GetIntFromConfig(ConfigSection(),KRemotePort, iRmtPort);
		TPtrC temp;
		if(GetStringFromConfig(ConfigSection(),KWapLocalHost, temp))
			{
			error = iLocalAddr.Input(temp);
			if(error)
				{
				User::Leave(error);
				}
			}
		if(GetStringFromConfig(ConfigSection(),KWapRemoteHost, temp ))
			{
			error = iRemoteAddr.Input(temp);
			if(error)
				{
				User::Leave(error);
				}
			}
//		error = GetStringFromConfig(ConfigSection(),KWapData, temp);

		RFs fS;
		RFile file;
		User::LeaveIfError(fS.Connect());
		_LIT(KPduFilename,"z:\\testdata\\configs\\pushpdu.bin");
		_LIT(KPDUDataSize,"Data_Size");
		
		User::LeaveIfError(file.Open(fS, KPduFilename , EFileStream | EFileRead));
		/* Read PDU from the file */

		TInt dataSize(1024);        
		GetIntFromConfig(ConfigSection(),KPDUDataSize, dataSize);
		iSendBuffPtr = HBufC8::NewL(dataSize);
		TPtr8 tmpbuf = iSendBuffPtr->Des();
		User::LeaveIfError(file.Read(tmpbuf));

		file.Close();
		fS.Close();
		/**
		Settings
		*/
		iLocalAddr.SetPort(iLclPort);
		iRemoteAddr.SetPort(iRmtPort);
//		iSendBuffPtr = HBufC8::NewL(temp.Length());
//		TPtr8 tmp = iSendBuffPtr->Des();
//		tmp.Copy(temp);
		INFO_PRINTF2(_L("Sending PDU of size %d over UDP to a concurrent test... "), dataSize);
		/**
		socket server 
		*/
		RSocketServ socketServer;
	    User::LeaveIfError(socketServer.Connect());
		CleanupClosePushL(socketServer);
		RSocket sock;
		User::LeaveIfError(sock.Open(socketServer, KAfInet, KSockDatagram, KProtocolInetUdp));
		CleanupClosePushL(sock);
		TRequestStatus status;
		sock.SendTo(iSendBuffPtr->Des(),iRemoteAddr , 0, status);
		User::WaitForRequest(status);
		INFO_PRINTF2(_L("Send Push Message - status = %d"), status.Int());
		User::LeaveIfError(status.Int());
		sock.Close();
		socketServer.Close();
		CleanupStack::PopAndDestroy(&sock);
		CleanupStack::Pop(&socketServer);
		TVerdict verdict = (status.Int() == KErrNone)?EPass:EFail;
		SetTestStepResult(verdict);
		}
	return TestStepResult();
	}
TInt CBoundCLPushStep_9::CallStateMachine()
	{
			return (0);
	}




