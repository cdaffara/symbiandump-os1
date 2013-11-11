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

#include "tsu_mdf_videocodecs_play_cov.h"
#include <mmf/server/mmfhwdevicesetup.h>
#include "videoplayfile_cov.h"

const TInt KMaxHeapSize = 20000000;
const TInt KTBufSize = 256;


// RTestStepVideoDecoderPlayCov
RTestStepVideoDecoderPlayCov::RTestStepVideoDecoderPlayCov()
 	{
 	iTestStepName = _L("MM-MDF-VIDEOCODECS-0201");
 	iHeapSize = KMaxHeapSize;
 	}

TVerdict RTestStepVideoDecoderPlayCov::DoTestStepL()
	{
	__UHEAP_MARK;
	TBuf<KTBufSize> buf;
	buf.Format(_L("Heap Count Start: %d"), User::CountAllocCells());
	InfoMessage(buf);
	iPlayVideoFile = CPlayVideoFileCov::NewL(this);
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
RTestStepVideoDecoderPlayDSACov::RTestStepVideoDecoderPlayDSACov()
 	{
 	iTestStepName = _L("MM-MDF-VIDEOCODECS-0202");
 	iHeapSize = KMaxHeapSize;
 	
 	}

TVerdict RTestStepVideoDecoderPlayDSACov::DoTestStepL()
	{
	__UHEAP_MARK;
	TBuf<KTBufSize> buf;
	buf.Format(_L("Heap Count Start: %d"), User::CountAllocCells());
	InfoMessage(buf);

	iPlayVideoFile = CPlayVideoFileCov::NewL(this);
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

RTestStepVideoDecoderPanicCov::RTestStepVideoDecoderPanicCov(TInt32 aSubTestNumber, TBuf<KMaxLenTestStepName> aTestStepName)
 	{
 	iHeapSize = KMaxHeapSize;
 	iSubTestNumber=aSubTestNumber;
 	iTestStepName=aTestStepName;
 	}

TVerdict RTestStepVideoDecoderPanicCov::DoTestStepL()
	{
	__UHEAP_MARK;
	TBuf<KTBufSize> buf;
	buf.Format(_L("Heap Count Start: %d"), User::CountAllocCells());
	InfoMessage(buf);

	iPlayVideoFile = CPlayVideoFileCov::NewL(this);
	
	switch(iSubTestNumber)
	{
		case 1:
		iPlayVideoFile->LoadCodecL();
		iPlayVideoFile->CancelTimedSnapshot();
		break;
	
		case 2:
		iPlayVideoFile->LoadCodecL();
		iPlayVideoFile->GetTimedSnapshot1();
		break;
	
		case 3:
		iPlayVideoFile->LoadCodecL();
		iPlayVideoFile->GetTimedSnapshot2();
		break;
	
		case 4:
		iPlayVideoFile->LoadCodecL();
		iPlayVideoFile->GetSupportedSnapshotFormats();
		break;
		
	
	
	}
	/*iPlayVideoFile->SetDSA(ETrue);
	iPlayVideoFile->StartPlaybackL();*/
	delete iPlayVideoFile;
	REComSession::FinalClose();
	buf.Format(_L("Heap Count End: %d"), User::CountAllocCells());
	InfoMessage(buf);
	__UHEAP_MARKEND;
	return Verdict();
	}	
//RTestStepVideoDecoderPlayNegTestsCov
RTestStepVideoDecoderPlayNegTestsCov::RTestStepVideoDecoderPlayNegTestsCov(TInt32 aSubTestNumber, TBuf<KMaxLenTestStepName> aTestStepName)
 	{
 	iTestStepName = aTestStepName;
 	iSubTestNumber = aSubTestNumber;
 	iHeapSize = KMaxHeapSize;
 	}

TVerdict RTestStepVideoDecoderPlayNegTestsCov::DoTestStepL()
	{
	__UHEAP_MARK;
	TBuf<KTBufSize> buf;
	buf.Format(_L("Heap Count Start: %d"), User::CountAllocCells());
	InfoMessage(buf);

	iPlayVideoFile = CPlayVideoFileCov::NewL(this);
	iPlayVideoFile->LoadCodecL();
	switch(iSubTestNumber)
	{
		case 1:
		iPlayVideoFile->NegTests();
		break;

		case 2:
		iPlayVideoFile->InitializeCov1();
		break;
		
		case 3:
		//iPlayVideoFile->InitializeCov2();
		break;
		
		default:
		break;
	}
	/*iPlayVideoFile->SetDSA(ETrue);
	iPlayVideoFile->StartPlaybackL();*/
	delete iPlayVideoFile;
	REComSession::FinalClose();
	buf.Format(_L("Heap Count End: %d"), User::CountAllocCells());
	InfoMessage(buf);
	__UHEAP_MARKEND;
	return Verdict();
	}	
