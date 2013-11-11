// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <ecam/ecamplugin.h>
#include <ecam/ecaminfoplugin.h>

#include <ecom/ecom.h>
#include <ecamuids.hrh>

#include <mm/mmpluginutils.h>

//
// CCameraInfo
//

CCameraInfoPlugin* CCameraInfoPlugin::NewL()
	{
	TUid interfaceUid = {KUidOnboardCameraInfoPlugin};
	CCameraInfoPlugin* self = 
		static_cast<CCameraInfoPlugin*>
			(MmPluginUtils::CreateImplementationL(interfaceUid, _FOFF(CCameraInfoPlugin,iDestructorKey), KECamPluginMatchString, KRomOnlyResolverUid));
	return self;

	}
	
EXPORT_C CCameraInfoPlugin::~CCameraInfoPlugin()
	{
	REComSession::DestroyedImplementation(iDestructorKey);
	}

EXPORT_C CCameraInfoPlugin::CCameraInfoPlugin()
	{
	}
