// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SUSUTILCLISRV_H__
#define __SUSUTILCLISRV_H__

/**
@internalComponent
@released
*/
_LIT(KSusUtilServerName, "!SusUtilSrv");

/**
A version must be specifyed when creating a session with the server
@internalComponent
@released
*/
const TUint KSusUtilServMajorVersionNumber=0;
/**
@internalComponent
@released
*/
const TUint KSusUtilServMinorVersionNumber=1;
/**
@internalComponent
@released
*/
const TUint KSusUtilServBuildVersionNumber=1;

/**
@internalComponent
@released
*/
const TInt  KSusUtilServFirstUsedOpCode = 1;

/**
Op-codes used in message passing between client and server.
@internalComponent
@released
@capability DiskAdmin PowerMgmt ProtServ SwEvent WriteDeviceData ReadDeviceData
*/
enum TSusUtilServerRequests
	{
	ERequestLoadSup = KSusUtilServFirstUsedOpCode,
	ERequestLoadSupCancel,
	ERequestUnLoadSup,
	//Only for testing purpose and used in debug builds only
	EDebugMarkHeap,
	EDebugMarkHeapEnd,
	EDebugCompressPluginArray,
	//End of supported opcodes
	EEndOfSusUtilOpCodes
	};

#endif
