/**
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file
*/

#ifndef __AMASTARTCONST_H__
#define __AMASTARTCONST_H__

/**
 * @internalTechnology
 * @released
 */
enum TAmaStartSrvCommand
	{
	
	EAmaStartSrvStartDsc = 1,
	EAmaStartSrvStartDscCancel
	};

#define __IN_RANGE( x, y ) ( (x >= 0) && (x < y) )	
	
_LIT( KAmaStartCmdListBadIdx,"AmaStartCmdListBadIdx" );	
_LIT( KAmaStartCmdListEmpty,"AmaStartCmdListEmpty" );
_LIT( KAmaStartSrvBadIdx,"AmaStartSrvBadIdx" );
_LIT( KCmdListExecRunL, "CmdListExecRunL" );
_LIT( KAmaStartCmdListNULL,"AmaStartCmdListNULL" );
_LIT( KAmaStartSrvBadExec,"AmaStartSrvBadExec" );
_LIT( KAmaStartHandleNotOpen, "AmaStartCliNotopen" );

const TInt KAmaStartSrvArrayIndexInvalid = 111;
const TInt KAmaStartSrvBadCmdList = 112;
const TInt KAmaStartSrvExecutorInvalid = 113;
const TInt KAmaStartSrvHandleNotOpen = 114;

const TInt KAmaStartStreamBufGranularity = 1024;


#endif // __AMASTARTCONST_H__
