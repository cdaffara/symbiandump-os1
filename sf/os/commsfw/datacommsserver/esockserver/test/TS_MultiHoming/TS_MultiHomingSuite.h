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
// Header for CTS_MultiHomingSuite class. This is the container
// class for all the MultiHoming test steps
// 
//

/**
 @file
*/

#if (!defined __TS_MULTIHOMING_H__)
#define __TS_MULTIHOMING_H__

#include <e32std.h>
#include <c32comm.h>

#include <es_sock.h>
#include <in_sock.h>
#include <networking/testsuite.h>
#include <commdbconnpref.h>

#define MAX_NAME_LENGTH		10			//< Maximum length of connection name
#if defined (__WINS__)
#define PDD_NAME _L("ECDRV")
#define LDD_NAME _L("ECOMM")
#else
#define PDD_NAME _L("EUART1")
#define LDD_NAME _L("ECOMM")
#endif


_LIT (KTxtVersion,"1.00");				//< Version string for ScheduleTest



class CTS_MultiHomingStep;
class CEnhancedScheduler;

/**
 * Struct to hold the connection details
 * Contains a Socketserver and a connection
 */
class TConnDetails
{
public:

	TBool MatchName(const TDesC& aName) 
		{
		TInt ret = iName.Compare(aName);
		if (ret==0)
			return ETrue;
		return EFalse;
		}
	TConnDetails() : iConnectionType(undefined), iClients(0), iNumStarts(0) {}

	// Type of connection
	enum { undefined, implicitConn, explicitConn, defaultConn } iConnectionType;

	RConnection iConnection;		//< Reference to the Connection object
	RSocketServ iSocketServ;		//< Reference to socket server object
	RSocket		iSocket;			//< Referece to the socket used in implicit connections
	TCommDbConnPref iConnPrefs;		//< Connection Prefererences
	TBuf<MAX_NAME_LENGTH>	iName;	//< Friendly name of connection

	TUint iClients;					//< Number of clients of this connection
private:
	TUint iNumStarts;				//< Number of times this connection has been started
};

class CTS_MultiHomingSuite : public CTestSuite
{
public:
	virtual		~CTS_MultiHomingSuite();
	void		InitialiseL( void );
	void		AddTestStepL( CTS_MultiHomingStep* ptrTestStep );
	TPtrC		GetVersion( void );	

	// Connection details encapsulation
	TConnDetails		*GetNewTConnection( );
	TInt				AddConnection(const TConnDetails& aConn);
	TConnDetails		*GetTConnection(const TDesC& aName);
	RConnection			*GetRConnection(const TDesC& aName);
	RSocketServ			*GetRSocketServ(const TDesC& aName);
	TInt				CloseConnection(const TDesC& aName);
	TInt				StopConnection(const TDesC& aName);
	TInt				DisplayConnectionDetailsL();
	TBool				CompareLinkLocalAddressesL();

	CEnhancedScheduler*		iScheduler;		//< The ActiveScheduler
	enum { MAX_CONNECTIONS = 100 };
private:
	TInt					GetConnIndex(const TDesC& aName);
	TConnDetails	    iConnections[MAX_CONNECTIONS];	//< List of connections
	
};





class CEnhancedScheduler : public CActiveScheduler
	{
	public:
		CEnhancedScheduler() : iEchoSocketCounter(0) {};
		virtual ~CEnhancedScheduler() {};
		
		// Additional counter...
		TInt GetCount() { return iEchoSocketCounter;};
		void IncCount() { iEchoSocketCounter++;	};
		void DecCount() { iEchoSocketCounter--; };

	private:
		TInt	iEchoSocketCounter;
	};



#endif /* __TS_MULTIHOMING_H__ */

