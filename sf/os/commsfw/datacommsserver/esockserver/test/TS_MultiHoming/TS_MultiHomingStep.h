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
// This defines the CTS_MultiHomingStep class which is the base class for all 
// the MultiHoming test step classes
// 
//

/**
 @file
*/
 
#if (!defined __TS_MULTIHOMINGSTEP_H__)
#define __TS_MULTIHOMINGSTEP_H__

#include <e32std.h>
#include <networking/teststep.h>
#include <cdbstore.h>
#include <es_sock.h>
#include <in_sock.h>

#include "TS_MultiHomingSuite.h"



#include "CEchoSocket.h"




// Connection Configuration Lits
_LIT(KNameDefault, "default");			
_LIT(KNumOfConns, "NumberOfConns");		
_LIT(KNumOfResolvers, "NumberOfResolvers");	
_LIT(KConnName, "ConnName");			
_LIT(KConnBearer, "ConnBearer");		
_LIT(KConnIap, "ConnIAP");	
_LIT(KConnNet, "ConnNET");
_LIT(KConnType, "ConnType");
_LIT(KExpectSameAddress, "ExpectSameAddress");	


//_LIT(KConnType, "ConnType");	
_LIT(KConnExplicit, "EXPLICIT");	
_LIT(KConnImplicit, "IMPLICIT");
_LIT(KConnDefault,  "DEFAULT");

// Socket Configuration Lits
_LIT(KNumOfSocks, "NumberOfSockets");	
_LIT(KSocket, "Socket");			
_LIT(KSrcAddr, "SrcAddr");			
_LIT(KSrcPort, "SrcPort");		
_LIT(KDestAddr, "DestAddr");			
_LIT(KDestPort, "DestPort");		
_LIT(KProtocol, "Protocol");
_LIT(KTcp, "TCP");
_LIT(KUdp, "UDP");
_LIT(KPacketSize, "PacketSize");	
_LIT(KNumOfPackets, "NumOfPackets");	
_LIT(KUdpTol, "UDPTolerance");
_LIT(KSocketName, "SocketName");
_LIT(KIsListener, "IsListener");
_LIT(KIsSenderOnly, "IsSenderOnly");


// DNS Lits
_LIT(KDestName, "DestName");
_LIT(KResolver, "Resolver");
_LIT(KExpectSuccess, "ExpectSuccess");
_LIT(KExpectTimeout, "ExpectTimeout");
_LIT(KExplicitResolve, "ExplicitResolve");
_LIT(KExpectNoDnsServer, "ExpectNoDnsServer");


// Error / Reporting
_LIT(KEConnNotFound, "Connection not found");	
_LIT(KDoingDefault, "No details found; doing the default (see documentation)");
_LIT(KConnXOpen, "Connection [%S] open");	
_LIT(KConnXInsert, "Connection [%S] added to list");
_LIT(KConnXFail, "Connection [%S] failed to start with error [%S].");
_LIT(KEConfigFile, "There is a problem with the .ini file. Please create/check it.");
_LIT(KELogger, "Error [%S] occurred.");			//< General Error logger
_LIT(KSockRun, "Socket [%i] running.");
_LIT(KSockPassed, "Socket %i passed, sent %i packet loss was %i%%.");
_LIT(KSockFailed, "Socket %i failed, error %S, sent %i packet loss was %i%%.");
_LIT(KConnXClose, "Closing connection [%S].");
_LIT(KConnXStop,  "Stoping connection [%S].");

_LIT(KOomTest, "OOMTest");
_LIT(KConnectionInfo,   "ConnectionInfo");	
_LIT(KRConnectionStart, "RConnectionStart");			
_LIT(KRConnectionClose, "RConnectionClose");			
_LIT(KRConnectionStop,  "RConnectionStop");					
_LIT(KTestConnection,   "TestConnections");	
_LIT(KTestIncomming,	"TestIncomming");
_LIT(KShowConnections,   "ShowConnections");	
_LIT(KExpectedNumberOfIfs, "ExpectedNumOfIfs");
_LIT(KGetConnectionInfo, "GetConnectionInfo");
_LIT(KResolveName,      "ResolveName");			
_LIT(KResolveAddress,   "ResolveAddress");
_LIT(KCopyComDbFile,    "CopyComDbFile");
_LIT(KSourceFileName,   "SourceFileName");
_LIT(KCheckLinkLocalSame,"CheckLinkLocalSame");
_LIT(KDelay,             "Delay");
_LIT(KDelayInMs,         "DelayInMs");

			



class CTS_MultiHomingSuite;

/* CTS_MultiHomingStep class declaration
 * Provides the base class for all multihoming test steps
 *
 */
class CTS_MultiHomingStep : public CTestStep
	{
	// Test Step Defaults - not #defines
public:
	
	enum { ECHO_PORT        = 7 };
	enum { PACKET_SIZE      = 512 };
	enum { MAX_PACKET_SIZE  = 1024 };
	enum { NUM_OF_PACKETS	= 20 };
	enum { UDP_TOLERANCE    = 5 };

public:
	CTS_MultiHomingStep();

	void ExpLogExtra(const TText8* aFile, TInt aLine, TInt aSeverity,
		TRefByValue<const TDesC> aFmt,...);
	virtual ~CTS_MultiHomingStep();
	
	CTS_MultiHomingSuite* iOwnerSuite;	//< Pointer to suite which owns this test step
	void StepLog(TRefByValue<const TDesC16> aText, ...);	//< Log function for CEchoSocket class
private:
	enum TVerdict doTestStepPreambleL()=0; //< Used for OOMTest flag

	
protected:
	TBool iIsOOMTest;
	TInt SetConnPrefs(TCommDbConnPref& aPrefs, TDesC& aName);
	TInt GetConnectionType(TConnDetails *conn, TDesC& aName);
	TInt GetSockConfig(const TInt aIndex, TInt& aProtocol,
						TInt& aPackSize, TInt& aNumPackets,
						TInt& aUdp, TInetAddr& aDstAddr, TInetAddr& aSrcAddr, 
						TBool& aExpectPass, TBool& isListener, TConnDetails **aConnDetails);
	TInt GetResolverConfig(const TInt aIndex, TName &aHostName,TInt& aProtocol,																				 
						TBool& aExpectSuccess, TBool& aExpectTimeout, TBool& aExpectNotReady,
						TBool& aExplicitResolve, TConnDetails **aConnDetails);
						
	};

#endif /* __TS_MULTIHOMINGSTEP_H__ */

