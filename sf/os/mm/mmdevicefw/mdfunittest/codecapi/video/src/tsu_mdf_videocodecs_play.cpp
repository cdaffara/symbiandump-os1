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
//

#include "tsu_mdf_videocodecs_play.h"
#include <mmf/server/mmfhwdevicesetup.h>
#include "videoplayfile.h"

const TInt KMaxHeapSize = 20000000;
const TInt KTBufSize = 256;

// RTestStepVideoDecoderLoad	
RTestStepVideoDecoderLoad::RTestStepVideoDecoderLoad()
 	{
 	iTestStepName = _L("MM-MDF-VIDEOCODECS-0001");
 	iHeapSize = KMaxHeapSize;
 	}

TVerdict RTestStepVideoDecoderLoad::DoTestStepL()
	{
	__UHEAP_MARK;
	TBuf<KTBufSize> buf;
	buf.Format(_L("Heap Count Start: %d"), User::CountAllocCells());
	InfoMessage(buf);
	iPlayVideoFile = CPlayVideoFile::NewL(this);
	iPlayVideoFile->LoadCodecL();
	delete iPlayVideoFile;
	REComSession::FinalClose();
	buf.Format(_L("Heap Count End: %d"), User::CountAllocCells());
	InfoMessage(buf);
	__UHEAP_MARKEND;
	return Verdict();
	}	

// RTestStepVideoDecoderPlay
RTestStepVideoDecoderPlay::RTestStepVideoDecoderPlay()
 	{
 	iTestStepName = _L("MM-MDF-VIDEOCODECS-0002");
 	iHeapSize = KMaxHeapSize;
 	}

TVerdict RTestStepVideoDecoderPlay::DoTestStepL()
	{
	__UHEAP_MARK;
	TBuf<KTBufSize> buf;
	buf.Format(_L("Heap Count Start: %d"), User::CountAllocCells());
	InfoMessage(buf);
	iPlayVideoFile = CPlayVideoFile::NewL(this);
	iPlayVideoFile->LoadCodecL();
	iPlayVideoFile->SetDSA(EFalse);
	iPlayVideoFile->StartPlaybackL();
	delete iPlayVideoFile;
	REComSession::FinalClose();
	buf.Format(_L("Heap Count End: %d"), User::CountAllocCells());
	InfoMessage(buf);
	__UHEAP_MARKEND;
	return Verdict();
	}	

// RTestStepVideoDecoderPlayDSA
RTestStepVideoDecoderPlayDSA::RTestStepVideoDecoderPlayDSA()
 	{
 	iTestStepName = _L("MM-MDF-VIDEOCODECS-0003");
 	iHeapSize = KMaxHeapSize;
 	}

TVerdict RTestStepVideoDecoderPlayDSA::DoTestStepL()
	{
	__UHEAP_MARK;
	TBuf<KTBufSize> buf;
	buf.Format(_L("Heap Count Start: %d"), User::CountAllocCells());
	InfoMessage(buf);

	iPlayVideoFile = CPlayVideoFile::NewL(this);
	iPlayVideoFile->LoadCodecL();
	iPlayVideoFile->SetDSA(ETrue);
	iPlayVideoFile->StartPlaybackL();
	delete iPlayVideoFile;
	REComSession::FinalClose();
	buf.Format(_L("Heap Count End: %d"), User::CountAllocCells());
	InfoMessage(buf);
	__UHEAP_MARKEND;
	return Verdict();
	}	
