/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef CPOSPSYLOG_H
#define CPOSPSYLOG_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>
#include "epos_tpospsyinfo.h"
#include "epos_cpospsyresult.h"
#include "epos_tpospsytestparameters.h"

// CONSTANTS
_LIT(KDelim, "********************************************************************\r\n");
_LIT(KNewLine, "\r\n");
const TInt KSummaryBufSize = 120;
const TInt KMaxNumberSize = 10;
    
// CLASS DECLARATION

/**
* Class that handles logging of test results to file.
*/
class CPosPSYLog : public CBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CPosPSYLog();

        /**
        * Two-phased constructor.
        */
        static CPosPSYLog* NewL();

        /**
        * Destructor.
        */
        virtual ~CPosPSYLog();

    public: // New functions

        /**
        * AppendResultL.
        * Function Appending a test result to log file
        * @param const CPosPSYResult& aPSYResult The result to 
        *        append to the log file
        */
        void AppendResultL(
        /* IN */ const CPosPSYResult& aPSYResult
        );
        
        // Added a new parameter, aTestParams to this method
        /**
        * CreateLogL.
        * Function Creating\Opening a file for test results to be written to
        * @param const TPosPSYInfo& aInfo information for creating/opening log file
        * @param const TPosPSYTestParameters& aTestParams PSY Tester parameters read from
        * the resource file
        */
        void CreateLogL(
        /* IN */ const TPosPSYInfo& aInfo,
        /* IN */ const TPosPSYTestParameters& aTestParams
        );
        
        // A new method to write the PSY Tester parameters 
        // to the log file
        /**
        * WriteTestParameters.
        * Function that writes the PSY Tester parameters to 
        * the log file
        * @param const TPosPSYTestParameters& aTestParams the Test
        * parameters that are read from the resource file
        */
        void WriteTestParametersL(
        /* IN */ const TPosPSYTestParameters& aTestParams
        );
        
        /**
        * MakeTestCaseHeaderL.
        * Function Creating a test case header in the log file
        * @param const TDesC& aTestName the name of the test case
        */
        void MakeTestCaseHeaderL(
        /* IN */ const TDesC& aTestName
        );

        /**
        * WriteSummaryAndCloseL.
        * Function Writing a summary of the tests and closing file server session
        */
        void WriteSummaryAndCloseL(TInt aStatus);
        
        /**
        * OpenLog.
        * Function that opens the Log file
        */
        void OpenLog();
        
        /**
        * CloseLog.
        * Function that closes the log file
        */
        void CloseLog();
        
  private:
    
        /**
        * By default EPOC constructor is private.
        */
        void ConstructL();
        
        void AppendLineL(
        /* IN */ const TDesC& aLine
        );

        void AppendLineL(
        /* IN */ const TInt aNumber
        );

        void MakeTestHeaderL(
        /* IN */ const TPosPSYInfo& aInfo
        );

        /**
        * Resets number of info:s, errors and warnings
        */
        void ResetResults();

        /**
        * Get logs dir
        */
        void GetLogsDirL(TDes& aLogsDir);

        /**
        * Get PSY Tester logs dir
        */
        void GetPsyTesterLogsDirL(TDes& aTesterLogsDir);


        // By default, prohibit copy constructor
        CPosPSYLog( const CPosPSYLog& );
        
        // Prohibit assigment operator
        CPosPSYLog& operator= ( const CPosPSYLog& );

    private:     // Data
        
        TBuf<KMaxFileName> iFileName;   //Stores the name of the Log File. Required for OpenLog() and CloseLog()
        RFile iCurrentLogFile;
        RFs   iFileSession;
        TInt  iCurrentPosition;
        TInt  iInfo;
        TInt  iWarning;
        TInt  iError;

    };

#endif      // CPOSPSYLOG_H

// End of File
