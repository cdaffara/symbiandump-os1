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
* Description: AudioInput API header
*
*/


#ifndef CAUDIOINPUT_H
#define CAUDIOINPUT_H

//  INCLUDES
#include <e32base.h>
#include <midiclientutility.h>

// FORWARD DECLARATIONS
class CMdaAudioRecorderUtility;
class CMMFDevSound;
class MCustomInterface;
class MCustomCommand;
class CMdaAudioInputStream;
class CVideoRecorderUtility;

// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*/
class CAudioInput : public CBase
	{
	public:  // Data Types
	
		enum TAudioInputPreference
			{
			EDefaultMic,
			EOutputToSpeaker,
			EFMRadio,
			EVoiceCall,
			ELineIn
			};

		typedef TArray<TAudioInputPreference> TAudioInputArray;

    public:  // Constructors and destructor
        
		/**
		* Two-phased constructor.
		*/
		IMPORT_C static CAudioInput* NewL(CMdaAudioRecorderUtility& aUtility);
//	  IMPORT_C static CAudioInput* NewL(CMidiClientUtility& aUtility);	
		IMPORT_C static CAudioInput* NewL(CMMFDevSound& aDevSound);
		IMPORT_C static CAudioInput* NewL(MCustomInterface& aUtility);
		IMPORT_C static CAudioInput* NewL(MCustomCommand& aUtility);
  	    IMPORT_C static CAudioInput* NewL(CMdaAudioInputStream& aUtility);
		IMPORT_C static CAudioInput* NewL(CVideoRecorderUtility& aUtility);

		/**
		* ?member_description.
		* @param ?arg1 ?description
		* @return ?description
		*/
		virtual TAudioInputArray AudioInput() = 0;

		/**
		* ?member_description.
		* @param ?arg1 ?description
		* @return ?description
		*/
		virtual void SetAudioInputL(TAudioInputArray aAudioInputs) = 0;        

	};

#endif      // CAUDIOINPUT_H
            
// End of File
