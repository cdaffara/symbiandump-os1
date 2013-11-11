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
// Template for writing Anim DLL's
// 
//

/**
 @file
 @test
 @internalComponent
*/

#if !defined(__STRESSTESTANIMCMD_H__)
#define __STRESSTESTANIMCMD_H__

struct TStressTestAnimParams
	{
	TPoint pos;
	TInt interval;
	TInt bit1;
	TInt bit2;
	TInt mask;
	TInt font;
	};
	
enum TStressTestAnimDllOpcodes
	{
	EADllOpcodeMove,
	EADllOpcodePolyLineRect,
	EADllToggleBitmapMask,
	EADllTextPos,
	EADllQuerySync
	};


#endif // __STRESSTESTANIMCMD_H__
