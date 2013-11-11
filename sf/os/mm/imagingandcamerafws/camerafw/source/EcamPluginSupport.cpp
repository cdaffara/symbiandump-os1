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
#include <mm/mmpluginutils.h>

#include <ecamuids.hrh>

//
// CCameraPlugin
//

EXPORT_C TInt CCameraPlugin::CamerasAvailable()
	{
	CCameraInfoPlugin* info = NULL;
	TRAPD(error, info = CCameraInfoPlugin::NewL());
	if (error!=KErrNone)
		{
		// error during open, default to 0
		// TODO - can we return an error here?
		return 0;
		}
	TInt result = info->CamerasAvailable();
	delete info;
	REComSession::FinalClose(); // don't have to do this here, but might as well tidy up
	return result;
	}
	
CCameraPlugin* CCameraPlugin::NewL(TInt aCameraVersion)
	{
	TUid interfaceUid = {KUidOnboardCameraPlugin};
	TUid dtor;

#ifdef _DEBUG
	CCameraPlugin* self = 
		static_cast<CCameraPlugin*>
			(MmPluginUtils::CreateImplementationL(interfaceUid, dtor, KECamPluginMatchString));
#else
	CCameraPlugin* self = 
		static_cast<CCameraPlugin*>
			(MmPluginUtils::CreateImplementationL(interfaceUid, dtor, KECamPluginMatchString, KRomOnlyResolverUid));
#endif

	//create CCameraStructure here
	self->iCameraStructure = new CCameraStructure();
	if (!self->iCameraStructure)
		{
		delete self;
		REComSession::DestroyedImplementation(dtor);
		User::Leave(KErrNoMemory);
		}
	else
		{
		self->iCameraStructure->iCameraVersion = aCameraVersion;
	 	self->iCameraStructure->iDestructorKey = dtor;	
		}
	return self;
	}

CCameraPlugin* CCameraPlugin::NewLC(TInt aCameraVersion)
	{
	CCameraPlugin* self = NewL(aCameraVersion);
	CleanupStack::PushL(self);
	return self;	
	}

EXPORT_C CCameraPlugin* CCameraPlugin::NewL(MCameraObserver2& aObserver,TInt aCameraIndex,TInt aPriority,TInt aCameraVersion)
	{
	CCameraPlugin* self = NewLC(aCameraVersion);
	self->Construct2L(aObserver, aCameraIndex, aPriority);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CCameraPlugin* CCameraPlugin::NewDuplicateL(MCameraObserver2& aObserver,TInt aCameraHandle,TInt aCameraVersion)
	{
	CCameraPlugin* self = NewLC(aCameraVersion);
	self->Construct2DupL(aObserver, aCameraHandle);
	CleanupStack::Pop(self);
	return self;
	}


EXPORT_C CCameraPlugin* CCameraPlugin::NewL(MCameraObserver& aObserver,TInt aCameraIndex,TInt aCameraVersion)
	{
	CCameraPlugin* self = NewLC(aCameraVersion);
	self->Construct2L(aObserver, aCameraIndex);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CCameraPlugin* CCameraPlugin::NewDuplicateL(MCameraObserver& aObserver,TInt aCameraHandle,TInt aCameraVersion)
	{
	CCameraPlugin* self = NewLC(aCameraVersion);
	self->Construct2DupL(aObserver, aCameraHandle);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CCameraPlugin::~CCameraPlugin()
	{
	if (iCameraStructure)
		{
		REComSession::DestroyedImplementation(iCameraStructure->iDestructorKey);
		}
	delete iCameraStructure;
	}
	
EXPORT_C CCameraPlugin::CCameraPlugin()
	{
	}
	
EXPORT_C TInt CCameraPlugin::CameraVersion()
	{
	return iCameraStructure->iCameraVersion;
	}

