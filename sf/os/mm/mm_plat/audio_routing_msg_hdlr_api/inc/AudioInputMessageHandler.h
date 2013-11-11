/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Audio Input Message Handler Apis
*
*/


#ifndef CAUDIOINPUTMESSAGEHANDLER_H
#define CAUDIOINPUTMESSAGEHANDLER_H

//  INCLUDES
#include <mmf/common/mmfcontroller.h>

// FORWARD DECLARATIONS
class CAudioInput;

// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*/
class CAudioInputMessageHandler: public CMMFObject
	{
    public:  // Constructors and destructor
        IMPORT_C static CAudioInputMessageHandler* NewL(TAny*                customInterface,
                                                        CMMFObjectContainer& aContainer);

		/**
		* Destructor.
		*/
		virtual ~CAudioInputMessageHandler();

		IMPORT_C static TUid Uid();

		/**
		* ?member_description.
		* @param ?arg1 ?description
		* @return ?description
		*/
		virtual void HandleRequest(TMMFMessage& aMessage);        

	private:

		/**
		* C++ default constructor.
		*/
		CAudioInputMessageHandler(CAudioInput*         aAudioInput,
		                          CMMFObjectContainer& aContainer);

		void DoHandleRequestL(TMMFMessage& aMessage);

		void DoDeleteL(TMMFMessage& aMessage);

		void DoSetInputsL(TMMFMessage& aMessage);

    private:    // Data
        CAudioInput*         iAudioInput;
        CMMFObjectContainer& iContainer;

	};

#endif      // CAUDIOINPUTMESSAGEHANDLER_H
            
// End of File
