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
// This defines the TE_RConnectionStep class which is the base class for all 
// the RConnection multihoming test step classes
// 
//

/**
 @file
*/
 
#if (!defined __TE_RConnectionSTEP_H__)
#define __TE_RConnectionSTEP_H__

#include <e32std.h>
#include <e32base.h>
#include <cdbstore.h>
#include <es_sock.h>
#include <in_sock.h>
#include <es_enum.h>
#include <es_enum_internal.h>
#include <es_enum_partner.h>
#include <commdbconnpref.h>
#include <test/testexecutestepbase.h>
#include <test/testexecuteserverbase.h>
#include <etel.h>
#include "TE_RConnectionSuite.h"

// Test Step Defaults
#define PACKET_SIZE		512
#define	MAX_PACKET_SIZE 2000		
#define NUM_OF_PACKETS	20
#define UDP_TOLERANCE	5
#define	MAX_LOG_LINE_LENGTH		256

const TUint KOneSecondDelay = 1000000;

/* A class used to pass around timeout values (obviously) */
class TTimeoutValues
	{
public:
	TUint32	iShortTimeout;
	TUint32	iMediumTimeout;
	TUint32	iLongTimeout;
	};

// Since stray event is raised when asynchronous events are active
// at the time when ActiveScheduler starts, objects of this class will
// be used to wait for them to complete while the active scheduler is active.
NONSHARABLE_CLASS(CStatus) : public CActive
{
public:
	CStatus();
	~CStatus();

	void Start(){};
	
private:
	void DoCancel(){};
	void RunL(){};
};

/*
class CTestStep_OOMCapable : public CTestStep
	{
public:
	~CTestStep_OOMCapable()
		{
		iOOMSess.Close();
		}
		
	void SetHeapFailNext(TInt aFailureIndex)
		{
		if(iOOMSess.Handle() == KNullHandle)
			{
			iOOMSess.Connect();
			}
		if(iOOMSess.Handle() != KNullHandle)
			{
			iOOMSess.__DbgFailNext(aFailureIndex - 1);	// want to start with 0th
			}
		}
		
	void ResetHeapFailNext()
		{
		if(iOOMSess.Handle() != KNullHandle)
			{
			iOOMSess.__DbgFailNext(-1);
			}
		}
		
	TBool CheckForHeapFailNext()
		{
		if(iOOMSess.Handle() != KNullHandle)
		{
		// If ESOCK heaps are still in FailNext mode then we try to flag the termination
		// of the loop to TEF by setting the local heap to fail. Because this might get
		// "eaten" inside TEF before it checks we walk a failure point here too
		return iOOMSess.__DbgCheckFailNext();
		}
	return ETrue;
	}
	// virtual enum TVerdict InternalDoTestStepL() =0;
private:
	RSocketServ iOOMSess;
	};*/

class TE_RConnectionStep : public CTestStep // _OOMCapable
/**
* TE_RConnectionStep implements all generic connection behaviour needed by all the tests
* @internalComponent
* @test
*/
{
public:
	TE_RConnectionStep();
	virtual ~TE_RConnectionStep();
	
protected:
	virtual enum TVerdict doTestStepPreambleL();
	virtual enum TVerdict doTestStepPostambleL();

	TInt ReadIniFile(void);

	// to all of these methods pass in a pointer to objects you have created on the stack...
	// lots of them will also create temporary automatics but don't worry about that

	// socket server specific stuff
	TInt OpenSocketServer(RSocketServ& ss);
	void CloseSocketServer(RSocketServ& ss);

	// connection specific stuff
	TInt OpenConnection(RConnection& conn, RSocketServ& ss);
	TInt OpenConnection(RConnection& conn, RSocketServ& ss, TName& name);
	TInt StartConnection(RConnection& conn);
	TInt StartConnectionSynchronous(RConnection& conn);
	void StartConnectionAsynchronous(RConnection& conn, TRequestStatus& status);
	TInt StartConnectionWithOverrides(RConnection& conn, TInt iap);
	TInt StartConnectionWithOverridesSynchronous(RConnection& conn, TInt iap);
	void StartConnectionWithOverridesAsynchronous(RConnection& conn, TCommDbConnPref& aPrefs, TInt iap, TRequestStatus& status);
	void CloseConnection(RConnection& conn);
	TInt StopConnection(RConnection& conn);
	TInt EnumerateConnections(RConnection& conn, TUint& num);
	TInt GetTimeoutValues(RConnection& conn, TTimeoutValues& timeouts);
	TInt AttachNormal(RConnection& conn, const TDesC8& info);
	TInt AttachMonitor(RConnection& conn, const TDesC8& info);
	void ProgressNotification(RConnection& conn, TRequestStatus& status, TNifProgressBuf& progress, TUint aSelectedProgress);
	void ProgressNotification(RConnection& conn, TRequestStatus& status, TNifProgressBuf& progress);
	TInt Progress(RConnection& conn, TNifProgress& progress);
	TInt LastProgressError(RConnection& conn, TNifProgress& progress);
	void CancelProgressNotification(RConnection& conn);
	TUint32 GetInterfaceAddress(RSocket& sock,const TDesC& iFName);
	TUint32 GetInterfaceIndexL(RSocket& sock,const TDesC& iFName);
    TInt LoadNapt(RSocket& aNaptsock,RSocket& aTestServSock, RSocketServ& aSockServer);

	// finding out about connections
	// this uses the RConnection::getConnectionInfo method
	TInt GetConnectionInfo(RConnection& conn, TUint index, TDes8& aConnectionInfo);
	// this just reads from comm db
	TInt GetInfo(RConnection& conn, TPckgBuf<TConnectionInfo>& info);

	// socket specific stuff
	TInt OpenUdpSocketL(RSocket& sock, RSocketServ& ss);
	TInt OpenUdpSocketExplicitL(RSocket& sock, RSocketServ& ss, RConnection& conn);
	TInt BindUdpSocket(RSocket& sock, TSockAddr& dest);
	enum {	KSmallBufferLength = 32,	// for when you want that nice small packet
			KBufferLength = 512,		// octets of data transferred per packet (normally)
			KMaxBufferLength = 2048 };	// absolute max size (arbitrarily chosen)
	enum {	KUdpPacketOverhead = 28,
			KTcpPacketOverhead = 52,
			KSmallBufferUdpTestPacketSize = KSmallBufferLength + KUdpPacketOverhead,
			KSmallBufferTcpTestPacketSize = KSmallBufferLength + KTcpPacketOverhead,
			KTotalTcpTestPacketSize = KBufferLength + KTcpPacketOverhead,
			KTotalUdpTestPacketSize = KBufferLength + KUdpPacketOverhead }; // octets transferred per packet
	enum {	KLowThresholdSize = 1024,			// threshold when sending small packets
			KNormalThresholdSize = 32768 };		// threshold when sending normal (large) packets
	TInt SendUdpData(RSocket& aSock, TSockAddr& aDest, TUint aPayloadSize = KBufferLength);
	TInt RecvUdpData(RSocket& aSock, TSockAddr& aDest, TUint aPayloadSize = KBufferLength);
	TInt TestUdpDataPathL(RSocket& sock, TSockAddr& dest, TUint aPayloadSize = KBufferLength); // was timeout 300
	void DestroyUdpSocket(RSocket& sock);

	TInt OpenTcpSocket(RSocket& sock, RSocketServ& ss);
	TInt OpenTcpSocketExplicit(RSocket& sock, RSocketServ& ss, RConnection& conn);
	TInt ConnectTcpSocket(RSocket& sock, TSockAddr& dest);
	TInt ConnectFourTcpSockets(RSocket& sock1, RSocket& sock2, RSocket& sock3, RSocket& sock4, TSockAddr& dest);
	TInt SendTcpData(RSocket& aSock);
	TInt RecvTcpData(RSocket& aSock);
	TInt TestTcpDataPathL(RSocket& sock);
	TInt DestroyTcpSocket(RSocket& sock);

	TInt OpenNullSocket(RSocket& sock, RSocketServ& ss);
	TInt TransferSocket(RSocket& source, RSocket& target, RSocketServ& ss);

	// host resolver specific things
	TInt OpenHostResolver(RHostResolver& hr, RSocketServ& ss);
	TInt OpenHostResolverExplicit(RHostResolver& hr, RSocketServ& ss, RConnection& conn);
	TInt MakeNameRequest(RHostResolver& hr, const TDesC& aName);
	TInt MakeSynchronousNameRequest(RHostResolver& hr, const TDesC& aName);
	TInt MakeAddressRequest(RHostResolver& hr, const TSockAddr& addr);
	TInt MakeSynchronousAddressRequest(RHostResolver& hr, const TSockAddr& addr);
	void DestroyHostResolver(RHostResolver& hr);

	// general helper methods
	enum {	KMaxInterfaceCloseTime = 200 };		// maximum time that WaitForAllInterfacesToCloseL() will wait
	TInt NumberOfInterfacesL(RSocketServ& ss);
	TInt TimeUntilRequestComplete(TRequestStatus& status, TTimeIntervalSeconds& timeElapsed);
	TInt RequestInterfaceDownL(RConnection& conn, RSocketServ& ss);
	TInt RequestSetStausL(RConnection& conn, RSocketServ& ss, TSubConnectionUniqueId aSubConnId);
	TInt WaitForAllInterfacesToCloseL(RSocketServ& ss, TTimeIntervalSeconds aMaxSecsToWait = KMaxInterfaceCloseTime);
	void PerformDummyStartAttachOperationsL(RSocketServ& ss, TPckgBuf<TConnectionInfo>& info);

	// all interface notification helper methods
	void AllInterfaceNotification(RConnection& conn, TRequestStatus& status, TDes8& aNotification);
	void CancelAllInterfaceNotificationL(RConnection& conn);
	void CheckInterfaceNotificationL(TRequestStatus& status, TInterfaceNotificationBuf& info, TConnInterfaceState interfaceState, RConnection* sourceConnection=NULL);
	void CheckConnectionInfoL(RConnection& connection, TConnectionInfo& info);

	// extra enumeration methods
	TInt EnumerateClients(RConnection& aConn, TUint aIndex, TUint& aCount);
	TInt GetConnectionClientInfo(RConnection& aConn, TUint aIndex, TConnectionClientInfo& aClientInfo);

	TInt EnumerateSockets(RConnection& aConn, TUint aIndex, TUint& aCount);
	TInt GetConnectionSocketInfo(RConnection& aConn, TUint aIndex, TConnectionSocketInfo& aSocketInfo);

	// method to get guqos (if used) to recognise the primary context has come up
	TInt ProdQoSL(RConnection& aConn, RSocketServ& aSS);
	
	
	void WaitForRelevantProgress(RConnection& aConn, TNifProgressBuf& aProgress, TInt aRelevantProgress);
	
	/**
	@param aStatus The TRequestStatus you want to wait on
	@param aTimeOut The time in microseconds to wait for the request to complete before returning
	@return ETrue if the request completed, EFalse if the timeout happened first 
	*/
	TBool WaitForRequestOrTimeOutL(TRequestStatus& aStatus, TInt aTimeOut);

protected:
	// this is the config data read from the .ini file in ReadIniFile().
	// IAP numbers used when creating connections with overrides
	TInt iDummyNifIap;
	TInt iDummyNifLongTimeoutIap;
	TInt iDummyNifTwoBinderIap;
	TInt iHungryNifLongTimeoutIap;
	TInt iHungryNifIap;
	TInt iDefaultIap;
	TInt iNtRasIapOnlyShortTimer;
	TInt iBadNtRasIap;
	TInt iMissingNifIap;
	TInt iTestNifIap;
    TInt iSimTSYIap;

	TInt iDummyNifAgentIap;
	// these ones only used for all interface notification tests
	TInt iSecondHungryNifIap;
	TInt iThirdHungryNifIap;
	TInt iSecondDummyNifIap;
	TInt iThirdDummyNifIap;
	TInt iFourthDummyNifIap;
	TInt iFifthDummyNifIap;
	TInt iStaticDnsDynamicAddr;

	// timeout values (for dummy nif)
	TInt iShortTimeout;
	TInt iMediumTimeout;
	TInt iLongTimeout;
	// the address to send packets to externally (and to connect to with TCP sockets)
	TInetAddr iEchoServerAddr;
	// the name to attempt to resolve when using host resolver
	TBuf<64> iTestName;
	// the echo port
	TInt iEchoPortNum;
	// the address to use when trying out GetByAddress()
	TInetAddr iLookupAddress;

	// the address to send UDP packets to when using Dummy Nif (doesn't come from the config file)
	TInetAddr iDummyNifSendAddr;
	// the address to send packets to when using the Hungry Nif (doesn't come from the config file)
	TInetAddr iHungryNifSendAddr;
	CActiveScheduler* iScheduler;
	TInetAddr iNameServerAddr;
	TBuf<20> iInterfaceName;
	
	TPtrC  iNaptStatus;
	//napt public IAP 
	TInt iNaptPublicIap;
	//napt private IAP 
	TInt iNaptPrivateIap;
	//napt test server port
	TInt iNaptTestServerPort;
	//napt TestServer Address
	TInetAddr iNaptTestServAddr;
	
	CStatus* iStatus;
	
};


class TE_RConnectionQoSStep : public TE_RConnectionStep
/**
* A base class for test steps making use of QoS events. See tests 441-456
* @internalComponent
* @test
*/
{
public:
	void Log( TRefByValue<const TDesC16> format, ... );
	
	// see also ProdQoS() in TE_RConnectionStep which is used with QoS but doesn't actually do any QoS specific operations
	class TS_QosChannel
	/**
	* An (non-OO) abstraction of a QoS Channel. The need for this class arouse
	* when comms-infras had to manage the coexistence (8.1x vs. 9.0) of RQoSChannel 
	* and RSubConnection. They both represent the same concept, but are mutually
	* exclusive have different interfaces. TS_QosChannel defines a single API for 
	* them to share (and for test code to call).
	* @internalComponent
	* @test
	*/
	{
		public:
			TS_QosChannel(TE_RConnectionQoSStep& aTestStep);
			virtual ~TS_QosChannel();
			
			void ConstructL();	
			TInt Open(RSocketServ& aServer, RConnection& aConnection);
		TInt Start() {
			return iChannel.Start();
		}
	TInt Stop() {
			return iChannel.Stop();
		}
	TInt Add(RSocket aSocket);
			TInt SetQoSParametersL();
			TInt Close();
		
			RSubConnection 	iChannel;
		private:
			TE_RConnectionQoSStep& iTestStep;
	};

	protected:
		TE_RConnectionQoSStep();
	
};

/* Base class for tests steps wishing to test CSD data call 
 * testing using SIM TSY
 */
class TE_RConnectionSimTsyDataStep : public TE_RConnectionStep
{	
public:
	static TInt NTRasThreadFunction(TAny* aThreadData);
	
public:
	TInt StartNTRasSimulation();
	
protected:
	virtual enum TVerdict doTestStepPreambleL();
	virtual enum TVerdict doTestStepPostambleL();

protected:
	RTelServer iTelServer;
	RPhone iPhone;
};
#endif /* __TE_RConnectionSTEP_H__ */

