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

#ifndef TSU_MDF_AUDIOCODECS_CONSTANTS_H
#define TSU_MDF_AUDIOCODECS_CONSTANTS_H

#define KUidHwDeviceAudioAdapter	0x10273791
#define KMaxDataTypeLength			126
#define KUidMdfProcessingUnit 		0x10273789
const TUint KVersionOne = 1;

// file we are going to record into
_LIT(KTestRecordedFile, "c:\\mm\\mmf\\testfiles\\mdf\\Record.wav");

//const TTimeIntervalMicroSeconds32 KMdfTest0003RecordTime = 3000000;

const TUint32 KMMFFourCCCodeTestPCM8 = 0x38505420;		//( ' ', 'T', 'P', '8' )
const TUint32 KMMFFourCCCodeTestPCMU8 = 0x38555054;		//('T', 'P', 'U', '8')

#endif // TSU_MDF_AUDIOCODECS_CONSTANTS_H
