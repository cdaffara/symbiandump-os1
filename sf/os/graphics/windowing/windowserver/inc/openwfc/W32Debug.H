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
// Debug definitions designed to be used by client side test apps
// requiring access to the related special debug features.
// 
//

#ifndef W32DEBUG_H_
#define W32DEBUG_H_

/**
@file
@publishedpartner 
@released
*/

enum TWsDebugInfoFunc
	{
	EWsDebugInfoHeap=1,
	EWsDebugSetCheckHeapOnDisconnectClient,
	EWsDebugSetCheckHeapOnDisconnectMode,
	EWsDebugFetchCheckHeapResult,
	EWsDebugHeartBeatState,
	EWsDebugSetAutoHeartBeatPauseState,
	EWsDebugSetEventQueueTest,
	
	//Insert more unclassified diagnostics above
	
	EWsDebugUnclassifiedMax,	//Please insert new unclassified IDs before this 

	EWsDebugClassMask=				0xFF000000,
	EWsDebugClassUnclassified=		0x00000000,
	
	EWsDebugClassScreenUiElement=		0x01000000,		//specify EWsDebugArgScreenMask
		EWsDebugGetFastpathMode,		//return: TInt
		EWsDebugSetFastpathMode,		//set new mode in EWsDebugArgExtraMask (use 2-param method)
		EWsDebugGetUIElementInfoList,		//return: TInt[[ElementCount], [ElementId, ElementType, IsDisabled] * ElementCount].
		EWsDebugGetUIElementBase,			//return: TRect[]
		EWsDebugSetFastpathTestMode,	//return: TBool
		EWsDebugSetFastpathOomMode,		//return: TInt
		EWsDebugGetUIElementIds,			//return: TInt[]
		EWsDebugGetSceneElementIdOrder,		//return: TInt[]
		EWsDebugGetUIElementConfig,		//return: TSurfaceConfig		//Index UI Element via EWsDebugArgElementMask
		EWsDebugGetUIRegionCommittedTime,	//return: TTime
		EWsDebugGetLastOptimizerError,	//return: TInt
		EWsDebugGetFastpathTypeDetected,	//return: TInt
		
		EWsDebugClassScreenUiElementMax,
	EWsDebugClassScreenElementSet=	0x02000000,		//specify EWsDebugArgScreenMask
		EWsDebugSerialSurfacesUpdated,	//return: TInt - Note non-param version returns value not length
		EWsDebugSurfaceWindowList,		//return: TWsDebugWindowId[]
		
		EWsDebugClassScreenElementMax,
	EWsDebugClassElementSetWindow=	0x03000000,		//specify EWsDebugArgScreenMask, EWsDebugArgWindowMask
		EWsDebugElementIdList,			//return: TInt[]	//entry 0 reserved for background
		EWsDebugBackgroundConfig,		//return: TSurfaceConfig
		EWsDebugBackgroundBase,			//return: TRect[]
		EWsDebugBackgroundFlags,		//return: TInt[2]
		EWsDebugClassElementSetWindowMax,
	EWsDebugClassElementSetElement=		0x04000000,		//specify EWsDebugArgScreenMask, EWsDebugArgWindowMask, EWsDebugArgElementMask
		EWsDebugPlacedConfig,			//return: TSurfaceConfig
		EWsDebugPlacedBase,				//return: TRect[]
		EWsDebugPlacedFlags,			//return: TInt[2]
		
		EWsDebugClassElementSetElementMax,
	//next elementset based class
	EWsDebugClassElementNextAvail=	0x05000000,
		
	EWsDebugClassNonScreen=			0x10000000,
	EWsDebugClassClientWindow=		0x11000000,		//specify client identifier in aParam. Only owned windows can be accessed.
		//Window diagnostics were not used during development so function ids and code have been removed.
	EWsDebugCliWinGroupIdMask=		0x00FFFF00,
	EWsDebugCliWinGroupIdShift=		16,
	
	};

struct TWsDebugWindowId
	{						//Root window has GroupId of 0x0000ffff, and clientid=screen number...
	TInt iClientId;			//Should be unique across all windows per client
	TInt iOtherGroupId;		//If zero, the window is owned by the calling RWsSession instance, 
	};						//else the wserv generated group ID is enough to map to other sessions.
	
/** Use these masks with aParam to index the object being accessed
 * 
 **/
enum TWsDebugClassArgMask
	{
	EWsDebugArgScreenMask=	0x000000ff,
	EWsDebugArgScreenShift=	0,
	EWsDebugArgWindowMask=	0x0000ff00,
	EWsDebugArgWindowShift=	8,
	EWsDebugArgElementMask=	0x00ff0000,
	EWsDebugArgElementShift=	16,
	EWsDebugArgExtraMask=	0xff000000,
	EWsDebugArgExtraShift=	24,
	};

enum TWsCheckHeapOnDisconnectMode
	{
	EWsCheckHeapOnDisconnectModeNone=1,
	EWsCheckHeapOnDisconnectModeOnce,
	EWsCheckHeapOnDisconnectModeAlways,
	};

struct TWsDebugHeapInfo
	{
	TInt iCount;
	TInt iTotal;
	TInt iAvailable;
	TInt iLargestAvailable;
	};

#endif //W32DEBUG_H_
