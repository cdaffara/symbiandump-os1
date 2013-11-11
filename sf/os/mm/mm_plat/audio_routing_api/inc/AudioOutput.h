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
* Description:  This is the definition of the CAudioOutput class.
*
*/



#ifndef AUDIOOUTPUT_H
#define AUDIOOUTPUT_H

//  INCLUDES
#include <e32base.h>
#include <mdaaudiotoneplayer.h>
#include <mdaaudiooutputstream.h>
#include <midiclientutility.h>
#include <MCustomInterface.h>

// FORWARD DECLARATIONS
class MAudioOutputObserver;
class MCustomCommand;
class CMdaAudioPlayerUtility;
class CMdaAudioRecorderUtility;
class CMMFDevSound;
class CDrmPlayerUtility;
class CVideoPlayerUtility;
// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*/
class CAudioOutput : public CBase
	{
	public:  // DATA TYPES
		enum TAudioOutputPreference
			{
			ENoPreference,
			EAll,
			ENoOutput,
			EPrivate,
			EPublic
			};

	public:  // Constructors and destructor

		/**
		* Two-phased constructor.
		*/
		IMPORT_C static CAudioOutput* NewL(CMdaAudioPlayerUtility& aUtility);
		/**
        * Factory function for creating audio output object.
        * @since 3.0
        * @param CMdaAudioRecorderUtility& - a reference to CMdaAudioRecorderUtility object
        * @return pointer to CAudioOutput object
        */		
		IMPORT_C static CAudioOutput* CAudioOutput::NewL(CMdaAudioRecorderUtility& aUtility, TBool aRecordStream);
		/**
        * Factory function for creating audio output object.
        * @since 3.0
        * @param CMdaAudioOutputStream& - a reference to CMdaAudioOutputStream object
        * @return pointer to CAudioOutput object
        */		
		IMPORT_C static CAudioOutput* CAudioOutput::NewL(CMdaAudioOutputStream& aUtility);
		/**
        * Factory function for creating audio output object.
        * @since 3.0
        * @param CMdaAudioToneUtility& - a reference to CMdaAudioToneUtility object
        * @return pointer to CAudioOutput object
        */		
		IMPORT_C static CAudioOutput* CAudioOutput::NewL(CMdaAudioToneUtility& aUtility);
		/**
        * Factory function for creating audio output object.
        * @since 3.0
        * @param CMMFDevSound& - a reference to CMMFDevSound object
        * @return pointer to CAudioOutput object
        */		
		IMPORT_C static CAudioOutput* CAudioOutput::NewL(CMMFDevSound& aDevSound);
		/**
        * Factory function for creating audio output object.
        * @since 3.0
        * @param MCustomInterface& - a reference to MCustomInterface object
        * @return pointer to CAudioOutput object
        */		
		IMPORT_C static CAudioOutput* CAudioOutput::NewL(MCustomInterface& aUtility);
		/**
        * Factory function for creating audio output object.
        * @since 3.0
        * @param MCustomCommand& - a reference to MCustomCommand object
        * @return pointer to CAudioOutput object
        */		
		IMPORT_C static CAudioOutput* CAudioOutput::NewL(MCustomCommand& aUtility);
		/**
        * Factory function for creating audio output object.
        * @since 3.0
        * @param CMidiClientUtility& - a reference to CMidiClientUtility object
        * @return pointer to CAudioOutput object
        */		
		IMPORT_C static CAudioOutput* CAudioOutput::NewL(CMidiClientUtility& aUtility);
		/**
        * Factory function for creating audio output object.
        * @since 3.0
        * @param CDrmPlayerUtility& - a reference to CDrmPlayerUtility object
        * @return pointer to CAudioOutput object
        */		
		IMPORT_C static CAudioOutput* /*CAudioOutput::*/NewL(CDrmPlayerUtility& aUtility);
		
		/**
        * Factory function for creating audio output object.
        * @since 3.2
        * @param CVideoPlayerUtility& - a reference to CVideoPlayerUtility object
        * @return pointer to CAudioOutput object
        */		
		IMPORT_C static CAudioOutput* /*CAudioOutput::*/NewL(CVideoPlayerUtility& aUtility);

		/**
		* ?member_description.
		* @param ?arg1 ?description
		* @return ?description
		*/
		virtual TAudioOutputPreference AudioOutput() = 0;

		/**
		* ?member_description.
		* @param ?arg1 ?description
		* @return ?description
		*/
		virtual TAudioOutputPreference DefaultAudioOutput() = 0;

		/**
		* ?member_description.
		* @param ?arg1 ?description
		* @return ?description
		*/
		virtual void RegisterObserverL( MAudioOutputObserver& aObserver ) = 0;

		/**
		* ?member_description.
		* @param ?arg1 ?description
		* @return ?description
		*/
		virtual TBool SecureOutput() = 0;

		/**
		* ?member_description.
		* @param ?arg1 ?description
		* @return ?description
		*/
		virtual void SetAudioOutputL( TAudioOutputPreference aAudioOutput = ENoPreference ) = 0;

		/**
		* ?member_description.
		* @param ?arg1 ?description
		* @return ?description
		*/
		virtual void SetSecureOutputL( TBool aSecure = EFalse ) = 0;

		/**
		* ?member_description.
		* @param ?arg1 ?description
		* @return ?description
		*/
		virtual void UnregisterObserver( MAudioOutputObserver& aObserver ) = 0;

	};

#endif      // CAUDIOOUTPUT_H

// End of File
