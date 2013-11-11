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


#ifndef __CSSYChannelXYZ_H_
#define __CSSYChannelXYZ_H_

//  INCLUDES
#include <e32std.h>
#include <e32base.h>

#include "SSYChannelBase.h"


// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  XYZ channel
*  ?other_description_lines
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
*/
class CSSYChannelXYZ : public CSSYChannelBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param 
		* @return The created object.
        */
        static CSSYChannelXYZ* NewL( MSsyCallback* aSsyCallback, MSsyPropertyProvider* aPropertyProvider, TSensrvContextType aContextType,
	                     TSensrvQuantity aQuantity, TSensrvChannelTypeId aChannelType,
	                     TDesC8& aLocation, TDesC8& aVendorId );
        
        /**
        * Destructor.
        */
        virtual ~CSSYChannelXYZ();
        
    public: // From MSsyChanelDataProvider
        /**
        * Get size of the data object
        *
        * @since S60 5.0
        * @return TInt Data object size
        */	
		TInt DataObjectSize() const;
		

                
    protected: //From CSSYChannelBase
    
        void DoAppendData();
        
    
        
    protected:
	    
	    CSSYChannelXYZ( MSsyCallback* aSsyCallback, MSsyPropertyProvider* aPropertyProvider );
	                     
	    void ConstructL( TSensrvContextType aContextType,
	                     TSensrvQuantity aQuantity, TSensrvChannelTypeId aChannelType,
	                     TDesC8& aLocation, TDesC8& aVendorId );
	    
	    
	    

    private:    // Data
        TUint8* iWritePointer;
        TInt    iDataCounter;
        
        
    	
   	public:     // Friend classes
    	
    };

#endif // __CSSYChannelXYZ_H_
