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
// This contains CTS_MultiHomingStep class which is
// the base class for all the MultiHoming test steps
// 
//

/**
 @file
*/
#include "TS_MultiHomingStep.h"


// Constructor
CTS_MultiHomingStep::CTS_MultiHomingStep() : iIsOOMTest(EFalse)
	{
	}

// Destructor
CTS_MultiHomingStep::~CTS_MultiHomingStep()
	{ 
	}	

TInt CTS_MultiHomingStep::GetConnectionType(TConnDetails *conn, TDesC& aName)
	{
		TPtrC ptrBuf;
		TInt err = GetStringFromConfig(aName, KConnType, ptrBuf);

		if(err != 1)
		{
			LogExtra((TText8*)__FILE__, __LINE__, ESevrWarn, KEConfigFile);
			return err;
		}


		if(ptrBuf.Compare(KConnDefault) ==0)
			conn->iConnectionType = TConnDetails::defaultConn;
		else if(ptrBuf.Compare(KConnImplicit) ==0)
			conn->iConnectionType = TConnDetails::implicitConn;
		else if(ptrBuf.Compare(KConnExplicit) ==0)
			conn->iConnectionType = TConnDetails::explicitConn;
		else
			{
			_LIT(KTypeErr, "Connection type should be one of %S %S or %S");
			LogExtra((TText8*)__FILE__, __LINE__, ESevrErr, KTypeErr, KConnExplicit, KConnImplicit, KConnDefault );
			return KErrNotFound;
			}
		return KErrNone;			

	}

TInt CTS_MultiHomingStep::SetConnPrefs(TCommDbConnPref& aPrefs, TDesC& aName)
/**
 * Fills up a CStoreableOverrideSettings* with script file configs
 * @param aPrefs A pointer to a CStoreableOverrideSettings object
 * @param aName Name of the connection to get prefs for
 * @return KErrNone if sucessful, otherwise system wide error codes
 * @exception Can leave
 */
	{

	TInt theIap = 0;
	TInt theNet = 0;
	// Get the IAP index and use it to complete the .ini file section name
	if(GetIntFromConfig(aName, KConnIap, theIap))
		{
		aPrefs.SetIapId(theIap);		
		}

	if(GetIntFromConfig(aName, KConnNet, theNet))
		{
		aPrefs.SetNetId(theNet);
		}
	return KErrNone;
	}


TInt CTS_MultiHomingStep::GetSockConfig(const TInt aIndex, TInt& aProtocol,
										TInt& aPackSize, TInt& aNumPackets,
										TInt& aUdp, TInetAddr& aDstAddr, 
										TInetAddr& aSrcAddr, 
										TBool& aExpectSuccess,
										TBool& aIsListener,
										TConnDetails **aConnDetails)
/**
 * Gets socket configuration from file, using defaults if necessary
 * @param aIndex The index for the socket configkey
 * @param aProtocol The protocol to be used
 * @param aPackSize The packet size in bytes
 * @param aNumPackets Number of packets to send
 * @param aUdp Percentage of UDP packets that can be dropped
 * @param aAddr The destination address for the socket
 * @param aSS The socket server sub-session to use for the socket
 * @param aNetName The network connection name in the suite
 * @return System wide error code
 */
	{
	TInt err=KErrNone;
	TInt val=0;
	TBuf<10> socketNum;		// Create the Key for the config lookup
	socketNum= KSocket;
	socketNum.AppendNum(aIndex);
	
	TPtrC ptrBuf;


	

	// Port 0 means we dont care about binding to port

	err = GetStringFromConfig(socketNum, KSrcAddr, ptrBuf);
	if (err)
		{
		// String found in table
		err = aSrcAddr.Input(ptrBuf);
		TESTE(err==KErrNone,err);
		if (err !=KErrNone)
			{
			iTestStepResult= EInconclusive;
			return KErrBadName;
			}
		}
		else 
			aSrcAddr.SetAddress(KInetAddrAny);

	err = GetIntFromConfig(socketNum, KSrcPort, val);
	if (err)
		aSrcAddr.SetPort((TUint) val);
	else
		aSrcAddr.SetPort(0);		// Default to zero - ie source address NOT uses
	

	err = GetStringFromConfig(socketNum, KDestAddr, ptrBuf);
	if (!err)
		{
		LogExtra((TText8*)__FILE__, __LINE__, ESevrWarn, KEConfigFile);
		iTestStepResult= EInconclusive;
		return KErrNotFound;
		}


	err = aDstAddr.Input(ptrBuf);
	TESTE(err==KErrNone,err);
	if (err !=KErrNone)
		{
		iTestStepResult= EInconclusive;
		return KErrBadName;
		}

	err = GetIntFromConfig(socketNum, KDestPort, val);
	if (!err)
		aDstAddr.SetPort(ECHO_PORT);		// Default to echo port
	aDstAddr.SetPort((TUint) val);
	
	err = GetStringFromConfig(socketNum, KProtocol, ptrBuf);
	if (err && (ptrBuf.Compare(KTcp)==0))
		aProtocol = KProtocolInetTcp;
	else
		aProtocol = KProtocolInetUdp;
	
	err = GetIntFromConfig(socketNum, KPacketSize, val);
	if (!err)
		aPackSize=PACKET_SIZE;
	aPackSize=val;
	
	err = GetIntFromConfig(socketNum, KNumOfPackets, val);
	if (!err)
		aNumPackets=NUM_OF_PACKETS;
	aNumPackets=val;
		
	err = GetIntFromConfig(socketNum, KUdpTol, val);
	if (!err)
		aUdp=UDP_TOLERANCE;
	aUdp=val;

	aExpectSuccess = ETrue;
	GetBoolFromConfig(socketNum, KExpectSuccess, aExpectSuccess);
	
	aIsListener = EFalse;
	GetBoolFromConfig(socketNum, KIsListener, aIsListener);

	err = GetStringFromConfig(socketNum, KConnName, ptrBuf);
	if (!err)
		{
		LogExtra((TText8*)__FILE__, __LINE__, ESevrWarn, KEConfigFile);
		iTestStepResult= EInconclusive;
		return KErrNotFound;
		}
		// Use default

	*aConnDetails = iOwnerSuite->GetTConnection(ptrBuf);								

	return KErrNone;
	}

void CTS_MultiHomingStep::StepLog(TRefByValue<const TDesC16> aText, ...)
/**
 * Logging wrapper for CEchoSocket class
 * @param aText Text for the Logging, with any other parameters
 */
	{
	VA_LIST aList;
	VA_START( aList, aText );

	// Create a formated descriptor
	TBuf <MAX_LOG_LINE_LENGTH> LineBuf;
	LineBuf.AppendFormatList( aText, aList);

	// Log to the suite's logger
	iSuite->Log( _L("%S"),&LineBuf );

	VA_END( aList ); 
	}


void CTS_MultiHomingStep::ExpLogExtra(const TText8* aFile, TInt aLine, TInt aSeverity,
		TRefByValue<const TDesC> aFmt,...)
	{
	VA_LIST aList;
	VA_START( aList, aFmt );


	// Log to the suite's logger
	iSuite->LogExtra( aFile, aLine, aSeverity, aFmt, aList );

	VA_END( aList ); 

	}


TInt CTS_MultiHomingStep::GetResolverConfig(const TInt aIndex, TName &aHostName, TInt& aProtocol,																				 
										TBool& aExpectSuccess, TBool& aExpectTimeout, TBool& aExpectNotReady, 
										TBool& aExplicitResolve, TConnDetails **aConnDetails)
/**
 * Gets resolver configuration from file, using defaults if necessary
 * @param aIndex The index for the socket configkey
 * @param aHostName The host to be resolved
 * @param aProtocol The protocol to be used
 * @param aExpectSuccess Flag indicating if name should be resolved ok
 * @param aExpectTimeout Flag indicating if name resolution should timeout
 * @param aConnDetails The connection for an explicit resolver
 * @return System wide error code
 */
	{
	TInt err=KErrNone;	
	TName resolverName;		// Create the Key for the config lookup
	resolverName = KResolver;
	resolverName.AppendNum(aIndex);
	
	TPtrC ptrBuf;
	err = GetStringFromConfig(resolverName, KDestName, ptrBuf);
	if (!err)
		{
		LogExtra((TText8*)__FILE__, __LINE__, ESevrWarn, KEConfigFile);
		iTestStepResult= EInconclusive;
		return KErrNotFound;
		}
	aHostName.Copy(ptrBuf.Ptr(), ptrBuf.Length());
		
	
	aExpectSuccess = ETrue;
	GetBoolFromConfig(resolverName, KExpectSuccess, aExpectSuccess);
	
	aExpectTimeout = EFalse;
	GetBoolFromConfig(resolverName, KExpectTimeout, aExpectTimeout);

	aExpectNotReady = EFalse;
    GetBoolFromConfig(resolverName, KExpectNoDnsServer, aExpectNotReady);

	aExplicitResolve = EFalse;
	GetBoolFromConfig(resolverName, KExplicitResolve, aExplicitResolve);
		

	err = GetStringFromConfig(resolverName, KProtocol, ptrBuf);
	if (err && (ptrBuf.Compare(KTcp)==0))
		aProtocol = KProtocolInetTcp;
	else
		aProtocol = KProtocolInetUdp;	
		
	
	err = GetStringFromConfig(resolverName, KConnName, ptrBuf);
	if (!err)
		{
		return KErrNotFound;
		}		

	*aConnDetails = iOwnerSuite->GetTConnection(ptrBuf);								

	return KErrNone;
	}


