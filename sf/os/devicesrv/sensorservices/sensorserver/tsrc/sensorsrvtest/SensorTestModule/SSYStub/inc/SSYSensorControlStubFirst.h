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


#ifndef __CSSYSensorControlStubFirstFirst_H_
#define __CSSYSensorControlStubFirstFirst_H_

//  INCLUDES
#include <e32std.h>
#include <e32base.h>

#include "ssysensorcontrolstubbase.h"



// CONSTANTS

_LIT8( KLocationFirst, "First%d" );
_LIT8( KNewLocStringBaseFirst, "NewFirst%d" );
_LIT8( KChannelVendorIDFirst, "VendorFirst" );

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  SSY sensor stub
*  ?other_description_lines
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
*/
class CSSYSensorControlStubFirst : public CSSYSensorControlStubBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aSensorObserver Pointer to sensor observer
		* @return The created object.
        */
        static CSSYSensorControlStubFirst* NewL( MSsyCallback* aSsyCallback );
        
        /**
        * Destructor.
        */
        virtual ~CSSYSensorControlStubFirst();

    protected: 
        virtual void GetNewLocString(TDes8& aTarget, TInt aId) const;
		
	private:
	    CSSYSensorControlStubFirst( MSsyCallback* aSsyCallback );
	    void ConstructL();
	    
	    void CreateChannelsL();

    	
    };

#endif // __CSSYSensorControlStubFirstFirst_H_
