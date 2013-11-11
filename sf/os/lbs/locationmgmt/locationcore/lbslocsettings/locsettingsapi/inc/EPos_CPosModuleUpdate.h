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
* Name        : EPos_CPosModuleUpdate.h
* Part of     : Location Framework
* Interface   : Location Settings API
* CPosModuleUpdate class declaration
* Version     : %version %
*
*/



#ifndef CPOSMODULEUPDATE_H
#define CPOSMODULEUPDATE_H

#include <e32base.h>
#include <lbscommon.h>

/**
*  Encapsulates an update of the changeable attributes of a positioning module.
*
*  This class keeps track of which parameters have been set. Only those
*  parameters will be updated.
*
*  To edit an attribute, create a CPosModuleUpdate and set the attribute
*  which should be changed. Then use CPosModules::UpdateModuleL() to
*  commit the change to Location Settings.
*
* @publishedPartner
* @released
*/

class CPosModuleUpdate : public CBase
    {
    public:

        /** Two-phased constructor. */
        IMPORT_C static CPosModuleUpdate* NewLC();

        /** Destructor. */
        virtual ~CPosModuleUpdate();

    public:
        
        IMPORT_C void SetUpdateAvailability( TBool aIsAvailable );
        IMPORT_C void SetUpdateCost( TPositionQuality::TCostIndicator aCost );
        /** Returns the update bitmap.
        */
        TUint UpdateMap() const;

        /** 
        * Returns the update value for the module status attribute.
        */
        TBool AvailabilityUpdate() const;
        /**
        * Returns the update value for the cost attribute.
        */
        TPositionQuality::TCostIndicator CostUpdate() const;
        /**
        * Returns the update value for the visibility attribute.
        */
        TBool VisibilityUpdate() const;

        IMPORT_C void SetUpdateVisibility( TBool aIsVisible );

    private:

        CPosModuleUpdate();
        void ConstructL();

        // Prohibit copy constructor
        CPosModuleUpdate( const CPosModuleUpdate& );
        // Prohibit assigment operator
        CPosModuleUpdate& operator= ( const CPosModuleUpdate& );

    private:
    
        TUint iUpdateMask;
        TBool iIsAvailableUpdate;
        TPositionQuality::TCostIndicator iCostUpdate;
        TBool iIsVisibleUpdate;
    };

#endif      // CPOSMODULEUPDATE_H  
            
