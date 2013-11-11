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
//

/**
 @file
 @internalComponent
 @deprecated since v9.1. Functionality is replaced with commsdat.
*/

#include <comms-infras/commsdebugutility.h>


#if !defined(__COMMSDB_LOG_H__)
#define __COMMSDB_LOG_H__

/**
@internalComponent
*/
/**
	The name is used as a parameter for subsystem filter within comsdbg.ini
  	It should be used in cases when subcomponent is not defined
*/
	_LIT8(KDefaultSubSustem,"default");
/**
	The name is used as a parameter for component name within comsdbg.ini
	It should be used as main component for all CommsDat logs.
*/
	_LIT8(KLogComponent,"commdbshim");

/**
	The name is used as a parameter for subsystem filter within comsdbg.ini
	It will mark error logs
*/
	_LIT8(KCDErrLog,"commsdatErr");

/**
	The name is used as a parameter for subsystem filter within comsdbg.ini
	It will mark error logs
*/
	_LIT8(KCDInfoLog,"commsdatInfo");

/**
	The name is used as a parameter for subsystem filter within comsdbg.ini
	It will mark error logs
*/
	_LIT8(KCommDbShim,"commdbshim");

#endif // __COMMSDB_LOG_H__
