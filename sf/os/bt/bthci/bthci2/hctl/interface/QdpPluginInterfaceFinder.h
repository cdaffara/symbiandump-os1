// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedPartner
 @released
*/

#ifndef QDPPLUGININTERFACEFINDER_H
#define QDPPLUGININTERFACEFINDER_H

#include <e32cmn.h>

/**
Mixin for the QDP Plugin Interface Finder. This finds
any QDP interfaces externally available via Uid from the
command queue.
*/
class MQdpPluginInterfaceFinder
	{
public:
	virtual TAny* MqpifQdpPluginInterface(TUid aUid) = 0;
	};

#endif // QDPPLUGININTERFACEFINDER_H
