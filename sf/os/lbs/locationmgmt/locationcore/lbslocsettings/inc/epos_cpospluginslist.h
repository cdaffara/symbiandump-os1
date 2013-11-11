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



#ifndef EPOS_CPOSPLUGINSLIST_H_
#define EPOS_CPOSPLUGINSLIST_H_

#include <e32base.h>
#include <f32file.h>
#include <lbscommon.h>

class CPosModuleIdList;
class CPosPluginProperties;

NONSHARABLE_CLASS( CPosPluginsList ) : public CBase
    {
    public:
        /** Two-phased constructor. */
        static CPosPluginsList* NewL( RFs& aFsSession );

        /** Destructor. */
    	virtual ~CPosPluginsList();

    public:

        TInt Find( TPositionModuleId aModuleId ) const;
        const CPosPluginProperties& Plugin( TInt aIndex ) const;
        void UpdateL();
        void SortAsInstalledL( CPosModuleIdList& aPostInstalledList );
        TInt Count() const;

    private:

		void MoveL( TInt aOldIndex, TInt aNewIndex );
        void Remove( TInt aIndex );

    private:

        CPosPluginsList( RFs& aFsSession );
        void ConstructL();

        // Prohibit copy constructor
        CPosPluginsList( const CPosPluginsList& );
        // Prohibit assigment operator
        CPosPluginsList& operator= ( const CPosPluginsList& );

    private:
        RFs& iFs;

        RPointerArray<CPosPluginProperties> iPlugins;
    };

#endif /*EPOS_CPOSPLUGINSLIST_H_*/
