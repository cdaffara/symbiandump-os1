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
// CStepOne derived implementation
// 
//

/**
 @file
 @internalComponent
*/

#include "WapBoundWSPSteps.h"
#include "WapStackSuiteDefs.h"
#include <simtsy.h>
#include <ecom/ecom.h>

CBoundWSPStepBase::CBoundWSPStepBase()
/**
* Constructor
*/
	{
	}

CBoundWSPStepBase::~CBoundWSPStepBase()
/**
* Destructor
*/
	{
	if(iImp) 
		{
		UnloadInterface();
		}
	}

CWapBoundCLWSPService* CBoundWSPStepBase::LoadInterface()
	{
	TInt trapValue;
	CWapBoundCLWSPService* implementation = NULL;
	// UID {0x101FBB3C} - SWS implementation
	TUid KInterfaceUid = {0x101FBB3C}; 	
	INFO_PRINTF2(_L("Calling new NewL(0x%08X) method"), KInterfaceUid);
	TRAP(trapValue, implementation = CWapBoundCLWSPService::NewL(KInterfaceUid));
	if(!trapValue)
		{
		INFO_PRINTF2(_L("Interface is loaded: implementation.addr = 0x%08x"), implementation);
		return implementation;
		}
	else return NULL;
	}

void CBoundWSPStepBase::UnloadInterface()
	{
	INFO_PRINTF2(_L("Interface at implementation.addr = 0x%08x is deleted"), iImp);
	delete iImp;
	iImp = NULL;
	REComSession::FinalClose();	
	}

CBoundWSPStep_1::~CBoundWSPStep_1()
/**
* Destructor
*/
	{
	}

CBoundWSPStep_1::CBoundWSPStep_1()
/**
* Constructor
*/
	{
	}

TVerdict CBoundWSPStep_1::doTestStepL()
/**
* @return - TVerdict code
* Override of base class pure virtual
* Our implementation only gets called if the base class doTestStepPreambleL() did
* not leave. That being the case, the current test result value will be EPass.
*/
	{
	//
	// CWapBoundCLWSPService
	// 
	INFO_PRINTF1(_L("CreateImplementation - CWapBoundCLWSPService"));
	INFO_PRINTF1(_L("Calling old NewL() method"));
	
	TInt trapValue; 
	
	SetTestStepResult(EFail);
	TRAP(trapValue, iImp = CWapBoundCLWSPService::NewL());
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

CBoundWSPStep_2::CBoundWSPStep_2()
/**
* Constructor
*/
	{
	}

CBoundWSPStep_2::~CBoundWSPStep_2() 
/**
* Destructor
*/
	{
	}

TVerdict CBoundWSPStep_2::doTestStepL()
/** 
* Test Connect methoths 
* @return - TVerdict code
* 
*/
	{
	INFO_PRINTF1(_L("BOUND-WSP: Testing Connect"));
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
			TBool delay = EFalse;
			SetLastError(KErrNone);
			
			if(GetIntFromConfig(ConfigSection(),KWapBearer, bearer)) iBearer = (Wap::TBearer)bearer;
			if(GetIntFromConfig(ConfigSection(),KWapLocalPort, port)) iLocalPort = (Wap::TPort)port;
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

TInt CBoundWSPStep_2::CallStateMachine()
	{
	TInt err = KErrNone;
	INFO_PRINTF2(_L("CallStateMachine: state %d"), State());
	switch(iState)
		{
		case EConnect:
			INFO_PRINTF1(_L("EConnect"));
			err = iImp->Connect(iBearer, iLocalPort, iSecure);
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
			// it's necessary to delay port closure, let the other concurrent test step
			// can fail opening the same port with expected error EPortAlreadyBound (-5498)
			if (iDelayPortClosure)
				User::After(5000000);

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

CBoundWSPStep_3::CBoundWSPStep_3()
/**
* Constructor
*/
	{
	}

CBoundWSPStep_3::~CBoundWSPStep_3() 
/**
* Destructor
*/
	{
	}

TVerdict CBoundWSPStep_3::doTestStepL()
/*
* Test the BoundWSP APIs in unconnected state.
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
			ERR_PRINTF1(_L("Unable to load BoundWSP Interface"));
			verdict = EFail;
			}
		else
			{
			GetIntFromConfig(ConfigSection(),KWapExError,(TInt&)iExError);
			ShowConnectionInfo();
			TBuf8<100> header, body;
			TPckgBuf<TUint8> idPckg;
			TWSPStatus wspStatus;
			TRequestStatus reqStatus;
			TUint32 timeout = 0;
			iImp->MethodResult(header, body, idPckg, wspStatus, reqStatus, timeout);
			TRAP(ret, TEST_CHECKL(reqStatus.Int(), iExError, _L("MethodResult Fails not as expected")))
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
												else
													{
													TBuf<100> uri;
													TUint8 id = 0;
													err=iImp->MethodInvoke(iBearer, iRemoteHost, iRemotePort, 0x40, uri, header, body, id);
													TRAP(ret, TEST_CHECKL(err, iExError, _L("MethodInvoke Fails not as expected")))
														if (ret) 
															verdict = EFail;
													}
											}
									}
							}
					}
				UnloadInterface();
			}
		}
	if (verdict==EPass)
		{
		INFO_PRINTF1(_L("BoundWSP. Requests Fail as expected"));
		}
	SetTestStepResult(verdict);
	return TestStepResult();
	}

TInt CBoundWSPStep_3::CallStateMachine()
	{
	return (0);
	}


CBoundWSPStep_4::CBoundWSPStep_4()
/**
* Constructor
*/
	{
	}

CBoundWSPStep_4::~CBoundWSPStep_4() 
/**
* Destructor
*/
	{
	}

TVerdict CBoundWSPStep_4::doTestStepL()
/** 
* Test Connect methoths 
* @return - TVerdict code
* 
*/
	{
	INFO_PRINTF1(_L("BOUND-WSP: UnitInvoke-UnitResult"));
	if(TestStepResult() == EPass )
		{
		SetTestStepResult(EFail);
		iImp = LoadInterface();
		if(iImp)
			{
			SetLastError(KErrNone);
			
			TInt bearer, port, repeat, method;
			TInt error = KErrNone;
			TPtrC headers;
			TPtrC body;
			TPtrC uri;
			
			if(GetIntFromConfig(ConfigSection(),KWapBearer, bearer)) iBearer = (Wap::TBearer)bearer;
			if(GetIntFromConfig(ConfigSection(),KWapLocalPort, port)) iLocalPort = (Wap::TPort)port;
			if(GetIntFromConfig(ConfigSection(),KWapExError, error)) SetExpectedError(error);
			if(GetIntFromConfig(ConfigSection(),KWapWSPMethod, method)) iMethod = method;
			if(GetIntFromConfig(ConfigSection(), KWapRepeat, repeat)) iRepeat = repeat;

			if(!GetStringFromConfig(ConfigSection(),KWapURI, uri)) 
				{
				User::Leave(KErrNoMemory);
				}
			
			GetStringFromConfig(ConfigSection(),KWapWSPHeaders, headers);
			GetStringFromConfig(ConfigSection(),KWapWSPBody, body);

			User::LeaveIfError(RProperty::Set(KUidSystemCategory, KUidPhonePwr.iUid, ESAPhoneOn));
			
			if(UseSimTsy())
				{
				User::LeaveIfError(RProperty::Set(KUidPSSimTsyCategory, KPSSimTsyTestNumber, 41));
				}
			
			
			iURI = HBufC16::NewL(uri.Length());
			
			iSendHeaders = HBufC8::NewL(headers.Length());
			
			iSendBody = HBufC8::NewL(body.Length());
			
			iRecvHeaders = HBufC8::NewL(KMaxWapWspHeaderBufferLength);
			iRecvBody = HBufC8::NewL(KMaxWapWspDataBufferLength);

			TPtr16 tmp16 = iURI->Des();
			tmp16.Copy(uri);
			TPtr8 tmp1 = iSendHeaders->Des();
			tmp1.Copy(headers);
			TPtr8 tmp2 = iSendBody->Des();
			tmp2.Copy(body);

			iRecvHeadersBuf.Set(iRecvHeaders->Des());
			iRecvHeadersBuf.Zero();
			iRecvBodyBuf.Set(iRecvBody->Des());
			iRecvBodyBuf.Zero();
			
			SetState(EConnect);
			iControl->ReStart();
			ShowConnectionInfo();

			StartScheduler();

			delete iRecvBody;
			iRecvBody=NULL;
			delete iRecvHeaders;
			iRecvHeaders=NULL;

			delete iSendBody;
			iSendBody=NULL;
			delete iSendHeaders;
			iSendHeaders=NULL;
			delete iURI;
			iURI=NULL;

			UnloadInterface();
			}
		}
	return TestStepResult();
	}

TInt CBoundWSPStep_4::CallStateMachine()
	{
	TVerdict verdict = EFail;
	TInt err = KErrNone;
	TInt activate = 0;
	static TUint total_headers = 0;		
	static TUint total_body = 0;		
	static TUint repeat = 0;		
	
	INFO_PRINTF2(_L("CallStateMachine [state %d]"), State());

	switch(iState)
		{
		case EConnect:
			total_headers = 0;
			total_body = 0;
			err = iImp->Connect(iBearer, iLocalPort, EFalse);
			if( err == KErrNone)
				{
				SetState(EUnitInvoke);
				}
			else
				{
				SetState(EFinish);
				}
			SetLastError(err);
			INFO_PRINTF2(_L("Connection completed with code = %d"), err);
			iControl->ReStart();
			break;

		case EUnitInvoke:
			INFO_PRINTF2(_L("EUnitInvoke: Method - 0x%02x"), iMethod);
			if(iBearer != Wap::EIP)
			{
				TRAP(err, WaitForInitializeL());
			}
			if(iSendHeaders->Length() !=0 )
				{
				INFO_PRINTF1(_L("Headers to send:"));
				DumpBuf(iSendHeaders->Des());
				}	
			if(iSendBody->Length() !=0 )
				{
				INFO_PRINTF1(_L("Data to send:"));
				DumpBuf(iSendBody->Des());
				}
			err = iImp->MethodInvoke(iBearer, iRemoteHost, iRemotePort, iMethod, iURI->Des(), iSendHeaders->Des(), iSendBody->Des(), iTransactionId);
			INFO_PRINTF3(_L("MethodInvoke() retcode: - dec: %d hex: 0x%08x"), err, err);
			if(err == KErrNone) 
				{	
				SetState(EUnitResult);
				}
			else 
				{
				SetState(EFinish);
				}
			SetLastError(err);
			iControl->ReStart();
			break;

		case EUnitResult:

			iRecvHeadersBuf.Set(iRecvHeaders->Des());
			iRecvHeadersBuf.Zero();
			iRecvBodyBuf.Set(iRecvBody->Des());
			iRecvBodyBuf.Zero();
			iImp->MethodResult(iRecvHeadersBuf, iRecvBodyBuf, iTransactionIdPckg, iWspStatus, *iStatus, iTimeout);
			INFO_PRINTF2(_L("EUnitResult: status 0x%08x"), iStatus->Int());
			activate = 1;
			SetLastError(err);
			SetState(EFinish);
			break;
			
		case EFinish:
			
			INFO_PRINTF4(_L("EFinish: Status: %d, WSP status: %d (0x%02x)"), iStatus->Int(), iWspStatus, iWspStatus);

			if(iRecvHeadersBuf.Length() != 0)
				{
				INFO_PRINTF1(_L("Headers"));
				total_headers += iRecvHeadersBuf.Length();
				DumpBuf(iRecvHeadersBuf);
				}

			if(iRecvBodyBuf.Length() != 0)
				{
				INFO_PRINTF1(_L("Body"));
				total_body += iRecvBodyBuf.Length();
				DumpBuf(iRecvBodyBuf);
				}

			if(iStatus->Int() != KErrNone) 
				{
				SetLastError(iStatus->Int());
				if(iStatus->Int() == Wap::EMoreData)
					{
					INFO_PRINTF1(_L("NOTE: More data... calling MethodResult()"));
					SetState(EUnitResult);
					iControl->ReStart();
					break;
					}
				}

			INFO_PRINTF3(_L("Data received: headers - %d bytes, body - %d bytes"), total_headers, total_body);
			total_headers = 0;
			total_body = 0;
			if((--iRepeat != 0) && (iStatus->Int() == KErrNone))
				{
				++repeat;
				INFO_PRINTF2(_L("REPEAT TEST - %d"), repeat);
				if(err == KErrNone) 
					{	
					SetState(EUnitInvoke);
					iControl->ReStart();
					break;
					}
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

CBoundWSPStep_5::CBoundWSPStep_5()
/**
* Constructor
*/
	{
	}

CBoundWSPStep_5::~CBoundWSPStep_5() 
/**
* Destructor
*/
	{
	}

TVerdict CBoundWSPStep_5::doTestStepL()
/** 
* Test Cancel Request 
* @return - TVerdict code
* 
*/
	{
	INFO_PRINTF1(_L("BoundCLPush. Testing Cancel Request"));
	if(TestStepResult() == EPass )
		{
		SetTestStepResult(EFail);
		iImp = LoadInterface();
		if(iImp)
			{
			TInt bearer, port;
			TInt err = KErrNone;
			TBool secure= EFalse;
			TBool reconnect= EFalse;
			SetLastError(KErrNone);
			
			if(GetIntFromConfig(ConfigSection(),KWapBearer, bearer)) iBearer = (Wap::TBearer)bearer;
			if(GetIntFromConfig(ConfigSection(),KWapLocalPort, port)) iLocalPort = (Wap::TPort)port;
			if(GetIntFromConfig(ConfigSection(), KWapExError, err)) SetExpectedError(err);
			if(GetBoolFromConfig(ConfigSection(), KWapSecure, secure)) iSecure = secure;
			if(GetBoolFromConfig(ConfigSection(),_L("Reconnect"), reconnect)) iReconnect = reconnect;

			ShowConnectionInfo();
			err = iImp->Connect(iBearer, iLocalPort, iSecure);
			INFO_PRINTF2(_L("Connection completed with code = %d"), err);
			if( err == KErrNone)
				{
				TBuf8<100> header, body;
				TPckgBuf<TUint8> idPckg;
				TWSPStatus wspStatus;
				TRequestStatus reqStatus;
				TUint32 timeout=0;
				iImp->MethodResult(header, body, idPckg, wspStatus, reqStatus, timeout);
				SetState(EUnitResult);
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

TInt CBoundWSPStep_5::CallStateMachine()
	{
	INFO_PRINTF2(_L("CallStateMachine: step %d"), State());
	switch(iState)
		{
		case EUnitResult:
			INFO_PRINTF1(_L("EUnitResult"));
			iImp->CancelReq();
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

CBoundWSPStep_6::CBoundWSPStep_6() 
	{
	}
CBoundWSPStep_6::~CBoundWSPStep_6()  
	{
	}

TVerdict CBoundWSPStep_6::doTestStepL()
/*
* Test receive time out
*/
	{
	INFO_PRINTF1(_L("BOUND-WSP: Connect-UnitResult-TimeOut"));
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
			TInt headerLength, bufLength, timeOut; 
			if(GetIntFromConfig(ConfigSection(),KWapWSPRecvHeaderLength, headerLength))
				{
				iHeaderRecvBufLength=static_cast<TUint16>(headerLength);
				}
			if(GetIntFromConfig(ConfigSection(),KWapWSPRecvBodyLength, bufLength))
				{
				iBodyRecvBufLength=static_cast<TUint16>(bufLength);
				}
			if(GetIntFromConfig(ConfigSection(),KWapTimeOut, timeOut))
				{
				iTimeout=timeOut;
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

TInt CBoundWSPStep_6::CallStateMachine()
/*
* Test receive time out
*/
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
//				WaitForInitializeL();
				iRecvHeadersBuf.Set(iRecvHeaders->Des());
				iRecvBodyBuf.Set(iRecvBody->Des());
				iImp->MethodResult(iRecvHeadersBuf, iRecvBodyBuf, iTransactionIdPckg, iWspStatus, *iStatus, iTimeout);
				INFO_PRINTF1(_L("MethodResult"));
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

CBoundWSPStep_7::CBoundWSPStep_7() 
	{
	}
CBoundWSPStep_7::~CBoundWSPStep_7()
	{
	}

TVerdict CBoundWSPStep_7::doTestStepL()
//
//Wrong sequence
//
	{
	INFO_PRINTF1(_L("BOUND-WSP: Connect-UnitResult-UnitResult"));
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

TInt CBoundWSPStep_7::CallStateMachine()
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
//				WaitForInitializeL();
				TRequestStatus reqStatus;
				iRecvHeadersBuf.Set(iRecvHeaders->Des());
				iRecvBodyBuf.Set(iRecvBody->Des());
				iImp->MethodResult(iRecvHeadersBuf, iRecvBodyBuf, iTransactionIdPckg, iWspStatus, reqStatus, iTimeout);
				iImp->MethodResult(iRecvHeadersBuf, iRecvBodyBuf, iTransactionIdPckg, iWspStatus, *iStatus, iTimeout);
				INFO_PRINTF1(_L("MethodResult"));
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
			iImp->CancelReq();
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
			INFO_PRINTF2(_L("Error: EIdle or unknown state %d, State = %d"), State());
			SetTestStepResult(verdict);
			StopScheduler();
			
			break;
		}
	return (activate);
	}

CBoundWSPStep_8::CBoundWSPStep_8() 
	{
	}
CBoundWSPStep_8::~CBoundWSPStep_8()  
	{
	}
TVerdict CBoundWSPStep_8::doTestStepL()
/*
*	Using invalid values for API parameters:
*	- Invalid TUid on construction
*	- Invalid method for MethodInvoke
*	- Invalid timeout for MethodResult
*	Note: wrong bearer value has already been implemented as part of test 2
*/
	{
	INFO_PRINTF1(_L("BOUND-WSP: Wrong parameters testing"));
	if(TestStepResult() == EPass )
		{
		TInt error;
		if(GetIntFromConfig(ConfigSection(),KWapExError2, error))
			{
			iExError2=error;
			}
		TInt trapValue;
		TUid invalidInterfaceUid = {0x00000000}; /*{0x101FBB3C};*/
		INFO_PRINTF2(_L("Calling new NewL(0x%08X) method"), invalidInterfaceUid);
		TRAP(trapValue, iImp = CWapBoundCLWSPService::NewL(invalidInterfaceUid));
		TVerdict verdict = (trapValue == iExError2)?EPass:EFail;
		if (verdict==EFail)
			{
			SetTestStepResult(EFail);
			return TestStepResult();
			}
		else
			{
			INFO_PRINTF2(_L("CreateImplementation with wrong Uid- Failed as expected with error %d"),trapValue);
			// Now use the correct Uid
			iImp = LoadInterface();
			if(iImp)
				{
				SetLastError(KErrNone);
				TInt method;
				TInt error = KErrNone;
				TPtrC headers;
				TPtrC body;
				TPtrC uri;
				
				if(GetIntFromConfig(ConfigSection(), KWapExError, error))
					{
					SetExpectedError(error);
					}
				if (GetIntFromConfig(ConfigSection(),KWapWSPMethod, method))
					{
					iMethod=method;
					}
				
				if(!GetStringFromConfig(ConfigSection(),KWapURI, uri) || \
					!GetStringFromConfig(ConfigSection(),KWapWSPHeaders, headers) || \
					!GetStringFromConfig(ConfigSection(),KWapWSPBody, body))
					{
					User::Leave(KErrNoMemory);
					}
				
				iURI = HBufC16::NewL(uri.Length());
				
				iSendHeaders = HBufC8::NewL(headers.Length());
				
				iSendBody = HBufC8::NewL(body.Length());
				
				TPtr16 tmp16 = iURI->Des();
				tmp16.Copy(uri);
				TPtr8 tmp = iSendHeaders->Des();
				tmp.Copy(headers);
				TPtr8 tmp1 = iSendBody->Des();
				tmp1.Copy(body);

				ShowConnectionInfo();
				error = iImp->Connect(iBearer, iLocalPort, EFalse);
				INFO_PRINTF2(_L("Connection completed with code = %d"), error);
				if( error == KErrNone)
					{
					error = iImp->MethodInvoke(iBearer, iRemoteHost, iRemotePort, iMethod, iURI->Des(), iSendHeaders->Des(), iSendBody->Des(), iTransactionId);
					}
				TVerdict verdict = (error == GetExpectedError())?EPass:EFail;
				SetTestStepResult(verdict);
				
				delete iSendBody;
				iSendBody=NULL;
				delete iSendHeaders;
				iSendHeaders=NULL;
				delete iURI;
				iURI=NULL;
				UnloadInterface();
				}
			}
		}
	return TestStepResult();
	}
TInt CBoundWSPStep_8::CallStateMachine()
	{
	return (0);
	}

CBoundWSPStep_9::CBoundWSPStep_9() {}
CBoundWSPStep_9::~CBoundWSPStep_9()  {}

TVerdict CBoundWSPStep_9::doTestStepL()
	{
	return TestStepResult();
	}

TInt CBoundWSPStep_9::CallStateMachine()
	{
	return (0);
	}
