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

/**
 @file
 @internalComponent
*/

#include <barsread.h>
#include <mmf/devvideo/devvideobase.h>
#include <videorenderer.rsg>
#include "resourcefilereader.h"
#include "rendererutil.h"


CResourceFileReader* CResourceFileReader::NewL(const TDesC& aResourceFile)
	{
	CResourceFileReader* self = CResourceFileReader::NewLC(aResourceFile);
	CleanupStack::Pop(self);
	return self;
	}

CResourceFileReader* CResourceFileReader::NewLC(const TDesC& aResourceFile)
	{
	CResourceFileReader* self = new (ELeave) CResourceFileReader();
	CleanupStack::PushL(self);
	self->ConstructL(aResourceFile);
	return self;
	}

CResourceFileReader::CResourceFileReader()
	{
	}

void CResourceFileReader::ConstructL(const TDesC& aResourceFile)
	{
	User::LeaveIfError(iFs.Connect());
    iResourceFile.OpenL(iFs, aResourceFile);
	}

CResourceFileReader::~CResourceFileReader()
	{
	iResourceFile.Close();
	iFs.Close();
	}

void CResourceFileReader::ReadSupportedFormatL(RArray<TUncompressedVideoFormat>& aArray)
	{
	aArray.Reset(); // first clear the old data

	HBufC8* res = iResourceFile.AllocReadLC(GCE_SUPPORTED_FORMAT);
	TResourceReader reader;
	reader.SetBuffer(res);

	TInt count = reader.ReadInt16();
	for (TInt i = 0; i < count; ++i)
		{
		TVideoRendererPixelFormat format = static_cast<TVideoRendererPixelFormat>(reader.ReadUint32());
		TUncompressedVideoFormat uncompressedFormat = VideoRendererUtil::ConvertPixelFormatToUncompressedVideoFormatL(format);
		aArray.AppendL(uncompressedFormat);
		}
	
	CleanupStack::PopAndDestroy(res);
	}

void CResourceFileReader::ReadTimerInfoL(TInt64& aDefaultDelay, TInt64& aMaxDelay)
	{
	HBufC8* res = iResourceFile.AllocReadLC(TIMER);
	TResourceReader reader;
	reader.SetBuffer(res);
	
	aDefaultDelay = static_cast<TInt64>(reader.ReadInt32());
	aMaxDelay = static_cast<TInt64>(reader.ReadInt32());
	
	CleanupStack::PopAndDestroy(res);
	}
