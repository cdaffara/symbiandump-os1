/**
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file Te_loopbackSuiteStepBase.h
 @internalTechnology
*/

#if (!defined __TE_LOOPBACK_STEP_BASE__)
#define __TE_LOOPBACK_STEP_BASE__
#include <c32comm.h>
#include <test/testexecutestepbase.h>
// Please add your include here if you have 

/****************************************************************************
* The reason to have a new step base is that it is very much possible
* that the all individual test steps have project related common variables 
* and members 
* and this is the place to define these common variable and members.
* 
****************************************************************************/

//
// constants
//
//// This should be even and equal to 'KLoopbackCount' constant that is defined in 'loopback.cpp'
const TUint KSupportedPorts=8;

_LIT(KCsyName, "loopback.csy");			//LOOPBACK.CSY module name
_LIT(KCommPortFmtStr, "Loopback::%d");	//LOOPBACK.CSY provided ports names

class CTe_loopbackSuiteStepBase : public CTestStep
	{
public:
	virtual ~CTe_loopbackSuiteStepBase();
	CTe_loopbackSuiteStepBase();
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();

//Please add your class members which will be common to all individual test steps:
protected:

	HBufC* AllocPortNameL(TInt aIndex);
	void TestErrorCodeL(TInt aErrCode, const TDesC& aMessage);
	void TestErrorCodeL(TInt aErrCode, TInt aExpectedErrCode, const TDesC& aMessage);
    TBool IsEven(TInt aVal);
    TVerdict OpenAllAvailablePortsL();
    TVerdict CloseAllAvailablePortsL();

	HBufC8*		iReadData;
	HBufC8*		iWriteData;

	RCommServ iCommServer;

	struct 
		{
		RComm port;
		HBufC* name;
		} iPortList[KSupportedPorts];
	};

#endif

