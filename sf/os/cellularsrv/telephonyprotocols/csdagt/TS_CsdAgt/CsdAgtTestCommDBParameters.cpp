// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains CsdAgt TestCase Test 1.1 - 1.8
// 
//

// EPOC includes
#include <e32base.h>
#include <in_sock.h>
#include <dummyetel.h> // include here to avoid picking up the real etel.h
#include <etelmm.h>

// Test system includes
#include "log.h"
#include "teststep.h"
#include "TestStepCsdAgt.h"
#include "TestSuiteCsdAgt.h"

// COMMDB Database Undo
#include "DbUndo.h"

// CsdAgt progress
#include <csdprog.h>

// CsdAgt errors
#include <nd_err.h>

// Class definitions
#include "CsdAgtTestCommDBParameters.h"

// Test case 1.1,  CSD connect with invalid number
enum TVerdict CCsdAgtTest1_1::doCsdAgtTestStepL( void )
	{

    TRealAgtNotify AgtNotify;

	AgtNotify.SetTestCase(10101);
	AgtNotify.SetMode(RTelServer::ENetworkModeGsm);

	CDbCsdTable *csd = CDbCsdTable::NewL();
	CleanupStack::PushL(csd);
	csd->SetColumnTextL(TPtrC(ISP_DEFAULT_TEL_NUM),_L("929292929292929"));

	// Expect dial to fail as telnumber is invalid
	ConnectExpectErrorL(KErrCommsLineFail,ECsdFinishedDialling);

	// Close database object, forcing it to undo all changes that were made
	Log(_L("Reverting COMMDB changes"));
	csd->UndoDatabaseChangesL();
	CleanupStack::PopAndDestroy(csd);

	return iTestStepResult;
	}

// Test case 1.2 CSD connect and break
enum TVerdict CCsdAgtTest1_2::doCsdAgtTestStepL( void )
	{
	TRealAgtNotify AgtNotify;

	AgtNotify.SetTestCase(10102);
	AgtNotify.SetMode(RTelServer::ENetworkModeGsm);

	// Call basic connect / disconnect function from base class
	NormalConnectL();

	return iTestStepResult;
	}

// Test case 1.3 CSD connect with invalid protocol
enum TVerdict CCsdAgtTest1_3::doCsdAgtTestStepL( void )
	{
	TRealAgtNotify AgtNotify;

	AgtNotify.SetTestCase(10103);
	AgtNotify.SetMode(RTelServer::ENetworkModeGsm);

	CDbCsdTable *csd = CDbCsdTable::NewL();
	CleanupStack::PushL(csd);
	csd->SetColumnIntL(TPtrC(ISP_BEARER_PROTOCOL),(TUint32)RMobileCall::EProtocolBitTransparent);

	ConnectExpectErrorL(KErrNotSupported,ECsdFinishedDialling);

	// Close database object, forcing it to undo all changes that were made
	Log(_L("Reverting COMMDB changes"));
	csd->UndoDatabaseChangesL();
	CleanupStack::PopAndDestroy(csd);

	return iTestStepResult;
	}


