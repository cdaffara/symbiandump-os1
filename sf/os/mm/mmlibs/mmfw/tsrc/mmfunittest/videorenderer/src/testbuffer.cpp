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

#include <videoframebuffer.h>
#include "testbuffer.h"


RTestVideoFrameBufferStep* RTestVideoFrameBufferStep::NewL()
	{
	RTestVideoFrameBufferStep* self = new (ELeave) RTestVideoFrameBufferStep();
	return self;	
	}
	
RTestVideoFrameBufferStep::RTestVideoFrameBufferStep()
	{
	iTestStepName = _L("MM-MMF-VIDREND-U-0001");
	}
	
TVerdict RTestVideoFrameBufferStep::DoTestStepL()
	{
	INFO_PRINTF1(_L("Check TVideoFrameBuffer"));
	
	TVerdict result = EPass;

	const TInt KStride = 8;
	const TInt KBufferId = 2;
	const TInt KOffsetInChunk = 4;
	const TInt KChunkSize = 20;
	const TInt KMaxChunkSize = 40;
	RChunk chunk;
	TInt err = chunk.CreateLocal(KChunkSize, KMaxChunkSize);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Error - RChunk::CreateLocal returned %d"), err);
		result = EFail;
		}
	
	TUncompressedVideoFormat testFormat;
	testFormat.iDataFormat = ERgbRawData;
	testFormat.iRgbFormat = ERgb32bit888;
	
	TVideoFrameBuffer buffer(testFormat, KStride, KBufferId, chunk, KOffsetInChunk);
	
	if (buffer.Stride() != KStride)
		{
		ERR_PRINTF2(_L("Error - unexpected stride %d returned"), buffer.Stride());
		result = EFail;
		}
	else if (buffer.BufferId() != KBufferId)
		{
		ERR_PRINTF2(_L("Error - unexpected buffer id %d returned"), buffer.BufferId());
		result = EFail;
		}
	else if (buffer.Chunk().Handle() != chunk.Handle())
		{
		ERR_PRINTF1(_L("Error - unexpected chunk returned"));
		result = EFail;
		}
	else if (buffer.Buffer() != chunk.Base()+KOffsetInChunk)
		{
		ERR_PRINTF1(_L("Error - unexpected buffer"));
		result = EFail;
		}
	else if (!(buffer.Format() == testFormat))
		{
		ERR_PRINTF1(_L("Error - unexpected format returned"));
		result = EFail;
		}
	
	chunk.Close();
	return result;
	}
