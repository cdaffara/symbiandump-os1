// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// data which identifies the server used to communicate with the LBS
// Network Request Handler
// 
//

/**
 @file
 @internalComponent
 @released
*/

#ifndef __LBSNRHSERVERDATA_H__
#define __LBSNRHSERVERDATA_H__

/**
 * Define the file and process name used to set up a session with the
 * Network Request Handler
 */
_LIT(KNrhServerFileName, "\\sys\\bin\\lbsnetworkrequesthandler.exe");
_LIT(KNrhServerName, "lbsnetworkrequesthandler.exe");

/* Current version number of the Network Request Handler Server.
*/
const TInt8 KNrhServerMajorVersionNumber 	= 1;
const TInt8 KNrhServerMinorVersionNumber 	= 0;
const TInt16 KNrhServerBuildVersionNumber	= 0;

/* Enum that defines the types of subsession
   supported by the Network Request Handler.
   
   One of these is passed in as the first
   RMessage2 argument when creating a sub-session.
*/
enum TNrhSubSessionType
	{
	ENrhSubSessionUnknown = 0,
	ENrhSubSessionPrivacyController,
	ENrhSubSessionX3P
	};

/** Current version number of MLbsPrivacyObserver.
*/
const TInt8 KLbsPrivacyObserverVersionMajor = 0; 
const TInt8 KLbsPrivacyObserverVersionMinor = 0; 
const TInt16 KLbsPrivacyObserverVersionBuild = 0; 


/** Current version number of MLbsPrivacyObserver2.
*/
const TInt8 KLbsPrivacyObserverVersion2Major = 2; 
const TInt8 KLbsPrivacyObserverVersion2Minor = 0; 
const TInt16 KLbsPrivacyObserverVersion2Build = 0; 


/* Initialisation data sent to the NRH server
   when creating a new privacy controller subsession.
*/
NONSHARABLE_CLASS(TNrhPrivacyControllerData)
	{
public:
	TVersion iVersion; /** Version of the MLbsPrivacyObserver the client is using. */
	};

/* Initialisation data sent to the NRH server
   when creating a new X3P subsession.
*/
NONSHARABLE_CLASS(TNrhX3PData)
	{
public:
	TInt aUnused; /* Reserved for future use */
	};


#endif // __LBSNRHSERVERDATA_H__

