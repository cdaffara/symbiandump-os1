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
* Description:  Proxy CustomInterface Utility
*
*/

#ifndef __CPROXYCUSTOMINTERFACEUTILITY_H
#define __CPROXYCUSTOMINTERFACEUTILITY_H

#include <e32std.h>
#include <MCustomCommand.h>
#include <CustomInterfaceUtility.h>

class CProxyCustomInterfaceUtility : public CCustomInterfaceUtility
	{
public:

	IMPORT_C static CProxyCustomInterfaceUtility* NewL(CCustomCommandUtility* aCustomCommandUtility);
	virtual ~CProxyCustomInterfaceUtility();

	IMPORT_C TAny* CustomInterface(TUid aInterfaceId);
	IMPORT_C void RemoveCustomInterface(TMMFMessageDestinationPckg aMessageHandler);

	IMPORT_C static CProxyCustomInterfaceUtility* NewL(MCustomCommand& aMCustomCommand);

private:
	CProxyCustomInterfaceUtility(CCustomCommandUtility* aCustomCommandUtility);
    CProxyCustomInterfaceUtility(MCustomCommand& aMCustomCommand);
	void ConstructL();
	
	TAny* BuildProxy(TUid                       aInterfaceId,
	                 TMMFMessageDestinationPckg aHandle);



	CCustomCommandUtility* iCustomCommandUtility;
	TMMFMessageDestinationPckg    iBuilderHandle;
    MCustomCommand*         iMCustomCommand;
	};

#endif

