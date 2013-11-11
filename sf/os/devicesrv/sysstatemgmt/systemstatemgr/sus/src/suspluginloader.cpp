// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <f32file.h>
#include <ssm/ssmutility.h>
#include <ssm/ssmsuscli.h>
#include "suspluginloader.h"
#include "suspluginframe.h"
#include "ssmdebug.h"
#include "suspanic.h"

_LIT(KRomDriveLetter, "Z:");

/**
 Loads the specified DLL into an RLibrary. DLL is typechecked and optionally its uid3 can be verified.
 If the RLibrariy::iHandle was opened pushed onto and left on the CleanupStack.
 */
void SusPluginLoader::LoadDllFileLC(RLibrary& aLibrary, const TSsmSupInfo& aSupInfo)
	{
	//Enforce that plugins can only be loaded from ROM
	TBuf<KMaxFileName + 2> filename = aSupInfo.FileName(); //KMaxFileName + KRomDriveLetter().Length()
	if( filename.Left(2).CompareF(KRomDriveLetter) )
		{
		filename.Insert(0, KRomDriveLetter);
		}
	
	const TUid expectedUid3 = aSupInfo.Identity();
	DoLoadDllFileLC(aLibrary, filename, expectedUid3);
	}

/**
 */
CSusPluginFrame* SusPluginLoader::CreatePluginLC(RLibrary& aLibrary, TInt aNewLOrdinal)
	{
	
	CSusPluginFrame* plugin = SusPluginLoader::DoCreatePluginL(aLibrary, aNewLOrdinal);
	CleanupStack::PushL(plugin);
#ifdef _DEBUG 
	TFileName name = aLibrary.FileName(); 
#endif
	DEBUGPRINT2(_L("Utility Plugin [%S] loaded."), &name);	
	
	plugin->InitializeL();
	DEBUGPRINT2(_L("Utility Plugin [%S] initialized."), &name);	
	
	plugin->StartL();
	DEBUGPRINT2(_L("Utility Plugin [%S] started."), &name);	
	
	return plugin;
	}

/**
 Load the DLL file from the file system.
 @leave KErrNotSupported If UID2 of the DLL is not KSsmUtilityPluginDllTypeUid
 @leave KErrNotFound If the requested policy DLL file is missing.
 @leave KErrBadName If @c aSupInfo contains an Identity different from UID3 of the DLL.
 @leave KErrCorrupt Or any other system wide error code that the fileserver can rise.
 */
void SusPluginLoader::DoLoadDllFileLC(RLibrary& aLibrary, const TDesC& aFileName, TUid aExpectedUid3)
	{
	const TInt fileErr = aLibrary.Load(aFileName);
	CleanupClosePushL(aLibrary);
	
	//Leave if we couldn't open aLibrary
	if (fileErr != KErrNone)
		{
		DEBUGPRINT3(_L("Failed to load utility plugin DLL named %S, file error: %d"), &aFileName, fileErr);
		User::Leave(fileErr);
		}
	
	//Check that the DLL is the SUP type
	if (aLibrary.Type()[1] != KSsmUtilityPluginDllTypeUid)
		{
		DEBUGPRINT4(_L("Wrong type (uid2) in the utility plugin DLL %S. Expected %x found %x"), 
				&aFileName, KSsmUtilityPluginDllTypeUid, aLibrary.Type()[1]);;
		User::Leave(KErrNotSupported);
		}
	
	//If we were given a identity, check that the loaded file conforms
	if ( (aExpectedUid3 != KNullUid) && (aExpectedUid3 != aLibrary.Type()[2]) )
		{
		DEBUGPRINT4(_L("Wrong identity (uid3) in the utility plugin DLL %S. Expected %x found %x"), 
				&aFileName, aExpectedUid3, aLibrary.Type()[2]);
		User::Leave(KErrBadName);
		}
	} //lint !e1746 suppress parameter 'aExpectedUid3' could be made const reference

/**
 */
CSusPluginFrame* SusPluginLoader::DoCreatePluginL(const RLibrary& aLibrary, TInt aNewLOrdinal)
	{
	__ASSERT_DEBUG(KNullHandle != aLibrary.Handle(), User::Panic(KPanicSsmSus, EPluginLoaderError1));
	
	CSusPluginFrame* frame = NULL;
	TRAPD(err, frame = CSusPluginFrame::NewL(aLibrary.Lookup(aNewLOrdinal), aNewLOrdinal));
#ifdef _DEBUG
	if(KErrNone != err)
		{
		TFileName name = aLibrary.FileName();
		DEBUGPRINT4(_L("Specified ordinal function (%d) in DLL %S didn't create a new instance, err=%d."), aNewLOrdinal, &name, err);
		}
#endif	
	SSMLOGLEAVEIFERROR(err);
	return frame;
	}

