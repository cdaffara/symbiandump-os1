/**
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Header file for the file Te_SimPacketQos.cpp - test programme for SIM.TSY.
* 
*
*/



/**
 @file
 @internalComponent
*/

#ifndef TE_SIMPACKETQOS_H
#define TE_SIMPACKETQOS_H

#include "Te_SimTestStepBase.h"
#include "Te_SimConstants.h"

_LIT(KTestDataFile,"Te_simPacketQoS.ini");

const TUint KReq = 4;
const TUint KMin = 2; 

class CSimPacketQoSTest : public CPhBkAndPacketTestsTestStepBase
/**
 *Base class for QoS tests
 *
 */
	{
protected:
	void TestFixtureLC(TUint aTestNumber);	
	void TearDownL();

protected:
	RPacketService  iPacketService;			//Service
	RPacketContext  iPacketContext;			//Context
	RPacketQoS		iPacketQoS;				//QoS
	RPacketQoS::TQoSR99_R4Requested iR99QoSParameters;	//parameters
	
	};
    
//QoS delay = 5 and count == 1
class CSetQoSDelayOnFirstSet : public CSimPacketQoSTest
/**
	This test case is delaying set of QoS on the first set
*/
	{
public:
    CSetQoSDelayOnFirstSet();
	virtual enum TVerdict doTestStepL();
    };

//QoS delay = 7 and count == 3
class CSetQoSDelayOnThirdSet : public CSimPacketQoSTest
/**
	This test case is delaying set of QoS on the third set
*/
	{
public:
    CSetQoSDelayOnThirdSet();
	virtual enum TVerdict doTestStepL();	
    };

//QoS delay = 0 and count == 0
class CSetQoSDelayUnset : public CSimPacketQoSTest
/**
	This test case is setting the delay time to zero on the first set of QoS
*/
	{
public:
    CSetQoSDelayUnset();
	virtual enum TVerdict doTestStepL();
    };

//QoS delay < 0 and count == 0
class CSetQoSNegativeDelay : public CSimPacketQoSTest
/**
	This test case is setting the delay time to zero on the first set of QoS
*/
	{
public:
    CSetQoSNegativeDelay();
	virtual enum TVerdict doTestStepL();
    };

#endif


