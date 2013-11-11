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
* Name        : EPos_CPosModules.h
* Part of     : Location Framework / Location Settings
* Interface   : Location Settings API
* CPosModules class declaration
* Version     : %version %
*
*/



#ifndef CPOSMODULES_H
#define CPOSMODULES_H

#include <e32base.h>
#include <lbscommon.h>

class MPosModulesObserver;
class CPosModuleUpdate;
class CPosModuleIdList;
class CPosModulesEngine;

/**
*   The main interface to location settings.
*
*   This class is used for accessing positioning plug-ins attributes using
*   GetModuleInfoL() and accessing and managing priority list of PSYs (see methods
*   ModuleIdListL() and SetModulePriorityL() ). A positioning plug-in (or "module")
*   is referenced by its unique TUid value.
*
*   All PSYs are ordered into priority list, which is used by Location Server
*   in fallback situations (when used via RPositioner::Open() without parameters).
*   Note however, that modules with visibility attribute set to 0
*   are not used in these cases and have no priority assigned.
*   Whenever client attempts to get or set priority of such a module,
*   the appropriate methods leave with KErrNotFound.
*   Module must be set visible before its priority can be retrieved or modified, see
*   CPosModuleUpdate::SetUpdateVisibility() .
*
*   A client can also listen for changes in location
*   settings by setting an observer via SetObserverL().
* 	@publishedPartner
*	@released
*/
class CPosModules : public CBase
    {
    public:

  
        IMPORT_C static CPosModules* OpenL();
        IMPORT_C virtual ~CPosModules();

    private:

        // C++ default constructor
        CPosModules();

        void ConstructL();

    public:


        IMPORT_C void SetObserverL( MPosModulesObserver& aObserver );
        IMPORT_C void RemoveObserver();
        IMPORT_C void SetModulePriorityL( TPositionModuleId aModuleId, TInt aNewPriority );
        IMPORT_C void SetModulePriorityL( TInt aOldPriority, TInt aNewPriority );
        IMPORT_C void  SetModulePrioritiesL( const CPosModuleIdList& aModuleIdList );
        IMPORT_C TInt PriorityL( TPositionModuleId aModuleId );
        IMPORT_C CPosModuleIdList* ModuleIdListLC();
        IMPORT_C CPosModuleIdList* ModuleIdListL();
        IMPORT_C void GetModuleInfoL( TPositionModuleId aModuleId, TPositionModuleInfoBase& aInfo );
        IMPORT_C void UpdateModuleL( TPositionModuleId aModuleId, const CPosModuleUpdate& aUpdate );
        IMPORT_C TBool IsModuleInstalled( TPositionModuleId aModuleId );
        IMPORT_C TBool IsModuleVisibleL( TPositionModuleId aModuleId );

    private:

        // Prohibit copy constructor
        CPosModules( const CPosModules& );
        // Prohibit assigment operator
        CPosModules& operator= ( const CPosModules& );

    private:

        CPosModulesEngine* iEngine;
    };

#endif      // CPOSMODULES_H
