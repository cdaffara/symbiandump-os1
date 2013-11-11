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
// This defines the CTS_MultiHomingSuite class which is the 
// container class for the TS_MultiHomingStep-derived test steps
// 
//

/**
 @file
*/

#include "TS_MultiHomingSuite.h"
#include "TS_RConnectionStart.h"
#include "TS_RConnectionClose.h"
#include "TS_RConnectionStop.h"
#include "TS_TestConnection.h"
#ifdef SYMBIAN_STRICT_EXPLICIT_SOCKET_BINDING
#include "ts_testincomming.h"
#endif //SYMBIAN_STRICT_EXPLICIT_SOCKET_BINDING
#include "TS_CopyComDbFile.h"
#include "TS_ShowConnections.h"
#include "TS_ResolveName.h"
#include "TS_ResolveAddress.h"
#include "TS_GetConnectionInfo.h"
#include "TS_CheckLinkLocalSame.h"
#include "TS_Delay.h"
#include <e32cons.h>
#include <in_sock.h>
#include <in_iface.h>
#include <nifman.h>
					  

EXPORT_C CTS_MultiHomingSuite* NewTS_MultiHomingSuite( void ) 
/** Polymorphic interface, exported ordinal 1.  Called by scheduletest
 * and used to instantiate the suite
 * @return A pointer to the created CTS_MultiHomingSuite object
 */
    {
	CTS_MultiHomingSuite* ts = 0;
	TRAPD(err,ts = new (ELeave) CTS_MultiHomingSuite);
	if (err == KErrNone)
		return ts;
	return 0;
    }

CTS_MultiHomingSuite::~CTS_MultiHomingSuite()
/** 
 * Destructor has to clean up any TConnDetails left hanging around
 * 
 */
	{
	delete iScheduler;
	}

void CTS_MultiHomingSuite::AddTestStepL( CTS_MultiHomingStep* ptrTestStep )
/**
 * Adds a test step to the test suite object, using the base class method
 * @param ptrTestStep A pointer to the test step to be added
 * @exception Can leave
 */
	{
	ptrTestStep->iOwnerSuite = this; 
	CTestSuite::AddTestStepL(ptrTestStep);
	}

void CTS_MultiHomingSuite::InitialiseL( void )
/**
 * Effective second-phase constructor.  Creates all suite test steps
 * and associates them with the suite.
 */
	{
	// Load the serial drivers
	TInt ret = User::LoadPhysicalDevice(PDD_NAME);
	if ( KErrNone != ret && KErrAlreadyExists != ret )
		{
		User::Leave( ret );
		}
		
	ret = User::LoadLogicalDevice(LDD_NAME);
	if ( KErrNone != ret && KErrAlreadyExists != ret )
		{
		User::Leave( ret );
		}

 	// When bootstrapping C32 we have to avoid the PhBkSyncServer being started, since
 	// it needs a different CommDB
 	_LIT(KPhbkSyncCMI, "phbsync.cmi");
    ret = StartC32WithCMISuppressions(KPhbkSyncCMI);
	if ( KErrNone != ret && KErrAlreadyExists != ret )
		{
		User::Leave( ret );
		}

	//Make the scheduler...
	
	iScheduler=new(ELeave) CEnhancedScheduler();
	CEnhancedScheduler::Install(iScheduler);
	

	// Add Test steps	
	AddTestStepL( new(ELeave) CTS_RConnectionStart );
	AddTestStepL( new(ELeave) CTS_RConnectionClose );
	AddTestStepL( new(ELeave) CTS_RConnectionStop  );
	AddTestStepL( new(ELeave) CTS_TestConnection   );

#ifdef SYMBIAN_STRICT_EXPLICIT_SOCKET_BINDING
	AddTestStepL( new(ELeave) CTS_TestIncomming   );
#endif //SYMBIAN_STRICT_EXPLICIT_SOCKET_BINDING
	AddTestStepL( new(ELeave) CTS_ShowConnections  );
	AddTestStepL( new(ELeave) CTS_ResolveName );
	AddTestStepL( new(ELeave) CTS_ResolveAddress );
	AddTestStepL( new(ELeave) CTS_GetConnectionInfo );
	AddTestStepL( new(ELeave) CTS_CheckLinkLocalSame );
	AddTestStepL( new(ELeave) CTS_CopyComDbFile );
	AddTestStepL( new(ELeave) CTS_Delay );
	}


TPtrC CTS_MultiHomingSuite::GetVersion( void )
/**
 * Give version information back to Schedultest
 * @return The descriptor of the version
 */
	{
	return KTxtVersion();
	}


TInt CTS_MultiHomingSuite::GetConnIndex(const TDesC& aName)
/**
 * Gets the array index of the name passed in
 * @param aName Descriptor containing the connection name to match
 * @return The index, or KErrNotFound
 */
	{
	for (TInt i=0; i < CTS_MultiHomingSuite::MAX_CONNECTIONS; i++)
		{
		if (iConnections[i].MatchName(aName))
			return i;
		}
	return KErrNotFound;
	}


TConnDetails *CTS_MultiHomingSuite::GetNewTConnection( )
/**
 * Retreive 
 *
 */
	{
	for (TInt i=0; i < CTS_MultiHomingSuite::MAX_CONNECTIONS; i++)
		{
		if (iConnections[i].iName.Length() == 0)
			return &iConnections[i];
		}
	return 0;
	}

TInt CTS_MultiHomingSuite::AddConnection(const TConnDetails& aConn)
/**
 * Appends a connection to the array of connections in the suite
 * @param aName Reference to the object to pass in
 * @return System wide error code 
 */
	{
	TInt err= KErrNone;			// Check the name doesn't already exist
	TInt connIndex = GetConnIndex(aConn.iName);
	if(KErrNotFound == connIndex)
		{
		//err = iConnections.Append(aConn);
		}
	else
		{
		err = KErrAlreadyExists;
		_LIT(KMultipleConns, "Connection has already been started. Check configuration files.");
		LogExtra((TText8*)__FILE__, __LINE__, ESevrInfo, KMultipleConns);
		}

	return err;
	}

TConnDetails *CTS_MultiHomingSuite::GetTConnection(const TDesC& aName)
/**
 * Returns the TConnection object for a specific connection
 * @param aName Name of connection to look up
 * @return The RConnection object
 */

	{
	TInt err= KErrNone;
	err = GetConnIndex(aName);
	if (err == KErrNotFound)
		{
		//User::Panic(KTxtSuiteName, err);
		return 0;
		}
	return &iConnections[err];
	}
	
RConnection *CTS_MultiHomingSuite::GetRConnection(const TDesC& aName)
/**
 * Returns the RConnection object for a specific connection
 * @param aName Name of connection to look up
 * @return The RConnection object
 */
    {
	TInt err= KErrNone;
	err = GetConnIndex(aName);
	if (err == KErrNotFound)
		{
		//User::Panic(KTxtSuiteName, err);
		return 0;
		}
	return &iConnections[err].iConnection;
	}

RSocketServ *CTS_MultiHomingSuite::GetRSocketServ(const TDesC& aName)
/**
 * Returns the RSocketServ object for a specific connection
 * @param aName Name of connection to look up
 * @return The RSocketServ object
 */
	{
	TInt connIndex = -1;

	connIndex = GetConnIndex(aName);
	if (connIndex < 0)
		{
		return 0;
		}
	return &iConnections[connIndex].iSocketServ;
	}


TInt CTS_MultiHomingSuite::CloseConnection(const TDesC& aName)
/** 
 * Closes a connection and removes it from the array
 * @param aName The name of the connection to close
 * @return System wide error code
 */
	{
	TInt index= KErrNone;
	index= GetConnIndex(aName);
	if (index != KErrNotFound)
		{
		if (iConnections[index].iClients == 0)		// No open sockets, so can close
			{
			if(iConnections[index].iConnectionType 
				!= TConnDetails::implicitConn)
				{
				iConnections[index].iConnection.Close();
				}
			else
				{
				iConnections[index].iSocket.Close();
				}

			iConnections[index].iSocketServ.Close();
			iConnections[index].iName.Zero();
			return KErrNone;
			}
		return KErrInUse;
		}
	return index;
	}


TInt CTS_MultiHomingSuite::StopConnection(const TDesC& aName)
/** 
 * Closes a connection and removes it from the array
 * @param aName The name of the connection to close
 * @return System wide error code
 */
	{
	TInt index= KErrNone;
	index= GetConnIndex(aName);
	if (index != KErrNotFound)
		{
		if (iConnections[index].iClients == 0)		// No open sockets, so can close
			{
			if(iConnections[index].iConnectionType 
				!= TConnDetails::implicitConn)
				{
				iConnections[index].iConnection.Stop();	//< Any connections present killed
				}
			else
				{
				iConnections[index].iSocket.Close();
				}

			iConnections[index].iSocketServ.Close();
			iConnections[index].iName.Zero();
			return KErrNone;
			}
		return KErrInUse;
		}
	return index;
	}


inline void RemoveScopeFromAddress(TName& address)
	{
	TChar percent('%');
	TInt percentIndex = address.Locate(percent);
	
	if( percentIndex >= 0 )
		{
		address.SetLength(percentIndex);
		}
	}

TInt CTS_MultiHomingSuite::DisplayConnectionDetailsL()
/**
 * Displays the IP interfaces that are currently in the system
 * @return number of non loopback interfaces
 */
	{
	TInt err = KErrNone;
	for (TInt i=0; i < CTS_MultiHomingSuite::MAX_CONNECTIONS; i++)
		{
		if(iConnections[i].iName.Length() == 0)
			{
			continue;
			}
		_LIT (KConnLogString, "Name [%S], RConnection[%x], RSocketServ[%x]");
		Log(KConnLogString, &iConnections[i].iName,
							&iConnections[i].iConnection,
							&iConnections[i].iSocketServ);
	
		}

	TAutoClose<RSocketServ> ss;
	err = ss.iObj.Connect();
	ss.PushL();

	TAutoClose<RSocket> sock;
	err = sock.iObj.Open(ss.iObj, _L("udp"));

	sock.PushL();

	User::LeaveIfError(sock.iObj.SetOpt(KSoInetEnumInterfaces, KSolInetIfCtrl));

	TProtocolDesc in;
	User::LeaveIfError(sock.iObj.Info(in));
	//LogExtra(((TText8*)(__FILE__)), (__LINE__), ESevrErr, _L("EPOC32 IP Stack Version %d.%d.%d ---- INTERFACE START"), in.iVersion.iMajor, in.iVersion.iMinor, in.iVersion.iBuild);


	TPckgBuf<TSoInetInterfaceInfo> info;
	
	err = sock.iObj.GetOpt(KSoInetNextInterface, KSolInetIfCtrl, info);
	// The first interface is always empty
	err = sock.iObj.GetOpt(KSoInetNextInterface, KSolInetIfCtrl, info);
	TInt numberOfInterfaces = 0;
	while(err == KErrNone)
		{
		// Skip loopback interfaces
		if(info().iFeatures & KIfIsLoopback || info().iState != EIfUp) 
			{
			err = sock.iObj.GetOpt(KSoInetNextInterface, KSolInetIfCtrl, info);
			continue;
			}

		
		TBuf <MAX_LOG_LINE_LENGTH> LineBuf;
		

		LineBuf.AppendFormat(_L("%S: Tag: \"%S\" is ")
					,&info().iName
					,&info().iTag);

		LineBuf.Append(info().iState == EIfPending ? _L("PENDING") :
					   info().iState == EIfUp      ? _L("UP")      :
					   info().iState == EIfDown    ? _L("DOWN")    :
					   info().iState == EIfBusy    ? _L("BUSY")    :
					   _L("UNDEFINED_STATE"));
		LogExtra(((TText8*)(__FILE__)), (__LINE__), ESevrErr, LineBuf);

		LineBuf.Zero();
		LineBuf.AppendFormat(_L("%S: "), &info().iName);

		LineBuf.AppendFormat(_L("Mtu: %d Speed: %d "), info().iMtu, info().iSpeedMetric);
		LogExtra(((TText8*)(__FILE__)), (__LINE__), ESevrErr, LineBuf);

		LineBuf.Zero();
		LineBuf.AppendFormat(_L("%S: "), &info().iName);


		TName address;
		info().iAddress.Output(address);
		LineBuf.Append(_L("Address "));
		//RemoveScopeFromAddress(address);
		LineBuf.Append(address);		
		TName netmask;
		info().iNetMask.OutputWithScope(netmask);
		LineBuf.Append(_L("/"));
		LineBuf.Append(netmask);
		info().iAddress.IsLinkLocal() ? LineBuf.Append(_L("   is Link local")) :
		info().iAddress.IsSiteLocal() ? LineBuf.Append(_L("   is Site local")) :
		LineBuf.Append(_L("     is Global"));


		LogExtra(((TText8*)(__FILE__)), (__LINE__), ESevrErr, LineBuf);
		LineBuf.Zero();
		LineBuf.AppendFormat(_L("%S: "), &info().iName);


		info().iBrdAddr.Output(address);
		//RemoveScopeFromAddress(address);
		TChar colon(':');
		// Assume single IPv4 addres per interface
		if(address.Locate(colon) < 0)
			{
			numberOfInterfaces++;
			}
		// 2 == :: ie zeros
		if(address.Length() > 3)
			LogExtra(((TText8*)(__FILE__)), (__LINE__), ESevrErr, _L("%S: Broadcast address %S"), &info().iName, &address);
		info().iDefGate.Output(address);
		//RemoveScopeFromAddress(address);
		if(address.Length() > 3)
			LogExtra(((TText8*)(__FILE__)), (__LINE__), ESevrErr, _L("%S: Default gateway %S"), &info().iName, &address);
		info().iNameSer1.Output(address);
		//RemoveScopeFromAddress(address);
		if(address.Length() > 3)
			LogExtra(((TText8*)(__FILE__)), (__LINE__), ESevrErr, _L("%S: Primary DNS %S"), &info().iName, &address);
		info().iNameSer2.Output(address);
		//RemoveScopeFromAddress(address);
		if(address.Length() > 3)
			LogExtra(((TText8*)(__FILE__)), (__LINE__), ESevrErr, _L("%S: Secondary DNS %S"), &info().iName, &address);

	

		if(info().iFeatures & KIfIsLoopback) 
			{
			LineBuf.Append(_L(" LOOPBACK"));
			}
		if(info().iFeatures & KIfIsDialup)
			{
			LineBuf.Append(_L(" DIALUP"));
			}
		if(info().iFeatures & KIfIsPointToPoint)
			{
			LineBuf.Append(_L(" POINTTOPOINT"));
			}
		if(info().iFeatures & KIfCanBroadcast)
			{
			LineBuf.Append(_L(" BROADCAST"));
			}
		
		if(info().iFeatures & KIfCanMulticast)
			{
			LineBuf.Append(_L(" MULTICASTCAST"));
			}
		if(info().iFeatures & KIfCanSetMTU)
			{
			LineBuf.Append(_L(" SETMTU"));   
			}
		if(info().iFeatures & KIfCanSetHardwareAddr)
			{
			LineBuf.Append(_L(" CANSETHWADDR"));
			}

		LogExtra(((TText8*)(__FILE__)), (__LINE__), ESevrErr, LineBuf);
		LineBuf.Zero();


		if(info().iFeatures & KIfHasHardwareAddr)
			{
			LogExtra(((TText8*)(__FILE__)), (__LINE__), ESevrErr, _L(" Hardware address "));		
			TUint j;
			for(j=sizeof(SSockAddr) ; j<sizeof(SSockAddr)+6 ; ++j)
				{
				if(j<(TUint)info().iHwAddr.Length())
					LogExtra(((TText8*)(__FILE__)), (__LINE__), ESevrErr, _L("%02X"), info().iHwAddr[j]);
				else
					LogExtra(((TText8*)(__FILE__)), (__LINE__), ESevrErr, _L("??"));
				if(j<sizeof(SSockAddr)+5)
				   LogExtra(((TText8*)(__FILE__)), (__LINE__), ESevrErr, _L("-"));
				else
					LogExtra(((TText8*)(__FILE__)), (__LINE__), ESevrErr, _L(""));
				}
			}
			err = sock.iObj.GetOpt(KSoInetNextInterface, KSolInetIfCtrl, info);
			
		}
		
	sock.Pop();
	ss.Pop();
	
//	LogExtra(((TText8*)(__FILE__)), (__LINE__), ESevrErr, _L("EPOC32 IP Stack Version %d.%d.%d ---- INTERFACE LIST END"), in.iVersion.iMajor, in.iVersion.iMinor, in.iVersion.iBuild);

	return numberOfInterfaces;
	}

	
/**
 * Compares first two non-loopback link-local addresses
 * @return TRUE if equal else FALSE
 */
	TBool CTS_MultiHomingSuite::CompareLinkLocalAddressesL()
	{
		
		TName  addresses[2];
		TAutoClose<RSocketServ> ss;
		TInt err = ss.iObj.Connect();
		ss.PushL();
		TAutoClose<RSocket> sock;
		err = sock.iObj.Open(ss.iObj, _L("udp"));
		sock.PushL();

		User::LeaveIfError(sock.iObj.SetOpt(KSoInetEnumInterfaces, KSolInetIfCtrl));
		
		TPckgBuf<TSoInetInterfaceInfo> info;
		err = sock.iObj.GetOpt(KSoInetNextInterface, KSolInetIfCtrl, info);
		// The first interface is always empty
		err = sock.iObj.GetOpt(KSoInetNextInterface, KSolInetIfCtrl, info);
		
		TInt i = 0;		
		while((i < 2) && (err== KErrNone))
			{
			// Skip loopback interfaces
			if(info().iFeatures & KIfIsLoopback) 
				{
				err = sock.iObj.GetOpt(KSoInetNextInterface, KSolInetIfCtrl, info);
				continue;
				}		
							
			if (info().iAddress.IsLinkLocal())
				{
				//TName address;
				//addresses[i].FillZ();				
				info().iAddress.Output(addresses[i]);						
				RemoveScopeFromAddress(addresses[i]);
				i++;
				}

			err = sock.iObj.GetOpt(KSoInetNextInterface, KSolInetIfCtrl, info);
			}

		sock.Pop();
		ss.Pop();
		
		if ((i == 2) && (addresses[0].Compare(addresses[1]) == 0))					
			return TRUE;
		return FALSE;
	}

