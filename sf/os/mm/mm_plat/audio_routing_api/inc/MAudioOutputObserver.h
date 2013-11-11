/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Audio Output Observer Class 
*
*/


#ifndef MAUDIOOUTPUTOBSERVER_H
#define MAUDIOOUTPUTOBSERVER_H

// INCLUDES
#include "AudioOutput.h"

// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*/
class MAudioOutputObserver
	{
	public:

		/**
		* ?member_description.
		* @param ?arg1 ?description
		* @return ?description
		*/
		virtual void DefaultAudioOutputChanged( CAudioOutput&                        aAudioOutput,
		                                CAudioOutput::TAudioOutputPreference aNewDefault ) = 0;

	};

#endif      // MAUDIOOUTPUTOBSERVER_H  

// End of File
