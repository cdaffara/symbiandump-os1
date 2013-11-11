// SensibleServer_server_specific.h
// 
// Copyright (c) 2006 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifdef SENSIBLESERVER_SERVER_SPECIFIC_H
#error This file should only ever be included from one place
#else
#define SENSIBLESERVER_SERVER_SPECIFIC_H

// Some tweaks to sensibleserver.cpp etc cannot be avoided,
// so we put them all in one place

/* Define this to specify that the server should start up an appropriate CEikonEnv
 */
//#define RUN_SERVER_WITH_EIKONENV

/* This must be the name of your server subclass
 */
#define CServer_Class_Name CCloggerServer

/* Include where the above subclass is defined
 */
#include "CloggerServer.h"

/* Pick a name to identify the server in panic codes
 */
_LIT(KDebugPanic, "Clogger");

#endif