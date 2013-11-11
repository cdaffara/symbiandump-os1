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


#ifndef __CSSYSensorControlStubSecond_H_
#define __CSSYSensorControlStubSecond_H_

//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "SSYSensorControlStubBase.h"



// CONSTANTS

_LIT8( KLocationSecond, "Second%d" );
_LIT8( KNewLocStringBaseSecond, "NewSecond%d" );
_LIT8( KChannelVendorIDSecond, "VendorSecond" );

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
class CSSYSensorControlStubSecond : public CSSYSensorControlStubBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aSensorObserver Pointer to sensor observer
		* @return The created object.
        */
        static CSSYSensorControlStubSecond* NewL( MSsyCallback* aSsyCallback );
        
        /**
        * Destructor.
        */
        virtual ~CSSYSensorControlStubSecond();

    protected: 
        virtual void GetNewLocString(TDes8& aTarget, TInt aId) const;
		
	private:
	    CSSYSensorControlStubSecond( MSsyCallback* aSsyCallback );
	    void ConstructL();
	    
	    void CreateChannelsL();

    	
    };

#endif // CSSYSensorControlStubSecond_H_
