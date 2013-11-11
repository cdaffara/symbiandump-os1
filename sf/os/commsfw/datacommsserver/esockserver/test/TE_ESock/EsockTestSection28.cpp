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
// This contains ESock Test cases from section 28
// Integration test for IPv6 static DNS configuration.
//
//

// EPOC includes
#include <e32base.h>
#include <in_sock.h>
#include <es_sock.h>
#include <nifman.h>
#include <commdbconnpref.h>

// Test system includes
#include "EsockTestSection28.h"
#include <comms-infras/esocktestutils.h>

//
// Base class for Test 28 containing common code
// 

CEsockTest28::~CEsockTest28()
/**
Destructor for common base class CEsockTest28

  destroy the interface names array
  */
	{
	iNames.ResetAndDestroy();
	iNames.Close();
	}


void CEsockTest28::NameInit()
	{
	}

TInt CEsockTest28::NameAppendL(const TDesC& aName)
/**
Add a name to the interface names array

  @param aName interface name to add
  @return KErrNone or a system wide error code
  */
	{
	return iNames.Append(aName.AllocL());
	}

TBool CEsockTest28::NameFound(const TDesC& aName)
/**
Check whether a name is present in the interface names array

  @param aName interface name to check
  @return ETrue if name is present else EFalse
  */
	{
	TInt count = iNames.Count();
	for (TInt i = 0 ; i < count ; i++)
		{
		if (*iNames[i] == aName)
			{
			return ETrue;
			}
		}
	return EFalse;
	}

void CEsockTest28::NamePrint()
/**
Print out the interface names array
*/
	{
	Logger().WriteFormat(_L("Interface names: "));
	TInt count = iNames.Count();
	for (TInt i = 0 ; i < count ; i++)
		{
		Logger().WriteFormat(_L("\t%S"), iNames[i]);
		}
	}

TVerdict CEsockTest28::Ipv6StaticDnsTestL(TInt aSubStep)
/**
Test IPv6 Static DNS Configuration for different NIF's

  This routine contains common base class code for tests in
  section 28.
  
	The test is used to check that a particular NIF is taking
	into account the following CommDb parameters:
	
	  
		Ip6DNSAddrFromServer
		Ip6NameServer1
		Ip6NameServer2
		
		  Basic strategy:
		  
			1.  Retrieve configuration information from ini file (IAP, DNS addresses)
			2.  Start the NIF to test via RConnection
			3.  Retrieve the interface binder names (via RConnection KConnGetInterfaceName)
			4.  Retrieve all TCP/IP interface information (via KSoInetNextInterface socket option)
			5.  For each interface retrieved in (4), match its name against the names retrieved
			in (3) (to find the TCP/IP interface(s) corresponding to the RConnection) and
			ensure that its DNS addresses match those in the Ini file (which, in turn, match
			those in the CommDb).
			
			  The test code in this routine is executed for each Test 28 substep.
			  Each substep is used to test a different NIF, with the aSubStep parameter being used
			  to index into a different section of the ini file to retrieve the parameters
			  needed for the particular substep (in particular, the IAP to start).
			  
				@param aSubStep Sub step number
				@return TVerdict code
				*/
	{
    TVerdict    verdict = EPass;
	
    Logger().WriteFormat(_L("Testing IPv6 static DNS configuration"));
	
	TInt err;
	
	
	// Retrieve the IAP for the particular test from the Ini file.
	
	TBuf<50> sectionBuf;
	sectionBuf.Format(_L("Test_28.%d"), aSubStep);
	TInt iap;
	
	if (!GetIntFromConfig(sectionBuf, KKeyIap(), iap))					// retrieve IAP
		{
		Logger().WriteFormat(_L("Cannot retrieve IAP from Ini file"));
		return EAbort;
		}
	
	// Connect to ESOCK
	
	RSocketServ ss;
	err = ss.Connect();
	TESTEL(KErrNone == err, err);
	
	RConnection conn;
	err = conn.Open(ss);
	TESTEL(KErrNone == err, err);
	
	CleanupClosePushL(conn);
	
	// Start up the RConnection
	
	TCommDbConnPref pref;
	pref.SetIapId(iap);
	pref.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
	err = conn.Start(pref);
	
	if (err == KErrHardwareNotAvailable)
		{
		TPtrC isEthernet;
		if (GetStringFromConfig(sectionBuf, KKeyEthernet(), isEthernet))
			{
			if (isEthernet == _L("true"))
				Logger().WriteFormat(_L("*** Test step requires Ethernet setup ***"));
			}
		}
	
	TESTEL(KErrNone == err, err);
	
	// Read the primary and secondary DNS addresses from CommDb
	
	TBuf<KMaxAddressTextSize> IniPrimary;
	TBuf<KMaxAddressTextSize> IniSecondary;
	
	err = conn.GetDesSetting(TPtrC(SERVICE_IP6_NAME_SERVER1), IniPrimary);
	TESTEL(KErrNone == err, err);
	
	err = conn.GetDesSetting(TPtrC(SERVICE_IP6_NAME_SERVER2), IniSecondary);
	TESTEL(KErrNone == err, err);
	
	TInetAddr primaryAddr;
	TInetAddr secondaryAddr;
	
	primaryAddr.Input(IniPrimary);
	secondaryAddr.Input(IniSecondary);
	
	// Retrieve the interface names
	
	TConnInterfaceName name;
	name.iIndex = 1;
	
	NameInit();
	
	TPckg<TConnInterfaceName> namePkg(name);
	do
		{
		err = conn.Control(KCOLProvider, KConnGetInterfaceName, namePkg);
		
		if (!NameFound(name.iName))
			{
			NameAppendL(name.iName);
			}
		
		name.iIndex++;
		}
	while (err == KErrNone);
	
	NamePrint();
	
	// - read information from TCPIP6 about each interface and its addresses (via
	//   socket options).
	// - find the TCPIP6 interfaces relevant to the RConnection by matching the
	//	 interface names retrieved above with each interface entry returned by TCPIP6.
	// - for each matching interface, check to see if the primary and secondary DNS
	//   addresses match those that we are testing.  If a match is found, the test passes,
	//   else it fails.
	
	RSocket sock;
	err = sock.Open(ss, KAfInet, KSockDatagram, KProtocolInetUdp);
	TESTEL(KErrNone == err, err);
	
	CleanupClosePushL(sock);
	
	Logger().WriteFormat(_L("Interface DNS addresses:"));
	err = sock.SetOpt(KSoInetEnumInterfaces, KSolInetIfCtrl);
	TESTEL(KErrNone == err, err);
	
	TSoInetInterfaceInfo info;
	TPckg<TSoInetInterfaceInfo> opt(info);
	
	
	TBool found = EFalse;
	
	do {
		// Retrieve information about next interface (actually, interface address)
		err = sock.GetOpt(KSoInetNextInterface, KSolInetIfCtrl, opt);
		if (err == KErrNone)
			{
			if (!info.iAddress.IsUnspecified())
				{
				// Check first if it is an address associated with the RConnection
				if (NameFound(info.iName))
					{
					if (!info.iNameSer1.IsUnspecified() && !info.iNameSer2.IsUnspecified())
						{
						TBuf<KMaxAddressTextSize> primary;
						TBuf<KMaxAddressTextSize> secondary;
						
						info.iNameSer1.Output(primary);
						info.iNameSer2.Output(secondary);
						
						if (primary.Length() > 0 && secondary.Length() > 0)
							{
							Logger().WriteFormat(_L("\t%S:\tprimary = %S, secondary = %S"), &info.iName, &primary, &secondary);
							// Match primary and secondary addresses with the interface
							if (info.iNameSer1.Match(primaryAddr) && info.iNameSer2.Match(secondaryAddr))
								{
								Logger().WriteFormat(_L("Interface DNS addresses match for %S"), &info.iName);
								found = ETrue;
								break;
								}
							}
						}
					}
				}
			}
		else
			TESTEL(KErrNotFound == err, err);
		}
	while (err != KErrNotFound);
	
	// Close socket
	
	sock.Close();
	CleanupStack::Pop(&sock);
	
	if (!found)
		{
		verdict = EFail;
		}
	
	// Close connection
	
	conn.Stop();
	conn.Close();
	CleanupStack::Pop(&conn);
	
	ss.Close();
	
	return verdict;
	}
	
	
	
const TDesC& CEsockTest28_1::GetTestName()
/**
Constructor for Test 28.1
*/
	{
	// store the name of this test case
	_LIT(ret,"Test28.1");
	
	return ret;
	}

TVerdict CEsockTest28_1::doTestStepPreambleL()
/**
Preamble for Test 28.1

  Cleans up pending connections, otherwise KErrInUse occurs as
  we try to start up a IAP using same COMM port.
  
	@return TVerdict code indicating test result of preamble
	*/
	{
	TBuf<200> errDesc;
	ESockTestUtils::StopAllInterfaces(errDesc);
	return EPass;
	}

TVerdict CEsockTest28_1::easyTestStepL()
/**
Test step 28.1

  IPv6 static DNS configuration (PPP NIF)
  
	Requires a connection
	
	  @return TVerdict code indicating test result
	  */
	{
	return Ipv6StaticDnsTestL(1);
	}


const TDesC& CEsockTest28_2::GetTestName()
/**
Constructor for Test 28.2
*/
	{
	// store the name of this test case
	_LIT(ret,"Test28.2");
	
	return ret;
	}

TVerdict CEsockTest28_2::easyTestStepL()
/**
Test step 28.2

  IPv6 static DNS configuration (Ethernet NIF)
  
	Requires Ethernet NIF to be setup (but does not transmit packets)
	
	  @return TVerdict code indicating test result
	  */
	{
	return Ipv6StaticDnsTestL(2);
	}
//


