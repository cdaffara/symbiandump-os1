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



#ifndef RTdPrivacyServer_H
#define RTdPrivacyServer_H

//  INCLUDES
#include <e32base.h>
#include <EPos_Privacy.h>


// CLASS DECLARATION

class RTdPrivacyServer : public RSessionBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        RTdPrivacyServer();

    public: // New functions

        /**
        * Opens a connection with the server. This method must be called
        * before any other operation.
        * @param aRequestSource The source of the location request.
        * @return @p KErrNone if the operation was successful, otherwise
        * one of the system-wide error codes.
        */
        TInt Connect();

		TInt SendCrap(TInt aFunction);
				 
    public: // Functions from base classes

        /**
        * From RSessionBase
        */
        void Close();


    private:

        TInt StartServer();

        // By default, prohibit copy constructor
        RTdPrivacyServer( const RTdPrivacyServer& );
        // Prohibit assigment operator
        RTdPrivacyServer& operator= ( const RTdPrivacyServer& );

    private:    // Data

    };

#endif      // RTdPrivacyServer_H

// End of File
