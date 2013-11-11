/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


/** @file

 This is the header file for MultiHoming test section 1
 */

#if !defined (__TS_TESTINCOMMING_H__)
#define __TS_TESTINCOMMING_H__

#include "TS_MultiHomingStep.h"
#include "TS_MultiHomingSuite.h"


NONSHARABLE_CLASS(CTS_TestIncomming) : public CTS_MultiHomingStep
	{
	public:
		CTS_TestIncomming();
		virtual ~CTS_TestIncomming();
		virtual enum TVerdict doTestStepL( void );

	private:

		enum TVerdict doTestStepPreambleL(void);
	};


#endif
		// __TS_TESTCONNECTION_H__

