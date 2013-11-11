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
// CAF wrapper classes for Multimedia.
// 
//

#include <mm/mmcaf.h>

EXPORT_C TMMSource::TMMSource(TUid aSourceType, const TDesC& aUniqueID, ContentAccess::TIntent aIntent)
	: iSourceType(aSourceType),
	  iUniqueId(aUniqueID),
	  iIntent(aIntent),
	  iEnableUI(EFalse)
	{
	}

EXPORT_C TMMSource::TMMSource(TUid aSourceType, const TDesC& aUniqueID, ContentAccess::TIntent aIntent, TBool aEnableUI)
	: iSourceType(aSourceType),
	  iUniqueId(aUniqueID),
	  iIntent(aIntent),
	  iEnableUI(aEnableUI)
	{
	}

EXPORT_C const TDesC& TMMSource::UniqueId() const
	{
	return iUniqueId;
	}

EXPORT_C ContentAccess::TIntent TMMSource::Intent() const
	{
	return iIntent;
	}
	
EXPORT_C TUid TMMSource::SourceType() const
	{
	return iSourceType;
	}

EXPORT_C TBool TMMSource::IsUIEnabled() const
	{
	return iEnableUI;
	}
	
EXPORT_C TMMFileSource::TMMFileSource(const TDesC& aName)
	: TMMSource(KUidMMFileSource, ContentAccess::KDefaultContentObject, ContentAccess::EUnknown),
	  iName(aName)
	{
	}

EXPORT_C TMMFileSource::TMMFileSource(const TDesC& aName, const TDesC& aUniqueId)
	: TMMSource(KUidMMFileSource, aUniqueId, ContentAccess::EUnknown),
	  iName(aName)
	{
	}

EXPORT_C TMMFileSource::TMMFileSource(const TDesC& aName, const TDesC& aUniqueId, ContentAccess::TIntent aIntent)
	: TMMSource(KUidMMFileSource, aUniqueId, aIntent),
	  iName(aName)
	{
	}
	
EXPORT_C TMMFileSource::TMMFileSource(const TDesC& aName, const TDesC& aUniqueId, ContentAccess::TIntent aIntent, TBool aEnableUI)
	: TMMSource(KUidMMFileSource, aUniqueId, aIntent, aEnableUI),
	  iName(aName)
	{
	}	

EXPORT_C TMMFileHandleSource::TMMFileHandleSource(RFile& aFile)
	: TMMSource(KUidMMFileHandleSource, ContentAccess::KDefaultContentObject, ContentAccess::EUnknown),
	  iFile(aFile)
	{
	}

EXPORT_C TMMFileHandleSource::TMMFileHandleSource(RFile& aFile, const TDesC& aUniqueId)
	: TMMSource(KUidMMFileHandleSource, aUniqueId, ContentAccess::EUnknown),
	  iFile(aFile)
	{
	}

EXPORT_C TMMFileHandleSource::TMMFileHandleSource(RFile& aFile, const TDesC& aUniqueId, ContentAccess::TIntent aIntent)
	: TMMSource(KUidMMFileHandleSource, aUniqueId, aIntent),
	  iFile(aFile)
	{
	}

EXPORT_C TMMFileHandleSource::TMMFileHandleSource(RFile& aFile, const TDesC& aUniqueId, ContentAccess::TIntent aIntent, TBool aEnableUI)
	: TMMSource(KUidMMFileHandleSource, aUniqueId, aIntent, aEnableUI),
	  iFile(aFile)
	{
	}

EXPORT_C const TDesC& TMMFileSource::Name() const
	{
	return iName;
	}

EXPORT_C RFile& TMMFileHandleSource::Handle() const
	{
	return iFile;
	}
