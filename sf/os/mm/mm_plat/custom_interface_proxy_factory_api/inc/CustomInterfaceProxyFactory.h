/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CustomInterface Proxy Factory
*
*/

#ifndef __CUSTOMINTERFACEPROXYFACTORY_H
#define __CUSTOMINTERFACEPROXYFACTORY_H

#include <e32std.h>
#include <MCustomCommand.h>
#include <CustomInterfaceUtility.h>

class CCustomInterfaceProxyFactory
	{
public:
	IMPORT_C static TAny* CreateProxy(TUid aInterfaceUid,
					 TMMFMessageDestinationPckg aHandlePckg,
                     MCustomCommand& aCustomCommand,
                      CCustomInterfaceUtility* aCustomInterfaceUtility);
	};

#endif

