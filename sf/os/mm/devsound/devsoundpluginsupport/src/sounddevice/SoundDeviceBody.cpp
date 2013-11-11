// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "SoundDeviceBody.h"
#include <mmf/plugin/devsoundplugin.h>
#include <mmf/plugin/mmfdevsound.hrh>
#include <ecom/ecom.h>
#include <mm/mmpluginutils.h>


/*
 *	Default Constructor.
 */
CMMFDevSound::CBody::CBody()
	{
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Destructor.
 *
 *	Deletes all objects and releases all resource owned by this
 *	instance.
 *
 */
CMMFDevSound::CBody::~CBody()
	{
	delete iPlugin;
	REComSession::DestroyedImplementation(iDestructorKey);
	REComSession::FinalClose();
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Constructs, and returns a pointer to, a new CMMFDevSound::CBody object.
 *
 *	Leaves on failure.
 *
 */
CMMFDevSound::CBody* CMMFDevSound::CBody::NewL()
	{
	CBody* self = new (ELeave) CBody;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Second phase constructor.
 *
 */
void CMMFDevSound::CBody::ConstructL()
	{
	TUid interfaceUid = {KMmfUidDevSoundPlugin};
	iPlugin = 
		static_cast<MMMFDevSoundPlugin*>
			(MmPluginUtils::CreateImplementationL(interfaceUid, iDestructorKey, KDevSoundPluginMatchString, KRomOnlyResolverUid));
	}



