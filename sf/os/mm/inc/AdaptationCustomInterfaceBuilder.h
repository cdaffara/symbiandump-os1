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
* Description:  Adaptation CustomInterface Builder
*
*/

#ifndef __ADAPTATIONCUSTOMINTERFACEBUILDER_H
#define __ADAPTATIONCUSTOMINTERFACEBUILDER_H

#include <e32std.h>

class CMMFObject;

class CAdaptationCustomInterfaceBuilder
	{
public:
	IMPORT_C static CAdaptationCustomInterfaceBuilder* NewL();
	
	IMPORT_C CMMFObject* BuildMessageHandler(TUid  aInterfaceId,
	                                         TAny* aCustomInterface);

private:
	CAdaptationCustomInterfaceBuilder();
	};

#endif
