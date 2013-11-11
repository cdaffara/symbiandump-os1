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
// tcp_server
// System Includes
// 
//

#include <stdio.h>
#include <stdlib.h>


/***************************************************************************************
 *
 * Local Includes
 *
 **************************************************************************************/
#include "CTcpServer.h"


/***************************************************************************************
 *
 * Prototypes
 *
 **************************************************************************************/
void usage();

/***************************************************************************************
 *
 * main()
 *
 **************************************************************************************/
int main(int argc, char* argv[])
{
	char *local_interface = NULL;
	char *local_port = NULL;
	CTcpServer tcp_server;
	
	// check params
	if( argc < 3 ) {
		usage();
		return E_INCORRECTARG;
	}

	// start the server
	tcp_server.StartServer( argv[1], argv[2] );
	return ITS_OK;
}


//
// Print Usage.
//
void usage()
{
	fprintf( stderr, "tcp_server <local_interface> <local_port>\n" );
}
