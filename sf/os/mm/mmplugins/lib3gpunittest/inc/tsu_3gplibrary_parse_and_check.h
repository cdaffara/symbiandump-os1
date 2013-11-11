// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef TSU_3GPLIBRARY_PARSE_MAIN_HIGH_H
#define TSU_3GPLIBRARY_PARSE_MAIN_HIGH_H

#include <test/testexecutestepbase.h>
#include <c3gplibrary.h>

_LIT(K3GPLibParseAndCheckTypes, "3GPLibParseAndCheckTypes");
class C3GPLibParseAndCheckTypes : public CTestStep
    {
public:
    C3GPLibParseAndCheckTypes();
    TVerdict doTestStepPreambleL();
    TVerdict doTestStepL();
    TVerdict doTestStepPostambleL();
    
protected:
    virtual void ParseAndCheckFileL(const TDesC& aInputFile);
    
private:
    // file properties
    TInt ParseVideoProperties(C3GPParse& aParser,
                              RBuf8& aDecoderSpecificInfo,              
                              T3GPVideoPropertiesBase*& aProperties,
                              TUint& aLengthInMs);
    TInt ParseAudioProperties(C3GPParse& aParser, 
                              RBuf8& aAudioDecoderSpecificInfo,              
                              T3GPAudioPropertiesBase*& aAudioProperties,
                              TUint& aLength);
    TInt GetAudioDecoderSpecificInfo(RBuf8& aBuffer);
    TInt GetVideoDecoderSpecificInfo(RBuf8& aBuffer);       
    
private:     
    RBuf iInputFilePath;
    C3GPParse* iParser;
    
    T3GPVideoType iExpectedVideoType;
    T3GPAudioType iExpectedAudioType;
    };

#endif  // TSU_3GPLIBRARY_PARSE_MAIN_HIGH_H
