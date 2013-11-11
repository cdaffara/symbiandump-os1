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
// Generic Event class.
// XEventBase exists to support interworking with old CSubConNotificationEvent class.
// 
//

/**
 @file
 @internalTechnology
 @prototype
*/

#ifndef __ES_EVENT_H__
#define __ES_EVENT_H__

#include <comms-infras/metadata.h>

namespace ESock
{

const TInt KEventTypeItflUid = 0x10272F51;
const TInt KEventTypeImplUid = 0x10272F52;

class XEventBase : public Meta::SMetaDataECom
	{
	};

class XPlaneEvent : public XEventBase
	{
public:
	IMPORT_C static XPlaneEvent* NewL(Meta::STypeId aTypeId);
	IMPORT_C virtual ~XPlaneEvent();

private:
	EXPORT_TYPEID_TABLE
	};

}

#endif // ES_EVENT_H

