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
//

/**
 @file SocketServer.TestSteps.cpp
*/

#include <commdbconnpref.h>
#include "SocketServer.TestSteps.h"


// Create Socket Server
//---------------------

CCreateRSocketServStep::CCreateRSocketServStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KCreateRSocketServStep);
	}

TVerdict CCreateRSocketServStep::doTestStepPreambleL()
	{
	SetTestStepResult(EFail);

	if (iEsockTest==NULL)
	    iEsockTest = new (ELeave) CCEsockTestBase;

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TInt CCreateRSocketServStep::ConfigureFromIni()
	{
	iSockServName.Set(KNullDesC);
	
	// Read in appropriate fields
	if((GetStringFromConfig(iSection, KTe_SocketServName, iSockServName) != 1)
		|| (iSockServName.Length() == 0))
		{
		INFO_PRINTF1(_L("Couldn't find appropriate field in config file"));
		return KErrNotFound;
		}
		
    // All ok if we got this far
    return KErrNone;
	}

TVerdict CCreateRSocketServStep::doSingleTestStep()
	{
    TInt error = iEsockTest->CreateSocketServer(iSockServName);
    if (error!=KErrNone)
        {
        INFO_PRINTF2(_L("Could not create socket server (%S)."),&iSockServName);
        INFO_PRINTF2(_L("Error: %d."),error);
		return EFail;
		}
	return EPass;
	}



// Connect Socket Server
//----------------------

CConnectRSocketServStep::CConnectRSocketServStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KConnectRSocketServStep);
	}

TInt CConnectRSocketServStep::ConfigureFromIni()
	{
    iSockServName.Set(KNullDesC);
	
	// Read in appropriate fields
	if((GetStringFromConfig(iSection, KTe_SocketServName, iSockServName) != 1)
		|| (iSockServName.Length() == 0))
		{
		INFO_PRINTF1(_L("Couldn't find appropriate field in config file"));
		return KErrNotFound;
		}
		
    // All ok if we got this far
    return KErrNone;
	}

TVerdict CConnectRSocketServStep::doSingleTestStep()
	{
    TInt error = iEsockTest->ConnectSocketServer(iSockServName);
    if (error!=KErrNone)
        {
        INFO_PRINTF2(_L("Could not connect socket server (%S)."),&iSockServName);
        INFO_PRINTF2(_L("Error: %d."),error);
        return EFail;
        }
	return EPass;
	}


// Close Socket Server
//--------------------

CCloseRSocketServStep::CCloseRSocketServStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KCloseRSocketServStep);
	}

TInt CCloseRSocketServStep::ConfigureFromIni()
	{
	iSockServName.Set(KNullDesC);

	if((GetStringFromConfig(iSection, KTe_SocketServName, iSockServName) != 1)
		|| (iSockServName.Length() == 0))
		{
		INFO_PRINTF1(_L("Couldn't find appropriate field in config file"));
		return KErrNotFound;
		}
		
    // All ok if we got this far
    return KErrNone;
	}

TVerdict CCloseRSocketServStep::doSingleTestStep()
	{
    TInt error = iEsockTest->CloseSocketServer(iSockServName);
    if (error!=KErrNone)
        {
        INFO_PRINTF2(_L("Could not close socket server (%S)."),&iSockServName);
        INFO_PRINTF2(_L("Error: %d."),error);
        return EFail;
        }
	return EPass;
	}



// Get number of protocols
//------------------------

_LIT(KNumberExpected, "NumberExpected");
_LIT(KSaveNumberAs, "SaveNumberAs");

CSocketServerNumProtocolsStep::CSocketServerNumProtocolsStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KSocketServerNumProtocolsStep);
	}

TInt CSocketServerNumProtocolsStep::ConfigureFromIni()
	{
	iParams.Reset();
	
	// The socket server instance to apply the query to
	if(GetStringFromConfig(iSection, KTe_SocketServName, iParams.iSocketServName) != 1)
		{
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KTe_SocketServName);
		return KErrNotFound;
		}
		
	// Expected number of protocols
	if (GetIntFromConfig(iSection, KNumberExpected, iParams.iNumberExpected) == 1)
        {
		iParams.iExpected = ETrue;
        }

    // Somewhere to save the number fetched
	if(GetStringFromConfig(iSection, KSaveNumberAs, iParams.iSavedAsName) == 1)
		{
		iParams.iToBeSaved = ETrue;
		}
		
    // Check
    if(!iParams.iExpected && !iParams.iToBeSaved)
    	{
		INFO_PRINTF1(_L("Must provide an expected value or a place to save the number fetched."));
		return KErrNotFound;
		}
    
    // All ok if we got this far
    return KErrNone;
	}

TVerdict CSocketServerNumProtocolsStep::doSingleTestStep()
	{
    TInt numProtocols;
    
    TInt error = iEsockTest->SocketServerNumProtocols(iParams, numProtocols);
    if (error != KErrNone)
        {
        SetTestStepError(error);
        INFO_PRINTF3(_L("Could not fetch number of protocols from socket server. socketServer:%S, error:%d"), &iParams.iSocketServName, error);
        return EFail;
        }

	// Validate against the expected number
	if(iParams.iExpected)
		{
		if(numProtocols == iParams.iNumberExpected)
			{
			return EPass;
			}
		else
			{
			INFO_PRINTF3(_L("Number of protocols differs from that expected. expected:%d, returned:%d"), iParams.iNumberExpected, numProtocols);
			return EFail;
			}
		}
	else
		{
		SetTestStepResult(EPass);
		return TestStepResult();
		}
	}



// Find protocol
//--------------

CSocketServerFindProtocolStep::CSocketServerFindProtocolStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KSocketServerFindProtocolStep);
	}

TInt CSocketServerFindProtocolStep::ConfigureFromIni()
	{
	iParams.Reset();
	
	// The socket server instance to apply the query to
	if(GetStringFromConfig(iSection, KTe_SocketServName, iParams.iSocketServName) != 1)
		{
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KTe_SocketServName);
		return KErrNotFound;
		}
		
	TInt descStatus = TProtocolDescriptionParams::FetchDescription(iParams.iProtocolDescParams, *this);
	return descStatus;
	}

TVerdict CSocketServerFindProtocolStep::doSingleTestStep()
	{
    TProtocolDesc desc;

    TInt error = iEsockTest->SocketServerFindProtocol(iParams, desc);
    if (error != KErrNone)
        {
        SetTestStepError(error);
        INFO_PRINTF3(_L("Could not fetch protocol description from socket server. socketServer:%S, error:%d"), &iParams.iSocketServName, error);
        return EFail;
        }

	// Validate against the expected protocol description
	if(iParams.iProtocolDescParams.MatchesExpected(desc))
		{
		return EPass;
		}
	else
		{
        INFO_PRINTF1(_L("Protocol description does not match that expected"));
        return EFail;
		}
	}



// Get protocol info
//------------------

CSocketServerGetProtocolInfoStep::CSocketServerGetProtocolInfoStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KSocketServerGetProtocolInfoStep);
	}

_LIT(KIndex, "Index");

TInt CSocketServerGetProtocolInfoStep::ConfigureFromIni()
	{
	iParams.Reset();
	
	// The socket server instance to apply the query to
	if(GetStringFromConfig(iSection, KTe_SocketServName, iParams.iSocketServName) != 1)
		{
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KTe_SocketServName);
		return KErrNotFound;
		}
		
	// Protocol index
	if (GetIntFromConfig(iSection, KIndex, iParams.iProtocolIndex) != 1)
        {
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, KIndex);
		return KErrNotFound;
        }

	TInt descStatus = TProtocolDescriptionParams::FetchDescription(iParams.iProtocolDescParams, *this);
	return descStatus;
	}

TVerdict CSocketServerGetProtocolInfoStep::doSingleTestStep()
	{
    TProtocolDesc desc;

    TInt error = iEsockTest->SocketServerGetProtocolInfo(iParams, desc);
    if (error != KErrNone)
        {
        SetTestStepError(error);
        INFO_PRINTF3(_L("Could not fetch protocol description from socket server. socketServer:%S, error:%d"), &iParams.iSocketServName, error);
        return EFail;
        }

	// Validate against the expected protocol description
	if(iParams.iProtocolDescParams.MatchesExpected(desc))
		{
		return EPass;
		}
	else
		{
        INFO_PRINTF1(_L("Protocol description does not match that expected"));
        return EFail;
		}
	}

// Start protocol
//---------------
_LIT(KAddrFamily, "AddrFamily");
_LIT(KProtocol, "Protocol");
_LIT(KSockType, "SockType");


CSocketServerStartProtocolStep::CSocketServerStartProtocolStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KSocketServerStartProtocolStep);
	}

TInt CSocketServerStartProtocolStep::ConfigureFromIni()
	{
	iParams.Reset();
	
	// The socket server instance to apply the query to
	if(GetStringFromConfig(iSection, KTe_SocketServName, iParams.iSocketServName) != 1)
		{
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KTe_SocketServName);
		return KErrNotFound;
		}
		
	// Address family
	if (GetIntFromConfig(iSection, KAddrFamily, iParams.iAddrFamily) != 1)
        {
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KAddrFamily);
		return KErrNotFound;
        }

	// Protocol
	if (GetIntFromConfig(iSection, KProtocol, iParams.iProtocol) != 1)
        {
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KProtocol);
		return KErrNotFound;
        }

	// Socket type
	if (GetIntFromConfig(iSection, KSockType, iParams.iSockType) != 1)
        {
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KSockType);
		return KErrNotFound;
        }

    // All ok if we got this far
    return KErrNone;
	}

TVerdict CSocketServerStartProtocolStep::doSingleTestStep()
	{
    TRequestStatus status;
    TInt error = iEsockTest->SocketServerStartProtocol(iParams, status);
    if (error != KErrNone)
        {
        SetTestStepError(error);
		SetTestStepResult(EFail);
        INFO_PRINTF3(_L("Could not start protocol on socket server. socketServer:%S, error:%d"), &iParams.iSocketServName, error);
		return TestStepResult();
        }

	User::WaitForRequest(status);
	if (status.Int() != KErrNone)
		{
        SetTestStepError(error);
		SetTestStepResult(EFail);
        INFO_PRINTF3(_L("Could not start protocol on socket server. socketServer:%S, error:%d"), &iParams.iSocketServName, error);
		}
	
	// Return the test step result
	return TestStepResult();
	}



// Stop protocol
//---------------
CSocketServerStopProtocolStep::CSocketServerStopProtocolStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KSocketServerStopProtocolStep);
	}

TInt CSocketServerStopProtocolStep::ConfigureFromIni()
	{
	iParams.Reset();
	
	// The socket server instance to apply the query to
	if(GetStringFromConfig(iSection, KTe_SocketServName, iParams.iSocketServName) != 1)
		{
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KTe_SocketServName);
		return KErrNotFound;
		}
		
	// Address family
	if (GetIntFromConfig(iSection, KAddrFamily, iParams.iAddrFamily) != 1)
        {
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KAddrFamily);
		return KErrNotFound;
        }

	// Protocol
	if (GetIntFromConfig(iSection, KProtocol, iParams.iProtocol) != 1)
        {
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KProtocol);
		return KErrNotFound;
        }

	// Socket type
	if (GetIntFromConfig(iSection, KSockType, iParams.iSockType) != 1)
        {
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KSockType);
		return KErrNotFound;
        }

    // All ok if we got this far
    return KErrNone;
	}

TVerdict CSocketServerStopProtocolStep::doSingleTestStep()
	{
    TRequestStatus status;
    TInt error = iEsockTest->SocketServerStopProtocol(iParams, status);
    if (error != KErrNone)
        {
        SetTestStepError(error);
		SetTestStepResult(EFail);
        INFO_PRINTF3(_L("Could not stop protocol on socket server. socketServer:%S, error:%d"), &iParams.iSocketServName, error);
		return TestStepResult();
        }

	User::WaitForRequest(status);
	if (status.Int() != KErrNone)
		{
        SetTestStepError(error);
		SetTestStepResult(EFail);
        INFO_PRINTF3(_L("Could not stop protocol on socket server. socketServer:%S, error:%d"), &iParams.iSocketServName, error);
		}
	
	// Return the test step result
	return TestStepResult();
	}



// Compare integer values
//-----------------------
_LIT(KValue1, "Value1");
_LIT(KValue2, "Value2");
_LIT(KDifference, "Difference");

CCompareIntegerValuesStep::CCompareIntegerValuesStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KCompareIntegerValuesStep);
	}

TInt CCompareIntegerValuesStep::ConfigureFromIni()
	{
	iParams.Reset();
	
	// The values to the apply the comparison to
	if(GetStringFromConfig(iSection, KValue1, iParams.iValue1) != 1)
		{
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KValue1);
		return KErrNotFound;
		}
		
	if(GetStringFromConfig(iSection, KValue2, iParams.iValue2) != 1)
		{
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KValue2);
		return KErrNotFound;
		}
		
	// The difference to apply
	if (GetIntFromConfig(iSection, KDifference, iParams.iDifference) != 1)
        {
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KDifference);
		return KErrNotFound;
        }

    // All ok if we got this far
    return KErrNone;
	}

TVerdict CCompareIntegerValuesStep::doSingleTestStep()
	{
    TInt result;
    TInt error = iEsockTest->CompareIntegerValues(iParams, result);

    if (error != KErrNone)
        {
        SetTestStepError(error);
		SetTestStepResult(EFail);
        INFO_PRINTF1(_L("Could not compare compare integer values"));
		return TestStepResult();
        }
	else
		{
		if(result == 0)
			{
	        INFO_PRINTF1(_L("Values match."));
			SetTestStepResult(EPass);
			}
		else
			{
	        INFO_PRINTF1(_L("Values do not match."));
			SetTestStepResult(EFail);
			}
		}
	// Return the test step result
	return TestStepResult();
	}




