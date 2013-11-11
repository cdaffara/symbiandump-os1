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
// The factory for creating graphic drawer instances
// 
//

#include "WSGRAPHICDRAWERFACTORY.H"
#include "Graphics/WSGRAPHICDRAWER.H"
#include <Graphics/WSGRAPHICDRAWERINTERFACE.H>
#include "panics.h"
#include <bitstd.h>

// WsGraphicDrawer \\\\\\\\\\\\\\\\\\\\\\\\

EXPORT_C CWsGraphicDrawer* WsGraphicDrawer::CreateLC(TUid aType,MWsGraphicDrawerEnvironment& aEnv,const TGraphicDrawerId& aId,MWsClient& aOwner,const TDesC8& aData)
	{
	//static const TUid KWsGraphicDrawerInterface = {0x10281924};
	//ECOM resolution
	CWsGraphicDrawer* drawer = reinterpret_cast<CWsGraphicDrawer*>(REComSession::CreateImplementationL(aType,_FOFF(CWsGraphicDrawer,iDtor_ID_Key)));
	User::LeaveIfNull(drawer);
	CleanupStack::PushL(drawer);
	drawer->ConstructL(aEnv,aId,aOwner,aData);
	return drawer; // drawer is left on stack
	}

EXPORT_C void WsGraphicDrawer::FinalClose()
	{
	REComSession::FinalClose();
	}

