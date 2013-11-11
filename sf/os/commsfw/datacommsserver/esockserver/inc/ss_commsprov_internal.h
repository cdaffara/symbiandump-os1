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
//

/**
 @file
 @internalTechnology
*/

#ifndef SYMBIAN_SS_COMMSPROV_INTERNAL_H
#define SYMBIAN_SS_COMMSPROV_INTERNAL_H

#include <comms-infras/api_ext_list.h>
#include <comms-infras/ss_rmetaextensioncontainer.h>

namespace ESock
{
class RMetaExtensionContainerC;

class MAccessPointConfigApi
/**
@internalTechnology
*/
	{
public:
	static const TInt KInterfaceId = EAccessPointConfigApi;

	virtual const RMetaExtensionContainerC& GetAccessPointConfig() const = 0;
	};


class CCommsProviderBase : public CBase, public NetInterfaces::AApiExtBase
/**
Base class for comms providers.

@internalTechnology
*/
	{
public:
	IMPORT_C RMetaExtensionContainerC& AccessPointConfig() const;
    	
	IMPORT_C ~CCommsProviderBase();

protected:
    mutable RMetaExtensionContainerC iAccessPointConfig;
	};

} //namespace ESock

#endif
//SYMBIAN_SS_COMMSPROV_H

