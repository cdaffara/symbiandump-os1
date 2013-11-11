// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Panics in CWsGraphicDrawer
// 
//

#ifndef __GRAPHICDRAWER_PANICS_H__
#define __GRAPHICDRAWER_PANICS_H__

enum TWsGraphicDrawerPanic
	{
	// those suppressed in release builds:
	EWsGraphicDrawerPanicDoubleInit,
	EWsGraphicDrawerPanicBadArgument,
	EWsGraphicDrawerPanicBadAddLCCleanup,
	EWsGraphicDrawerPanicBadSwapLCCleanup
	// those even in release builds:
	};

GLREF_C void Panic(TWsGraphicDrawerPanic aPanic);

#endif //__GRAPHICDRAWER_PANICS_H__
