// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TE_C32BASE_H
#define TE_C32BASE_H

#include <testconfigfileparser.h>
#include <test/testexecuteserverbase.h>

#include <e32base.h>
#include <e32test.h>
#include <c32comm.h>
#include <e32hal.h>
#include <cs_port.h>
#include <d32comm.h>
#include "DParams.h"

//
// constants
//
#define NUMBER_OF_PORTS (2) //< number of ports for this machine
const TTimeIntervalMicroSeconds32 KHugeTimeoutValue(1000000); 
const TUint KDefaultHeapSizeThread = 0x4000; //< default heap size for the test Threads
const TCommRole KCommDefaultPreemptRole = ECommRoleDTE; // default role for OpenWhenAvailble testing

//
// literals
//
_LIT8(KC32CompInfo, "C32 server");
_LIT(KCommServerPanic, "!CommServer"); 	// for EKA2 from cc_utl.cpp
_LIT(KCommServerFault, "C32-fault");  	// from cc_utl.cpp
_LIT(KCommServerSuccess, "Kill");  		// Standard value for normal exit
_LIT(KCsyName, "dummy");			   //< name of the CSY to load for the test
_LIT(KCommPort0, "COMM::0");           //< first comm port name
_LIT(KCommPort1, "COMM::1");           //< second comm port name
_LIT(KDummyPortName,"dummy::6");

/**
 * External representation of the state of a CPort object.
 */
struct TCommDebugInfo 
	{
	TInternalCommAccess iMode;  //< The current main access mode of the port
	TInt iAccessCount;          //< Number of open clients
	TCommRole iRole;            //< DCE or DTE
	TInt iOutstandingCommands;  //< Number of outstanding Opens, Closes, Waits etc.
						        //< (excludes SetAccess requests)
	};

//
// Test macros
//
#define TESTCHECKL(a, b) \
    { \
    if((a)!=(b)) \
	    { \
	    INFO_PRINTF3(_L("Failed: Got %d while expecting %d."),a,b); \
        SetTestStepResult(EFail); \
        User::Leave(EFail); \
        } \
    }

#define TESTCHECKCONDITIONL(a) \
    { \
    if (!(a)) \
	    { \
	    INFO_PRINTF1(_L("Failed: Condition was false.")); \
        SetTestStepResult(EFail); \
        User::Leave(EFail); \
        } \
    }
    
#define TESTCHECKSTRL(a, b) \
    { \
    if((a)!=(b)) \
	    { \
	    TBuf<255> temp = a ; \
	    INFO_PRINTF3(_L("Failed: Got \"%S\" while expecting \"%S\"."), &temp,&(b)); \
        SetTestStepResult(EFail); \
        User::Leave(EFail); \
        } \
    }
    
#define TESTCHECK(a, b) \
    { \
    if((a)!=(b)) \
	    { \
	    INFO_PRINTF3(_L("Failed: Got %d while expecting %d."),a,b); \
        SetTestStepResult(EFail); \
        } \
    }

#define TESTCHECKCONDITION(a) \
    { \
    if (!(a)) \
	    { \
	    INFO_PRINTF1(_L("Failed: Condition was false.")); \
        SetTestStepResult(EFail); \
        } \
    }
    
#define TESTCHECKSTR(a, b) \
    { \
    if((a)!=(b)) \
	    { \
	    TBuf<255> temp = a ; \
	    INFO_PRINTF3(_L("Failed: Got \"%S\" while expecting \"%S\"."), &temp,&(b)); \
        SetTestStepResult(EFail); \
        } \
    }
    

class CC32TestStep : public CTestStep
	{
protected:			
	void ShowReceived(const TPtr8& aBuf);	
protected:		
 	virtual TVerdict doTestStepPreambleL( void );
	virtual TVerdict doTestStepPostambleL( void );
protected:
	RCommServ iCommSession; 				 //< the Comm server
    RComm iSerialPortList[NUMBER_OF_PORTS];  //< the Comm ports used here	
	};

#endif

