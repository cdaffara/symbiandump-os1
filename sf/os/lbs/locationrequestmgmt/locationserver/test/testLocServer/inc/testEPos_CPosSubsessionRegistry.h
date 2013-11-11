/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CPOSSUBSESSIONREGISTRY_H
#define CPOSSUBSESSIONREGISTRY_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CPosSubSession;

// CLASS DECLARATION

/**
*  Class to handle loading/unloading and referencing of PSYs.
*/
class CPosSubsessionRegistry :public CBase
	{
    public:  // Constructors and destructor
		static CPosSubsessionRegistry* NewL();
		~CPosSubsessionRegistry();

	public:		// New functions
        CPosSubSession* SubSessionFromHandleL(TUint aHandle);
        CPosSubSession* SubSessionFromIndex(TInt aIndex);
        TInt Count();
        void CloseSubSession(TUint aHandle);
        TInt AddInstanceL(CPosSubSession* aSubSession);

    private:
        CPosSubsessionRegistry();
        void ConstructL();
        CPosSubsessionRegistry( const CPosSubsessionRegistry& );
        CPosSubsessionRegistry& operator= ( const CPosSubsessionRegistry& );

    private:	// Data

        CObjectIx*      iRegistryIndex;
        CObjectCon*     iRegistryContainer;
        CObjectConIx*   iRegistryContainerIndex;
    };


#endif // CPOSSUBSESSIONREGISTRY_H

// End of File