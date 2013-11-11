/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/


#ifndef PSMPLUGINSTUBFIRST_H
#define PSMPLUGINSTUBFIRST_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>

#include <psmpluginbase.h>

class MPsmSettingsProvider;

/**
*  PSM plugin stub
*  ?other_description_lines
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
*/
class CPSMPluginFirst : public CPsmPluginBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
		* @return The created object.
        */
        static CPSMPluginFirst* NewL( TPsmPluginCTorParams& initParams );
        
        /**
        * Destructor.
        */
        virtual ~CPSMPluginFirst();

    public: 
        
        // From CPsmPluginBase
        void NotifyModeChange( const TInt aMode );
		
	private:
	    CPSMPluginFirst( TPsmPluginCTorParams& aInitParams );
	    void ConstructL();

    private: // data
    

    };

#endif // PSMPLUGINSTUBFIRST_H
