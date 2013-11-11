// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CLECONST_H__
#define __CLECONST_H__


enum TCleSrvCommand
	{
	ECleSrvExecuteCmdList = 1,
	ECleSrvExecuteCmdListCancel,
	//Only for testing purpose and used in debug builds only
	EDebugMarkHeap,
	EDebugMarkHeapEnd,
	EDebugSetHeapFailure,
	EDebugHeapReset,
	EDebugCloseNeverUnloadLibs,
	EEndOfCleOpCodes
	};

const TInt KCleSrvVersionMajor = 0; // 0-127
const TInt KCleSrvVersionMinor = 0; // 0-99
const TInt KCleSrvVersionBuild = 0; // 0-32767
	
#define __IN_RANGE( x, y ) ( (x >= 0) && (x < y) )	
	
_LIT( KCleCmdListBadIdx,"CleCmdListBadIdx" );	
_LIT( KCleCmdListEmpty,"CleCmdListEmpty" );
_LIT( KCleSrvBadIdx,"CleSrvBadIdx" );
_LIT( KCmdListExecRunL, "CmdListExecRunL" );
_LIT( KCleCmdListNULL,"CleCmdListNULL" );
_LIT( KCleSrvBadExec,"CleSrvBadExec" );
_LIT( KCleHandleNotOpen, "CleCliNotopen" );

const TInt KCleSrvArrayIndexInvalid = 111;
const TInt KCleSrvBadCmdList = 112;
const TInt KCleSrvExecutorInvalid = 113;
const TInt KCleSrvHandleNotOpen = 114;

const TInt KCleStreamBufGranularity = 1024;


#endif // __CLECONST_H__


