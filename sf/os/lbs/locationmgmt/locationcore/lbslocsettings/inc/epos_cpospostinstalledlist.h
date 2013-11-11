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



#ifndef EPOS_CPOSPOSTINSTALLEDLIST_H_
#define EPOS_CPOSPOSTINSTALLEDLIST_H_

#include <lbscommon.h>
#include <centralrepository.h>
#include <lbs/epos_cposmoduleidlist.h>

NONSHARABLE_CLASS( CPosPostInstalledList ): public CPosModuleIdList
{
    public:

        /** Two-phased constructor. */
        static CPosPostInstalledList* NewL( const TDesC& aKey );

    public:

    	void Remove( TPositionModuleId aModuleId );
		HBufC* FormatLC() const;

	private:
		CPosPostInstalledList();
		void ConstructL( const TDesC& aKey );

        // Prohibit copy constructor
        CPosPostInstalledList( const CPosPostInstalledList& );
        // Prohibit assigment operator
        CPosPostInstalledList& operator= ( const CPosPostInstalledList& );

	private:

    	void ParseIdListL( const TDesC& aString );
};

#endif /*EPOS_CPOSPOSTINSTALLEDLIST_H_*/
