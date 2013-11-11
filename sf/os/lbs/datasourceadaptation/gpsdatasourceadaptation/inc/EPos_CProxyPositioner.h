/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef CPROXYPOSITIONER_H
#define CPROXYPOSITIONER_H

#include <lbs/epos_cpositioner.h>

// FORWARD DECLARATIONS
class TPositionCriteriaBase;
class MPosModuleSettingsManager;
class MPosModuleStatusManager;

// CLASS DECLARATION

/**
* Base class for all proxy positioners.
*/
class CProxyPositioner : public CPositioner
    {
    public:  // Constructors and destructor

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CProxyPositioner();

    protected:  // Constructors

        /**
        * C++ default constructor.
        */
        IMPORT_C CProxyPositioner();

        /**
        * Creates the internals of the positioner.
        *
        * This function must be called first thing in the proxy
        *  positioner's * @b ConstructL() method.
        *
        *
        * @param aConstructionParameters The construction parameters supplied
        *                                in the factory call.
        */
        IMPORT_C void BaseConstructL( TAny* aConstructionParameters );
        
    public:  

        /**
        * Called by the Location Framework to set the position criteria
        * specified by the client.
        *
        * The position criteria object must be cast to the correct
        * TPositionCriteriaBase subclass.
        *
        * @param aPositionCriteria The requested positioning criteria.
        */
        IMPORT_C virtual void SetPositionCriteriaL(
        /* IN  */       const TPositionCriteriaBase& aPositionCriteria
        );
        
    protected:
        /**
        * Get pointer to module settings.
        */
        IMPORT_C MPosModuleSettingsManager& ModuleSettingsManager() const;
        
        /**
        * Get pointer to module status manager.
        */
        IMPORT_C MPosModuleStatusManager& ModuleStatusManager() const;

    private:
        MPosModuleStatusManager* iModuleStatusManager;
        MPosModuleSettingsManager* iModuleSettingsManager;
        
        TUint iReserved[8];
    };  

#endif  // CPROXYPOSITIONER_H

// End of File
