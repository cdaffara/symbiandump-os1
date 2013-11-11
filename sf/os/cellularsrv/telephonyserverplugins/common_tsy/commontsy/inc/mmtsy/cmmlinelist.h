/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/



#ifndef CMMLINELIST_H
#define CMMLINELIST_H

//  INCLUDES
#include "cmmtsyobjectlist.h"
#include "MmTsy_conf.h"

// FORWARD DECLARATIONS
class CMmPhoneTsy;
class CMmLineTsy;

// CLASS DECLARATION

/**
 * This class provides the means for the Phone object (CMmPhoneTsy class) to 
 * keep track of opened Lines. When a Line object is opened it should be put 
 * on the Phone's LineList.
 */
NONSHARABLE_CLASS( CMmLineList ) : public CMmTsyObjectList
    {
    public: // Enumerations
    
         /** Lines */ 
         enum TLines
            {
            ELine0Voice1,
            ELine1Voice2,
            ELine2Data,
            ELine3Fax
            };

    public:     // Constructors and destructor
        /**
         * Two-phased constructor.
         * @param CMmPhoneTsy* aMmPhone: pointer to the Phone
         * @return created CallList object 
         */        
        static CMmLineList* NewL( CMmPhoneTsy* aMmPhone );

        /**
         * Destructor.
         */
        virtual ~CMmLineList();

        // New functions

        /**
         * Returns the Line that has the given mode
         *          
         *
         * @param const TUint8 aLineMode: Line mode
         * @return Line object
         */
        CMmLineTsy* GetMmLineByMode( const RMobilePhone::TMobileService 
        	aLineMode ) const;

        /**
         * Returns the Line that has the given index in container
         *          
         *
         * @param const TInt aIndex: index of the Line which is to be returned.
         * @return Call object
         */
        CMmLineTsy* GetMmLineByIndex( const TInt aIndex ); 

		    /**
         * This method adds a TSY object to the container
         *          
         *
         * @param CTelObject* aObject: object to be added to the list.
		     * @param TName name: name of the line which is going to be added
         * @return status success/failure
         */  
		    TInt AddLineObject ( CTelObject* aTsyObject, const TName& aLineName );

        /**
         * This method removes a named line from the container
         *          
         *
         * @param const TName& aLineName: name of the line to be marked as removed
         * @return status success/failure
         */
        TInt RemoveLine( const TName& aLineName);

		    /**
         * Delete Call Container
         *          
         *
         */  
		void DeleteContainer();

		    /**
		     * Returns the object that has the given index in container
		     * 		     
         *
         * @param const TUint aIndex: index of the object which is to be returned.
         * @return Object to be returned
         */
		    CBase* GetObjectByIndex( const TInt aIndex ) const;

    private:     // Constructors and destructor

        /**
         * C++ default constructor.
         */
        CMmLineList();

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

    private:    // Data
    
		    /**
         * Boolean array which permits to check if line is in use or not.
         */
        TBool iIsLineInUse[ KMmTsyNumOfSupportedLines ];
        
		    /**
         * None
         */
        TBool iInitialised;

    };

#endif // CMMLINELIST_H

//  End of file
