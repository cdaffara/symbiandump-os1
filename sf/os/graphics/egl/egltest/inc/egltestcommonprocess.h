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

/**
 @file
 @test
*/

#ifndef EGLTESTCOMMONPROCESS_H
#define EGLTESTCOMMONPROCESS_H

#include <e32base.h>
#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
#include <sgresource/sgresource.h>
#else
#include <graphics/sgresource.h>
#endif //SYMBIAN_GRAPHICS_EGL_SGIMAGELITE

_LIT(KEglTestServerWrapperProcess, "egltestcommonprocess.exe");

struct TProcessInfo
	{
	TInt 					 iIdx; // << index for example used to identify the thread to semaphores
	TSgDrawableId			 iSgId;
	};

enum TEglTestProcessSlots
	{
	EProcSlotTestDllName=1, // can't use slot 0
	EProcSlotTestStepName,
	EProcSlotParams,
	EProcSlotSemaphore0,
	EProcSlotSemaphore1,
	EProcSlotMsgQueueSgId,
	EProcSlotMsgQueueProcId,
	EProcSlotSourceFormat,
	EProcSlotSurfaceFormat,
	EProcSlotCustomClientParam //data that could be sent from client to main process. Concrete implementation will need to specify the format
	};

class EglTestCommonProcess
	{
	public:
		static void MainL();
	};

#endif // EGLTESTCOMMOMPROCESS_H
