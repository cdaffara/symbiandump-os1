// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// test_client_wintunnel.cpp
// System Includes
// 
//

#include <e32base.h>
#include <e32test.h>
#include <e32svr.h>
#include <c32comm.h>

/********************************************************************************
 *
 * Local Includes
 *
 *******************************************************************************/
#include "f32file.h"
#include "CommonFiles.h"

/********************************************************************************
 *
 * Constants
 *
 *******************************************************************************/

// Driver names
_LIT(PDD_NAME,"ECDRV");
_LIT(LDD_NAME,"ECOMM");
_LIT(CSY_NAME, "ECUART" );
_LIT(PORT_NAME,"COMM::1");

// Test data
const TInt DATA_STRING_LENGTH (45);
_LIT8(DATA_STRING,"Diana is the greatest person who ever lived\n\r");

// Other
const TTimeIntervalMicroSeconds32 KTimeOut(4000000);

/********************************************************************************
 *
 * Prototypes
 *
 *******************************************************************************/
LOCAL_C void Init();

/********************************************************************************
 *
 * Implementation
 *
 *******************************************************************************/
LOCAL_C void doExampleL()
    {
	_LIT(KStatus0,"Connect to file server\n");
	_LIT(KStatus1,"Connect to comm server\n");
	_LIT(KStatus2,"Load ECUART.CSY\n");
	_LIT(KStatus3,"Open COMM::1\n");
	_LIT(KStatus4,"Writing to the port...\n");
	_LIT(KStatus4a,"Reading from the port...\n");
	_LIT(KStatus5,"Closing the port...\n");
	_LIT(KStatus6,"Closing the server connection...\n");
	_LIT(KSuccess,"Successfully read string from the port.\n");
	_LIT(KFailure,"Failed reading string from the port.\n");


	// connect to the file server
	console->Printf( KStatus0 );
	RFs f;
	User::LeaveIfError(f.Connect());
	f.Close();

	// load up the drivers
	Init();

	// start the serial server			
	RCommServ server;
	console->Printf(KStatus1);
	server.Connect();

	// load the plugin
	console->Printf(KStatus2);
	TInt ret=server.LoadCommModule(CSY_NAME);
	User::LeaveIfError(ret);
		
	// open the port
	RComm commPort;
	console->Printf(KStatus3);
	ret=commPort.Open(server,PORT_NAME,ECommExclusive);
	User::LeaveIfError(ret);

	// write to the port
	TRequestStatus status;
	console->Printf(KStatus4);
	commPort.Write(status,KTimeOut,DATA_STRING);
	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());

	// read from the port 
	TBuf8<DATA_STRING_LENGTH> rbuf;
	console->Printf(KStatus4a);
	commPort.Read( status, rbuf ); 
	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());

	//check the string matches
	if(rbuf==DATA_STRING)
		{
		console->Printf(KSuccess);
		}
	else
		{
		console->Printf(KFailure);
		}

	// close port and server
	console->Printf(KStatus5);
	commPort.Close();
	console->Printf(KStatus6);
	server.Close();
	}


LOCAL_C void Init()
//
// Initialisation code - loads the serial LDD and PDD
// starts the comm subsystem (for EPOC32 builds)
// On a full Symbian OS implementation, this code would not
// be required because higher level GUI components  
// automatically start the services.
//
	{
	// Load the physical device driver
	// The OS will automatically append .PDD and 
	// search /System/Libs on all drives.

	TInt r=User::LoadPhysicalDevice(PDD_NAME);
	if (r != KErrNone && r!= KErrAlreadyExists)
		User::Leave(r);
	r=User::LoadLogicalDevice(LDD_NAME);
	if (r != KErrNone && r != KErrAlreadyExists)
		User::Leave(r);

#if defined (__EPOC32__)
	// For EPOC builds we need to start the comms subsystem
	// This call actually starts the comms server process
	r=StartC32();
	if (r != KErrAlreadyExists)
		User::LeaveIfError(r);
#endif
	}


