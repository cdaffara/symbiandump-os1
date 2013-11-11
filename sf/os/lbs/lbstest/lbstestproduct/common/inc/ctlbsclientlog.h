/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __CT_LBS_CLIENT_LOG_H__
#define __CT_LBS_CLIENT_LOG_H__

//  INCLUDES
#include <e32base.h>
#include <test/testexecutestepbase.h>

// CONSTANTS

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Implements the CLbsLog interface, and puts log entries in TEF log.
*/
class CT_LbsClientLog : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CT_LbsClientLog* NewL(CTestExecuteLogger& aLogger);

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CT_LbsClientLog* NewLC(CTestExecuteLogger& aLogger);
        
        /**
        * Destructor.
        */
        virtual ~CT_LbsClientLog();

    public: // New functions
    
    	void SetLogger(CTestExecuteLogger& aLogger){ iLogger = aLogger; };
	    CTestExecuteLogger& Logger() {return iLogger;};

    public: // Functions  from base classes

        /**
        * Add a line to the log file.
        * @param aLine The line of text to append.
        */
        void Put(const TDesC& aLine);

        /**
        * Add an error line to the log file.
        * @param aLine The error line of text to append.
        */
        void PutError(const TDesC& aLine);
        
    private:

        /**
        * C++ default constructor.
        */
    	CT_LbsClientLog();

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL(CTestExecuteLogger& aLogger);

        // By default, prohibit copy constructor
        CT_LbsClientLog( const CT_LbsClientLog& );
        // Prohibit assigment operator
        CT_LbsClientLog& operator= ( const CT_LbsClientLog& );

    private:  // Data
        CTestExecuteLogger		iLogger;
    };

#endif // __CT_LBS_CLIENT_LOG_H__   
            
// End of File
