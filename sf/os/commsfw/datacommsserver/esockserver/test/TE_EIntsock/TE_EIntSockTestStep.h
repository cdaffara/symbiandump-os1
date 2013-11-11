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

#if (!defined __TE_EINTSOCKTESTSTEP_H__)
#define __TE_EINTSOCKTESTSTEP_H__
//#include <in_sock.h>
#include <test/testexecutestepbase.h>  



class CEIntSockTestStep : public CTestStep
	{
	public:
		CEIntSockTestStep();
		~CEIntSockTestStep();
		
		enum TVerdict doTestStepPreambleL( void );
		enum TVerdict doTestStepL( void );
		enum TVerdict doTestStepPostambleL( void );
		
	protected:
		enum TVerdict InternalDoTestStepL( void );
		void doPanicStepTestPreambleL( void );
		void doPanicStepTestPostambleL( TRequestStatus& aStat );
		const TPtrC& EpocErrorToText(const TInt aErrCode);
	private:
		TPtrC iErrText;
		TBuf<16> iErrBuff;		// size must at least satisfy the number of digits used to represent a TInt
		RThread iESockProxyThread;
		TRequestStatus iESockProxyThreadStatus;
	};

#endif /* __TE_EINTSOCKTESTSTEP_H__*/

