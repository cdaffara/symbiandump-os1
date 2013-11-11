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
 @file Sockets.TestSteps.cpp
*/

#include "Sockets.TestSteps.h"
#include "SocketServer.TestSteps.h"
#include "Connections.TestSteps.h"
#include "SubConnections.TestSteps.h"

#include <utf.h>


// Open Socket
//------------

COpenRSocketStep::COpenRSocketStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KOpenRSocketStep);
	}

TInt COpenRSocketStep::ConfigureFromIni()
	{
	iParams.Reset();
	
	// Read in appropriate fields
	if((GetStringFromConfig(iSection, KTe_SocketName, iParams.iSocketName) != 1)
		|| (iParams.iSocketName.Length() == 0))
		{
		INFO_PRINTF1(_L("Couldn't find appropriate field in config file"));
		return KErrNotFound;
		}
		
    if (GetStringFromConfig(iSection,KTe_SocketServName,iParams.iSockServName)!=1)
        {
        INFO_PRINTF2(_L("%S: Socket server name missing."),&iParams.iSocketName);
        return KErrNotFound;
        }

    if ((GetStringFromConfig(iSection,KTe_ConnectionName,iParams.iConnectionName)==1)
    	&& iParams.iConnectionName.Length()>0)
        {
        INFO_PRINTF2(_L("%S: connection name found - opening with RConnection."),&iParams.iSocketName);
        }
    else if ((GetStringFromConfig(iSection,KTe_SubConnectionName,iParams.iSubconnectionName)==1)
    	&& iParams.iSubconnectionName.Length()>0)
        {
        INFO_PRINTF2(_L("%S: subconnection name found - opening with RSubConnection."),&iParams.iSocketName);
        }
    else
        {
        INFO_PRINTF2(_L("%S: connection or subconnection names missing - opening implicit."),&iParams.iSocketName);
        }

	if (GetStringFromConfig(iSection,KTe_ProtocolName,iParams.iProtocolName)==1)
        {
        INFO_PRINTF2(_L("%S: Protocol name found."),&iParams.iProtocolName);
        }
	else
		{
        TInt protocolInt = 0;
        //Try obtaining protocol type as int, failing that, go to predefined types
        if (GetIntFromConfig(iSection, KTe_Protocol, protocolInt)!=1)
            {
            TPtrC protocolStr;
            if (GetStringFromConfig(iSection, KTe_Protocol,protocolStr)!=1 )
                {
                INFO_PRINTF2(_L("%S: Protocol missing."),&iParams.iSocketName);
                return KErrNotFound;        
                }
            if (protocolStr.Compare(KTe_TCPName)==0)
                { protocolInt = KProtocolInetTcp; iParams.iSocketType = KSockStream;}
            else if (protocolStr.Compare(KTe_UDPName)==0)
                { protocolInt = KProtocolInetUdp; iParams.iSocketType = KSockDatagram;}
            else if (protocolStr.Compare(KTe_DummyNetworkProtocolName)==0)
                { protocolInt = KDummyNetworkProtocol; iParams.iSocketType = KSockDatagram;} 
            else if (protocolStr.Compare(KTe_DummyProtocolName)==0)
                { protocolInt = KDummyProtocol; iParams.iSocketType = KSockDatagram;} 
            else
                {
                INFO_PRINTF3(_L("%S: Protocol (%S) not recognised."),&iParams.iSocketName,&protocolStr);
                return KErrNotFound;
                }
            }
        iParams.iProtocol = protocolInt;
        }
    
	TPtrC socketTypeStr;	
    if (GetStringFromConfig(iSection, KTe_SocketType,socketTypeStr)!=1 )
        {
        INFO_PRINTF2(_L("%S: Socket type missing, defaulting based on specified protocol"),&iParams.iSocketName);
        }
    else
        {
        if (socketTypeStr.Compare(KTe_SocketDatagram)==0)
             { iParams.iSocketType = KSockDatagram;}
        else if (socketTypeStr.Compare(KTe_SocketStream)==0)
             { iParams.iSocketType = KSockStream; }
        else
            {
            INFO_PRINTF3(_L("%S: Socket type (%S) not recognised."),&iParams.iSocketName,&socketTypeStr);
            return KErrNotFound;
            }
        }
    
    
    TInt protocolInt = KAfInet;
    if (GetIntFromConfig(iSection, KTe_ConnectionType, protocolInt)!=1 )
        {
        INFO_PRINTF2(_L("%S: Address Family (ConnType) missing, defaulting to KAfInet"),&iParams.iSocketName);
        }
    iParams.iAddrFamily = protocolInt;

    // All ok if we got this far
    return KErrNone;
	}

TVerdict COpenRSocketStep::doSingleTestStep()
	{
    TInt error = iEsockTest->OpenSocket(iParams);
    if (error!=KErrNone)
        {
        INFO_PRINTF2(_L("Could not open socket (%S)."),&iParams.iSocketName);
        INFO_PRINTF2(_L("Error: %d."),error);
		SetTestStepError(error); 
		return EFail;
        }
	return EPass;
	}


// Close Socket
//-------------

CCloseRSocketStep::CCloseRSocketStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KCloseRSocketStep);
	}

TInt CCloseRSocketStep::ConfigureFromIni()
	{
	iSocketName.Set(KNullDesC);

	if((GetStringFromConfig(iSection, KTe_SocketName, iSocketName) != 1)
		|| (iSocketName.Length() == 0))
		{
		INFO_PRINTF1(_L("Couldn't find appropriate field in config file"));
		return KErrNotFound;
		}
		
    // All ok if we got this far
    return KErrNone;
	}

TVerdict CCloseRSocketStep::doSingleTestStep()
	{
    TInt error = iEsockTest->CloseSocket(iSocketName);
    if (error!=KErrNone)
        {
        INFO_PRINTF2(_L("Could not close socket (%S)."),&iSocketName);
        INFO_PRINTF2(_L("Error: %d."),error);
        return EFail;
        }
	return EPass;
	}


// Create Socket
//--------------

CCreateRSocketStep::CCreateRSocketStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KCreateRSocketStep);
	}

TVerdict CCreateRSocketStep::doTestStepPreambleL()
	{
	SetTestStepResult(EFail);

	if (iEsockTest==NULL)
	    iEsockTest = new (ELeave) CCEsockTestBase;

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TInt CCreateRSocketStep::ConfigureFromIni()
	{
	iParams.Reset();
	
	// Read in appropriate fields
	if((GetStringFromConfig(iSection, KTe_SocketName, iParams.iSocketName) != 1)
		|| (iParams.iSocketName.Length() == 0))
		{
		INFO_PRINTF1(_L("Couldn't find appropriate field in config file"));
		return KErrNotFound;
		}
		
    // All ok if we got this far
    return KErrNone;
	}

TVerdict CCreateRSocketStep::doSingleTestStep()
	{
    TInt error = iEsockTest->CreateSocket(iParams.iSocketName);
    if (error!=KErrNone)
        {
        INFO_PRINTF2(_L("Could not create socket (%S)."),&iParams.iSocketName);
        INFO_PRINTF2(_L("Error: %d."),error);
        return EFail;
        }
	return EPass;
    }

// Bind Socket
//---------------

CBindRSocketStep::CBindRSocketStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
    {
    SetTestStepName(KBindRSocketStep);
    }

TInt CBindRSocketStep::ConfigureFromIni()
    {
    // Read in appropriate fields
    if((GetStringFromConfig(iSection, KTe_SocketName, iParams.iSocketName) != 1)
        || (iParams.iSocketName.Length() == 0))
        {
        INFO_PRINTF1(_L("Couldn't find appropriate field in config file"));
        return KErrNotFound;
        }
        
    TInt protocolInt = 0;
    if (GetIntFromConfig(iSection, KTe_Protocol, protocolInt)!=1)
        {
        TPtrC protocolStr;
        if (GetStringFromConfig(iSection, KTe_Protocol,protocolStr)!=1 )
            {
            INFO_PRINTF2(_L("%S: Protocol missing."),&iParams.iSocketName);
            return KErrNotFound;        
            }
        if (protocolStr.Compare(KTe_TCPName)==0)
            { protocolInt = KProtocolInetTcp; }
        else if (protocolStr.Compare(KTe_UDPName)==0)
            { protocolInt = KProtocolInetUdp; }
        else if (protocolStr.Compare(KTe_DummyNetworkProtocolName)==0)
            { protocolInt = KDummyNetworkProtocol; }
        else if (protocolStr.Compare(KTe_DummyProtocolName)==0)
            { protocolInt = KDummyProtocol; }
        else
            {
            INFO_PRINTF3(_L("%S: Protocol (%S) not recognised."),&iParams.iSocketName,&protocolStr);
            return KErrNotFound;
            }
        }
    iParams.iProtocol = protocolInt;
    
    // IP Address Local
    if (GetIpAddressFromConfig(iSection,KTe_SourceAddressName,iParams.iLocalIP)!=1)
        {
        INFO_PRINTF2(_L("%S: Local address missing."),&iParams.iSocketName);
        }    
    
    // All ok if we got this far
    return KErrNone;
    }

TVerdict CBindRSocketStep::doSingleTestStep()
    {
    TInt error = iEsockTest->BindSocket(iParams/*,reqStat*/);
    if (error!=KErrNone)
        {
        INFO_PRINTF2(_L("Could not bind socket (%S)."),&iParams.iSocketName);
        INFO_PRINTF2(_L("Error: %d."),error);
        SetTestStepResult(EFail);
        }
    return TestStepResult();
    }

// Connect Socket
//---------------

CConnectRSocketStep::CConnectRSocketStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KConnectRSocketStep);
	}

TInt CConnectRSocketStep::ConfigureFromIni()
	{
	iParams.Reset();
	
	// Read in appropriate fields
	if((GetStringFromConfig(iSection, KTe_SocketName, iParams.iSocketName) != 1)
		|| (iParams.iSocketName.Length() == 0))
		{
		INFO_PRINTF1(_L("Couldn't find appropriate field in config file"));
		return KErrNotFound;
		}
		
    TPtrC protocol;
	if (GetStringFromConfig(iSection,KTe_Protocol,protocol)!=1)
        {
        INFO_PRINTF2(_L("%S: Protocol missing."),&iParams.iSocketName);
        return KErrNotFound;
        }

    if (protocol.Compare(KTe_TCPName)==0)
	    { iParams.iProtocol = KProtocolInetTcp; }
    else if (protocol.Compare(KTe_UDPName)==0)
	    { iParams.iProtocol = KProtocolInetUdp; }
	else if (protocol.Compare(KTe_DummyNetworkProtocolName)==0)
		{ iParams.iProtocol = KDummyNetworkProtocol; }
    else if (protocol.Compare(KTe_DummyProtocolName)==0)
        { iParams.iProtocol = KDummyProtocol; }    
    else
        {
        INFO_PRINTF3(_L("%S: Protocol (%S) not recognised."),&iParams.iSocketName,&protocol);
        return KErrNotFound;
        }

	// IP Address Local
	if (GetIpAddressFromConfig(iSection,KTe_SourceAddressName,iParams.iLocalIP)!=1)
        {
        INFO_PRINTF2(_L("%S: Local address missing."),&iParams.iSocketName);
        //return KErrNotFound;  // Source Address is not mandatory always.
        }

	// IP Address Remote
	if (GetIpAddressFromConfig(iSection,KTe_DestinationAddressName,iParams.iRemoteIP)!=1)
        {
        INFO_PRINTF2(_L("%S: Remote address missing."),&iParams.iSocketName);
        return KErrNotFound;
        }

	// Port Number Local
    TInt portLocal;
	if (GetIntFromConfig(iSection,KTe_SourcePortName,portLocal)!=1)
        {
        portLocal = 0;
        INFO_PRINTF2(_L("%S: Source port missing. Defaulting to unassigned (port=0)."), &iParams.iSocketName);
        }
    iParams.iLocalIP.SetPort(portLocal);

	// Port Number
	// Port Number Remote
    TInt port;
	if (GetIntFromConfig(iSection,KTe_DestinationPortName,port)!=1)
        {
        INFO_PRINTF2(_L("%S: Remote port missing."),&iParams.iSocketName);
        return KErrNotFound;
        }

	// Attach the port number to remote IP
	iParams.iRemoteIP.SetPort(port);
	
    iCancelFlag = EFalse;
    GetBoolFromConfig(iSection,KTe_ImmediateCancelFlagName,iCancelFlag);

    // All ok if we got this far
    return KErrNone;
	}

TVerdict CConnectRSocketStep::doSingleTestStep()
	{
    TRequestStatus reqStat;
    TInt error = iEsockTest->ConnectSocket(iParams,reqStat);
    if (error!=KErrNone)
        {
        INFO_PRINTF2(_L("Could not connect socket (%S)."),&iParams.iSocketName);
        INFO_PRINTF2(_L("Error: %d."),error);
        SetTestStepResult(EFail);
        }
	else
		{
		if (iCancelFlag)
			{
	        error = iEsockTest->CancelConnectSocket(iParams);
	        if (error!=KErrNone)
		        {
                INFO_PRINTF2(_L("Could not cancel connecting socket (%S)."),&iParams.iSocketName);
                INFO_PRINTF2(_L("Error: %d."),error);
				SetTestStepResult(EFail);
				}
			}
		User::WaitForRequest(reqStat);
	    if (reqStat.Int()!=KErrNone)
		    {
            INFO_PRINTF2(_L("Could not connect socket (%S)."),&iParams.iSocketName);
            INFO_PRINTF2(_L("Error: %d."),error);
	        SetTestStepResult(EFail);
	        }
		}
	return TestStepResult();
	}


// Send Receive on Socket
//-----------------------
_LIT(KPayloadColour, "PayloadColour");

CSendReceiveRSocketStep::CSendReceiveRSocketStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KSendReceiveRSocketStep);
	}

TInt CSendReceiveRSocketStep::ConfigureFromIni()
	{
	iParams.Reset();
	
	// Read in appropriate fields
	if((GetStringFromConfig(iSection, KTe_SocketName, iParams.iSocketName) != 1)
		|| (iParams.iSocketName.Length() == 0))
		{
		INFO_PRINTF1(_L("Couldn't find appropriate field in config file"));
		return KErrNotFound;
		}
		
	if (GetIntFromConfig(iSection,KTe_PacketSizeName,iParams.iPacketSize)!=1)
        {
        INFO_PRINTF2(_L("%S: Packet size missing."),&iParams.iSocketName);
        return KErrNotFound;
        }

    if (GetIntFromConfig(iSection,KTe_NrOfPacketsName,iParams.iNrOfPackets)!=1)
        {
        INFO_PRINTF2(_L("%S: Nr of packets missing."),&iParams.iSocketName);
        return KErrNotFound;
        }

    TPtrC protocol;
	if (GetStringFromConfig(iSection,KTe_Protocol,protocol)!=1)
        {
        INFO_PRINTF2(_L("%S: Protocol missing."),&iParams.iSocketName);
        return KErrNotFound;
        }

    if (protocol.Compare(KTe_TCPName)==0)
	    { iParams.iProtocol = KProtocolInetTcp; }
    else if (protocol.Compare(KTe_UDPName)==0)
	    { iParams.iProtocol = KProtocolInetUdp; }
	else if (protocol.Compare(KTe_DummyNetworkProtocolName)==0)
		{ iParams.iProtocol = KDummyNetworkProtocol; }
    else if (protocol.Compare(KTe_DummyProtocolName)==0)
        { iParams.iProtocol = KDummyProtocol; }    
    else
        {
        INFO_PRINTF3(_L("%S: Protocol (%S) not recognised."),&iParams.iSocketName,&protocol);
        return KErrNotFound;
        }

	// OPTIONAL: colouring data by access point Id
	iParams.iPayloadColouringIsEnabled = EFalse;
	if (GetIntFromConfig(iSection, KPayloadColour, iParams.iPayloadColour) == 1)
		{
		if((iParams.iPayloadColour < 0) || (iParams.iPayloadColour > 255))
			{
			INFO_PRINTF2(_L("Payload colour out of range. Must be (0 <= x <= 255)."), iParams.iPayloadColour);
	        return KErrArgument;
			}
		else if (iParams.iProtocol != KProtocolInetUdp)
			{
			INFO_PRINTF2(_L("Payload colouring only available on UDP sockets."), iParams.iPayloadColour);
	        return KErrArgument;
			}
		else
			{
			// All good
			iParams.iPayloadColouringIsEnabled = ETrue;
			INFO_PRINTF2(_L("Payload colouring enabled. payloadColour:%d"), iParams.iPayloadColour);
			}
		}

    // All ok if we got this far
    return KErrNone;
	}

TVerdict CSendReceiveRSocketStep::doSingleTestStep()
	{
    TRAPD(error,iEsockTest->SendAndReceiveDataL(iParams));
    SetTestStepError(error);

    if (error!=KErrNone)
        {
        INFO_PRINTF2(_L("Could not send/receive on socket (%S)."),&iParams.iSocketName);
        INFO_PRINTF2(_L("Error: %d."),error);
        return EFail;
        }
	return EPass;
	}



// Add Socket to SubConnection
//----------------------------

CAddRSocketToRSubConnectionStep::CAddRSocketToRSubConnectionStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KAddRSocketToRSubConnectionStep);
	}

TInt CAddRSocketToRSubConnectionStep::ConfigureFromIni()
	{
	iParams.Reset();

	// Read in appropriate fields
	if((GetStringFromConfig(iSection, KTe_SocketName, iParams.iSocketName) != 1)
		|| (iParams.iSocketName.Length() == 0))
		{
		INFO_PRINTF1(_L("Couldn't find appropriate field in config file"));
		return KErrNotFound;
		}

    if (GetStringFromConfig(iSection,KTe_SubConnectionName,iParams.iSubConnectionName)!=1
        || iParams.iSubConnectionName.Length()==0)
            {
            INFO_PRINTF2(_L("%S: SubConnection name missing."),&iParams.iSocketName);
            return KErrNotFound;
            }

    // All ok if we got this far
    return KErrNone;
	}

TVerdict CAddRSocketToRSubConnectionStep::doSingleTestStep()
	{
    TRequestStatus reqStat;
    TInt error = iEsockTest->AddSocketToSubConnection(iParams,reqStat);
    if (error!=KErrNone)
        {
        INFO_PRINTF3(_L("Could not add socket (%S) to subconnection (%S)."),&iParams.iSocketName,&iParams.iSubConnectionName);
        INFO_PRINTF2(_L("Error: %d."),error);
        return EFail;
        }

	User::WaitForRequest(reqStat);
	error = reqStat.Int();
	if (error!=KErrNone)
		{
		INFO_PRINTF3(_L("Could not add socket (%S) to subconnection (%S)."),&iParams.iSocketName,&iParams.iSubConnectionName);
        INFO_PRINTF2(_L("Error: %d."),error);
	    return EFail;
		}
	return EPass;
	}


// Add Socket to SubConnection without calling User::WaitForRequest
// Tests PDEF134595
//----------------------------

CAddRSocketToRSubConNoWaitStep::CAddRSocketToRSubConNoWaitStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
    {
    SetTestStepName(KAddRSocketToRSubConNoWaitStep);
    }

TInt CAddRSocketToRSubConNoWaitStep::ConfigureFromIni()
    {
    //clean parameters from previous runs
    iParams.Reset();

    if (GetStringFromConfig(iSection, KTe_SocketName, iParams.iSocketName) != 1
        || iParams.iSocketName.Length()==0)
            return KErrNotFound;
    
    if (GetStringFromConfig(iSection,KTe_SubConnectionName,iParams.iSubConnectionName)!=1
        || iParams.iSubConnectionName.Length()==0)
            {
            INFO_PRINTF2(_L("%S: SubConnection name missing."),&iParams.iSocketName);
            return KErrNotFound;
            }

    return KErrNone;
    }

TVerdict CAddRSocketToRSubConNoWaitStep::doSingleTestStep()
    {
        TRequestStatus reqStat,reqStat2;
        TInt error;
        error = iEsockTest->AddSocketToSubConnection(iParams, reqStat);
        if (error != KErrNone)
            {
            INFO_PRINTF2(_L("Could not add socket (%d)!"), error);
            SetTestStepResult(EFail);
            }
        return TestStepResult();
    }


// Add Socket to SubConnection (OOM)
//----------------------------------

CAddRSocketToRSubConnectionOOMStep::CAddRSocketToRSubConnectionOOMStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KAddRSocketToRSubConnectionOOMStep);
	}

TInt CAddRSocketToRSubConnectionOOMStep::ConfigureFromIni()
	{
	iParams.Reset();

	// Read in appropriate fields
	if((GetStringFromConfig(iSection, KTe_SocketName, iParams.iSocketName) != 1)
		|| (iParams.iSocketName.Length() == 0))
		{
		INFO_PRINTF1(_L("Couldn't find appropriate field in config file"));
		return KErrNotFound;
		}

    if (GetStringFromConfig(iSection, KTe_SubConnectionName, iParams.iSubConnectionName)!=1
        || iParams.iSubConnectionName.Length()==0)
		{
		INFO_PRINTF2(_L("%S: SubConnection name missing."), &iParams.iSocketName);
		return KErrNotFound;
		}
		
    if (GetStringFromConfig(iSection,KTe_SocketServName,iParams.iSockServName)!=1
        || iParams.iSockServName.Length()==0)
        {
        INFO_PRINTF2(_L("%S: Socket server name missing."),&iParams.iSockServName);
        return KErrNotFound;
        }
		

    // All ok if we got this far
    return KErrNone;
	}

TInt CAddRSocketToRSubConnectionOOMStep::AddSocketToSubConnectionOOM()
	{
	TInt errorAddingSocket = KErrNoMemory;
	TInt errorRemovingSocket;
	TBool errorNextWillFail = iEsockTest->__DbgCheckFailNext(iParams.iSockServName);
	TRequestStatus reqStatus;

	for (TInt j = 1; !errorNextWillFail; ++j)
		{		
		INFO_PRINTF3(_L("CCEsockTestBase::__DbgCheckFailNext() returned (%d) in  OOM Loop (%d)..."), errorNextWillFail, j);	
		iEsockTest->__DbgFailNext(iParams.iSockServName, j);

		iEsockTest->AddSocketToSubConnection(iParams, reqStatus);
		User::WaitForRequest(reqStatus);
		errorAddingSocket = reqStatus.Int();
		INFO_PRINTF3(_L("RSubConnection::AddSocket() returned (%d) in OOM loop (%d)..."), errorAddingSocket, j);  
		
		if (errorAddingSocket == KErrNone)
			{
			iEsockTest->RemoveSocketFromSubConnection(iParams, reqStatus);
			User::WaitForRequest(reqStatus);
			errorRemovingSocket = reqStatus.Int();
			INFO_PRINTF3(_L("RSubConnection::RemoveSocket() returned (%d) in OOM loop (%d)..."), errorRemovingSocket, j);  
			
			// Reset the "FailNext" flag in iEsockTest
			iEsockTest->__DbgFailNext(iParams.iSockServName, -1);
			return KErrNone;
			}

		errorNextWillFail = iEsockTest->__DbgCheckFailNext(iParams.iSockServName);
		}

	INFO_PRINTF2(_L("CCEsockTestBase::__DbgCheckFailNext() returned (%d). OOM Loop Finished."), errorNextWillFail);	

	// Reset the "FailNext" flag in iEsockTest
	iEsockTest->__DbgFailNext(iParams.iSockServName, -1);

	return errorAddingSocket;
	}

TVerdict CAddRSocketToRSubConnectionOOMStep::doSingleTestStep()
	{
    TInt error = AddSocketToSubConnectionOOM();
    if (error != KErrNoMemory && error != KErrNone)
        {
        INFO_PRINTF3(_L("Could not add socket (%S) to subconnectiont (%S)."),&iParams.iSocketName,&iParams.iSubConnectionName);
        INFO_PRINTF2(_L("Error: %d."),error);
        return EFail;
        }
	return EPass;
	}


// Remove Socket from SubConnection
//---------------------------------

CRemoveRSocketFromRSubConnectionStep::CRemoveRSocketFromRSubConnectionStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KRemoveRSocketFromRSubConnectionStep);
	}

TInt CRemoveRSocketFromRSubConnectionStep::ConfigureFromIni()
    {
	//clean parameters from previous runs
	iParams.Reset();

    if (GetStringFromConfig(iSection,KTe_SocketName,iParams.iSocketName)!=1
        || iParams.iSocketName.Length()==0)
            return KErrNotFound;

    if (GetStringFromConfig(iSection,KTe_SubConnectionName,iParams.iSubConnectionName)!=1
        || iParams.iSubConnectionName.Length()==0)
            {
            INFO_PRINTF2(_L("%S: SubConnection name missing."),&iParams.iSocketName);
            return KErrNotFound;
            }

    return KErrNone;
    }

TVerdict CRemoveRSocketFromRSubConnectionStep::doSingleTestStep()
	{
    TRequestStatus reqStat;
    TInt error = iEsockTest->RemoveSocketFromSubConnection(iParams,reqStat);
    if (error!=KErrNone)
        {
        INFO_PRINTF3(_L("Could not remove socket (%S) from subconnection (%S)."),&iParams.iSocketName,&iParams.iSubConnectionName);
        INFO_PRINTF2(_L("Error: %d."),error);
        return EFail;
        }

	User::WaitForRequest(reqStat);
	error = reqStat.Int();
	if (error!=KErrNone)
		{
        INFO_PRINTF3(_L("Could not remove socket (%S) from subconnection (%S)."),&iParams.iSocketName,&iParams.iSubConnectionName);
        INFO_PRINTF2(_L("Error: %d."),error);
	    return EFail;
		}
	return EPass;
	}


// Remove Socket from SubConnection (OOM)
//---------------------------------------

CRemoveRSocketFromRSubConnectionOOMStep::CRemoveRSocketFromRSubConnectionOOMStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KRemoveRSocketFromRSubConnectionOOMStep);
	}

TInt CRemoveRSocketFromRSubConnectionOOMStep::ConfigureFromIni()
    {
	//clean parameters from previous runs
	iParams.Reset();

    if (GetStringFromConfig(iSection, KTe_SocketName, iParams.iSocketName)!=1
        || iParams.iSocketName.Length()==0)
            return KErrNotFound;

    if (GetStringFromConfig(iSection,KTe_SubConnectionName,iParams.iSubConnectionName)!=1
        || iParams.iSubConnectionName.Length()==0)
            {
            INFO_PRINTF2(_L("%S: SubConnection name missing."),&iParams.iSocketName);
            return KErrNotFound;
            }

    return KErrNone;
    }


TInt CRemoveRSocketFromRSubConnectionOOMStep::RemoveSocketFromSubConnectionOOM()
	{
	TInt error = KErrNoMemory;
	for (TInt j = 1; error == KErrNoMemory; j++)
		{
		TRequestStatus status;
#ifdef _DEBUG
		iEsockTest->__DbgFailNext(iParams.iSockServName, j);
#endif
		iEsockTest->RemoveSocketFromSubConnection(iParams,status);
		User::WaitForRequest(status);
		error = status.Int();
		INFO_PRINTF2(_L("Error = (%d)!."),error);

		if (error != KErrNoMemory && error != KErrNone)
			{
			INFO_PRINTF2(_L("RSubConnection::RemoveSocket() failed in OOM (%d)!.."),error);
			SetTestStepResult(EFail);
			}
		}
#ifdef _DEBUG
	iEsockTest->__DbgFailNext(iParams.iSockServName, -1);
#endif
	return error;
	}

TVerdict CRemoveRSocketFromRSubConnectionOOMStep::doSingleTestStep()
	{
    TInt error = RemoveSocketFromSubConnectionOOM();
    if (error!=KErrNone)
        {
        INFO_PRINTF2(_L("Could not remove socket from subconnection (%S)."),&iParams.iSubConnectionName);
        INFO_PRINTF2(_L("Error: %d."),error);
        return EFail;
        }
	return EPass;
	}


// Move Connected Socket
//----------------------

CmoveConnectedSocketToSubconnectionStep::CmoveConnectedSocketToSubconnectionStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KmoveConnectedSocketToSubconnectionStep);
	}

TVerdict CmoveConnectedSocketToSubconnectionStep::doTestStepPreambleL()
	{
	SetTestStepResult(EFail);

	if (iEsockTest==NULL)
		{
		iEsockTest = new (ELeave) CCEsockTestBase;
		}
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TInt CmoveConnectedSocketToSubconnectionStep::ConfigureFromIni()
 	{
 	// This test configures itself
 	return KErrNone;
    }

TVerdict CmoveConnectedSocketToSubconnectionStep::doSingleTestStep()
	{
	
	RSocket			socket;
	TInetAddr		dstAddr;
	RSocketServ		socketServer;
	RConnection		conn;
	RSubConnection	subconn;
	TRequestStatus	status;
	TRequestStatus 	eventStatus;
	TNotificationEventBuf	subconnNotifBuf;
	TInt				ret;
	
	CleanupClosePushL(socketServer);
   	CleanupClosePushL(conn);
    	CleanupClosePushL(socket);
    	CleanupClosePushL(subconn);

    	// Connect to ESOCK
    	ret = socketServer.Connect();
    	if (ret != KErrNone)
    		{
    		INFO_PRINTF2(_L("Failed to connect toEsock due to Error: %d."),ret);
    		return EFail;
    		}
  
        	// Open a connection
    	ret = conn.Open(socketServer);
	if (ret != KErrNone)
    		{
    		INFO_PRINTF2(_L("Failed to open connection due to Error: %d."),ret);
    		return EFail;
    		}

	// Start the connection
	conn.Start(status);
	User::WaitForRequest(status);
	
	if (status != KErrNone)
    		{
    		INFO_PRINTF2(_L("Failed to start connection due to Error: %d."),status.Int());
    		return EFail;
    		}


	// Open UDP socket
	ret = socket.Open(socketServer,
			KAfInet,
	                      KSockDatagram,
	                      KProtocolInetUdp,
	                      conn);
	if (ret != KErrNone)
    		{
    		INFO_PRINTF2(_L("Failure to open socket due to Error: %d."),ret);
    		return EFail;
    		}
	
	
	dstAddr.SetPort(3441);
	_LIT(KText50, "127.0.0.1");
	dstAddr.Input( KText50 );

	ret = socket.SetLocalPort(3442);
	if (ret != KErrNone)
    		{
    		INFO_PRINTF2(_L("Failed to set port on socket due to Error: %d."),ret);
    		return EFail;
    		}
    		
	socket.Connect(dstAddr, status);
	User::WaitForRequest(status);

	if (status != KErrNone)
    		{
    		INFO_PRINTF2(_L("Socket failed to connect to dst addr due to Error: %d."),status.Int());
    		return EFail;
    		}

	// Create a new sub-connection
	ret = subconn.Open(socketServer,
	                   RSubConnection::ECreateNew,
	                   conn);
	if (ret != KErrNone)
    		{
    		INFO_PRINTF2(_L("Failed to create new sub-connection due to Error: %d."),ret);
    		return EFail;
    		}
    		
	 // Move the connected socket onto the new sub-connection
	subconn.Add(socket, status);
	User::WaitForRequest(status);

	if (status != KErrNotReady)
    		{
    		INFO_PRINTF2(_L("Socket could not be added to subconn due to  Error: %d."),status.Int());
    		return EFail;
    		}

	 // close and destroy
	CleanupStack::PopAndDestroy(&subconn);
	CleanupStack::PopAndDestroy(&socket);
	CleanupStack::PopAndDestroy(&conn);
	CleanupStack::PopAndDestroy(&socketServer);
	
 	return EPass;
	}


// Shutdown Socket
//----------------

CShutdownRSocketStep::CShutdownRSocketStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KShutdownRSocketStep);
	}

TInt CShutdownRSocketStep::ConfigureFromIni()
	{
	iParams.Reset();

	// Read in appropriate fields
	if((GetStringFromConfig(iSection, KTe_SocketName, iParams.iSocketName) != 1)
		|| (iParams.iSocketName.Length() == 0))
		{
		INFO_PRINTF1(_L("Couldn't find appropriate field in config file"));
		return KErrNotFound;
		}

    TPtrC shutdownType;
    if (GetStringFromConfig(iSection,KTe_ShutdownTypeName,shutdownType)!=1)
        {
        INFO_PRINTF2(_L("%S: Shutdown type missing."),&iParams.iSocketName);
        return KErrNotFound;
        }

    if (shutdownType.Compare(KTe_ShutdownNormal)==0)
	    { iParams.iShutdownType = RSocket::ENormal; }
    else if (shutdownType.Compare(KTe_ShutdownStopInput)==0)
	    { iParams.iShutdownType = RSocket::EStopInput; }
    else if (shutdownType.Compare(KTe_ShutdownStopOutput)==0)
	    { iParams.iShutdownType = RSocket::EStopOutput; }
    else if (shutdownType.Compare(KTe_ShutdownImmediate)==0)
	    { iParams.iShutdownType = RSocket::EImmediate; }
    else
        {
        INFO_PRINTF3(_L("%S: Shutdown type (%S) not recognised."),&iParams.iSocketName,&shutdownType);
        return KErrNotFound;
        }

    // All ok if we got this far
    return KErrNone;
	}

TVerdict CShutdownRSocketStep::doSingleTestStep()
	{
    TRequestStatus reqStat;
	TInt error = iEsockTest->ShutdownSocket(iParams,reqStat);
    if (error!=KErrNone)
        {
        INFO_PRINTF2(_L("Could not connect socket (%S)."),&iParams.iSocketName);
        INFO_PRINTF2(_L("Error: %d."),error);
        return EFail;
        }

	User::WaitForRequest(reqStat);
	error = reqStat.Int();
	if (error!=KErrNone)
		{
        INFO_PRINTF2(_L("Could not connect socket (%S)."),&iParams.iSocketName);
        INFO_PRINTF2(_L("Error: %d."),error);
		return EFail;
		}
	return EPass;
	}


// Ioctls
//-------

CSendReceiveIoctlStep::CSendReceiveIoctlStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KSendReceiveIoctlStep);
	}

_LIT(KIoctlCommand, "IoctlCommand");
_LIT(KIoctlLevel, "IoctlLevel");
_LIT(KExpectedIoctlOutput, "ExpectedOutput");

TInt CSendReceiveIoctlStep::ConfigureFromIni()
	{
	// Reset the parameters and read in necessary fields
	iParams.Reset();

	// Socket to apply it to
	if((GetStringFromConfig(iSection, KTe_SocketName, iParams.iSocketName) != 1)
		|| (iParams.iSocketName.Length() == 0))
		{
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KTe_SocketName);
		return KErrNotFound;
		}

    // Ioctl command
	if (GetIntFromConfig(iSection, KIoctlCommand, iParams.iIoctlCommand) != 1)
        {
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KIoctlCommand);
		return KErrNotFound;
        }

    // Ioctl level
    // Default to 
	if (GetIntFromConfig(iSection, KIoctlLevel, iParams.iIoctlLevel) != 1)
        {
		INFO_PRINTF1(_L("Ioctl level not specified: defaulting"));
		iParams.iIoctlLevel = KLevelUnspecified;
        }

	// Output to compare it with
	if(GetStringFromConfig(iSection, KExpectedIoctlOutput, iParams.iExpectedIoctlOutput) != 1)
		{
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KExpectedIoctlOutput);
		return KErrNotFound;
		}

    // All ok if we got this far
    return KErrNone;
	}

TVerdict CSendReceiveIoctlStep::doSingleTestStep()
	{
    TRequestStatus requestStatus;

	TBuf8<256> outputBuffer;
	TInt error = iEsockTest->IoctlSocket(iParams, outputBuffer, requestStatus);

    if (error != KErrNone)
        {
        INFO_PRINTF2(_L("Could not find socket (%S)."),&iParams.iSocketName);
        INFO_PRINTF2(_L("Error: %d."),error);
        return EFail;
        }

	User::WaitForRequest(requestStatus);

	error = requestStatus.Int();

	if (error!=KErrNone)
		{
        INFO_PRINTF2(_L("Could not complete ioctl (%S)."),&iParams.iSocketName);
        INFO_PRINTF2(_L("Error: %d."),error);
		return EFail;
		}

	// Convert the output to wide chars
	TBuf<256> outputConvertedToWideChars;
	CnvUtfConverter::ConvertToUnicodeFromUtf8(outputConvertedToWideChars, outputBuffer);

	// Validate the output with that expected
	return (outputConvertedToWideChars == iParams.iExpectedIoctlOutput) ?
		EPass :
		EFail;
	}


// Get options
//------------
_LIT(KOptionName, "OptionName");
_LIT(KOptionLevel, "OptionLevel");

CSocketGetOptionStep::CSocketGetOptionStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KSocketGetOptionStep);
	}

_LIT(KExpectedOptionText, "ExpectedOptionText");
_LIT(KExpectedOptionValue, "ExpectedOptionValue");

TInt CSocketGetOptionStep::ConfigureFromIni()
	{
	// Reset the parameters and read in necessary fields
	iParams.Reset();

	// Socket to apply it to
	if((GetStringFromConfig(iSection, KTe_SocketName, iParams.iSocketName) != 1)
		|| (iParams.iSocketName.Length() == 0))
		{
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KTe_SocketName);
		return KErrNotFound;
		}

    // Option "name"
	if (GetIntFromConfig(iSection, KOptionName, iParams.iOptionName) != 1)
        {
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KOptionName);
		return KErrNotFound;
        }

    // Option level
	if (GetIntFromConfig(iSection, KOptionLevel, iParams.iOptionLevel) != 1)
        {
		INFO_PRINTF1(_L("Option level not specified: defaulting to KLevelUnspecified"));
		iParams.iOptionLevel = KLevelUnspecified;
        }

	// Output string to compare it with
	if(GetStringFromConfig(iSection, KExpectedOptionText, iParams.iExpectedOptionOutputText) == 1)
		{
		iParams.iTextExpected = ETrue;
		}

	// Output value to compare it with
	if(GetIntFromConfig(iSection, KExpectedOptionValue, iParams.iExpectedOptionOutputValue) == 1)
		{
		iParams.iValueExpected = ETrue;
		}

    // Checks on config values
    if(iParams.iValueExpected && iParams.iTextExpected)
    	{
		INFO_PRINTF1(_L("Can be only value or text expected. Not both"));
		return KErrCorrupt;
		}
		
    if(!iParams.iValueExpected && !iParams.iTextExpected)
    	{
		INFO_PRINTF1(_L("Must have value or text expected."));
		return KErrCorrupt;
		}
		
    
    // All ok if we got this far
    return KErrNone;
	}

TVerdict CSocketGetOptionStep::doSingleTestStep()
	{
    TRequestStatus requestStatus;

	TBuf8<256> outputBuffer;
	TInt outputValue;
	TInt error;
	if(iParams.iTextExpected)
		{
		error = iEsockTest->GetOptSocket(iParams, outputBuffer);
		}
	else
		{
		error = iEsockTest->GetOptSocket(iParams, outputValue);
		}

    if (error != KErrNone)
        {
        INFO_PRINTF3(_L("Socket get option error. socket:%S, error:%d"), &iParams.iSocketName, error);
        SetTestStepError(error);
        return EFail;
        }

	if(iParams.iTextExpected)
		{
		// Convert the output to wide chars
		TBuf<256> outputConvertedToWideChars;
		CnvUtfConverter::ConvertToUnicodeFromUtf8(outputConvertedToWideChars, outputBuffer);

		// Log what was returned
		INFO_PRINTF2(_L("Socket returned get opt value. value:%S"), &outputConvertedToWideChars);
		INFO_PRINTF2(_L("Expected get opt value.        value:%S"), &iParams.iExpectedOptionOutputText);

		// Validate the output with that expected
		TVerdict verdict = (outputConvertedToWideChars == iParams.iExpectedOptionOutputText) ? EPass : EFail;
		SetTestStepResult(verdict);
		}
	else
		{
		// Log what was returned
		INFO_PRINTF2(_L("Socket returned get opt value. value:%d"), outputValue);
		INFO_PRINTF2(_L("Expected get opt value.        value:%d"), iParams.iExpectedOptionOutputValue);

		// Validate the output with that expected
		TVerdict verdict = (outputValue == iParams.iExpectedOptionOutputValue) ? EPass : EFail;
		SetTestStepResult(verdict);
		}

	// Return the result
	return TestStepResult();
	}

// Set options
//------------

CSocketSetOptionStep::CSocketSetOptionStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
    {
    SetTestStepName(KSocketSetOptionStep);
    }

_LIT(KOptionToSetText, "OptionToSetText");
_LIT(KOptionToSetValue, "OptionToSetValue");

TInt CSocketSetOptionStep::ConfigureFromIni()
    {
    // Reset the parameters and read in necessary fields
    iParams.Reset();

    // Socket to apply it to
    if((GetStringFromConfig(iSection, KTe_SocketName, iParams.iSocketName) != 1)
        || (iParams.iSocketName.Length() == 0))
        {
        INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KTe_SocketName);
        return KErrNotFound;
        }

    // Option "name"
    if (GetIntFromConfig(iSection, KOptionName, iParams.iOptionName) != 1)
        {
        INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KOptionName);
        return KErrNotFound;
        }

    // Option level
    if (GetIntFromConfig(iSection, KOptionLevel, iParams.iOptionLevel) != 1)
        {
        INFO_PRINTF1(_L("Option level not specified: defaulting to KLevelUnspecified"));
        iParams.iOptionLevel = KLevelUnspecified;
        }

    // Input text to be set
    if(GetStringFromConfig(iSection, KOptionToSetText, iParams.iOptionToSetText) == 1)
        {
        iParams.iTextSet = ETrue;
        }

    // Input value to be set
    if(GetIntFromConfig(iSection, KOptionToSetValue, iParams.iOptionToSetValue) == 1)
        {
        iParams.iValueSet = ETrue;
        }
    
    // Checks on config values
    if(iParams.iValueSet && iParams.iTextSet)
           {
           INFO_PRINTF1(_L("Can be only value or text expected. Not both"));
           return KErrCorrupt;
           }
    
    if(!iParams.iValueSet && !iParams.iTextSet)
           {
           INFO_PRINTF1(_L("Must have value or text expected."));
           return KErrCorrupt;
           }

    // All ok if we got this far
    return KErrNone;
    }

TVerdict CSocketSetOptionStep::doSingleTestStep()
    {
    TRequestStatus requestStatus;

    TBuf8<256> inputBuffer;
    inputBuffer.Copy(iParams.iOptionToSetText);       
    TInt inputValue = iParams.iOptionToSetValue;
    TInt error;
    
    if(iParams.iTextSet)
        {
        error = iEsockTest->SetOptSocket(iParams, inputBuffer);
        }
    else
        {
        error = iEsockTest->SetOptSocket(iParams, inputValue);
        }

    if (error != KErrNone)
        {
        INFO_PRINTF3(_L("Socket set option error. socket:%S, error:%d"), &iParams.iSocketName, error);
        SetTestStepError(error);
        return EFail;
        }

    if(iParams.iTextSet)
        {
        // Convert the output to wide chars
        TBuf<256> inputConvertedToWideChars;
        CnvUtfConverter::ConvertToUnicodeFromUtf8(inputConvertedToWideChars, inputBuffer);

        // Log what was returned
        INFO_PRINTF2(_L("Socket set with opt text. value:%S"), &inputConvertedToWideChars);
        INFO_PRINTF2(_L("Text set by user.        value:%S"), &iParams.iOptionToSetText);

        // Validate the output with that expected
        TVerdict verdict = (inputConvertedToWideChars == iParams.iOptionToSetText) ? EPass : EFail;
        SetTestStepResult(verdict);
        }
    else
        {
        // Log what was returned
        INFO_PRINTF2(_L("Socket returned get opt value. value:%d"), inputValue);
        INFO_PRINTF2(_L("Value set by user.        value:%d"), iParams.iOptionToSetValue);

        // Validate the output with that expected
        TVerdict verdict = (inputValue == iParams.iOptionToSetValue) ? EPass : EFail;
        SetTestStepResult(verdict);
        }

    // Return the result
    return TestStepResult();
    }

// Socket protocol description
//----------------------------
CSocketProtocolDescriptionStep::CSocketProtocolDescriptionStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KSocketProtocolDescriptionStep);
	}

TInt CSocketProtocolDescriptionStep::ConfigureFromIni()
	{
	// Reset the parameters and read in necessary fields
	iParams.Reset();

	// Socket to apply it to
	if((GetStringFromConfig(iSection, KTe_SocketName, iParams.iSocketName) != 1)
		|| (iParams.iSocketName.Length() == 0))
		{
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KTe_SocketName);
		return KErrNotFound;
		}

	TInt descStatus = TProtocolDescriptionParams::FetchDescription(iParams.iProtocolDescParams, *this);
	return descStatus;
	}


TVerdict CSocketProtocolDescriptionStep::doSingleTestStep()
	{
	TProtocolDesc desc;
	TInt error = iEsockTest->ProtocolDescription(iParams, desc);

    if (error != KErrNone)
        {
        INFO_PRINTF3(_L("Could not fetch protocol description from socket. socket:%S, error:%d"), &iParams.iSocketName, error);
        return EFail;
        }

	// Validate the output with that expected
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



