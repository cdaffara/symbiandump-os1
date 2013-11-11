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
// This contains TE_RConnectionStep class which is
// the base class for all the RConnection multihoming test steps.
// Contains methods which simplify the API to RConnection, RSocket and 
// RHostResolver to be used by derived classes. This abstraction means 
// that the API can continue to evolve as it has done for a while without 
// impacting a large number of test cases - just alter this parent class.
//
//

/**
 @file
*/
#include "TE_RConnectionStep.h"
#include <commdbconnpref.h>
#include <dummynifvar.h>
#include <c32root.h>
#include <comms-infras/esocktestutils.h>
#include <cs_subconparams.h> 
#include <cs_subconevents.h>
#include <ecom/ecom.h>
#include <e32cons.h>
#include <comms-infras/es_commsdataobject.h>
#include <commdbconnpref.h>
#include <commsdattypeinfov1_1.h>
#include <comms-infras/connectionqueryset.h>

#ifdef SYMBIAN_OLD_EXPORT_LOCATION
#include <naptinterface.h>
#else
//after the header file relocation this header won't be exported, so it needs to be a user include.
#include "naptinterface.h"
#endif

#include <simtsy.h>

_LIT(KSimtsyName,"SIM");

TE_RConnectionStep::TE_RConnectionStep()
/**
 * Constructor for TS_RConnectonStep(). Does nothing.
 * @param none
 * @return doesn't it is a constructor
 */
{
}

TE_RConnectionStep::~TE_RConnectionStep()
/**
 * Destructor for TE_RConnectionStep(). Does nothing.
 */
{ 
	CActiveScheduler::Install(NULL);
	delete iScheduler;
}	

enum TVerdict TE_RConnectionStep::doTestStepPreambleL(void)
/**
 * Preamble called before every single test case which is derived from TE_RConnectionStep (unless overridden of course). Sets up addresses and other values to be used by the test code proper. Waits for any remaining interfaces from previous tests to close.
 * @param none
 * @return success (EPass) or failure (EFail) of initialisation
 */
{
	enum TVerdict result = EPass;
	_LIT(KPhbkSyncCMI, "phbsync.cmi");
	if (StartC32WithCMISuppressions(KPhbkSyncCMI) != KErrNone)
    	{
    	return EFail;
    	}
	iScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	/*
	 * Load the config from the .ini file
	 */

	if (KErrNone != ReadIniFile())
		result = EFail;
	
	iDummyNifSendAddr.SetAddress(KDummyNifLocalAddressBase + KDummyHungryNifReservedHostId);
	iDummyNifSendAddr.SetPort(iEchoPortNum);
	iHungryNifSendAddr.SetAddress(KDummyNifLocalAddressBase + KDummyHungryNifReservedHostId);
	iHungryNifSendAddr.SetPort(iEchoPortNum);
		 	 	
	_LIT(KNaptStatusCheck, "On");
	if (iNaptStatus.Compare(KNaptStatusCheck)!=0)
		{
		// Wait for all of the interfaces to close down
		RSocketServ ss;
		
		ss.Connect();
		CleanupClosePushL(ss);

		if (KErrNone != WaitForAllInterfacesToCloseL(ss))
			{
			INFO_PRINTF1(_L("Previous test case has left connections up that have not closed. Stopping them."));
			TBuf<200> errDesc;
			TInt err;
			if ((err = ESockTestUtils::StopAllInterfaces(errDesc)) == KErrNone)
				result = EPass;
			else
				{
				INFO_PRINTF3(_L("StopAllInterfaces() err=%d\n%S"), err, &errDesc);
				result = EFail;
			}
			}
		ss.Close();
		CleanupStack::Pop();
		}

	return result;
}

enum TVerdict TE_RConnectionStep::doTestStepPostambleL(void)
{
	
	if( iStatus && iStatus->IsActive() )
		{
		iStatus->Cancel();
		}
	delete iStatus;
	iStatus = NULL;
	CActiveScheduler::Install(NULL);
	REComSession::FinalClose();
	
	return EPass;
}

TInt TE_RConnectionStep::ReadIniFile(void)
/**
 * Opens the .ini file, reads the contents into some class members and then closes the file again
 * @return system wide error code
 */
{
	TInt result = KErrNone;

	TPtrC filename(_L("c:\\rconnectiontest.ini"));
	TBuf<64> tempBuf; // 16 is enough for an IP addr and the separating dots
	TPtrC temp(0,NULL);


	// this opens the file and loads everything in
	// LoadConfig(filename);

	// set the IAP values in the class members
	_LIT(KIapSection, "IAP");
	_LIT(KDummyNifIapKey, "DummyNifIapNumber");
	_LIT(KNtRasIapOnlyShortTimerKey, "DummyNifOnlyShortTimerIapNumber");
	_LIT(KDummyNifLongTimeoutIapKey, "DummyNifLongTimeoutIapNumber");
	_LIT(KDummyNifTwoBinderIapKey, "DummyNifTwoBinderIapNumber");
	_LIT(KHungryNifLongTimeoutIapKey, "HungryNifLongTimeoutIapNumber");
	_LIT(KHungryNifIapKey, "HungryNifIapNumber");
	_LIT(KDefaultIapKey, "DefaultIapNumber");
	_LIT(KBadNtRasIapKey, "BadNtRasIapNumber");
	_LIT(KMissingNifIapKey, "MissingNifIapNumber");
	_LIT(KTestNifIapKey, "TestNifIapNumber");
	_LIT(KDummyNifAgentIap, "DummyNifAgentIapNumber");
	_LIT(KSimTSYIap, "SimTSYIapNumber");
	_LIT(KStaticDnsDynamicAddr, "StaticDnsDynamicAddr");

	if (!(GetIntFromConfig(KIapSection, KDummyNifIapKey, iDummyNifIap)))
		result = KErrNotFound;
	if (!(GetIntFromConfig(KIapSection, KDummyNifLongTimeoutIapKey, iDummyNifLongTimeoutIap)))
		result = KErrNotFound;
	if (!(GetIntFromConfig(KIapSection, KDummyNifTwoBinderIapKey, iDummyNifTwoBinderIap)))
		result = KErrNotFound;
	if (!(GetIntFromConfig(KIapSection, KHungryNifLongTimeoutIapKey, iHungryNifLongTimeoutIap)))
		result = KErrNotFound;
	if (!(GetIntFromConfig(KIapSection, KHungryNifIapKey, iHungryNifIap)))
		result = KErrNotFound;
	if (!(GetIntFromConfig(KIapSection, KDefaultIapKey, iDefaultIap)))
		result = KErrNotFound;
	if (!(GetIntFromConfig(KIapSection, KNtRasIapOnlyShortTimerKey, iNtRasIapOnlyShortTimer)))
		result = KErrNotFound;
	if (!(GetIntFromConfig(KIapSection, KBadNtRasIapKey, iBadNtRasIap)))
		result = KErrNotFound;
	if (!(GetIntFromConfig(KIapSection, KMissingNifIapKey, iMissingNifIap)))
		result = KErrNotFound;
	if (!(GetIntFromConfig(KIapSection, KTestNifIapKey, iTestNifIap)))
		result = KErrNotFound;
	if (!(GetIntFromConfig(KIapSection, KDummyNifAgentIap, iDummyNifAgentIap)))
		result = KErrNotFound;
	if (!(GetIntFromConfig(KIapSection, KSimTSYIap, iSimTSYIap)))
		result = KErrNotFound;
	if (!(GetIntFromConfig(KIapSection, KStaticDnsDynamicAddr, iStaticDnsDynamicAddr)))
		result = KErrNotFound;


	// IAP numbers specifically used by the all interface notification tests
	_LIT(KAinIapSection, "AINIAPs");
	_LIT(KSecondHungryNifIapKey, "SecondHungryNifNumber");
	_LIT(KThirdHungryNifIapKey, "ThirdHungryNifNumber");
	_LIT(KSecondDummyNifIapKey, "SecondDummyNifNumber");
	_LIT(KThirdDummyNifIapKey, "ThirdDummyNifNumber");
	_LIT(KFourthDummyNifIapKey, "FourthDummyNifNumber");
	_LIT(KFifthDummyNifIapKey, "FifthDummyNifNumber");
	_LIT(KFNaptPublicIapKey, "NaptPublicIapNumber");
	_LIT(KFNaptPrivateIapKey, "NaptPrivateIapNumber");
	

	if (!(GetIntFromConfig(KAinIapSection, KSecondHungryNifIapKey, iSecondHungryNifIap)))
		result = KErrNotFound;
	if (!(GetIntFromConfig(KAinIapSection, KThirdHungryNifIapKey, iThirdHungryNifIap)))
		result = KErrNotFound;
	if (!(GetIntFromConfig(KAinIapSection, KSecondDummyNifIapKey, iSecondDummyNifIap)))
		result = KErrNotFound;
	if (!(GetIntFromConfig(KAinIapSection, KThirdDummyNifIapKey, iThirdDummyNifIap)))
		result = KErrNotFound;
	if (!(GetIntFromConfig(KAinIapSection, KFourthDummyNifIapKey, iFourthDummyNifIap)))
		result = KErrNotFound;
	if (!(GetIntFromConfig(KAinIapSection, KFifthDummyNifIapKey, iFifthDummyNifIap)))
		result = KErrNotFound;
	if (!(GetIntFromConfig(KIapSection, KFNaptPublicIapKey, iNaptPublicIap)))
		result = KErrNotFound;
	if (!(GetIntFromConfig(KIapSection, KFNaptPrivateIapKey, iNaptPrivateIap)))
		result = KErrNotFound;

	// set the timeout values in the class members
	_LIT(KTimeoutSection, "Timeouts");
	_LIT(KShortTimeoutKey, "ShortTimeout");
	_LIT(KMediumTimeoutKey, "MediumTimeout");
	_LIT(KLongTimeoutKey, "LongTimeout");
	
	if (!(GetIntFromConfig(KTimeoutSection, KShortTimeoutKey, iShortTimeout)))
		result = KErrNotFound;
	if (!(GetIntFromConfig(KTimeoutSection, KMediumTimeoutKey, iMediumTimeout)))
		result = KErrNotFound;
	if (!(GetIntFromConfig(KTimeoutSection, KLongTimeoutKey, iLongTimeout)))
		result = KErrNotFound;
	
	// other general set up stuff
	_LIT(KGeneralSection, "General");
	_LIT(KInterfaceName, "Interface");
	if (!(GetStringFromConfig(KGeneralSection, KInterfaceName, temp)))
		result = KErrNotFound;
	iInterfaceName.Copy(temp);
	
	_LIT(KNameServerAddr, "NameServerAddr");
	if (!GetStringFromConfig(KGeneralSection, KNameServerAddr, temp))
		result = KErrNotFound;
	iNameServerAddr.Input(temp);

	_LIT(KEchoPortKey, "EchoPort");
	if (!(GetIntFromConfig(KGeneralSection, KEchoPortKey, iEchoPortNum)))
		result = KErrNotFound;
	_LIT(KNaptTestServerPort, "NaptTestServerPort");
	if (!(GetIntFromConfig(KGeneralSection, KNaptTestServerPort, iNaptTestServerPort)))
		result = KErrNotFound;

	// set the address which we should connect TCP sockets to (addr of TCP echo server)
	_LIT(KTcpConfigSection, "TCP Config");
	_LIT(KTcpSendAddrKey, "TcpSendAddress");
	if (!GetStringFromConfig(KTcpConfigSection, KTcpSendAddrKey, temp))
		result = KErrNotFound;
	iEchoServerAddr.Input(temp);
	// set it's port to the echo port
	iEchoServerAddr.SetPort(iEchoPortNum);
	
	// Napt
	_LIT(KNaptTestServer, "NaptTestServer");
	if (!GetStringFromConfig(KGeneralSection, KNaptTestServer, temp))
		result = KErrNotFound;
	iNaptTestServAddr.Input(temp);
	// set it's port to the NaptTestServer port
	iNaptTestServAddr.SetPort(iNaptTestServerPort);

	// the name which we will attempt to resolve many many times
	_LIT(KTestNameKey, "TestName");
	if (!GetStringFromConfig(KTcpConfigSection, KTestNameKey, temp))
		result = KErrNotFound;
	iTestName = temp;

	// the address which we will pass in when using GetByAddress()
	_LIT(KLookupAddressKey, "LookupAddress");
	if (!GetStringFromConfig(KGeneralSection, KLookupAddressKey, temp))
		result = KErrNotFound;
	iLookupAddress.Input(temp);
	
	_LIT(KNaptStatus, "NaptStatus");
	if (!GetStringFromConfig(KGeneralSection, KNaptStatus, iNaptStatus))
		result = KErrNotFound;

	return result;
}

TInt TE_RConnectionStep::OpenSocketServer(RSocketServ& ss)
/**
 * Opens the socket server that is pointed to
 * @param ss the socket server to open
 * @return system wide error code
 */
    {
    INFO_PRINTF1(_L("Opening Socket server"));
	return (ss.Connect());
    }

void TE_RConnectionStep::CloseSocketServer(RSocketServ& ss)
/**
 * Closes the socket server that is pointed to
 * @param ss the socket server to close
 * @return system wide error code
 */
{
	INFO_PRINTF1(_L("Closing Socket server"));
	ss.Close(); // Close() inherited from RHandleBase?
}

TInt TE_RConnectionStep::OpenConnection(RConnection& conn, RSocketServ& ss)
/**
 * Open the connection using the socket server too
 * @param conn the connection to open
 * @param ss the socket server within which the connection is to be opened
 * @return system wide error code
 */
{
	INFO_PRINTF1(_L("Opening Connection"));
	return (conn.Open(ss));
}

TInt TE_RConnectionStep::OpenConnection(RConnection& conn, RSocketServ& ss, TName& name)
/**
 * Open the connection (passing the name provided)
 * @param conn - the connection to open
 * @param ss - the socket server within which the connection is to be opened
 * @return system wide error code
 */
{
	INFO_PRINTF1(_L("Opening Connection"));
	return (conn.Open(ss, name));
}

TInt TE_RConnectionStep::StartConnection(RConnection& conn)
/**
 * Start a connection using the default comm db settings
 * @param conn the connection to start
 * @return system wide error code
 */
{
	TRequestStatus status;
	conn.Start(status);
	INFO_PRINTF1(_L("Starting Connection"));
	User::WaitForRequest(status);
	return status.Int();
}

TInt TE_RConnectionStep::StartConnectionSynchronous(RConnection& conn)
/**
 * Start a connection using the default comm db settings (using the synchronous call)
 * @param conn - the connection to start
 * @return system wide error code
 */
{
	INFO_PRINTF1(_L("Starting Connection"));
	return (conn.Start());
}

void TE_RConnectionStep::StartConnectionAsynchronous(RConnection& conn, TRequestStatus& status)
/**
 * Start a connection using the default comm db settings (returning before it completes)
 * @param conn - the connection to start
 * @param system wide error code
 */
{
	INFO_PRINTF1(_L("Starting Connection"));
	conn.Start(status);
}

TInt TE_RConnectionStep::StartConnectionWithOverrides(RConnection& conn, TInt iap)
/**
 * Start a connection on the IAP as specified by the param, do this by 
 * building some database overrides.
 * @param conn - the connection to start
 * @param iap - which IAP to create the connection over
 * @return system wide error code
 */
{
	TRequestStatus status;
	
	TCommDbConnPref prefs;
	prefs.SetIapId(iap);
	prefs.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
	conn.Start(prefs, status);
	INFO_PRINTF1(_L("Starting Connection with over rides"));
	User::WaitForRequest(status);

	return status.Int();
}

TInt TE_RConnectionStep::StartConnectionWithOverridesSynchronous(RConnection& conn, TInt iap)
/**
 * Start a connection on the IAP as specified by the param, do this by building some 
 * database overrides. Use the synchronous start method
 * @param conn - the connection to start
 * @param iap - which IAP to create the connection over
 * @return system wide error code
 */
{
	TCommDbConnPref prefs;
	prefs.SetIapId(iap);
	INFO_PRINTF1(_L("Starting Connection with over rides"));
	prefs.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
	return (conn.Start(prefs));
}

void TE_RConnectionStep::StartConnectionWithOverridesAsynchronous(RConnection& conn, TCommDbConnPref& aPrefs, TInt iap, TRequestStatus& status)
/**
 * Start a connection on the IAP as specified by the param - return before it completes
 * @param conn - the connection to start
 * @param iap - which IAP to create the connection over
 * @return system wide error code
 */
{
	aPrefs.SetIapId(iap);
	aPrefs.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
	INFO_PRINTF1(_L("Starting Connection with over rides"));
	conn.Start(aPrefs, status);
}



void TE_RConnectionStep::CloseConnection(RConnection& conn)
/**
 * Close a connection
 * @param conn the connection to close
 * @return system wide error code
 */
{
	INFO_PRINTF1(_L("Closing Connection "));
	conn.Close();
}


TInt TE_RConnectionStep::StopConnection(RConnection& conn)
/**
 * Force the connection to stop and remove the interface no matter what other connections are using it
 * @param conn the connection to stop
 * @return system wide error code
 */
{
	INFO_PRINTF1(_L("Stopping Connection "));
	return (conn.Stop());
}

TInt TE_RConnectionStep::EnumerateConnections(RConnection& conn, TUint& num)
/**
 * Read how many connections (==interfaces?) exist at the moment
 * @param conn - to be used to read the count
 * @param num - on completion holds the number of connections
 * @return system wide error code
 */
{
	return (conn.EnumerateConnections(num));
}

TInt TE_RConnectionStep::GetTimeoutValues(RConnection& conn, TTimeoutValues &timeouts)
/**
 * Read the timeout values from commdb
 * @param conn - the connection to read the values from
 * @param timeouts - where to put the timeout vals
 * @return system wide error code
 */
{
	INFO_PRINTF1(_L("Getting Timeout Values"));
	TInt ret = KErrNone;
	TInt tmp;
	if (KErrNone != (tmp = conn.GetIntSetting(_L("ModemBearer\\LastSessionClosedTimeout"), timeouts.iShortTimeout)))
		ret = tmp;
	if (KErrNone != (tmp = conn.GetIntSetting(_L("ModemBearer\\LastSocketClosedTimeout"), timeouts.iMediumTimeout)))
		ret = tmp;
	if (KErrNone != (tmp = conn.GetIntSetting(_L("ModemBearer\\LastSocketActivityTimeout"), timeouts.iLongTimeout)))
		ret = tmp;

	return ret;
}

TInt TE_RConnectionStep::AttachNormal(RConnection& conn, const TDesC8& info)
/**
 * Attach the connection supplied to the one described by the info
 * @param conn - the connection to be attached to something
 * @param info - the details of the connection to attach to
 * @return system wide error code
 */
{
	INFO_PRINTF1(_L("Connection Attach Normal"));
	return conn.Attach(info, RConnection::EAttachTypeNormal);
}

TInt TE_RConnectionStep::AttachMonitor(RConnection& conn, const TDesC8& info)
/**
 * Attach the connection supplied to the one described by the info (as a monitor)
 * @param conn - the connection to be attached to something
 * @param info - the details of the connection to attach to
 * @return system wide error code
 */
{
	INFO_PRINTF1(_L("Connection Attach Monitor"));
	return conn.Attach(info, RConnection::EAttachTypeMonitor);
}

void TE_RConnectionStep::ProgressNotification(RConnection& conn, TRequestStatus& status, 
											  TNifProgressBuf& progress, TUint aSelectedProgress)
/**
 * Request notification when the progress changed to that supplied
 * @param conn - the connection we are interested in the progress of
 * @param status - the status variable which should be updated eventually
 * @param aSelectedProgress - the progress we want
 * @return system wide error code
 */
{
	INFO_PRINTF1(_L("Connection Progress Notification "));
	conn.ProgressNotification(progress, status, aSelectedProgress);
}

void TE_RConnectionStep::ProgressNotification(RConnection& conn, TRequestStatus& status, 
											  TNifProgressBuf& progress)
/**
 * Request notification of the next progress change
 * @param conn - the connection whose status we are interested in
 * @param status - the status variable that should be updated
 * @return system wide error code
 */
{
	INFO_PRINTF1(_L("Connection Progress Notification "));
	conn.ProgressNotification(progress, status);
}

void TE_RConnectionStep::CancelProgressNotification(RConnection& conn)
/**
 * Cancel the notification of progress changes
 * @param conn - the connection to cancel progress notification for
 * @return system wide error code
 */
{
	INFO_PRINTF1(_L("Cancel Connection Progress Notification "));
	conn.CancelProgressNotification();
}

TInt TE_RConnectionStep::Progress(RConnection& conn, TNifProgress& progress)
/**
 * Read the current progress state
 * @param conn - the connection to read the progress of
 * @param progress - where to put the read progress
 * @return system wide error code
 */
{
	INFO_PRINTF1(_L("Connection Progress"));
	return conn.Progress(progress);
}

TInt TE_RConnectionStep::LastProgressError(RConnection& conn, TNifProgress& progress)
/**
 * Get the last progress error
 * @param conn - the connection we are interested in
 * @param progress - where to put the progress value (the error)
 * @return system wide error code
 */
{
	INFO_PRINTF1(_L("Connection Last Progress Error"));
	return conn.LastProgressError(progress);
}

TInt TE_RConnectionStep::GetConnectionInfo(RConnection& conn, TUint index, TDes8& aConnectionInfo)
/**
 * Read back the information about a particular connection (using the enumeration functionality)
 * @param conn - the connection to use to access the info
 * @param index - the index of the connection we want to find out about
 * @param aConnectionInfo - where to write the info into
 * @return system wide error code
 */
{
	INFO_PRINTF1(_L("Get Connection info "));
	return conn.GetConnectionInfo(index, aConnectionInfo);
}

TInt TE_RConnectionStep::GetInfo(RConnection& conn, TPckgBuf<TConnectionInfo>& info)
/**
 * Read the info about this connection from comm db
 * @param conn - the connection to read info about
 * @param info - where to write the info to
 * @return system wide error code
 */
{
	_LIT(KIapTableIdDes, "IAP\\Id" );
	_LIT(KIapTableIapNetworkDes, "IAP\\IAPNetwork" );

	TInt ret = KErrNone;
	ret = conn.GetIntSetting(KIapTableIdDes, info().iIapId);

	const CommsDat::TMDBElementId KIapTableIdFieldId = KCDTIdIAPRecord | KCDTIdRecordTag;
	const CommsDat::TMDBElementId KTapTableIapNetworkId = KCDTIdIAPNetwork;

	if (ret == KErrNone)
		{
		ret = conn.GetIntSetting(KIapTableIapNetworkDes, info().iNetId);
		}

	ESock::XConnectionQuerySet* querySet = NULL;
	if ( ret == KErrNone )
		{
		TRAP(ret, querySet = ESock::XConnectionQuerySet::NewL() );
		}

	if ( ret == KErrNone )
		{
		TRAP(ret, ESock::XUintQuery::NewL(KIapTableIdFieldId, *querySet) );
		}

	if ( ret == KErrNone )
		{
		TRAP(ret, ESock::XUintQuery::NewL(KTapTableIapNetworkId, *querySet) );
		}

	ESock::CConnectionQuerySet* connectionQuerySet = NULL;
	if ( ret == KErrNone )
		{
		TRAP(ret, connectionQuerySet = ESock::CConnectionQuerySet::NewL(querySet) );
		}

	if ( ret == KErrNone )
		{
		querySet = NULL;
		}

	if ( ret == KErrNone )
		{
		ret = conn.GetParameters( *connectionQuerySet );
		}

	if ( ret == KErrNone )
		{
		TBool match = EFalse;
		ESock::XConnectionQuerySet& outputQuerySet = connectionQuerySet->DataObject();
		ESock::XUintQuery* iapTableIdQuery = static_cast<ESock::XUintQuery*>(outputQuerySet.FindQuery( KIapTableIdFieldId ));
		ESock::XUintQuery* iapTableNetworkQuery = static_cast<ESock::XUintQuery*>(outputQuerySet.FindQuery( KTapTableIapNetworkId ));

		match = ( iapTableIdQuery
			&& iapTableNetworkQuery
			&& iapTableIdQuery->Error() == KErrNone
			&& iapTableNetworkQuery->Error() == KErrNone
			&& iapTableIdQuery->IsQueryAnswered()
			&& iapTableNetworkQuery->IsQueryAnswered()
			&& iapTableIdQuery->Data() == info().iIapId
			&& iapTableNetworkQuery->Data() == info().iNetId
			);

		if ( !match )
			{
			ret = KErrGeneral;
			}
		}

	delete querySet;
	delete connectionQuerySet;

	REComSession::FinalClose();
	return ret;
}

TInt TE_RConnectionStep::OpenUdpSocketL(RSocket& sock, RSocketServ& ss)
/**
 * Open a UDP socket within the supplied socket server. No connection is 
 * specified so let the system implicitly decide which to use
 * @param sock - the socket to open
 * @param ss - the socket server
 * @return system wide error code
 */
{
	TInt err = KErrNone;
	TInt ret = KErrNone;
	
	INFO_PRINTF1(_L("Opening Implicit UDP Socket"));

	err = sock.Open(ss, KAfInet, KSockDatagram, KProtocolInetUdp);
	TESTEL(KErrNone == ret, ret);
	if (err != KErrNone)
		ret = err;
	
	err = sock.SetOpt(KSoReuseAddr, KSolInetIp, 1);
	TESTEL(err == KErrNone, err);
	if (err != KErrNone)
		ret = err;

	err = sock.SetLocalPort(iEchoPortNum); 
	TESTEL(err == KErrNone, err);
	if (err != KErrNone)
		ret = err;

	return ret;
}

TInt TE_RConnectionStep::OpenUdpSocketExplicitL(RSocket& sock, RSocketServ& ss, RConnection& conn)
/**
 * Open a UDP socket with the supplied socket server. Associate it 
 * with the specified connection.
 * @param sock - the socket to open
 * @param ss - the socket server
 * @param conn - the connection to associate it with
 * @return system wide error code
 */
{
	TInt err;
	TInt ret = KErrNone;
	INFO_PRINTF1(_L("Opening Explicit UDP Socket"));

	err = sock.Open(ss, KAfInet, KSockDatagram, KProtocolInetUdp, conn);
	TESTEL(KErrNone == err, err);
	if (err != KErrNone)
		ret = err;

	err = sock.SetOpt(KSoReuseAddr, KSolInetIp, 1); 
	TESTEL(err == KErrNone, err);
	if (err != KErrNone)
		ret = err;

	err = sock.SetLocalPort(iEchoPortNum); 
	TESTEL(err == KErrNone, err);
	if (err != KErrNone)
		ret = err;
	
	return ret;
}

void TE_RConnectionStep::DestroyUdpSocket(RSocket& sock)
/**
 * Destroys the supplied socket
 * @param sock - the socket to destroy
 * @return system wide error code
 */
{
	INFO_PRINTF1(_L("Closing Socket"));
	sock.Close();
}

TInt TE_RConnectionStep::OpenTcpSocket(RSocket& sock, RSocketServ& ss)
/**
 * Open a TCP socket within the supplied socket server. No connection is 
 * specified so let the system implicitly decide which to use
 * @param sock - the socket to open
 * @param ss - the socket server
 * @return system wide error code
 */
{
	INFO_PRINTF1(_L("Opening Implicit TCP Socket"));
	return (sock.Open(ss, KAfInet, KSockStream, KProtocolInetTcp));
}

TInt TE_RConnectionStep::OpenTcpSocketExplicit(RSocket& sock, RSocketServ& ss, RConnection& conn)
/**
 * Open a TCP socket within the supplied socket server. Associate it with 
 * the specified connection.
 * @param sock - the socket to open
 * @param ss - the socket server
 * @param conn - the connection to associate it with
 * @return system wide error code
 */
{
	INFO_PRINTF1(_L("Opening Explicit TCP Socket"));
	return (sock.Open(ss, KAfInet, KSockStream, KProtocolInetTcp, conn));
}

TInt TE_RConnectionStep::ConnectTcpSocket(RSocket& sock, TSockAddr& addr)
/**
 * Connect the supplied TCP socket to the destination socket
 * @param sock - the socket to connect up
 * @param addr - the address of the destination to connect up to
 */
{
	TRequestStatus status;
	INFO_PRINTF1(_L("Connecting TCP Socket"));
	sock.Connect(addr, status);
	User::WaitForRequest(status);
	return status.Int();
}

TInt TE_RConnectionStep::ConnectFourTcpSockets(RSocket& sock1, RSocket& sock2, RSocket& sock3, RSocket& sock4, TSockAddr& dest)
/**
 * Connect the supplied TCP sockets to the destination socket
 * @param sock1 - the first of the sockets to connect up
 * @param sock2 - the second of the sockets to connect up
 * @param sock3 - the third of the sockets to connect up
 * @param sock4 - the fourth of the sockets to connect up
 * @param dest - the address of the destination to connect up to
 */
{
	TRequestStatus status[4];
	INFO_PRINTF1(_L("Connecting TCP Socket"));
	sock1.Connect(dest, status[0]);
	INFO_PRINTF1(_L("Connecting TCP Socket"));
	sock2.Connect(dest, status[1]);
	INFO_PRINTF1(_L("Connecting TCP Socket"));
	sock3.Connect(dest, status[2]);
	INFO_PRINTF1(_L("Connecting TCP Socket"));
	sock4.Connect(dest, status[3]);

	// To avoid stray events we need (a) four signals and (b) all four sockets to have completed - as always it doesn't matter if the
	// four signals aren't actually from these sockets, eg if lower priority things are completing
	TInt signalsOutstanding = 4;
	TBool allConnected;
	do
		{
		User::WaitForAnyRequest();
		--signalsOutstanding;
		allConnected = (status[0].Int() != KRequestPending) && (status[1].Int() != KRequestPending) &&
							(status[2].Int() != KRequestPending) && (status[3].Int() != KRequestPending);
		} while(signalsOutstanding > 0 || !allConnected);

	// If other requests complete while we're waiting for ours then have to re-add their signals
	while(signalsOutstanding < 0)
		{
		TRequestStatus status;
		TRequestStatus* dummyStatus = &status;
		User::RequestComplete(dummyStatus, KErrNone);
		++signalsOutstanding;
		}

	if (status[0].Int() != KErrNone)
		return status[0].Int();
	if (status[1].Int() != KErrNone)
		return status[1].Int();
	if (status[2].Int() != KErrNone)
		return status[2].Int();
	if (status[3].Int() != KErrNone)
		return status[3].Int();

	return KErrNone;
}

TInt TE_RConnectionStep::DestroyTcpSocket(RSocket& sock)
/**
 * Destroys the supplied socket
 * @param sock - the socket to destroy
 * @return system wide error code
 */
{
	TRequestStatus status;
	INFO_PRINTF1(_L("Shutting down TCP Socket"));
	sock.Shutdown(RSocket::EImmediate, status);
	User::WaitForRequest(status);
	sock.Close();

	return status.Int();
}

TInt TE_RConnectionStep::OpenNullSocket(RSocket& sock, RSocketServ& ss)
/**
 * Open a socket but not associated to any protocol
 * @param sock - the socket to open
 * @param ss - the socket server to use
 * @return system wide error code
 */
{
	INFO_PRINTF1(_L("Open Null Socket"));
	return (sock.Open(ss));
}

TInt TE_RConnectionStep::TransferSocket(RSocket& source, RSocket& target, RSocketServ& ss)
/**
 * Transfer a socket to another socket
 * @param source - the socket to transfer from
 * @param target - the socket to transfer to
 * @param ss - the socket server the original socket is in
 * @return system wide error code
 */
{
	//-- note that RSocket::Transfer() operation since 8.1b shall be additionally enabled by 
	//-- setting KSOEnableTransfer socket option with appropriate capabilities set for the 
	//-- process that "pulls" the socket. It shall be done before calling this function.
	TName name;

	source.Name(name);

	return target.Transfer(ss, name);
}


TInt TE_RConnectionStep::OpenHostResolver(RHostResolver& hr, RSocketServ& ss)
/**
 * Open a host resolver within the supplied socket server. No connection is 
 * specified sp let the system decide which to use
 * @param hr - the host resolver to open
 * @param ss - the socket server to use
 * @return system wide error code
 */
{
	return (hr.Open(ss, KAfInet, KProtocolInetTcp));
}

TInt TE_RConnectionStep::OpenHostResolverExplicit(RHostResolver& hr, RSocketServ& ss, RConnection& conn)
/**
 * Open a host resolver within the supplied socket server. Associate it with 
 * the specified connection
 * @param hr - the host resolver to open
 * @param ss - the socket server to use
 * @param conn - the connection to associate it with
 * @return system wide error code
 */
{
	return (hr.Open(ss, KAfInet, KProtocolInetUdp, conn));
}

TInt TE_RConnectionStep::MakeNameRequest(RHostResolver& hr, const TDesC& aName)
/**
 * Make a name request
 * @param hr - the host resolver to make the request with
 * @param aName - the name to resolve
 * @return system wide error code
 */
{	
	TRequestStatus status;
	TNameEntry result;
	INFO_PRINTF1(_L("Performing Asynchronous GetByName()"));
	hr.GetByName(aName, result, status);
	User::WaitForRequest(status);

	return status.Int();
}

TInt TE_RConnectionStep::MakeSynchronousNameRequest(RHostResolver& hr, const TDesC& aName)
/**
 * Make a name request using the synchronous method
 * @param hr - the host resolver to make the request with
 * @param aName - the name to resolve
 * @return system wide error code
 */
{
	TNameEntry result;
	INFO_PRINTF1(_L("Performing Synchronous GetByName()"));
	return (hr.GetByName(aName, result));
}

TInt TE_RConnectionStep::MakeAddressRequest(RHostResolver& hr, const TSockAddr& addr)
/**
 * Make a request FOR a name using GetByAddress()
 * @param hr - the host resolver to use
 * @param addr - the address to pass in
 * @return system wide error code
 */
{
	TRequestStatus status;
	TNameEntry name;
	INFO_PRINTF1(_L("Performing Asynchronous GetByAddress()"));
	hr.GetByAddress(addr, name, status);
	User::WaitForRequest(status);

	return status.Int();
}

TInt TE_RConnectionStep::MakeSynchronousAddressRequest(RHostResolver& hr, const TSockAddr& addr)
/**
 * Make a request FOR a name using GetByAddress() [use the synchronous version]
 * @param hr - the host resolver to use
 * @param addr - the address to pass in
 * @return system wide error code
 */
{
	TNameEntry name;
	INFO_PRINTF1(_L("Performing Synchronous GetByAddress()"));
	return (hr.GetByAddress(addr, name));
}

void TE_RConnectionStep::DestroyHostResolver(RHostResolver& hr)
/**
 * Destroy the supplied host resolver
 * @param hr - the host resolver to destroy
 * @return system wide error code
 */
{
	hr.Close();
}

TInt TE_RConnectionStep::SendUdpData(RSocket& aSock, TSockAddr& aDest, TUint aPayloadSize)
/**
 * Send UDP data (an ICMP packet) over the socket specified to the address specified.
 * @param aSock The socket to use for the send
 * @param aDest The destination to send the data to
 * @param aPayloadSize The size of the packet to send
 * @return system wide error code
 */
{
	TBuf8<KMaxBufferLength> buffer;
	TRequestStatus status;

	// construct an ICMP packet to send on the socket
	buffer.SetMax();
	buffer.FillZ();
	buffer[0] = (TUint8) 0x08; // ICMP type = 8
	buffer[1] = (TUint8) 0x00; // ICMP code = 0
	buffer[2] = (TUint8) 0xF7; // ICMP checksum high byte
	buffer[3] = (TUint8) 0xFF; // ICMP checksum low byte
	buffer[13] = (TUint8) 0xFE;
	// NB the rest of the buffer is zero
	// hence the checksum (0xFFFF - 0x800) since 0x8
	// is the only non-zero element of the buffer

	// set the length of the data to be sent to that specified (payload size!)
	buffer.SetLength(aPayloadSize);

	// send the data out over the socket
	aSock.SendTo(buffer, aDest, 0, status);
	User::WaitForRequest(status);

	return status.Int();
}

TInt TE_RConnectionStep::RecvUdpData(RSocket& aSock, TSockAddr& aDest, TUint aPayloadSize)
/**
 * Receive UDP packet from the network
 * @param aSock The socket to receive the packet on
 * @param aDest The address to receive the packet from
 * @param aPayloadSize The size of the packet to receive
 * @return system wide error code
 */
{
	TBuf8<KMaxBufferLength> buffer;
	TRequestStatus status;
	TInt timeoutInSecs = 10;
	RTimer timer;
	TRequestStatus timerStatus;

	// Create a timer so that we dont rely on being errored by esock alone
	TInt ret;
	if ((ret = timer.CreateLocal()) != KErrNone)
	{
		INFO_PRINTF2(_L("RTimer::CreateLocal() returned %d"), ret);
		timer.Close(); // from looking at impl shouldn't do any harm to call Close() here
		return ret;
	}

	timer.After(timerStatus, timeoutInSecs * 1000000);

	buffer.Zero();
	buffer.SetLength(aPayloadSize);

	aSock.RecvFrom(buffer, aDest, 0, status);
	User::WaitForRequest(status, timerStatus);

	// Timer expired, cancel the read
	if(timerStatus != KRequestPending)
		{
		aSock.CancelAll();
		User::WaitForRequest(status);
		timer.Close();
		return KErrTimedOut;
		}
	// otherwise cancel the timer
	timer.Cancel();
	User::WaitForRequest(timerStatus);
	timer.Close();

	// don't use TESTEL here as the test case may be expecting us to fail and that's ok...
	if (status != KErrNone)
		return status.Int();
	else
	{   // check that what we sent is what we got back
		// if the receive times out and we access buffer we get a panic
		TEST(buffer[0] == 0x08);
		TEST(buffer[1] == 0x00);
		TEST(buffer[2] == 0xF7);
		TEST(buffer[3] == 0xFF);
	}

	return KErrNone;
}

TInt TE_RConnectionStep::TestUdpDataPathL(RSocket& aSock, TSockAddr& aDest, TUint aPayloadSize)
/**
 * Sends data to the address specified (dest) and expects to receive it looped back unchanged
 * @param aSock The socket to send the data over (and receive it over)
 * @param aDest The destination to send the traffic to
 * @param aPayloadSize The amount of data to sent (defaults to KBufferLength)
 * @return system wide error code
 */
{
	TInt err;
	INFO_PRINTF1(_L("Sending UDP Data"));
	err = SendUdpData(aSock, aDest, aPayloadSize);
	if (err != KErrNone)
		return err;

	INFO_PRINTF1(_L("Receiving UDP Data"));
	err = RecvUdpData(aSock, aDest, aPayloadSize);
	// don't use TESTEL here as the test case may be expecting us to fail and that's ok...
	return err;
}

TInt TE_RConnectionStep::SendTcpData(RSocket& aSock)
/**
 * Send TCP data over the socket specified to the address specified.
 * @param aSock The socket to use for the send
 * @param aPayloadSize The size of the packet to send
 * @return system wide error code
 */
{
	TRequestStatus status;
	TBuf8<KBufferLength> buffer;

	// build some data to send
	buffer.SetMax();
	buffer.FillZ();
	buffer[0] = (TUint8) 0x08; // ICMP type = 8;
	buffer[1] = (TUint8) 0x00; // ICMP code = 0;
	buffer[2] = (TUint8) 0xF7; // ICMP checksum high byte
	buffer[3] = (TUint8) 0xFF; // ICMP checksum low byte

	// send the data
	aSock.Send(buffer, 0, status);
	User::WaitForRequest(status);

	return status.Int();
}

TInt TE_RConnectionStep::RecvTcpData(RSocket& aSock)
/**
 * Receive TCP packet from the network
 * @param aSock The socket to receive the packet on
 * @param aPayloadSize The size of the packet to receive
 * @return system wide error code
 */
{
	TRequestStatus status;
	TBuf8<KBufferLength> buffer;
	buffer.Zero();

	// hopefully get the same data back that was sent
	aSock.Recv(buffer, 0, status);
	User::WaitForRequest(status);

	if (status != KErrNone)
		return status.Int();
	else
	{	// check that what we sent is what we got back
		// if the receive times out and we access the buffer we get a panic
		TEST(buffer[0] == 0x08);
		TEST(buffer[1] == 0x00);
		TEST(buffer[2] == 0xF7);
		TEST(buffer[3] == 0xFF);
	}

	return KErrNone;
}

TInt TE_RConnectionStep::TestTcpDataPathL(RSocket& sock)
/**
 * Sends data and expects to receive it looped back unchanged
 * @param sock The socket to send the data over (and receive it over)
 * @param aPayloadSize The amount of data to sent (defaults to KBufferLength)
 * @return system wide error code
 * @note there is no address parameter, the socket must already be connected
 */
{
	TInt err;

	err = SendTcpData(sock);
	if (err != KErrNone)
		return err;

	err = RecvTcpData(sock);

	return err;
}

TInt TE_RConnectionStep::NumberOfInterfacesL(RSocketServ& ss)
/**
 * Return a count of interfaces in this socket server.
 * Can be used even if this socket server has no open connections (because it 
 * temporarily opens one!).
 * @param ss - the interesting socket server to count in
 * @return number of interfaces, or it might leave
 */
{
	TInt err;
	RConnection conn;
	TUint numOfConnections;

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = EnumerateConnections(conn, numOfConnections);
	TESTEL(KErrNone == err, err);

	CloseConnection(conn);
	CleanupStack::Pop();

	return numOfConnections;
}

TInt TE_RConnectionStep::TimeUntilRequestComplete(TRequestStatus& status, TTimeIntervalSeconds& timeElapsed)
/**
 * Wait for the status to change. Count how long it takes.
 * @param status - the status which we need to wait for it to change
 * @param timeElapsed - the time it took (in seconds)
 * @return system wide error code
 */
{
	TTime time1, time2;

	time1.HomeTime();

	User::WaitForRequest(status);
		
	time2.HomeTime();
	time2.SecondsFrom(time1, timeElapsed);

	return (status.Int());
}

TInt TE_RConnectionStep::RequestInterfaceDownL(RConnection& conn, RSocketServ& ss)
/**
 * Sends a special packet to the dummy nif telling it to signal that the interface is down
 * @param conn The connection to signal
 * @param ss The socket server that the connection is within
 * @return system wide error code
 */
{
	TInt err = KErrNone;
	TRequestStatus status;

	TBuf8<KBufferLength> buffer;

	RSocket sock;
	TInetAddr dest;

	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	//err = sock.SetLocalPort(KDummyNifCmdPort);
	//TESTEL(KErrNone == err, err);

	dest.SetAddress(KDummyNifLocalAddressBase + 4);
	dest.SetPort(KDummyNifCmdPort);

	buffer.SetMax();
	buffer.FillZ();
	buffer[0] = (TUint8) KForceDisconnect;

	sock.SendTo(buffer, dest, 0, status);
	User::WaitForRequest(status);
	TESTEL(KErrNone == status.Int(), status.Int());

	buffer.Zero();
	// I expect some sort of response
	// this fails - possibly ok as the interface might have gone already??
	//sock.RecvFrom(buffer, dest, 0, status);
	//User::WaitForRequest(status);
	//TESTEL(KErrNone == status.Int(), status.Int());

	DestroyUdpSocket(sock);
	CleanupStack::Pop();

	return err;
}

TInt TE_RConnectionStep::RequestSetStausL(RConnection& conn, RSocketServ& ss, TSubConnectionUniqueId aSubConnId)
/**
 * Sends a special packet to the dummy nif allowing the setting of the connection progress
 * @param conn The connection to signal
 * @param ss The socket server that the connection is within
 * @return system wide error code
 */
{
	TInt err = KErrNone;
	TRequestStatus status;

	TBuf8<KBufferLength> buffer;

	RSocket sock;
	TInetAddr dest;

	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	dest.SetAddress(KDummyNifLocalAddressBase + 4);
	dest.SetPort(KDummyNifCmdPort);

	buffer.SetMax();
	buffer.FillZ();
	buffer[0] = (TUint8) KForceFinishedSelection;
	buffer[1] = (TUint8) aSubConnId;
	
	sock.SendTo(buffer, dest, 0, status);
	User::WaitForRequest(status);
	TESTEL(KErrNone == status.Int(), status.Int());

	buffer.Zero();

	DestroyUdpSocket(sock);
	CleanupStack::Pop();

	return err;
}


TInt TE_RConnectionStep::WaitForAllInterfacesToCloseL(RSocketServ& ss, TTimeIntervalSeconds aMaxSecsToWait)
/*
 * Sit around waiting for all the interfaces to die (essentially poll every now 
 * and again until there are zero interfaces in this socket server left)
 * @param ss Socket Server session to use
 * @param aMaxSecsToWait maximum number of seconds to wait for the interface to come down.
 * @return system wide error code
 */
	{
	TInt err;
	TUint numOfConnections;
	TInt count = 0;
	const TInt KOneSecond = 1000000;

	RConnection conn;

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = EnumerateConnections(conn, numOfConnections);
	TESTEL(KErrNone == err, err);

	while ((0 != numOfConnections) && (count < aMaxSecsToWait.Int()))
	{
		count++;
		User::After(KOneSecond); // wait a bit, a second sounds good
		err = EnumerateConnections(conn, numOfConnections);
	}

	CloseConnection(conn);
	CleanupStack::Pop();

	if (numOfConnections != 0)
		{
		return KErrTimedOut;
		}

	return KErrNone;
	}

void TE_RConnectionStep::PerformDummyStartAttachOperationsL(RSocketServ& ss, TPckgBuf<TConnectionInfo>& info)
/*
 * Do some attached and starts. One monitor, one normal and one actual interface.
 * Close in two different orders.
 * @param ss - the socket server to do this in
 * @param info - some info
 * @return system wide error code
 */
{
	TInt err;

	RConnection conn1, conn2, conn3;

	TInt count;

	for (count=0; count<2; count++)
	{
		// attach one as a monitor
		err = OpenConnection(conn1, ss);
		TESTEL(KErrNone == err, err);
		CleanupClosePushL(conn1);
		err = AttachMonitor(conn1, info);
		TESTEL(KErrNone == err, err);

		// attach one normally
		err = OpenConnection(conn2, ss);
		TESTEL(KErrNone == err, err);
		CleanupClosePushL(conn2);
		err = AttachNormal(conn2, info);
		TESTEL(KErrNone == err, err);

		// create an actual interface (maybe?)
		err = OpenConnection(conn3, ss);
		TESTEL(KErrNone == err, err);
		CleanupClosePushL(conn3);
		err = StartConnectionWithOverrides(conn3, iDummyNifIap);
		TESTEL(KErrNone == err, err);

		if (count == 0)
		{
			// On first time thru close the interfaces in reverse order
			CloseConnection(conn3);
			CleanupStack::Pop();
			CloseConnection(conn2);
			CleanupStack::Pop();
			CloseConnection(conn1);
			CleanupStack::Pop();
		}
		else
		{
			// Second time thru close in a different order (fwd order)
			CloseConnection(conn1);
			CleanupStack::Pop(3);
			CleanupClosePushL(conn3); // sneaky.... put them back on in the 
			CleanupClosePushL(conn2); // order that is easier for us below...
			CloseConnection(conn2);
			CleanupStack::Pop();
			CloseConnection(conn3);
			CleanupStack::Pop();
		}
	}
}

void TE_RConnectionStep::AllInterfaceNotification(RConnection& conn, TRequestStatus& status, TDes8& aNotification)
/**
 * Requests all interface notification
 * @param conn The connection to use to request notification
 * @param status Asynchronous status of the request
 * @param aNotification The descriptor which the notification is to be written into
 * @return nothing
 */
	{
	conn.AllInterfaceNotification(aNotification, status);
	}

void TE_RConnectionStep::CancelAllInterfaceNotificationL(RConnection& conn)
/**
 * Cancels outstanding request for all interface notification
 * @param conn The connection on which to cancel the request
 * @return nothing
 */
	{
	conn.CancelAllInterfaceNotification();
	}

void TE_RConnectionStep::CheckInterfaceNotificationL(TRequestStatus& status, TInterfaceNotificationBuf& info, TConnInterfaceState interfaceState, RConnection* sourceConnection)
/**
 * Checks for particular connection interface state
 * @param status Asynchronous status of request
 * @param info Buffer which info will be written into
 * @param interfaceState Also will be updated and then monitored
 * @param sourceConnection Connection which all this relates to
 * @return nothing
 */
	// checks for particular connection interface state
	{
	// wait for completion - to ensure that monitoring actually occurs
	User::WaitForRequest(status);

	// check request completed OK
	TESTEL(status.Int() == KErrNone, status.Int());

	// check connection state is as expected
	TESTEL(info().iState == interfaceState, info().iState);

	if(NULL != sourceConnection)
		{
		CheckConnectionInfoL(*sourceConnection,info().iConnectionInfo);
		}
	}

void TE_RConnectionStep::CheckConnectionInfoL(RConnection& connection, TConnectionInfo& info)
/**
 * Checks connection info with the expected
 * @param connection The connection all this relates to
 * @param info The info to compare with the actual settings for connection
 * @return nothing
 */
	{
	TInt err;
	TUint32 connIapId;
	TUint32 connNetId;

	// get settings for the connection
	err = connection.GetIntSetting(_L("IAP\\Id"), connIapId);
	TESTEL(err == KErrNone, err);
	err = connection.GetIntSetting(_L("IAP\\IAPNetwork"), connNetId);
	TESTEL(err == KErrNone, err);

	// check IAP Id matches
	TESTEL(info.iIapId == connIapId, info.iIapId);
	// check Network Id matches
	TESTEL(info.iNetId == connNetId, info.iNetId);
	}

TInt TE_RConnectionStep::EnumerateClients(RConnection& aConn, TUint aIndex, TUint& aCount)
/*
 * Count how many clients there are
 * @param aConn The connection to use to enumerate the clients
 * @param aIndex The index to use
 * @param aCount The count of how many clients there are
 * @return system wide error code
 */
	{

	TInt err(KErrNone);

	TConnEnumArgBuf args;
	args().iIndex = aIndex;
	err = aConn.Control(KCOLConnection, KCoEnumerateConnectionClients, args);
	aCount = args().iCount;
	
	return err;
	}

TInt TE_RConnectionStep::GetConnectionClientInfo(RConnection& aConn, TUint aIndex, TConnectionClientInfo& aClientInfo)
/**
 * Get the information about a client
 * @param aConn The connection to use
 * @param aIndex The index to use
 * @param aClientInfo Where to write the information to
 * @return system wide error code
 */
	{

	TInt err(KErrNone);

	TConnGetClientInfoArgBuf args;
	args().iIndex = aIndex;
	err = aConn.Control(KCOLConnection, KCoGetConnectionClientInfo, args);
	aClientInfo = args().iClientInfo;

	return err;
	}

TInt TE_RConnectionStep::EnumerateSockets(RConnection& aConn, TUint aIndex, TUint& aCount)
/**
 * Count how many sockets there are
 * @param aConn The connection to use
 * @param aIndex The index to use
 * @param aCount The count of how many sockets there are
 * @return system wide error code
 */
	{

	TInt err(KErrNone);

	TConnEnumArgBuf args;
	args().iIndex = aIndex;
	err = aConn.Control(KCOLConnection, KCoEnumerateConnectionSockets, args);
	aCount = args().iCount;

	return err;
	}

TInt TE_RConnectionStep::GetConnectionSocketInfo(RConnection& aConn, TUint aIndex, TConnectionSocketInfo& aSocketInfo)
/**
 * Get the information about a socket
 * @param aConn The connection to use
 * @param aIndex The index to use
 * @param aSocketInfo Where to write the information to
 * @return system wide error code
 */
	{

	TInt err(KErrNone);

	TConnGetSocketInfoArgBuf args;
	args().iIndex = aIndex;
	err = aConn.Control(KCOLConnection, KCoGetConnectionSocketInfo, args);
	aSocketInfo = args().iSocketInfo;

	return err;
	}
	
TInt TE_RConnectionStep::LoadNapt(RSocket& aNaptsock,RSocket& aTestServSock, RSocketServ& aSockServer)
/**
 * Load Napt.Provide TInterfaceLockInfo information 
 * @param naptsock - the napt socket to open
 * @param ss - the socket server
 * @param testServSock - socket connected to test server
 * @return system wide error code
 */
	{
	const TInt netMaskLength   = 8;
    TInt err(KErrNone);
     	
    err = aNaptsock.Open(aSockServer,_L("napt"));
    if(err != KErrNone)
    	{
      	INFO_PRINTF2(_L("Load NAPT failed with error: %d."),err);
      	return err;
		}
            
    _LIT(KInterfaceName,"ipcp::comm");
    _LIT(KInterfaceName1,"eth");
    TUint32 testaddr = GetInterfaceAddress(aTestServSock,KInterfaceName());
     	     
    TPckgBuf <TInterfaceLockInfo> info;    
	info().iPublicIap  = iNaptPublicIap;  
	info().iPrivateIap = iNaptPrivateIap;  
    TInetAddr::Cast	(info().iPrivateIp).SetV4MappedAddress(iNaptTestServAddr.Address()); 
    TInetAddr::Cast(info().iPublicIp).SetV4MappedAddress(testaddr) ; 
    info().iNetmaskLength = netMaskLength; 
    info().iIfIndex = GetInterfaceIndexL(aTestServSock, KInterfaceName1());
#ifdef SYMBIAN_NETWORKING_ADDRESS_PROVISION	
 	info().iUplinkAccess = ETrue;
#endif //SYMBIAN_NETWORKING_ADDRESS_PROVISION
    
	err = aNaptsock.SetOpt(KSoNaptSetup,KSolNapt,info);
	if(err != KErrNone)
    	{
      	INFO_PRINTF2(_L("Set Option for NAPT failed with error: %d."),err);
      	return err;
		} 
	return err;
	}

TInt TE_RConnectionStep::ProdQoSL(RConnection& aConn, RSocketServ& aSs)
/**
 * If you want to use the primary context on a NIF, GUQoS will drop the first packet you send (as it won't yet be aware of the primary). So call this method which sends a packet (thus prodding GUQoS into life).
 * @param aConn A connection to send the packet over (should really be an interface supporting multiple contexts!)
 * @param aSs A socket server within which the connection was created
 * @return system wide error code
 */
{
	TInt err;
	RSocket sock;

	err = OpenUdpSocketExplicitL(sock, aSs, aConn);
	TESTEL(KErrNone == err, err);

	err = SendUdpData(sock, iEchoServerAddr, KSmallBufferLength);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	DestroyUdpSocket(sock);
	CleanupStack::Pop(&sock);

	User::After(KOneSecondDelay);

	return err; // will always be KErrNone if we reach here
}

/**
 * Waits idefinitely until a relevant (specified) progress notification is received. 
 * All other progress notifications are ignored.
 * On return, aProgress contains the requested progress.
 * @param aConn A connection to wait on for a progress.
 * @param aStatus progress notification request status.
 * @param aProgress a buffer for the relevant progress.
 * @param aRelevantProgress a progress to wait for.
 */
void TE_RConnectionStep::WaitForRelevantProgress(RConnection& aConn, TNifProgressBuf& aProgress, TInt aRelevantProgress)
	{
	TRequestStatus reqStatus;
		
	for(;;) // WINS compiler generates a warning if while(ETrue) is used.
		{
		aConn.ProgressNotification(aProgress, reqStatus);
		User::WaitForRequest(reqStatus);				
		if(aRelevantProgress == aProgress().iStage)
			{	
			break;			
			}
		}
	}
	
TUint32 TE_RConnectionStep::GetInterfaceAddress(RSocket& sock,const TDesC& aFName)
/**
 * Get interface gloabal Ip Address
 * @param sock - socket connection
 * @param aFName - interface name
 * @return Ip address
 */
{ 
	TName address;
	TBool isSiteLocal,isLinkLocal;
	TInt retVal = -1;
	
	TPckgBuf<TSoInetInterfaceInfo> info; 
	 
 	retVal = sock.SetOpt(KSoInetEnumInterfaces, KSolInetIfCtrl);
 	TESTEL(retVal == KErrNone, retVal);
 	
    retVal = sock.GetOpt(KSoInetNextInterface, KSolInetIfCtrl, info);
    TESTEL(retVal == KErrNone, retVal);
            
   	while(retVal==KErrNone)
		 {
	 	 info().iAddress.Output(address);
				
		 isSiteLocal = info().iAddress.IsSiteLocal();	
		 isLinkLocal = info().iAddress.IsLinkLocal();
	
		 TInt str = info().iName.Find(aFName);
		 if((str == KErrNone) && (isSiteLocal == EFalse) && (isLinkLocal == EFalse))
		    {
	         INFO_PRINTF2(_L("Interface address obtained: %S."),&address);
		     break;
			}
		  
		  retVal = sock.GetOpt(KSoInetNextInterface, KSolInetIfCtrl, info);
	      }

	return info().iAddress.Address();
}

TUint32 TE_RConnectionStep::GetInterfaceIndexL(RSocket& sock,const TDesC& aFName)
/**
 * Get interface index
 * @param sock - socket connection
 * @param aFName - interface name
 * @return Interface index
 */
{ 
    TPckgBuf<TSoInetInterfaceInfo> interfaceInfo; 
    TPckgBuf<TSoInetIfQuery> queryInfo;
    
    TName address;
    TInt retVal = KErrNotFound;

    retVal = sock.SetOpt(KSoInetEnumInterfaces, KSolInetIfCtrl);
    TESTEL(retVal == KErrNone, retVal);
    
    retVal = sock.GetOpt(KSoInetNextInterface, KSolInetIfCtrl, interfaceInfo);
    TESTEL(retVal == KErrNone, retVal);
            
    while(retVal==KErrNone)
         {
         TInt str = interfaceInfo().iName.Find(aFName);
         if(str == KErrNone)
            {
            queryInfo().iName = interfaceInfo().iName;
            break;
            }
          retVal = sock.GetOpt(KSoInetNextInterface, KSolInetIfCtrl, interfaceInfo);
          }
    TESTEL(retVal == KErrNone, retVal);
    
    retVal = sock.GetOpt(KSoInetIfQueryByName, KSolInetIfQuery, queryInfo);
    TESTEL(retVal == KErrNone, retVal);
    
    return queryInfo().iIndex;
}


TBool TE_RConnectionStep::WaitForRequestOrTimeOutL(TRequestStatus& aStatus, TInt aTimeOut)
    {
    RTimer timer;
    User::LeaveIfError(timer.CreateLocal());

    TRequestStatus timerStatus;
    timer.After(timerStatus, aTimeOut);
    User::WaitForRequest(aStatus, timerStatus);
    TBool requestCompleted = EFalse;
    
    if (timerStatus == KRequestPending)
        {
        timer.Cancel();
        User::WaitForRequest(timerStatus);
        requestCompleted = ETrue;
        }
    
    timer.Close();
    return requestCompleted;
    }


/*
 * Now the stuff for the peculiar tests that need to use QoS
 */

TE_RConnectionQoSStep::TE_RConnectionQoSStep() : TE_RConnectionStep()
/**
 * Constructor for TE_RConnectionQoSStep, an abstract class derived from by any steps dealing with QoS. Must use two phase construction.
 */
{
}

void TE_RConnectionQoSStep::Log( TRefByValue<const TDesC16> format, ... )
	{
	VA_LIST aList;
	VA_START( aList, format );

	// decode formated data for display on console
	TBuf <MAX_LOG_LINE_LENGTH> LineBuf;
	LineBuf.AppendFormatList( format, aList );

	INFO_PRINTF1(LineBuf);

	VA_END( aList ); 
	}





TE_RConnectionQoSStep::TS_QosChannel::TS_QosChannel(TE_RConnectionQoSStep& aTestStep)
:iTestStep(aTestStep)
/**
 * Constructor for TE_RConnectionQoSStep, an abstract class derived from by any steps dealing with QoS. Must use two phase construction.
 */
{
}


TE_RConnectionQoSStep::TS_QosChannel::~TS_QosChannel()
/**
 * Destructor for ~TS_QosChannel
 */
{
}

void TE_RConnectionQoSStep::TS_QosChannel::ConstructL()
/**
 * Second phase constructor for TE_RConnectionQoSStep derived classes.
 * @param none
 * @return nothing
 */
{
}


TInt TE_RConnectionQoSStep::TS_QosChannel::Open(RSocketServ& aServer, RConnection& aConnection)
{
	return iChannel.Open(aServer, RSubConnection::ECreateNew, aConnection);
}


TInt TE_RConnectionQoSStep::TS_QosChannel::Add(RSocket aSocket)
{
	TRequestStatus subconnReqStat;
	iChannel.Add(aSocket, subconnReqStat);
	User::WaitForRequest(subconnReqStat);
	return subconnReqStat.Int();
}

TInt TE_RConnectionQoSStep::TS_QosChannel::Close()
{
	iChannel.Close();
	REComSession::FinalClose();
	return KErrNone;
}

TInt TE_RConnectionQoSStep::TS_QosChannel::SetQoSParametersL()
/**
 * Set up a standard set of QoS parameters to be used by derived classes
 * @param aParameters Where to write the standard parameters to
 * @return system wide error code
 */
{
	TInt err = KErrNone;
	//------------------------------------
	//1. Initialise QoS params structure;
	//------------------------------------
	//create a container for QoS sub connection parameters (Param bundle takes ownership)
	RSubConParameterBundle subconnParams;
	CleanupClosePushL(subconnParams);

	CSubConParameterFamily* family = CSubConParameterFamily::NewL(subconnParams, KSubConQoSFamily);

	//create the generic set
	CSubConQosGenericParamSet* subConQosGenParSet = CSubConQosGenericParamSet::NewL(*family, CSubConParameterFamily::ERequested);
	
	//Qos mapping extracted from ipcpr.cpp MapGenericParamsFromESockToPRTL(...)

	//1. TokenRateUplink
	//NO MAPPING for subConQosGenParSet->SetTokenRateUplink(1024);
	//mapping commented out from MapGenericParamsFromESockToPRTL:
	//params.SetTokenRateUplink(TUint32 aRate);

	//2. TokenBucketSizeUplink
	//Mapping based on:
	//	TInt uplinkBandwidth = generic->GetUplinkBandwidth();
	//  params.SetTokenBucketSizeUplink((TUint32)uplinkBandwidth);
	subConQosGenParSet->SetUplinkBandwidth(896);
	
	//3. MaxTransferRateUplink
	//Mapping based on:
	//  TInt upLinkMaximumBurstSize = generic->GetUpLinkMaximumBurstSize();
	//  params.SetMaxTransferRateUplink((TUint32)upLinkMaximumBurstSize);
	subConQosGenParSet->SetUpLinkMaximumBurstSize(1024);
	
	//4. MaxPacketSizeUplink
	//Mapping based on:
	//	TInt upLinkMaximumPacketSize = generic->GetUpLinkMaximumPacketSize();
	//	params.SetMaxPacketSizeUplink((TUint32)upLinkMaximumPacketSize);
	subConQosGenParSet->SetUpLinkMaximumPacketSize(735);
	
	//5. MinPolicedUnitUplink
	//Mapping based on:
	//	TInt upLinkDelayVariation = generic->GetUpLinkDelayVariation();
	//	params.SetMinPolicedUnitUplink((TUint32)upLinkDelayVariation);
	subConQosGenParSet->SetUpLinkDelayVariation(0);
	
	//6. DelayUplink
	//Mapping based on:
	//	TInt upLinkDelay = generic->GetUpLinkDelay();
	//	params.SetDelayUplink((TUint32)upLinkDelay);
	subConQosGenParSet->SetUpLinkDelay(0);

	//7. PriorityUplink
	//Mapping based on:
	//	TInt upLinkPriority = generic->GetUpLinkPriority();
	//	params.SetPriorityUplink((TUint16)upLinkPriority);
	subConQosGenParSet->SetUpLinkPriority(3);
	
	//8. DropModeUplink
	//NO MAPPING for subConQosGenParSet->SetDropModeUplink(KPfqosDropTail);
	//mapping commented out from MapGenericParamsFromESockToPRTL:
	//params.SetDropModeUplink(TUint8 aDropMode);

	//9. TokenRateDownlink
	//NO MAPPING for subConQosGenParSet->SetTokenRateDownlink(1024);
	//mapping commented out from MapGenericParamsFromESockToPRTL:
	//params.SetTokenRateDownlink(TUint32 aRate);
	
	//10. TokenBucketSizeDownlink
	//Mapping based on:
	//	TInt downlinkBandwidth = generic->GetDownlinkBandwidth();
	//	params.SetTokenBucketSizeDownlink((TUint32)downlinkBandwidth);
	subConQosGenParSet->SetDownlinkBandwidth(768);
	
	//11. MaxTransferRateDownlink
	//Mapping based on:
	//	TInt downLinkMaximumBurstSize = generic->GetDownLinkMaximumBurstSize();
	//	params.SetMaxTransferRateDownlink((TUint32)downLinkMaximumBurstSize);
	subConQosGenParSet->SetDownLinkMaximumBurstSize(1024);
	
	//12. MaxPacketSizeDownlink
	//Mapping based on:
	//	TInt downLinkMaximumPacketSize = generic->GetDownLinkMaximumPacketSize();
	//	params.SetMaxPacketSizeDownlink((TUint32)downLinkMaximumPacketSize);
	subConQosGenParSet->SetDownLinkMaximumPacketSize(512);
	
	//13. MinPolicedUnitDownlink
	//Mapping based on:
	//	TInt downLinkDelayVariation = generic->GetDownLinkDelayVariation();
	//	params.SetMinPolicedUnitDownlink((TUint32)downLinkDelayVariation);
	subConQosGenParSet->SetDownLinkDelayVariation(300);
	
	//14. DelayDownlink
	//Mapping based on:
	//	TInt downLinkDelay = generic->GetDownLinkDelay();
	//	params.SetDelayDownlink((TUint32)downLinkDelay);
	subConQosGenParSet->SetDownLinkDelay(200);
	
	//15. PriorityDownlink
	//Mapping based on:
	//	TInt downLinkPriority = generic->GetDownLinkPriority();
	//	params.SetPriorityDownlink((TUint16)downLinkPriority);
	subConQosGenParSet->SetDownLinkPriority(128);
	
	//16. DropModeDownlink
	//NO MAPPING for subConQosGenParSet->SetDropModeDownlink(KPfqosDropTail);
	//mapping commented out from MapGenericParamsFromESockToPRTL:
	//params.SetDropModeDownlink(TUint8 aDropMode);
	
	//17. AdaptationPriority
	//NO MAPPING for subConQosGenParSet->SetAdaptationPriority(KQoSLowestPriority);
	//mapping commented out from MapGenericParamsFromESockToPRTL:
	//params.SetAdaptationPriority(TUint8 aPriority);
	
	//17. AdaptationPriority
	//NO MAPPING for subConQosGenParSet->SetAdaptMode(EFalse);
	//mapping commented out from MapGenericParamsFromESockToPRTL:
	//params.SetAdaptMode(TBool aCanAdapt);
	
	//------------------------------------
	//2. Subscribe for QoS Params notification
	//------------------------------------
	TNotificationEventBuf subconnNotifBuf;
	TRequestStatus subconnReqStat;
	RSubConnection::TEventFilter subconnEventFilter(
				    KSubConnGenericEventsImplUid,
				    KSubConGenericEventParamsGranted | 
				    KSubConGenericEventParamsChanged |
				    KSubConGenericEventParamsRejected );
	iChannel.EventNotification(subconnNotifBuf, 
				&subconnEventFilter, 1, subconnReqStat);

	//------------------------------------
	//3. Set the QoS Parameters
	//------------------------------------
	User::LeaveIfError(iChannel.SetParameters(subconnParams));

	//------------------------------------
	//4. Receive & Process the event (expecting KSubConGenericEventParamsGranted)
	//------------------------------------
	User::WaitForRequest(subconnReqStat);
	User::LeaveIfError(subconnReqStat.Int());

	
	switch (subconnNotifBuf.Id())
	{
	case KSubConGenericEventParamsGranted:
			iTestStep.Log(_L("CSubConGenericEventParamsGranted received"));
			break;
		case KSubConGenericEventParamsChanged:
			iTestStep.Log(_L("CSubConGenericEventParamsChanged received"));
			break;
		case KSubConGenericEventParamsRejected:
			iTestStep.Log(_L("CSubConGenericEventParamsRejected received"));
			err = KErrNotSupported;
			break;
		default:
			iTestStep.Log(_L("unknown event received.  Id=%d"),subconnNotifBuf.Id());
			err = KErrNotSupported;
			break;
	}

	subconnParams.Close();
	CleanupStack::Pop(&subconnParams);

	return err;
}

 
CStatus::CStatus():CActive(0)
{
	CActiveScheduler::Add(this);
	SetActive();
}

CStatus::~CStatus()
{
	Cancel();
}

TInt TE_RConnectionSimTsyDataStep::NTRasThreadFunction(TAny* /*aThreadData*/)
	{
	_LIT(KCsyName,"LOOPBACK.CSY");
	_LIT(KPortName,"LOOPBACK::1");
	_LIT8(KServerText,"SERVER");

	CTrapCleanup* cleanup = CTrapCleanup::New();
	if (!cleanup)
		{
		return KErrNoMemory;
		}

	RCommServ commServ;
	TInt ret = commServ.Connect();
	if (KErrNone != ret)
		{
		return ret;
		}
	
	ret = commServ.LoadCommModule(KCsyName);
	if(KErrNone == ret)
		{
		RComm comm;
		ret = comm.Open(commServ, KPortName, ECommShared);
		if(KErrNone == ret)
			{
			TBuf8<6> readBuf;
			TRequestStatus stat0;
			comm.Read(stat0, readBuf);
			User::WaitForRequest(stat0);

			comm.Write(stat0, KServerText);
			User::WaitForRequest(stat0);
			}
		comm.Close();
		}
	commServ.Close();
	delete cleanup;
	return ret;
	}

TInt TE_RConnectionSimTsyDataStep::StartNTRasSimulation()
	{
	const TInt KStackSize=0x8000;
	const TInt KHeapSize=0x8000;
	const TInt KMaxHeapSize=0x80000;
	TInt res = KErrNone ;
	_LIT(KNtRasName,"NTRasEmulation");

	RThread t;
	res=t.Create(KNtRasName, NTRasThreadFunction,
					KStackSize,KHeapSize,KMaxHeapSize,NULL);
	t.Resume();
	t.Close();

	return res;
	}

enum TVerdict TE_RConnectionSimTsyDataStep::doTestStepPreambleL()
	{
	_LIT(KPhbkSyncCMI, "phbsync.cmi");
	if (StartC32WithCMISuppressions(KPhbkSyncCMI) != KErrNone)
    	{
    	return EFail;
    	}
				
	// Wait for all of the interfaces to close down		 
	RSocketServ ss;		
	User::LeaveIfError(ss.Connect());
	CleanupClosePushL(ss);

	if (KErrNone != WaitForAllInterfacesToCloseL(ss))
		{
		INFO_PRINTF1(_L("Previous test case has left connections up that have not closed. Stopping them."));
		TBuf<200> errDesc;
		TInt err;
		if ((err = ESockTestUtils::StopAllInterfaces(errDesc)) != KErrNone)
			{
			INFO_PRINTF3(_L("StopAllInterfaces() err=%d\n%S"), err, &errDesc);
			CleanupStack::PopAndDestroy(&ss);
			return EFail;
			}			
		}	
	CleanupStack::PopAndDestroy(&ss);	
	
	TInt ret = RProperty::Set(KUidPSSimTsyCategory, KPSSimTsyTestNumber, 0);
	if (ret != KErrNone)
		{
		INFO_PRINTF1(_L("Failed resetting"));
		User::Leave(ret);
		}

	ret = iTelServer.Connect();
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to connect to telephony server"));
		User::Leave(ret);
		}
	
   	ret=iTelServer.LoadPhoneModule(KSimtsyName);
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to load phone module"));
		iTelServer.Close();
		User::Leave(ret);
		}

	ret=iPhone.Open(iTelServer,KPhoneName) ;
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to open phone module"));
		iTelServer.UnloadPhoneModule(KSimtsyName);
		iTelServer.Close();
		User::Leave(ret);
		}
	ret=iPhone.Initialise();
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to initialise the phone"));
		iPhone.Close();
		iTelServer.UnloadPhoneModule(KSimtsyName);
		iTelServer.Close();
		User::Leave(ret);
		}

	RPhone::TCaps phoneCaps;
	ret = iPhone.GetCaps(phoneCaps) ;
	if ((KErrNone != ret) || !(phoneCaps.iFlags & RPhone::KCapsData))
		{
		INFO_PRINTF2(_L("This phone does not support data calls (caps=0x%x)"), phoneCaps.iFlags);
		iPhone.Close();
		iTelServer.UnloadPhoneModule(KSimtsyName);
		iTelServer.Close();
		User::Leave(ret);
		}

	return TestStepResult();
	}

enum TVerdict TE_RConnectionSimTsyDataStep::doTestStepPostambleL()
	{
	REComSession::FinalClose();
	iPhone.Close();
	iTelServer.UnloadPhoneModule(KSimtsyName);
	iTelServer.Close();

	__UHEAP_MARKEND;
	return TestStepResult();	
	}

