/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  
*	Event enumerations and uid:s of System Agent. Sysagent clients
*	can include this file and listen to these events. These events will be routed 
*   through System Agent.
*
*/


#ifndef SAE_H
#define SAE_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CSaePubSubNotifier;

// CLASS DECLARATION

/**
*  Creates and owns the SAE environment.
*/
NONSHARABLE_CLASS( CSae ): public CBase
	{
	public:

        /**
        * Two-phased constructor.
		*/
		static CSae* NewL();

		/**
		* Destructor
		*/
		virtual ~CSae();

	private:

		/**
		* By default EPOC constructor is private.
		*/
		void ConstructL();

		/**
		* C++ default constructor.
		*/
		CSae();	

	private: // Data

		CSaePubSubNotifier* iPubSubNotifier;
	};

#endif   // SAE_H  
            
// End of File

