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
// cs_notificationevent.cpp
// Implementation for XPlaneEvent.
// 
//

/**
 @file
 @internalTechnology
 @prototype
*/


#include <comms-infras/es_event.h>

using namespace ESock;

EXPORT_REGISTER_TYPEID( XPlaneEvent, KEventTypeImplUid, Meta::KNetMetaTypeAny )


EXPORT_C XPlaneEvent* XPlaneEvent::NewL(Meta::STypeId aTypeId)
	{
	return static_cast<XPlaneEvent*>(Meta::SMetaDataECom::NewInstanceL(aTypeId));
	}

EXPORT_C XPlaneEvent::~XPlaneEvent()
	{
	}


