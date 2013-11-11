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

#ifndef __RENDERSTAGEMANAGER_H__
#define __RENDERSTAGEMANAGER_H__

#include <graphics/wsrenderstage.h>

/**
@file
@internalTechnology
@prototype
*/

class CRenderStageManager : public CBase
	{
public:
	/**
	Connect to the Render Stage Plugins.
	
	Sets up a render stage chain hierarchy for a given screen and returns a
	pointer to the first render stage.  The caller is responsible for calling
	Release on the returned render stage when it is no longer needed, such as
	during shutdown.
	
	@param	aScreenNumber	Render stage chains created on a per-screen basis
	@param	aScreen			Screen object for the given screen number
	@param	aScreenRedraw	Mechanism to allow render stages to call back into
							the redraw store MWsScreenRedraw to for example cause
							a re-scheduling of render stages to execute.
	*/
	static CWsRenderStage *ConnectL(TInt aScreenNumber, MWsScreen *aScreen, MWsScreenRedraw *aScreenRedraw);
	
	/**
	Release Render Stage Resources.
	
	De-allocates the resources for the given render stage chain.
	
	@param	aRenderStage	Top-level render stage previously rendered by a
							call to @c ConnectL
	*/
	static void Release(CWsRenderStage *aRenderStage);
	};
#endif // __RENDERSTAGEMANAGER_H__
