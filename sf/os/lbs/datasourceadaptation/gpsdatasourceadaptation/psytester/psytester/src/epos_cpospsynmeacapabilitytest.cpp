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
#include <lbspositioninfo.h>
#include "epos_cpospsynmeacapabilitytest.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosPSYNMEACapabilityTest::CPosPSYNMEACapabilityTest(
    CPosPSYResults& aResults,
    const RPointerArray<TPositionInfo>& aPositionArray,
    const TPosPSYInfo& aPSYInfo)
    {
    BaseConstruct(aResults, aPositionArray, aPSYInfo);
    }

// Two-phased constructor.
CPosPSYNMEACapabilityTest* CPosPSYNMEACapabilityTest::NewLC(
    CPosPSYResults& aResults,
    const RPointerArray<TPositionInfo>& aPositionArray,
    const TPosPSYInfo& aPSYInfo)
    {
    CPosPSYNMEACapabilityTest* self = 
        new (ELeave) CPosPSYNMEACapabilityTest(aResults, aPositionArray, aPSYInfo);
    CleanupStack::PushL( self );
    return self;
    }

// Destructor
CPosPSYNMEACapabilityTest::~CPosPSYNMEACapabilityTest()
    {
    }

// ---------------------------------------------------------
// CPosPSYNMEACapabilityTest::CheckResultsL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYNMEACapabilityTest::CheckResultsL()
    { 
    TBuf<KCapabilityBufSize> buf;
    
	if (iNrOfIncompleteNmea > 0)
        {   
        _LIT(KError, "NMEA Capability supported but number of NMEA sentences is 0 in %d of %d cases.");
        buf.Format(KError, iNrOfIncompleteNmea, iPositionArray->Count());
        AddResultL(buf, EErrorMessage);
        }
    if (!iNrOfIncompleteNmea && !iNrOfNMEANotSet)
        {
        _LIT(KInfo, "NMEA Capability supported and all requests returned a number of NMEA sentences.");
        AddResultL(KInfo, EInfoMessage);
        }
    if (iNrOfNMEANotSet > 0)
        {   
        _LIT(KError, "NMEA Capability supported but number of sentences not set in %d of %d cases.");
        buf.Format(KError, iNrOfNMEANotSet, iPositionArray->Count());
        AddResultL(buf, EErrorMessage);
        }
    if (iNrOfNMEAStartNotSet > 0)
        {
        _LIT(KError, "Number of NMEA Capabilities set but not all sentences set in %d of %d cases.");
        buf.Format(KError, iNrOfNMEAStartNotSet, 
            (iPositionArray->Count() - iNrOfNMEANotSet - iNrOfIncompleteNmea - iNrOfEmptyNMEASentences));
        AddResultL(buf, EErrorMessage);
        }
    if (iNrOfEmptyNMEASentences)
        {
        _LIT(KError, "Number of NMEA Capabilities set but some sentences empty in %d of %d cases.");
        buf.Format(KError, iNrOfEmptyNMEASentences, 
            (iPositionArray->Count() - iNrOfNMEANotSet - iNrOfIncompleteNmea - iNrOfNMEAStartNotSet));
        AddResultL(buf, EErrorMessage);
        }
    }
// ---------------------------------------------------------
// CPosPSYNMEACapabilityTest::TestCapabilityL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYNMEACapabilityTest::TestCapabilityL()
    {
    TBuf<KCapabilityBufSize> buf;
	_LIT(KTests,"---- Testing NMEA Capability ----");
    AddResultL(KTests, EDisplayMessage);
    
    if (iPSYInfo.Capabilities() & TPositionModuleInfo::ECapabilityNmea)
        { 
        for (TInt i = 0; i < iPositionArray->Count(); i++)
            {
            TUint8 nrOfSentences = 0;
            HPositionGenericInfo* genInfo = 
                static_cast<HPositionGenericInfo*>((*iPositionArray)[i]);
            
            if (genInfo->GetValue(EPositionFieldNMEASentences, nrOfSentences) == KErrNotFound)
                {   
                iNrOfNMEANotSet++;
                }
            else if (nrOfSentences == 0)
                {
                iNrOfIncompleteNmea++;
                }
            else
                {
                TBuf8<KCapabilityBufSize> nmeaBuf;
				TInt nrOfSentencesNotSet = 0;
				TInt nrOfEmptySentences = 0;
                for (TInt sentenceNr = EPositionFieldNMEASentencesStart; 
                     sentenceNr < (EPositionFieldNMEASentencesStart + nrOfSentences); 
                     sentenceNr++)
                    {
                    nmeaBuf.Zero();
                    if (genInfo->GetValue((TUint16)sentenceNr, nmeaBuf) == KErrNotFound)
                        {   
                        nrOfSentencesNotSet++;
                        }
                    else if (nmeaBuf.Length() == 0)
                        {
                        nrOfEmptySentences++;
                        }
                    else
                        {
						//Correct do nothing
                        }
					}
				
				if (nrOfSentencesNotSet > 0)
					{
					iNrOfNMEAStartNotSet++;
					_LIT(KError, "Number of NMEA sentences is %d but only %d sentences are set in request %d.");
					buf.Format(KError, nrOfSentences, nrOfSentences - nrOfSentencesNotSet, i+1);
					AddResultL(buf, EErrorMessage);
					}
				if (nrOfEmptySentences > 0)
					{
					iNrOfEmptyNMEASentences++;
					_LIT(KError, "Number of NMEA sentences is %d but %d NMEA sentences is empty in request %d.");
					buf.Format(KError, nrOfSentences, nrOfEmptySentences, i+1);
					AddResultL(buf, EErrorMessage);
					}
                }
            }
        
        CheckResultsL();
        }
    else
        {
        _LIT(KASupportInfo, "NMEA Capability not checked because capability not supported.");
        AddResultL(KASupportInfo, EInfoMessage);
        }
   }

//  End of File
