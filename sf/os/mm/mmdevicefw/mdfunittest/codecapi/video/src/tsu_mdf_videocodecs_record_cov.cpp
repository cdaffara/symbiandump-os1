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

#include "tsu_mdf_videocodecs_record_cov.h"
#include <mmf/server/mmfhwdevicesetup.h>
#include "videorecordfile_cov.h"

const TInt KTBufSize = 256;



RTestStepVideoEncoderCov::RTestStepVideoEncoderCov()
 	{
 	iTestStepName = _L("MM-MDF-VIDEOCODECS-0301");
 	}

TVerdict RTestStepVideoEncoderCov::DoTestStepL()
	{
	__UHEAP_MARK;
	TBuf<KTBufSize> buf;
	buf.Format(_L("Heap Count Start: %d"), User::CountAllocCells());
	InfoMessage(buf);
	
	iRecordVideoFile = CRecordVideoFileCov::NewL(this);
	iRecordVideoFile->LoadCodecL();
	iRecordVideoFile->StartRecordL();
	delete iRecordVideoFile;
	REComSession::FinalClose();
	
	buf.Format(_L("Heap Count End: %d"), User::CountAllocCells());
	InfoMessage(buf);

	__UHEAP_MARKEND;
	return Verdict();
	}

//RTestStepVideoEncoderNegativeCov

RTestStepVideoEncoderNegativeCov::RTestStepVideoEncoderNegativeCov()
 	{
 	iTestStepName = _L("MM-MDF-VIDEOCODECS-0302");
 	}

TVerdict RTestStepVideoEncoderNegativeCov::DoTestStepL()
	{
	__UHEAP_MARK;
	TBuf<KTBufSize> buf;
	buf.Format(_L("Heap Count Start: %d"), User::CountAllocCells());
	InfoMessage(buf);
	
	iRecordVideoFile = CRecordVideoFileCov::NewL(this);
	iRecordVideoFile->LoadCodecL();
	iRecordVideoFile->NegTests1();
	delete iRecordVideoFile;
	
	iRecordVideoFile = CRecordVideoFileCov::NewL(this);
	iRecordVideoFile->LoadCodecL();
	iRecordVideoFile->NegTests2();
	delete iRecordVideoFile;
	
	iRecordVideoFile = CRecordVideoFileCov::NewL(this);
	iRecordVideoFile->LoadCodecL();
	iRecordVideoFile->NegTests3();
	delete iRecordVideoFile;
	REComSession::FinalClose();
	
	buf.Format(_L("Heap Count End: %d"), User::CountAllocCells());
	InfoMessage(buf);

	__UHEAP_MARKEND;
	return Verdict();
	}
