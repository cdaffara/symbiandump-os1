// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
    @file
    @test
*/

#include "TE_smsprtbase.h"

/**
 *	Pre-test step function for Encoding tests.
 * 
 *  @return EPass or EFail verdict.
 */
TVerdict CSmsPrtEncodingTestStep::doTestStepPreambleL()
	{
    CSmsBaseTestStep::doTestStepPreambleL();
	
	iTestAlphabet = CSmsPrtTestAlphabet::NewL();
	
	return TestStepResult();
	}

/**
 *	Post-test step function for Encoding tests.
 * 
 *  @return EPass or EFail verdict.
 */
TVerdict CSmsPrtEncodingTestStep::doTestStepPostambleL()
	{
	delete iTestAlphabet;
	iTestAlphabet = NULL;
	
    iCharSets.Reset();
    iAdditionalCharSets.Reset();
	
 	return CSmsBaseTestStep::doTestStepPostambleL();
 	}

/**
 *  Computes the expected number of PDUs and expected number of free characters
 *  (User Data Units) in the last PDU. This is computed from the number of
 *  octets in the message.
 * 
 *  @param aOctetsCount                   Number of octets.
 *  @param aAlphabet                      Data Coding Scheme in use.
 *  @param aExpectedPDUs                  Returned expected PDUs.
 *  @param aExpectedFreeUDUnitsInLastPDU  Returned expected free UDUnits.
 */
void CSmsPrtEncodingTestStep::GetExpectedPDUCountAndUDUnitsFreeL(TInt aOctetsCount, 
			                                                     TSmsDataCodingScheme::TSmsAlphabet aAlphabet,
			                                                     TSmsEncoding aEncodingExpected,
			                                                     TInt& aExpectedPDUs,
				                                                 TInt& aExpectedFreeUDUnitsInLastPDU) const
	{
	//
	// Calculate the IEs that will be added...
	//
	TInt  expectedIESize = 0;
	
	if (aAlphabet == TSmsDataCodingScheme::ESmsAlphabet7Bit)
		{
		if (aEncodingExpected == ESmsEncodingTurkishSingleShift  ||
		    aEncodingExpected == ESmsEncodingTurkishLockingShift  ||
		    aEncodingExpected == ESmsEncodingSpanishSingleShift  ||
		    aEncodingExpected == ESmsEncodingPortugueseSingleShift  ||
		    aEncodingExpected == ESmsEncodingPortugueseLockingShift)
			{
			// National Single Shift or Locking Shift is 3 bytes each.
			if (expectedIESize == 0)
				{
				expectedIESize += 3 + 1; // Extra 1 byte for the UDHL per PDU...
				}
			else
				{
				expectedIESize += 3;
				}
			}
		else if (aEncodingExpected == ESmsEncodingTurkishLockingAndSingleShift  ||
		         aEncodingExpected == ESmsEncodingPortugueseLockingAndSingleShift)
			{
			// National Single Shift and Locking Shift costs 6 bytes total.
			if (expectedIESize == 0)
				{
				expectedIESize += 6 + 1; // Extra 1 byte for the UDHL per PDU...
				}
			else
				{
				expectedIESize += 6;
				}
			}
		}
		
	if ((aAlphabet == TSmsDataCodingScheme::ESmsAlphabet7Bit  &&  (aOctetsCount + ((expectedIESize*8)+6)/7) > 160)  ||
		(aAlphabet != TSmsDataCodingScheme::ESmsAlphabet7Bit  &&  (aOctetsCount + expectedIESize) > 140))
		{
		// Concatenated IE is 5 bytes...
		if (expectedIESize == 0)
			{
			expectedIESize += 5 + 1; // Extra 1 byte for the UDHL per PDU...
			}
		else
			{
			expectedIESize += 5;
			}
		}
	
	//
	// Now compute the PDUs required.
	//
	if (aAlphabet == TSmsDataCodingScheme::ESmsAlphabet7Bit)
		{
		TInt  octetsPerPDU    = 160 - ((expectedIESize*8)+6)/7;
		TInt  completePDUs    = aOctetsCount / octetsPerPDU;
		TInt  octetsInLastPDU = aOctetsCount % octetsPerPDU;

		aExpectedPDUs                 = completePDUs + (octetsInLastPDU > 0);
		aExpectedFreeUDUnitsInLastPDU = octetsPerPDU - octetsInLastPDU;

		if (octetsInLastPDU == 0)
			{
			aExpectedFreeUDUnitsInLastPDU = 0;
			}
		}
	else if (aAlphabet == TSmsDataCodingScheme::ESmsAlphabet8Bit  ||
			 aAlphabet == TSmsDataCodingScheme::ESmsAlphabetUCS2)
		{
		TInt  octetsPerPDU    = 140 - expectedIESize;
		TInt  completePDUs    = aOctetsCount / octetsPerPDU;
		TInt  octetsInLastPDU = aOctetsCount % octetsPerPDU;

		aExpectedPDUs                 = completePDUs + (octetsInLastPDU > 0);
		aExpectedFreeUDUnitsInLastPDU = octetsPerPDU - octetsInLastPDU;

		if (octetsInLastPDU == 0)
			{
			aExpectedFreeUDUnitsInLastPDU = 0;
			}
		}
	else
		{
		User::Leave(KErrNotSupported);
		}
	} // CSmsPrtEncodingTestStep::GetExpectedPDUCountAndUDUnitsFreeL


/**
 *  Computes the expected number of supported characters in a string based
 *  on the old API behaviour.
 * 
 *  @param aDes                             String to test.
 *  @param aAlphabet                        Data Coding Scheme in use.
 *  @param aExpectedUnconvertibleChars      Returned expected unconvertible characters.
 *  @param aExpectedFirstUnconvertibleChar  Returned first unconvertible character.
 */
void CSmsPrtEncodingTestStep::GetExpectedEncodingInfoL(TDesC& aDes,
													   TSmsDataCodingScheme::TSmsAlphabet aAlphabet,
				                                       TInt& aExpectedUnconvertibleChars,
													   TInt& aExpectedFirstUnconvertibleChar) const
	{
	aExpectedUnconvertibleChars     = 0;
	aExpectedFirstUnconvertibleChar = aDes.Length();

	if (aAlphabet == TSmsDataCodingScheme::ESmsAlphabet7Bit)
		{
		for (TInt pos = aDes.Length()-1;  pos >= 0;  pos--)
			{
			// For each character in the descriptor check for support in the standard tables
			if (iTestAlphabet->IsCharListedInCharacterSetL(KCharacterSetStandard7bitGSM, aDes[pos]) == EFalse  &&
				iTestAlphabet->IsCharListedInCharacterSetL(KCharacterSetStandard7bitGSMShift, aDes[pos]) == EFalse)
				{
				aExpectedUnconvertibleChars++;
				aExpectedFirstUnconvertibleChar = pos;
				}
			}
		}
	else if (aAlphabet == TSmsDataCodingScheme::ESmsAlphabet8Bit)
		{
		for (TInt pos = aDes.Length()-1;  pos >= 0;  pos--)
			{
			if (aDes[pos] > 0xff)
				{
				aExpectedUnconvertibleChars++;
				aExpectedFirstUnconvertibleChar = pos;
				}
			}
		}
	else if (aAlphabet == TSmsDataCodingScheme::ESmsAlphabetUCS2)
		{
		// NOP, everything accepted!
		}
	else
		{
		User::Leave(KErrNotSupported);
		}
	} // CSmsPrtEncodingTestStep::GetExpectedEncodingInfoL


/**
 *  Computes the expected number of supported characters in a string based
 *  on the new API behaviour.
 * 
 *  @param aDes                               String to test.
 *  @param aAlphabet                          Data Coding Scheme in use.
 *  @param aEncoding                          Alternative 7bit Encoding (if used).
 *  @param aExpectedUnconvertibleChars        Returned expected unconvertible characters.
 *  @param aExpectedDowngradedChars           Returned expected downgraded characters.
 *  @param aExpectedAlternativeEncodingChars  Returned expected alternative encoding characters.
 *  @param aExpectedFirstUnconvertibleChar    Returned first unconvertible character.
 */
void CSmsPrtEncodingTestStep::GetExpectedEncodingInfoL(TDesC& aDes,
													   TSmsDataCodingScheme::TSmsAlphabet aAlphabet,
													   TSmsEncoding aEncoding,
				                                       TInt& aExpectedUnconvertibleChars,
				                                       TInt& aExpectedDowngradedChars,
				                                       TInt& aExpectedAlternativeEncodingChars,
													   TInt& aExpectedFirstUnconvertibleChar) const
	{
	aExpectedUnconvertibleChars       = 0;
	aExpectedDowngradedChars          = 0;
	aExpectedAlternativeEncodingChars = 0;
	aExpectedFirstUnconvertibleChar   = aDes.Length();

	if (aAlphabet == TSmsDataCodingScheme::ESmsAlphabet7Bit)
		{
		for (TInt pos = aDes.Length()-1;  pos >= 0;  pos--)
			{
			if (iTestAlphabet->IsCharListedInCharacterSetL(KCharacterSetStandard7bitGSM, aDes[pos]) == EFalse  &&
				iTestAlphabet->IsCharListedInCharacterSetL(KCharacterSetStandard7bitGSMShift, aDes[pos]) == EFalse)
				{
				// The test design tries to use non-overlapping character sets to make 
				// Encoding / Downgrading tests easier to write - thus KCharacterSetTurkish7bitGSMLocking
				// is chosen not to include any characters in KCharacterSetStandard7bitGSMShift
				if ((aEncoding == ESmsEncodingTurkishLockingShift  ||
					 aEncoding == ESmsEncodingTurkishLockingAndSingleShift)  &&
					iTestAlphabet->IsCharListedInCharacterSetL(KCharacterSetTurkish7bitGSMLocking, aDes[pos]))
					{
					aExpectedAlternativeEncodingChars++;
					if (iTestAlphabet->IsCharDowngradedByCharacterSetL(KCharacterSetTurkish7bitGSMLocking, aDes[pos]))
						{
						aExpectedDowngradedChars++;
						}
					}
                else if ((aEncoding == ESmsEncodingTurkishSingleShift  ||
                          aEncoding == ESmsEncodingTurkishLockingAndSingleShift)  &&
                          iTestAlphabet->IsCharListedInCharacterSetL(KCharacterSetTurkish7bitGSMShift, aDes[pos]))
                    {
                    aExpectedAlternativeEncodingChars++;
                    if (iTestAlphabet->IsCharDowngradedByCharacterSetL(KCharacterSetTurkish7bitGSMShift, aDes[pos]))
                        {
                        aExpectedDowngradedChars++;
                        }
                    }
                else if (aEncoding == ESmsEncodingSpanishSingleShift  &&
                          iTestAlphabet->IsCharListedInCharacterSetL(KCharacterSetSpanish7bitGSMShift, aDes[pos]))
                    {
                    aExpectedAlternativeEncodingChars++;
                    if (iTestAlphabet->IsCharDowngradedByCharacterSetL(KCharacterSetSpanish7bitGSMShift, aDes[pos]))
                        {
                        aExpectedDowngradedChars++;
                        }
                    }
                else if ((aEncoding == ESmsEncodingPortugueseLockingShift  ||
                     aEncoding == ESmsEncodingPortugueseLockingAndSingleShift)  &&
                    iTestAlphabet->IsCharListedInCharacterSetL(KCharacterSetPortuguese7bitGSMLocking, aDes[pos]))
                    {
                    aExpectedAlternativeEncodingChars++;
                    if (iTestAlphabet->IsCharDowngradedByCharacterSetL(KCharacterSetPortuguese7bitGSMLocking, aDes[pos]))
                        {
                        aExpectedDowngradedChars++;
                        }
                    }
                else if ((aEncoding == ESmsEncodingPortugueseSingleShift  ||
                          aEncoding == ESmsEncodingPortugueseLockingAndSingleShift)  &&
                          iTestAlphabet->IsCharListedInCharacterSetL(KCharacterSetPortuguese7bitGSMShift, aDes[pos]))
                    {
                    aExpectedAlternativeEncodingChars++;
                    if (iTestAlphabet->IsCharDowngradedByCharacterSetL(KCharacterSetPortuguese7bitGSMShift, aDes[pos]))
                        {
                        aExpectedDowngradedChars++;
                        }
                    }				
				else if (iTestAlphabet->IsCharDowngradedByCharacterSetL(KCharacterSetDowngrading7bitGSM, aDes[pos])  ||
					     iTestAlphabet->IsCharDowngradedByCharacterSetL(KCharacterSetDowngrading7bitGSMExtended, aDes[pos]))
					{
					aExpectedDowngradedChars++;
					}
				else
					{
					aExpectedUnconvertibleChars++;
					aExpectedFirstUnconvertibleChar = pos;
					}
				}
			else if (iTestAlphabet->IsCharDowngradedByCharacterSetL(KCharacterSetStandard7bitGSM, aDes[pos])  ||
					 iTestAlphabet->IsCharDowngradedByCharacterSetL(KCharacterSetStandard7bitGSMShift, aDes[pos]))
				{
				aExpectedDowngradedChars++;
				}
			}
		}
	else if (aAlphabet == TSmsDataCodingScheme::ESmsAlphabet8Bit)
		{
		for (TInt pos = aDes.Length()-1;  pos >= 0;  pos--)
			{
			if (aDes[pos] > 0xff)
				{
				aExpectedUnconvertibleChars++;
				aExpectedFirstUnconvertibleChar = pos;
				}
			}
		}
	else if (aAlphabet == TSmsDataCodingScheme::ESmsAlphabetUCS2)
		{
		// NOP, everything accepted!
		}
	else
		{
		User::Leave(KErrNotSupported);
		}
	} // CSmsPrtEncodingTestStep::GetExpectedEncodingInfoL

void CSmsPrtEncodingTestStep::DoEncodingTestL(RSocket aSocket,
                                              const TDesC& aTestName,
                                              TInt aTestSequences,
                                              RArray<TPtrC>& aTestCharSetNames,
                                              TSmsDataCodingScheme::TSmsAlphabet aAlphabet,
                                              TSmsEncoding aEncodingToUse,
                                              TSmsEncoding aEncodingExpected)
    {
   
    //
    // Work out the number of characters to test with...
    //
    TInt  charSetSize = 0;
    TInt countTestCharSetNames = aTestCharSetNames.Count();
    
    for (TInt charSet = 0; charSet < countTestCharSetNames ; ++charSet)
        {
        TInt  charactersInCharSet = iTestAlphabet->GetCharacterSetSizeL(aTestCharSetNames[charSet]);
        charSetSize += charactersInCharSet;
        }

    TInt  characters    = charSetSize * aTestSequences;
    TInt additionalTestCharacters = 0; 
    
    // perform the test
    DoEncodingTestL(aSocket, aTestName, characters, aTestCharSetNames,
                    additionalTestCharacters, aTestCharSetNames,
                    aAlphabet, aEncodingToUse, aEncodingExpected);
    }

void CSmsPrtEncodingTestStep::DoEncodingTestL(RSocket aSocket,
                                              const TDesC& /*aTestName*/,
                                              TInt aTestCharacters,
                                              RArray<TPtrC>& aTestCharSetNames,
                                              TInt aAdditionalTestCharacters,
                                              RArray<TPtrC>& aAdditionalTestCharSetNames,
                                              TSmsDataCodingScheme::TSmsAlphabet aAlphabet,
                                              TSmsEncoding aEncodingToUse,
                                              TSmsEncoding aEncodingExpected)
    {
    
	//
	// Display the test name and details...
	//
    TInt countTestCharSetNames = aTestCharSetNames.Count();
    
    for (TInt charSet = 0;  charSet < countTestCharSetNames;  ++charSet)
        {
        TInt  charactersInCharSet = iTestAlphabet->GetCharacterSetSizeL(aTestCharSetNames[charSet]);
        INFO_PRINTF3(_L("  Using character set \"%S\" which contains %d characters..."),
                     &aTestCharSetNames[charSet], charactersInCharSet);
        }

    INFO_PRINTF2(_L("Testing encode functions starting with %d characters."), aTestCharacters);
    TInt countAdditionalTestCharSetNames = aAdditionalTestCharSetNames.Count();
    
    for (TInt charSet = 0;  charSet < countAdditionalTestCharSetNames;  ++charSet)
        {
        TInt  charactersInCharSet = iTestAlphabet->GetCharacterSetSizeL(aAdditionalTestCharSetNames[charSet]);
        INFO_PRINTF3(_L("  Using additional character set \"%S\" which contains %d characters..."),
                     &aAdditionalTestCharSetNames[charSet], charactersInCharSet);
        }
    
	INFO_PRINTF2(_L("Total test length is %d characters."), aTestCharacters + aAdditionalTestCharacters);
	
	//
	// Build a message with all the characters in and monitor the Encoding
	// Info values. Allow a few extra bytes (e.g. the number of character sets
	// used) to allow for the fact that we add aTestCharSetNames.Count()
	// characters at a time, and therefore it may overrun by this amount.
	//
	TInt characters = aTestCharacters;
	TInt bufSize = characters + countTestCharSetNames;
	if (aAdditionalTestCharacters)
	    {
	    bufSize += aAdditionalTestCharacters + aAdditionalTestCharSetNames.Count();
	    }
	
	HBufC*  originalBuf = HBufC::NewLC(bufSize);
	HBufC*  expectedBuf = HBufC::NewLC(bufSize);
    TPtr  originalPtr = originalBuf->Des();
    TPtr  expectedPtr = expectedBuf->Des();
	TInt  octetsCount = 0;
	TInt  downgradesCount = 0;
	TInt  unconvertibleCount = 0;
	TInt  lastOctetCountTested = 0;
	
	iTestAlphabet->ResetAllCharacterSetPositionsL();
	for (TInt count = 0;  count < characters;  count += countTestCharSetNames)
		{
		TBool  extendedCharAdded = EFalse;
		
		//
		// Add one character from each character set...
		//
		for (TInt charSet = 0;  charSet < countTestCharSetNames;  ++charSet)
			{
			TInt  oldOctetCount = octetsCount;
			
			iTestAlphabet->AppendCharactersL(aTestCharSetNames[charSet], 1,
					                         originalPtr, expectedPtr,
					                         octetsCount, downgradesCount,
					                         unconvertibleCount);
			
			//
			// Check for characters that cross the PDU boundary. In this case
			// we will allow the GetEncodingInfoL() the benefit of the doubt
			// of how many PDUs and characters free, since it may know better.
			// But this only extends to 1 second guess.
			//
			if (octetsCount - oldOctetCount > 1)
				{
				extendedCharAdded = ETrue;
				}
			}

		//INFO_PRINTF2(_L("count=%d"), count);
		//INFO_PRINTF2(_L("originalPtr=%S"), &originalPtr);
		//INFO_PRINTF2(_L("expectedPtr=%S"), &expectedPtr);

		//
		// Skip non-important cases randomly to speed up testing. This only
		// occurs in the middle 90% of cases and never if the gap between tests
		// is more than half a PDU (since the adjusting of octet count due to
		// multi-octet 7bit character might miss a PDU boundary event).
		//
		if ((octetsCount - lastOctetCountTested  < 80)  && // ensure that at least one character per PDU is tested
			(count+countTestCharSetNames < characters) &&  // ensure that the last characters is tested
			lastOctetCountTested > 10)   // ensure that the first characters are tested
			{
			continue;
			}

		lastOctetCountTested = octetsCount;
		
		CSmsMessage*  smsMessage = CreateSmsMessageLC(originalPtr, aAlphabet);

		//
		// Perform various character tests on the smsMessage
		//
		octetsCount = PerformCharacterTestsL(smsMessage, originalPtr, aAlphabet, aEncodingToUse, aEncodingExpected, octetsCount, extendedCharAdded);
		
		//
		// If this is the last test case then prepare to send the message...
		//
		if (count+countTestCharSetNames >= characters)
		    {
			
		    for (TInt additionalcount = 0;  additionalcount < aAdditionalTestCharacters;  additionalcount += aAdditionalTestCharSetNames.Count())
		        {
		        
	            // 
	            // destroy the previous message
	            //
	            CleanupStack::PopAndDestroy(smsMessage);
	                
			    //
			    // Add one character from each character set up to the number of additional characters...
			    //
		        extendedCharAdded = EFalse;
			    for (TInt charSet = 0;  charSet < aAdditionalTestCharSetNames.Count();  charSet++)
			        {
			        TInt  oldOctetCount = octetsCount;
			                
			        iTestAlphabet->AppendCharactersL(aAdditionalTestCharSetNames[charSet], 1,
			                                                 originalPtr, expectedPtr,
			                                                 octetsCount, downgradesCount,
			                                                 unconvertibleCount);
			        
			        //
			        // Check for characters that cross the PDU boundary. In this case
			        // we will allow the GetEncodingInfoL() the benefit of the doubt
			        // of how many PDUs and characters free, since it may know better.
			        // But this only extends to 1 second guess.
			        //
			        if (octetsCount - oldOctetCount > 1)
			            {
			            extendedCharAdded = ETrue;
			            }
			        }   
			    
			    //
			    // Test all the additional characters
			    //
			        
			    //
			    // Create a CSmsMessage...
			    //
			    smsMessage = CreateSmsMessageL(originalPtr, aAlphabet);
			    CleanupStack::PushL(smsMessage);

			    //
			    // Perform various character tests on the smsMessage
			    //
			    octetsCount = PerformCharacterTestsL(smsMessage, originalPtr, aAlphabet, aEncodingToUse, aEncodingExpected, octetsCount, extendedCharAdded);
			                     
			    }
			
			
			INFO_PRINTF2(_L("Sending final message of %d characters..."),
					     originalPtr.Length());

			TRAPD(sendErr, SendSmsL(smsMessage, aSocket));
		    TESTCHECK(sendErr, KErrNone, "Sending SMS");
			}
		
		CleanupStack::PopAndDestroy(smsMessage);
		}
	
	//
	// Now receive the message and confirm the message is as expected...
	//
	INFO_PRINTF1(_L("Waiting for incoming SMS..."));
	if (TimedWaitForRecvL(aSocket, 60*1000000))
		{
		CSmsMessage*  smsMessage = RecvSmsL(aSocket);
		CleanupStack::PushL(smsMessage);
		TestSmsContentsL(smsMessage, expectedPtr);
		TESTCHECK(smsMessage->Alternative7bitEncoding(), aEncodingExpected, "Getting the Alternative7bitEncoding");
		CleanupStack::PopAndDestroy(smsMessage);
		}
	else
		{
		ERR_PRINTF1(_L("<font color=FF0000>Failed: No SMS message received.</font>"));
        SetTestStepResult(EFail);
		}

	CleanupStack::PopAndDestroy(2, originalBuf); // originalBuf, expectedBuf
	}

TInt CSmsPrtEncodingTestStep::PerformCharacterTestsL(
        CSmsMessage* aSmsMessage, 
        TDesC16& aOriginalPtr,
        TSmsDataCodingScheme::TSmsAlphabet& aAlphabet,
        TSmsEncoding aEncodingToUse,
        TSmsEncoding aEncodingExpected,
        TInt aOctetsCount,
        TBool aExtendedCharAdded
        )
    {
    TESTCHECK(aSmsMessage->SetAlternative7bitEncoding(aEncodingToUse), KErrNone, "Switch on Alternative 7Bit Encoding if required");

    //
    // Find the expected number of supported characters that would have
    // appeared using the old API...
    //
    TInt  expectedUnconvertibleChars, expectedFirstUnconvertibleChar;
    
    GetExpectedEncodingInfoL(aOriginalPtr, aAlphabet,
                             expectedUnconvertibleChars,
                             expectedFirstUnconvertibleChar);
    TBool  msgSupported = (expectedUnconvertibleChars == 0);

    //
    // Test the old IsSupportedL() methods from CSmsMessage...
    //
    // Note: Not supported for UCS2!
    //
    TInt  numberOfUnconvertibleCharacters, indexOfFirstUnconvertibleCharacter;
    
    if (aAlphabet != TSmsDataCodingScheme::ESmsAlphabetUCS2)
        {
        TESTCHECKCONDITION(aSmsMessage->IsSupportedL(aOriginalPtr, numberOfUnconvertibleCharacters,
                                                    indexOfFirstUnconvertibleCharacter) == msgSupported,
                                                    "Checking if a buffer can be encoded without loss of information");
        TESTCHECK(numberOfUnconvertibleCharacters, expectedUnconvertibleChars, "Checking number of unconvertable charachters");
        TESTCHECK(indexOfFirstUnconvertibleCharacter, expectedFirstUnconvertibleChar, "Checking index of the first unconvertable charachter");
        }

    //
    // Get the expected number of supported characters, downgraded characters
    // and alternative encodings that would have appeared using the new API...
    //
    TInt  expectedDowngradedChars, expectedAlternativeEncodingChars;

    GetExpectedEncodingInfoL(aOriginalPtr, aAlphabet, aEncodingToUse,
                             expectedUnconvertibleChars,
                             expectedDowngradedChars,
                             expectedAlternativeEncodingChars,
                             expectedFirstUnconvertibleChar);
    msgSupported = (expectedUnconvertibleChars == 0);

    //
    // Test the new IsSupportedL() method from CSmsMessage...
    //
    // Note: Not supported for UCS2!
    //
    TInt  numberOfDowngradedCharacters, numberRequiringAlternativeEncoding;
    
    if (aAlphabet != TSmsDataCodingScheme::ESmsAlphabetUCS2)
        {
        TESTCHECKCONDITION(aSmsMessage->IsSupportedL(aOriginalPtr, numberOfUnconvertibleCharacters,
                                                    numberOfDowngradedCharacters,
                                                    numberRequiringAlternativeEncoding,
                                                    indexOfFirstUnconvertibleCharacter) == msgSupported,
                                                    "Checking if a buffer can be encoded without loss of information");
        TESTCHECK(numberOfUnconvertibleCharacters, expectedUnconvertibleChars, "Checking number of unconvertable charachters");
        TESTCHECK(numberOfDowngradedCharacters, expectedDowngradedChars, "Checking number of Downgraded charachters");
        TESTCHECK(numberRequiringAlternativeEncoding, expectedAlternativeEncodingChars, "Checking number requiring alternative encoding charachters");
        TESTCHECK(indexOfFirstUnconvertibleCharacter, expectedFirstUnconvertibleChar, "Checking index of the first unconvertable charachter");
        }

    //
    // Get the expected PDU count and free units...
    //
    TInt  expectedPDUs, expectedFreeUDUnitsInLastPDU;
    
    GetExpectedPDUCountAndUDUnitsFreeL(aOctetsCount, aAlphabet, aEncodingExpected,
                                       expectedPDUs, expectedFreeUDUnitsInLastPDU);

    //
    // Check the new encoding info function...
    //
    TInt  pdus, freeUDUnitsInLastPDU;
    
    aSmsMessage->GetEncodingInfoL(pdus, numberOfUnconvertibleCharacters,
                                 numberOfDowngradedCharacters,
                                 freeUDUnitsInLastPDU);
    
    //
    // Number of octets can be mis-calculated so make the necessary adjustments
    //
    TInt octetsCount = aOctetsCount;
    
    if (aExtendedCharAdded  &&
        (pdus == expectedPDUs + 1  ||  freeUDUnitsInLastPDU + 1 == expectedFreeUDUnitsInLastPDU))
        {
        //
        // Possibly the extended character has confused our test function.
        // Re-calulate the test values.
        //
        INFO_PRINTF2(_L("Adjusting octet count due to multi-octet character."), pdus);
        octetsCount++;
        GetExpectedPDUCountAndUDUnitsFreeL(octetsCount, aAlphabet, aEncodingExpected,
                                           expectedPDUs, expectedFreeUDUnitsInLastPDU);
        }

    TESTCHECK(pdus, expectedPDUs, "Checking number of expected PDUs");
    TESTCHECK(numberOfUnconvertibleCharacters, expectedUnconvertibleChars,  "Checking number of unconvertable charachters");
    TESTCHECK(numberOfDowngradedCharacters, expectedDowngradedChars, "Checking number of Downgraded charachters");
    TESTCHECK(freeUDUnitsInLastPDU, expectedFreeUDUnitsInLastPDU, "Checking freeUDUnitsInLastPDU");

    //
    // Check the old PDU count function...
    //
    pdus = aSmsMessage->NumMessagePDUsL();
    TESTCHECK(pdus, expectedPDUs, "Checking number of expected PDUs");
    
    return octetsCount;
    }
