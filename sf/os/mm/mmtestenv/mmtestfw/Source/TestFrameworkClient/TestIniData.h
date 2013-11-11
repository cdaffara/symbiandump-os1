// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Local CTestIniData class, derived from CIniData.lib
// 
//

#if !defined(__CTESTINIDATA_H__)
#define __CTESTINIDATA_H__

#include <e32base.h>

enum TTestIniPanic
        {
        ESectionNameTooBig,
        EKeyNameTooBig,
        };

class CTestIniData : public CBase 
        {
public:
        // Constructor, pass in name of ini file to open
        // Default search path is 'c:\system\data' on target filesystem
        // ie. 'NewL(_L("c:\\system\\data\\ttools.ini"))' is equivalent 
        // to 'NewL(_L("ttools.ini"))' 
        static CTestIniData* NewL(const TDesC& aName);
        virtual ~CTestIniData();

		// Panic
		void Panic(TTestIniPanic aPanic);

        // Read text value reguardless of section
        // Returns: True(Success) or false(Failure)
        TBool FindVar(const TDesC &aKeyName,   // Key to look for
                      TPtrC &aResult);         // Buffer to store value

        // Read integer value reguardless of section
        // Returns: True(Success) or false(Failure)
        TBool FindVar(const TDesC &aKeyName,   // Key to look for
                      TInt &aResult);          // Int ref to store result

        // Read text value under section
        // Returns: True(Success) or false(Failure)
        TBool FindVar(const TDesC &aSection,   // Section to look under
                      const TDesC &aKeyName,   // Key to look for
                      TPtrC &aResult);         // Buffer to store result

        // Read integer value under section
        // Returns: True(Success) or false(Failure)
        TBool FindVar(const TDesC &aSection,   // Section to look under
                      const TDesC &aKeyName,   // Key to look for
                      TInt &aResult);          // Int ref to store result

protected:
        CTestIniData();
        void ConstructL(const TDesC& aName);
private:
        HBufC* iName;
        HBufC* iToken;
        TPtr iPtr;
        };

#endif
