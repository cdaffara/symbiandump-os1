// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include    "epos_cpospsymediacapabilitytest.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosPSYMediaCapabilityTest::CPosPSYMediaCapabilityTest(
    CPosPSYResults& aResults,
    const RPointerArray<TPositionInfo>& aPositionArray,
    const TPosPSYInfo& aPSYInfo)
    {
    BaseConstruct(aResults, aPositionArray, aPSYInfo);
    }

// Two-phased constructor.
CPosPSYMediaCapabilityTest* CPosPSYMediaCapabilityTest::NewLC(
    CPosPSYResults& aResults,
    const RPointerArray<TPositionInfo>& aPositionArray,
    const TPosPSYInfo& aPSYInfo)
    {
    CPosPSYMediaCapabilityTest* self = 
        new (ELeave) CPosPSYMediaCapabilityTest(aResults, aPositionArray, aPSYInfo);
    CleanupStack::PushL( self );
    return self;
    }

// Destructor
CPosPSYMediaCapabilityTest::~CPosPSYMediaCapabilityTest()
    {
    }

// ---------------------------------------------------------
// CPosPSYMediaCapabilityTest::TestCapabilityL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMediaCapabilityTest::TestCapabilityL()
    {
    TBuf<KCapabilityBufSize> buf;
	_LIT(KTests,"---- Testing Media Capability ----");
    AddResultL(KTests, EDisplayMessage);
    
    if (iPSYInfo.Capabilities() & TPositionModuleInfo::ECapabilityMedia)
        {    
		for (TInt i = 0; i < iPositionArray->Count(); i++)
            {
            TUint8 nrOfSentences = 0;
            HPositionGenericInfo* genInfo = 
                static_cast<HPositionGenericInfo*>((*iPositionArray)[i]);
            
            if (genInfo->GetValue(EPositionFieldMediaLinks, nrOfSentences) == KErrNotFound)
                {   
                iNrOfMediaNotSet++;
                }
            else if (nrOfSentences == 0)
                {
                iNrOfIncompleteMedia++;
                }
            else
                {
                TBuf8<KMediaLinkSize> mediaBuf;
				TInt nrOfSentencesNotSet = 0;
				TInt nrOfEmptySentences = 0;
                for (TInt sentenceNr = EPositionFieldMediaLinksStart; 
                     sentenceNr < (EPositionFieldMediaLinksStart + nrOfSentences); 
                     sentenceNr++)
                    {
                    mediaBuf.Zero();
                    if (genInfo->GetValue((TUint16)sentenceNr, mediaBuf) == KErrNotFound)
                        {   
                        nrOfSentencesNotSet++;
                        }
                    else if (mediaBuf.Length() == 0)
                        {
                        nrOfEmptySentences++;
                        }
					}
				
				if (nrOfSentencesNotSet > 0)
					{
					iNrOfMediaStartNotSet++;
					_LIT(KError, "Number of media sentences is %d but only %d sentences are set in request %d.");
					buf.Format(KError, nrOfSentences, nrOfSentences - nrOfSentencesNotSet, i+1);
					AddResultL(buf, EErrorMessage);
					}
				if (nrOfEmptySentences > 0)
					{
					iNrOfEmptyMediaSentences++;
					_LIT(KError, "Number of media sentences is %d but %d sentences is empty in request %d.");
					buf.Format(KError, nrOfSentences, nrOfEmptySentences, i+1);
					AddResultL(buf, EErrorMessage);
					}
                }
            }

        CheckResultsL();
        }
    else
        {
        _LIT(KASupportInfo, "Media Capability not checked because capability not supported.");
        AddResultL(KASupportInfo, EInfoMessage);
        }
    }

// ---------------------------------------------------------
// CPosPSYMediaCapabilityTest::CheckResultsL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMediaCapabilityTest::CheckResultsL()
    { 
	TBuf<KCapabilityBufSize> buf;
    
	if (iNrOfIncompleteMedia > 0)
        {   
        _LIT(KError, "Media Capability supported but number of sentences is 0 in %d of %d cases.");
        buf.Format(KError, iNrOfIncompleteMedia, iPositionArray->Count());
        AddResultL(buf, EErrorMessage);
        }
    if (!iNrOfIncompleteMedia && !iNrOfMediaNotSet)
        {
        _LIT(KInfo, "Media Capability supported and all requests returned a number of sentences.");
        AddResultL(KInfo, EInfoMessage);
        }
    if (iNrOfMediaNotSet > 0)
        {   
        _LIT(KError, "Media Capability supported but number of sentences not set in %d of %d cases.");
        buf.Format(KError, iNrOfMediaNotSet, iPositionArray->Count());
        AddResultL(buf, EErrorMessage);
        }
    if (iNrOfMediaStartNotSet > 0)
        {
        _LIT(KError, "Number of Media Capabilities set but not all sentences set in %d of %d cases.");
        buf.Format(KError, iNrOfMediaStartNotSet, 
            (iPositionArray->Count() - iNrOfMediaNotSet - iNrOfIncompleteMedia - iNrOfEmptyMediaSentences));
        AddResultL(buf, EErrorMessage);
        }
    if (iNrOfEmptyMediaSentences)
        {
        _LIT(KError, "Number of Media Capabilities set but some sentences empty in %d of %d cases.");
        buf.Format(KError, iNrOfEmptyMediaSentences, 
            (iPositionArray->Count() - iNrOfMediaNotSet - iNrOfIncompleteMedia - iNrOfMediaStartNotSet));
        AddResultL(buf, EErrorMessage);
        }
    }

//  End of File
