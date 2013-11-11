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


CMMFDevVideoPlay::TBufferOptions::TBufferOptions() : 
	iPreDecodeBufferSize(0), 
	iMaxPostDecodeBufferSize(0), 
	iPreDecoderBufferPeriod(0), 
	iPostDecoderBufferPeriod(0), 
	iMaxInputBufferSize(0), 
	iMinNumInputBuffers(0) 
	{
	}

CMMFDevVideoPlay::TPictureCounters::TPictureCounters() :
	iPicturesSkipped(0),
	iPicturesDecoded(0),
	iPicturesDisplayed(0),
	iTotalPictures(0)
	{
	}


CMMFDevVideoPlay::TBitstreamCounters::TBitstreamCounters() :
	iLostPackets(0),
	iTotalPackets(0)
	{
	}
