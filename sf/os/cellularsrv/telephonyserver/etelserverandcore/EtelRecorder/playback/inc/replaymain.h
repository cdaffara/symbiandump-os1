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
// Global definitions for EtelRecorder session replayer.
// 
//

/**
 @file
*/

#ifndef __REPLAYMAIN_H__
#define __REPLAYMAIN_H__

namespace ReplaySession
	{
	enum TIniPanic
		{
		EBufferOverflow,
		EBadByteOrdering,
		EBadByteAlignment,
		EMissingDelimiter,
		EUnknownDispatcher,
		EUnknownMessageType,
		ECouldNotFindSession,
		ECouldNotFindHeader,
		EUnsupportedRequestType,
		EBadSubsessionName,
		ECouldNotFindSubSession		
		};

	void Panic(TIniPanic aPanic);
	}
	
#endif
