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
// Interfaces for Mobile IP to Simple IP Fallback Mechanism Test classes.
// 
//

/**
 @file 
 @internalComponent
*/
#ifndef __FALLBACKTESTSTEPS_H__
#define __FALLBACKTESTSTEPS_H__

#include <testexecutestepbase.h>
#include "te_fallback.h"

#include <es_sock.h> 

namespace te_mobile_ip_to_simple_ip_fallback
{
/**
 Base class for Mobile IP to Simple IP Fallack Mechanism tests.
 Encapsulates functionality for setting up the tests, and defines utility methods.
 Does not contain functionality specific for test scenarios.
	 
 @internalComponent
 @test
 */
class CFallbackStepBase: public CTestStep 
	{
public:	
	TVerdict doTestStepPreambleL();	
	TVerdict doTestStepPostambleL();
	TVerdict doTestStepL();
protected:	
	virtual TVerdict doTestSequenceL(const TBool aLog, const TInt aIteration) = 0; 
	void LogAndPanicIfError(const TInt aErrorCode, const TDesC& aMessage);
	
	/** ESock, for access to RConnection */
	RSocketServ iEsock;
	
	/** RConnection: used to establish a PPP connection */
	RConnection iPppConn;
	
	/** Test sequence result */
	TVerdict iTestSequenceVerdict;
	};

/**
 Defines the test for successful fallback scenario.
	 
 @internalComponent
 @test
 */
class CFallbackSucceedsStep: public CFallbackStepBase	
	{
public:
	CFallbackSucceedsStep();
	TVerdict doTestSequenceL(const TBool aLog, const TInt aIteration);
	
	//TVerdict doTestStepL();
	};
_LIT(KFallbackSucceedsStepName,"CFallbackSucceedsStep"); 



/**
 Defines the test for unsuccessful fallback with graceful failure scenario.
 	 
 @internalComponent
 @test
 */
class CFallbackFailsStep: public CFallbackStepBase 
	{
public:
	CFallbackFailsStep();
	TVerdict doTestSequenceL(const TBool aLog, const TInt aIteration);
	
	//TVerdict doTestStepL();
	};	
_LIT(KFallbackFailsStepName,"CFallbackFailsStep");
 
}
#endif // __FALLBACKTESTSTEPS_H__

