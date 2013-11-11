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
//

#include "es_ptestinternalsocket.h"

#include <comms-infras/nifif_internal.h>

#include "proxyprotocol.h"
#include "ptestcases.h"


//---------------------------------------------------------------------------------------------------------

void Panic(TDPanic aPanic)
//
// Panic the Protocol
//
	{

	User::Panic(_L("TestInternalSocket Prot"),aPanic);
	}

	
CProxyProtocol::CProxyProtocol(TServerProtocolDesc* aProtoDesc)
//
//
//
	:CProtocolBase(),iProtoDesc(aProtoDesc),iProtocols(0x16)
	{
	__DECLARE_NAME(_S("CProxyProtocol"));
	iStarted=EFalse;
	}

CServProviderBase * CProxyProtocol::NewSAPL(TUint /*aProtocol*/)
	{
	return CProxyProvd::NewL(*this);
	}


CProxyProtocol::~CProxyProtocol()
	{
	for (TInt i=0;i<iProtocols.Count();i++)
		{
		iProtocols[i]->Close();
		}
	}

CProxyProtocol *CProxyProtocol::NewL(TInt aType, TServerProtocolDesc* aProtoDesc)
	{

	CProxyProtocol* prot=0;
	switch (aType)
		{
	case KProxy:
		prot=new (ELeave) CProxyProtocol(aProtoDesc);
		break;

	default:
		prot=(CProxyProtocol*)0xABCD; // keep lint happy
		Panic(EBadProtocol);
		}
	prot->iType=aType;
	return prot;
	}

void CProxyProtocol::InitL(TDesC& /*aTag*/)
	{
	// Force a fail on Memory tests
	char* ptr=new(ELeave) char;
	delete ptr;
	}

void CProxyProtocol::BindL(CProtocolBase* /*aProtocol*/, TUint /*anId*/)
	{
	// Force a fail on Memory tests
	char* ptr=new(ELeave) char;
	delete ptr;
	}

void CProxyProtocol::BindToL(CProtocolBase* aProtocol)
	{
	// Force a fail on Memory tests
	char* ptr=new(ELeave) char;
	delete ptr;
	iProtocols.AppendL(aProtocol);
	aProtocol->Open();
	}


void CProxyProtocol::StartL(void)
	{
	// Force a fail on Memory tests
	char* ptr=new(ELeave) char;
	delete ptr;
	iStarted=ETrue;
	}

TInt CProxyProtocol::Send(RMBufChain &,CProtocolBase* /*aSourceProtocol*/)
	{
	return 1;
	}

TInt CProxyProtocol::Send(TDes8 &, TSockAddr* /*to*/,TSockAddr* /*from*/,CProtocolBase* /*aSourceProtocol*/)
	{
	return 1;
	}

void CProxyProtocol::Process(RMBufChain &,CProtocolBase* /*aSourceProtocol*/)
	{
	}

void CProxyProtocol::Process(TDes8 & ,TSockAddr* /*from*/,TSockAddr* /*to*/,CProtocolBase* /*aSourceProtocol*/)
	{
	}

void CProxyProtocol::Identify(TServerProtocolDesc *aDesc) const
	{
	Mem::Copy(aDesc, iProtoDesc, sizeof(TServerProtocolDesc));
	}

TInt CProxyProtocol::GetOption(TUint level,TUint,TDes8&,CProtocolBase* /*aSourceProtocol*/)
	{
	
	if(level==KNifOptLevel)
		{
		return KErrNotSupported;
		}
	return KErrNone;
	}

TInt CProxyProtocol::SetOption(TUint /*level*/,TUint /*name*/,const TDesC8& /*option*/,CProtocolBase* /*aSourceProtocol*/)
	{
	return KErrNone;
	}

void CProxyProtocol::Error(TInt /*anerror*/,CProtocolBase* /*aSourceProtocol*/)
	{
	}
//----------------------------------------------------------------------------------------------
CProxyProvd *CProxyProvd::NewL(CProxyProtocol &aProtocol)
	{
	CProxyProvd *sp=new(ELeave) CProxyProvd(aProtocol);
//	CleanupStack::PushL(sp);
//	sp->ConstructL();
//	CleanupStack::Pop(sp);
	return sp;
	}

CProxyProvd::CProxyProvd(CProxyProtocol &aProtocol)
	{
	__DECLARE_NAME(_S("CTestIntSocketProvd"));
	iIsBound=EFalse;
	iProtocol=&aProtocol;
	iCompleteIoctl = FALSE;
	}


void CProxyProvd::LocalName(TSockAddr& anAddr) const
	{
	anAddr=iAddr;
	}

TInt CProxyProvd::SetLocalName(TSockAddr& anAddr)
	{
	iAddr=anAddr;
	iIsBound=ETrue;
	return KErrNone;
	}

void CProxyProvd::RemName(TSockAddr& /*anAddr*/)const
	{
	}

TInt CProxyProvd::SetRemName(TSockAddr& /*anAddr*/)
	{

//	test.Printf(_L("CTestIntSocketProvd::SetRemName Port %x, Addr %lx\n"),addr.Port(),addr.Address());
	return KErrNone;
	}

void CProxyProvd::Shutdown(TCloseType anOption)
	{
	if (iTestStepNotify)
		{
		iTestStepNotify->AboutToShutdown();
		}
	if (iCompleteIoctl)
		{
		iSocket->IoctlComplete(NULL);
		}
	if (anOption==ENormal)
		iSocket->CanClose();
	}

void CProxyProvd::Start()
//
//
//
	{
	}

void CProxyProvd::Shutdown(TCloseType /*anOption*/,const TDesC8 &/*aDisconnectData*/)
	{
	Panic(EBadCall);
	}

void CProxyProvd::AutoBind( void )
	{
//	test.Printf(_L("CTestIntSocketProvd::AutoBind\n"));
	iIsBound=ETrue;
	}
	

void CProxyProvd::Ioctl(TUint /*level*/,TUint name,TDes8* aOption)
	{
	switch (name)
		{
		case KProxyProtocolTestRequest:
			{
			const TProxyProtocolRequest* request = reinterpret_cast<const TProxyProtocolRequest*>(aOption->Ptr());
			TRAPD(r,CEIntSockTestBase::StartTestL(request->name,this));
			if (r == KErrNone)
				{
				break;
				}
			}
		default:
			iSocket->Error(KErrNotSupported,MSocketNotify::EErrorIoctl);
		}
	}
	
void CProxyProvd::TestComplete(TProxyProtocolTestResult aResult)
/**
Called by the state machine (CInternalSocketTester) when a test completes (or fails early). Completes the ioctl to pass the result back to the client side test code.
@param aResult	The result of the test (EPass or EFail)
*/
	{
	TProxyProtocolRequest resultStruct;
	resultStruct.result = aResult;
	TPckg<TProxyProtocolRequest> resultPckg(resultStruct);
	iSocket->IoctlComplete(&resultPckg);
	}
	
void CProxyProvd::CancelIoctl(TUint /*aLevel*/,TUint /*aName*/)
	{}
	
TInt CProxyProvd::Write(RMBufChain& /*aData*/, TUint /*options*/, TSockAddr* /*anAddr*/)
	{
	return KErrNotSupported;
	}
	
TUint CProxyProvd::Write(const TDesC8& /*aDesc*/, TUint /*options*/, TSockAddr* /*anAddr*/)
	{
	return (TUint)KErrNotSupported; //The return should really be a TInt possibly need to change the interface	
	}
	
TInt CProxyProvd::GetData(RMBufChain& /*aData*/, TUint /*aLength*/, TUint /*options*/,TSockAddr* /*anAddr*/)
	{
	return KErrNotSupported;
	}

void CProxyProvd::GetData(TDes8& /*aDesc*/,TUint /*options*/,TSockAddr* /*anAddr*/)
	{}

void CProxyProvd::ActiveOpen(void)
	{}
	
void CProxyProvd::ActiveOpen(const TDesC8 &/*aConnectionData*/)
	{}

TInt CProxyProvd::PassiveOpen(TUint /*aQue*/)
	{
	return KErrNotSupported;
	}
	
TInt CProxyProvd::PassiveOpen(TUint /*aQue*/,const TDesC8& /*aConnectionData*/)
	{
	return KErrNotSupported;
	}
	
TInt CProxyProvd::GetOption(TUint /*level*/,TUint /*name*/,TDes8& /*anOption*/ )const
	{
	return KErrNotSupported;
	}
	
TInt CProxyProvd::SetOption(TUint /*level*/,TUint /*name*/,const TDesC8& /*anOption*/)
	{
	return KErrNotSupported;
	}
	

