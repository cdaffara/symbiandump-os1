/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This is the definition of the audio equalizer effect message types.
*
*
*/


#ifndef AUDIOEQUALIZEREFFECTMESSAGETYPES_H
#define AUDIOEQUALIZEREFFECTMESSAGETYPES_H


enum AudioEqualizerFunctions
	{
	EAefInitialize = 1,
	EAefInitializeBand,
	EAefGetNumberOfBands,
	EAefApply,
	EAefApplyBand,
	EAefObserve
	};

#endif //AUDIOEQUALIZEREFFECTMESSAGETYPES_H
