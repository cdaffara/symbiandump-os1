
// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains Protocol test cases from section 21 
// CPROTOCOL TESTS
// 
//

/**
 @file SocketTestSectioncpp
*/

#include <e32base.h>
#include "ProtocolTestSection.h"
#include "ES_DUMMY.H"
#include <es_sock.h>
#include <es_mbuf.h>
#include <es_prot.h>
#include <e32test.h>
#include <comms-infras/nifif.h>
#include <es_mbuf.h>
#include <ss_pman.h>
#include <ss_glob.h>
#include <e32math.h>
#include <dns_qry.h>
#include "pdummy.h"
#include <agenterrors.h>


// Test step 1
const TDesC& CProtocolTest1::GetTestName()
	{
	_LIT(ret,"Test1");
	return ret;
	}

enum TVerdict CProtocolTest1::InternalDoTestStepL( void )
	// expected to Panic(EDoesNotBindAbove);
	{
	TServerProtocolDesc desc;
	desc.iAddrFamily = KDummyAddrFamily;
	iDummy = CDummyProtocol::NewL(KDummyOne, &iDesc);
	CDummyProtocol* prot2 = new (ELeave) CDummyProtocol(&desc);
	TInt ret = iDummy->ProtocolBaseBindTo(prot2);
	delete prot2;
	return EPass;
	}
	
	 
// Test step 2
const TDesC& CProtocolTest2::GetTestName()
	{
	_LIT(ret,"Test2");
	return ret;
	}

enum TVerdict CProtocolTest2::InternalDoTestStepL( void )
	// expected to Panic(EDoesNotBindBelow);
	{
	TServerProtocolDesc desc;
	desc.iAddrFamily = KDummyAddrFamily;
	iDummy = CDummyProtocol::NewL(KDummyOne, &desc);
	CDummyProtocol* prot2 = new (ELeave) CDummyProtocol(&desc);
	TUint id = 0;
	TInt ret = iDummy->ProtocolBaseBind(prot2, id);	
	if (ret != KErrNone)
		{
		return EFail;
		}
	delete prot2;	
	return EPass;
	}
	
// Test step 3
const TDesC& CProtocolTest3::GetTestName()
	{
	_LIT(ret,"Test3");
	return ret;
	}

enum TVerdict CProtocolTest3::InternalDoTestStepL( void )
	// expected to Panic(EErrorCallNotHandled)
	{
	TServerProtocolDesc desc;
	desc.iAddrFamily = KDummyAddrFamily;
	iDummy = CDummyProtocol::NewL(KDummyOne, &desc);
	CDummyProtocol* prot2 = new (ELeave) CDummyProtocol(&desc);
	TInt ret = iDummy->ProtocolBaseError(prot2);
	if (ret != KErrNone)
		{
		return EFail;
		}
	delete prot2;
	return EPass;
	}
	
	 
// Test step 4
const TDesC& CProtocolTest4::GetTestName()
	{
	_LIT(ret,"Test4");
	return ret;	
	}

enum TVerdict CProtocolTest4::InternalDoTestStepL( void )
	// expected to return KErrNotSupported
	{
	TServerProtocolDesc desc;
	desc.iAddrFamily = KDummyAddrFamily;
	iDummy = CDummyProtocol::NewL(KDummyOne, &desc);
	TUint level = 0;
	TUint name  = 0;
	TBuf8<20> buf;
	CDummyProtocol* prot2 = new (ELeave) CDummyProtocol(&desc);
	TInt ret = iDummy->ProtocolBaseGetOption(level, name, buf, prot2);
	if (ret != KErrNotSupported)
		{
		return EFail;
		}
	delete prot2;
	return EPass;
	}
		
// Test step 5
const TDesC& CProtocolTest5::GetTestName()
	{
	_LIT(ret,"Test5");
	return ret;
	}

enum TVerdict CProtocolTest5::InternalDoTestStepL( void )
	// expected to panic with Fault(EBadHostResolver);
	{
	TServerProtocolDesc desc;
	desc.iAddrFamily = KDummyAddrFamily;
	iDummy = CDummyProtocol::NewL(KDummyOne, &desc);
	TInt ret = iDummy->ProtocolBaseNewHostResolver();
	if (ret != KErrNone)
		{
		return EFail;
		}
	return EPass;
	}
	
		
// Test step 6
const TDesC& CProtocolTest6::GetTestName()
	{
	_LIT(ret,"Test6");
	return ret;
	}

enum TVerdict CProtocolTest6::InternalDoTestStepL( void )
	// expected to panic with Fault(EBadNetDBRequest)
	{
	TServerProtocolDesc desc;
	desc.iAddrFamily = KDummyAddrFamily;
	iDummy = CDummyProtocol::NewL(KDummyOne, &desc);
	TInt ret = iDummy->ProtocolBaseNewNetDatabase();
	if (ret != KErrNone)
		{
		return EFail;
		}
	return EPass;
	}
	
		
// Test step 7
const TDesC& CProtocolTest7::GetTestName()
	{
	_LIT(ret,"Test7");
	return ret;
	}

enum TVerdict CProtocolTest7::InternalDoTestStepL( void )
	// expected to panic with Fault(EOddSock)
	{
	TServerProtocolDesc desc;
	desc.iAddrFamily = KDummyAddrFamily;
	iDummy = CDummyProtocol::NewL(KDummyOne, &desc);
	TUint aProt = 0;
	TInt ret = iDummy->ProtocolBaseNewSAP(aProt);
	if (ret != KErrNone)
		{
		return EFail;
		}
	return EPass;
	}
	
		
// Test step 8
const TDesC& CProtocolTest8::GetTestName()
	{
	_LIT(ret,"Test8");
	return ret;
	}

enum TVerdict CProtocolTest8::InternalDoTestStepL( void )
	// expected to panic with Fault(EBadServiceResolver)
	{
	TServerProtocolDesc desc;
	desc.iAddrFamily = KDummyAddrFamily;
	iDummy = CDummyProtocol::NewL(KDummyOne, &desc);
	TInt ret = iDummy->ProtocolBaseNewServiceResolver();
	if (ret != KErrNone)
		{
		return EFail;
		}
	return EPass;
	}
	
		
// Test step 9
const TDesC& CProtocolTest9::GetTestName()
	{
	_LIT(ret,"Test9");
	return ret;
	}

enum TVerdict CProtocolTest9::InternalDoTestStepL( void )
	// expected to panic with Panic(ECantProcessMbufs)
	{
	TServerProtocolDesc desc;
	desc.iAddrFamily = KDummyAddrFamily;
	iDummy = CDummyProtocol::NewL(KDummyOne, &iDesc);
	CDummyProtocol* prot = new (ELeave) CDummyProtocol(&desc);
	RMBufChain chain;
	TInt ret = iDummy->ProtocolBaseProcess(chain, prot);
	if (ret != KErrNone)
		{
		return EFail;
		}
	delete prot;
	return EPass;
	}
	
// Test step 10
const TDesC& CProtocolTest10::GetTestName()
	{
	_LIT(ret,"Test10");
	return ret;
	}

enum TVerdict CProtocolTest10::InternalDoTestStepL( void )
	// expected to panic with Panic(ECantSendMBufs);
	{
	TServerProtocolDesc desc;
	desc.iAddrFamily = KDummyAddrFamily;
	iDummy = CDummyProtocol::NewL(KDummyOne, &iDesc);
	CDummyProtocol* prot = new (ELeave) CDummyProtocol(&desc);
	RMBufChain chain;
	TInt ret = iDummy->ProtocolBaseSend(chain, prot);
	if (ret != KErrNone)
		{
		return EFail;
		}
	delete prot;
	return EPass;
	}
	
// Test step 11
const TDesC& CProtocolTest11::GetTestName()
	{
	_LIT(ret,"Test11");
	return ret;
	}

enum TVerdict CProtocolTest11::InternalDoTestStepL( void )
	// expected to return KErrNotSupported
	{
	CDummyProtocol* iDummy = CDummyProtocol::NewL(KDummyOne, &iDesc);
	TUint level = 0;
	TUint name  = 0;
	TBuf8<20> buf;
	TServerProtocolDesc desc;
	desc.iAddrFamily = KDummyAddrFamily;
	CDummyProtocol* prot = new (ELeave) CDummyProtocol(&desc);
	TInt ret = iDummy->ProtocolBaseSetOption(level, name, buf, prot);
	if (ret != KErrNotSupported)
		{
		return EFail;
		}
	delete prot;
	return EPass;
	}
	
// Test step 12
const TDesC& CProtocolTest12::GetTestName()
	{
	_LIT(ret,"Test12");
	return ret;
	}

enum TVerdict CProtocolTest12::InternalDoTestStepL( void )
	{
	// expected to panic with Panic(ECantSendDescriptors);
	TServerProtocolDesc desc;
	desc.iAddrFamily = KDummyAddrFamily;
	TBuf8<20> buf;
	TSockAddr to;
	TSockAddr from;
	CDummyProtocol* prot2 = new (ELeave) CDummyProtocol(&desc);
	iDummy = CDummyProtocol::NewL(KDummyOne, &iDesc);
	TInt ret = iDummy->ProtocolBaseSendWithAddress(buf, &to, &from, prot2);
	if (ret != KErrNone)
		{
		return EFail;
		}
	delete prot2;
	return EPass;
	}
	

// Test step 13
const TDesC& CProtocolTest13::GetTestName()
	{
	_LIT(ret,"Test13");
	return ret;
	}

enum TVerdict CProtocolTest13::InternalDoTestStepL( void )
	// expected to panic with Panic(ECantProcessMbufs)	
	{
	TServerProtocolDesc desc;
	desc.iAddrFamily = KDummyAddrFamily;
	TBuf8<20> buf;
	TSockAddr to;
	TSockAddr from;
	CDummyProtocol* prot2 = new (ELeave) CDummyProtocol(&desc);
	iDummy = CDummyProtocol::NewL(KDummyOne, &iDesc);
	TInt ret = iDummy->ProtocolBaseProcessWithAddress(buf, &from, &to, prot2);
	if (ret != KErrNone)
		{
		return EFail;
		}
	delete prot2;
	return EPass;
	}

