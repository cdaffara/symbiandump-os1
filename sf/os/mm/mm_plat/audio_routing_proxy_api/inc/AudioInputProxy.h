/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Audio Input Proxy header
*
*/


#ifndef CAUDIOINPUTPROXY_H
#define CAUDIOINPUTPROXY_H

//  INCLUDES
#include "AudioInput.h"
#include <mmf/common/mmfcontrollerframework.h>
#include <MCustomCommand.h>
// FORWARD DECLARATIONS
class CCustomCommandUtility;
class CCustomInterfaceUtility;


// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*/
class CAudioInputProxy: public CAudioInput
	{
	friend class CAudioInput;
    public:  // Constructors and destructor

		IMPORT_C static CAudioInputProxy* NewL(TMMFMessageDestinationPckg aMessageHandler, 
												MCustomCommand& aCustomCommand,
												CCustomInterfaceUtility* aCustomInterfaceUtility);
		
		/**
		* Destructor.
		*/
		virtual ~CAudioInputProxy();

		IMPORT_C static const TUid Uid();
		
		/**
		* ?member_description.
		* @param ?arg1 ?description
		* @return ?description
		*/
		virtual TAudioInputArray AudioInput();

		/**
		* ?member_description.
		* @param ?arg1 ?description
		* @return ?description
		*/
		virtual void SetAudioInputL(TAudioInputArray aAudioInputs);        

	protected:
		/**
		* C++ default constructor.
		*/
		CAudioInputProxy(TMMFMessageDestinationPckg aMessageHandler, 
												MCustomCommand& aCustomCommand,
												CCustomInterfaceUtility* aCustomInterfaceUtility);
		/**
		* EPOC constructor.
		*/
		void ConstructL();

    private:    // Data
		MCustomCommand*                			iCustomCommand;
		CArrayFixFlat<TAudioInputPreference>  	iInputArray;
		TMMFMessageDestinationPckg            	iMessageHandler;
		CCustomInterfaceUtility* 				iCustomInterfaceUtility;
	};

#endif      // CAUDIOINPUTPROXY_H
            
// End of File
