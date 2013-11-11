/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef CSUPLPROXYPOSSUBSESSIONREGISTRY_H
#define CSUPLPROXYPOSSUBSESSIONREGISTRY_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CSuplProxyPosSubSession;

// CLASS DECLARATION

/**
*  Class to handle supl proxy pos msg api subsessions
*/
class CPosSubsessionRegistry :public CBase
	{
    public:  // Constructors and destructor
		static CPosSubsessionRegistry* NewL();
		~CPosSubsessionRegistry();

	public:		// New functions
        CSuplProxyPosSubSession* SubSessionFromHandleL(TUint aHandle);

        TInt Count();
        void CloseSubSession(TUint aHandle);
        TInt AddInstanceL(CSuplProxyPosSubSession* aSubSession);

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


#endif // CSUPLPROXYPOSSUBSESSIONREGISTRY_H

// End of File
