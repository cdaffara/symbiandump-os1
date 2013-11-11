// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This files contains a set of uids to be used both by
// the dll tsu_mmf_actrl_01 and CustomAudioMMfController
// as a means of communicating unique events
//

#ifndef __ACTRL_TEST_UIDS_H__
#define __ACTRL_TEST_UIDS_H__

#include <mmfformatimplementationuids.hrh>

const TUid KFormatRAWRead			= { KMmfUidFormatRAWRead };
const TUid KFormatRAWWrite			= { KMmfUidFormatRAWWrite };
const TUid KFormatWAVRead			= { KMmfUidFormatWAVRead};
const TUid KFormatWAVWrite			= { KMmfUidFormatWAVWrite };
const TUid KFormatAURead			= { KMmfUidFormatAURead };
const TUid KFormatAUWrite			= { KMmfUidFormatAUWrite };

const TUid KPlayTestId					= { 0x101F7D21 };
const TUid KPrimeTestId					= { 0x101F7D22 };
const TUid KPauseTestId					= { 0x101F7D23 };
const TUid KStopTestId					= { 0x101F7D24 };
const TUid KResetTestId					= { 0x101F7D25 };
const TUid KUidMmfAudioController		= { KMmfUidControllerAudio };
const TUid KRemoveDataSourceTestId		= { 0x101F7D26 };
const TUid KRemoveDataSinkTestId		= { 0x101F7D27 };
const TUid KMemoryAllocationTestId		= { 0x101F7D28 };
const TUid KUidCustomTestAudioPlugin	= { 0x101F7D2A };
const TUid KMarcSetMaxFileSizeId		= { 0x101f7d37 };
const TUid KMarcGetRecordTimeAvailId	= { 0x101f7d38 };
const TUid KPlayCompleteTestId			= { 0x101f7ea0 };

#endif // endif __ACTRL_TEST_UIDS_H__
