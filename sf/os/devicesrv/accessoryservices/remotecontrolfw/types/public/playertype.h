// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file
 @publishedAll
 @released
*/

#ifndef PLAYERTYPE_H
#define PLAYERTYPE_H

enum TPlayerType
	{
	ERemConAudioPlayer = 1,
	ERemConVideoPlayer = 1<<1,
	ERemConBroadcastingAudioPlayer = 1<<2,
	ERemConBroadcastingVideoPlayer = 1<<3,
	};

enum TPlayerSubType
	{
	ERemConNoSubType = 0,
	ERemConAudioBook = 1,
	ERemConPodcast = 1<<1,
	};

class TPlayerTypeInformation
	{
public:
	TPlayerType iPlayerType;
	TPlayerSubType iPlayerSubType;
	};

#endif // PLAYERTYPE_H
