/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  AudioOutput Message Types
*
*/

#ifndef AUDIOOUTPUTMESSAGETYPES_H
#define AUDIOOUTPUTMESSAGETYPES_H

const TUid KUidAudioOutput = {0x10200018};

enum AudioOutputFunctions
	{
	EAofDelete,
	EAofRegisterObserver,
	EAofSetAudioOutput,
	EAofGetAudioOutput,
	EAofSetSecureOutput,
	EAofUnregisterObserver
	};

#endif
