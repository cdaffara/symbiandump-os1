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
// This is the header file for MultiHoming GetConnectionInfo step    
// 
//

/**
 @file
*/
 
#if !defined (__TS_GETCONNECTIONINFO_H__)
#define __TS_GETCONNECTIONINFO_H__

#include "TS_MultiHomingStep.h"
#include "TS_MultiHomingSuite.h"

/******************************************************
 * Uses GetActive...() to retrieve info on a named RCconnection
 */
class CTS_GetConnectionInfo : public CTS_MultiHomingStep
	{
	public:
		CTS_GetConnectionInfo();
		virtual ~CTS_GetConnectionInfo();	// final
		virtual enum TVerdict doTestStepL();

	private:
		void PrintFeature(TUint& aMask, TUint aFeature, const TDesC& aDes);
		enum TVerdict doTestStepPreambleL();
	};


#endif 
		// __TS_GETCONNECTIONINFO_H__

