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
* Name        : EPos_CPosModuleIdList.h
* Part of     : Location Framework / Location Settings
* Interface   : Location Settings API
* CPosModuleIdList class declaration
* Version     : %version %
*
*/



#ifndef CPOSMODULEIDLIST_H
#define CPOSMODULEIDLIST_H

#include <e32base.h>
#include <lbscommon.h>

/**
*  An list of IDs of positioning modules.
*
*  This class represents a list of positioning module IDs. It
*  contains methods for appending, inserting, removing and finding module IDs
*  in the list.
*
* @publishedPartner
* @released
*/
class CPosModuleIdList :public CArrayFixFlat<TPositionModuleId>
    {
    public:

        IMPORT_C static CPosModuleIdList* NewL();
        IMPORT_C virtual ~CPosModuleIdList();

    public:

        IMPORT_C TInt Find( TPositionModuleId aModuleId ) const;
        IMPORT_C void MoveL( TInt aOldPos, TInt aNewPos );

    protected:

        // C++ default constructor
        CPosModuleIdList();

    private:

        // Prohibit copy constructor
        CPosModuleIdList( const CPosModuleIdList& );
        // Prohibit assigment operator
        CPosModuleIdList& operator= ( const CPosModuleIdList& );

    };

#endif      // CPOSMODULEIDLIST_H
