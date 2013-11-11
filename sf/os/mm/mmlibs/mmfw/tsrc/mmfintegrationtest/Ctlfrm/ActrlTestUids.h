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

static const TUid KPlayTestId				= { 0x101F7D21 };
static const TUid KPrimeTestId				= { 0x101F7D22 };
static const TUid KPauseTestId				= { 0x101F7D23 };
static const TUid KStopTestId				= { 0x101F7D24 };
static const TUid KResetTestId				= { 0x101F7D25 };
static const TUid KWavUid					= { 0x101F55E1 };
static const TUid KRemoveDataSourceTestId	= { 0x101F7D26 };
static const TUid KRemoveDataSinkTestId		= { 0x101F7D27 };
static const TUid KMemoryAllocationTestId	= { 0x101F7D28 };
static const TUid KMmfUidFormatRAWRead		= { 0x101F5C16 };
static const TUid KMmfUidFormatRAWWrite		= { 0x101F5C17 };
static const TUid KAuUid					= { 0x101F5C11 };
static const TUid KUidCustomTestAudioPlugin = { 0x101F7D2A };

// *** Jim - added for getting dummy video controller uid 
// stolen from CMmfVideoTestControllerUIDs.hrh
static const TUid KMmfVideoTestControllerUid= { 0x101F88D9 };

#endif // endif __ACTRL_TEST_UIDS_H__
