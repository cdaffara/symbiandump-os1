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

#include "tsu_mdf_videocodecs_record.h"
#include <mmf/server/mmfhwdevicesetup.h>
#include "videorecordfile.h"

const TInt KTBufSize = 256;


RTestStepVideoEncoderLoad::RTestStepVideoEncoderLoad()
 	{
 	iTestStepName = _L("MM-MDF-VIDEOCODECS-0101");
 	}

TVerdict RTestStepVideoEncoderLoad::DoTestStepL()
	{
	__UHEAP_MARK;
	TBuf<KTBufSize> buf;
	buf.Format(_L("Heap Count Start: %d"), User::CountAllocCells());
	InfoMessage(buf);
	
	iRecordVideoFile = CRecordVideoFile::NewL(this);
	iRecordVideoFile->LoadCodecL();
	delete iRecordVideoFile;
	REComSession::FinalClose();
	
	buf.Format(_L("Heap Count End: %d"), User::CountAllocCells());
	InfoMessage(buf);

	__UHEAP_MARKEND;
	return Verdict();
	}
	
	
RTestStepVideoEncoderRecord::RTestStepVideoEncoderRecord()
 	{
 	iTestStepName = _L("MM-MDF-VIDEOCODECS-0102");
 	}

TVerdict RTestStepVideoEncoderRecord::DoTestStepL()
	{
	__UHEAP_MARK;
	TBuf<KTBufSize> buf;
	buf.Format(_L("Heap Count Start: %d"), User::CountAllocCells());
	InfoMessage(buf);
	
	iRecordVideoFile = CRecordVideoFile::NewL(this);
	iRecordVideoFile->LoadCodecL();
	iRecordVideoFile->StartRecordL();
	delete iRecordVideoFile;
	REComSession::FinalClose();
	
	buf.Format(_L("Heap Count End: %d"), User::CountAllocCells());
	InfoMessage(buf);

	__UHEAP_MARKEND;
	return Verdict();
	}
