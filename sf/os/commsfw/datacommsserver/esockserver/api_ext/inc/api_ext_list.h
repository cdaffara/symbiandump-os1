// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @deprecated
*/

#ifndef API_EXT_LIST_H
#define API_EXT_LIST_H

#include <e32base.h>
#include <elements/interfacetable.h>

// Deprecated: Any new api extensions should be allocated as UIDs and defined as an interface
// id field of the corresponding interface class and referenced from there.

#define TSupportedCommsApiExt TInt

enum
/**
Please use <interface>::KInterfaceId instead
@publishedPartner
@deprecated
*/
	{
	EMobilityApiExt = 0,
	ELegacySubConnectionActiveApiExt = 1,
	EDataMonitoringApiExt = 2,
	ELegacyEnumerateSubConnectionsApiExt = 3,

	EAccessPointConfigApi = 10002
	};

#endif
// API_EXT_LIST_H


