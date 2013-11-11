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
// Server-side base-classes for graphic drawer plugins
// 
//

#ifndef __WSGRAPHICDRAWERFACTORY_H__
#define __WSGRAPHICDRAWERFACTORY_H__

#include <e32base.h>

class MWsGraphicDrawerEnvironment;
struct TGraphicDrawerId;
class MWsClient;
class CWsGraphicDrawer;

NONSHARABLE_CLASS(WsGraphicDrawer)
	{
public:
	IMPORT_C static CWsGraphicDrawer* CreateLC(TUid aType,MWsGraphicDrawerEnvironment& aEnv,const TGraphicDrawerId& aId,MWsClient& aOwner,const TDesC8& aData);
	IMPORT_C static void FinalClose();
private:
	WsGraphicDrawer();
	};

#endif //__WSGRAPHICDRAWERFACTORY_H__
