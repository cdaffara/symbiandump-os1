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
// Contains declaration of CTestStepProtocol class that is
// base class for all test step classes in this test.
// 
//

/**
 @file TestStepProtocol.h
*/
#if (!defined __TESTSTEPPROTOCOL_H__)
#define __TESTSTEPPROTOCOL_H__

#include <in_sock.h>
#include <test/testexecutestepbase.h>
#include <es_prot.h>
#include <comms-infras/nifif.h>
#include <es_mbuf.h>
#include <dns_qry.h>
#include <es_prot.h>
#include "pdummy.h"

class CTestStepProtocol : public CTestStep
	{
	public:
		
		CTestStepProtocol();
		~CTestStepProtocol();
		
	struct TSocketThreadArg
		{
		CTestStepProtocol* iHandle;
		RSemaphore* iSem;
		TUint iNumSockets;
		};
	
	protected:
			
		const TPtrC& EpocErrorToText(const TInt aErrCode);
		enum TVerdict doTestStepL();
		virtual enum TVerdict InternalDoTestStepL() =0;
		
		// test threads
		static const TUint KNumTestSockets;
		static const TInt KNumExhaustiveSockets;
		static const TInt KNumStretchOpens;
		
		TServerProtocolDesc iDesc;
		CDummyProtocol* iDummy;
		
	private:
		TPtrC iErrText;
		TBuf<16> iErrBuf;		// size must at least satisfy the number of digits used to represent a TInt
	};
	

#endif /* __TESTSTEPPROTOCOL_H__ */

