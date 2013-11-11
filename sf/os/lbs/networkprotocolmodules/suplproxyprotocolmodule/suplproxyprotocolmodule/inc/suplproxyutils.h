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



#ifndef SULPPROXYUTILS_H
#define SULPPROXYUTILS_H

//  INCLUDES
#include <e32std.h>

#ifdef _DEBUG
#include <e32debug.h>
#endif

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  utility functions for the supl proxy
*  
*/
class SuplProxyUtils
    {
    public: // New functions
        static TInt Read(const RMessage2& aMessage, const TInt aParam, TDes8& aDes, TInt aOffset=0);
        static TInt Write(const RMessage2& aMessage, const TInt aParam, const TDesC8& aDes, TInt aOffset=0);

    private:
        SuplProxyUtils( const SuplProxyUtils& );
        SuplProxyUtils& operator= ( const SuplProxyUtils& );
    };

#endif      // SULPPROXYUTILS_H  
            
// End of File
