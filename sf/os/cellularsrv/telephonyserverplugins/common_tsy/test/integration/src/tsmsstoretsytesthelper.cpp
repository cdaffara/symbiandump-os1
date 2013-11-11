// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "tsmsstoretsytesthelper.h" 
#include "cctsyinidata.h"


TSmsStoreTsyTestHelper::TSmsStoreTsyTestHelper(CCTSYIntegrationTestSuiteStepBase& aTestStep)
			: TTsyTestHelperBase( aTestStep )
/**
* Constructor
*/	
			
	{
	}
	


void TSmsStoreTsyTestHelper::WaitForSmsStoreNotifyStoreEvent(RMobileSmsStore& aStore,
															 TEtelRequestBase& aRequestStatus,
															 TUint32 aEvent,
															 TUint32 aExpectedEvent,
															 TInt aIndex,
															 TInt aExpectedStatus)
/**
 * Wait for store event
 *
 * @param aStore is a subsession handle to the sms store
 * @param aRequestStatus is a reference to the request
 * @param aEvent is the event which occured
 * @param aExpectedEvent is the event which is expected
 * @param aIndex is the index of the changed slot
 * @param aExpectedStatus is the expected value of the completed status request
*/	
 	{

	// Wait for the request to complete
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeMedium);
	ASSERT_EQUALS(err, KErrNone, _L("RMobileSmsStore::NotifyStoreEvent did not complete"));

	while (	err == KErrNone && 
			aRequestStatus.Int() & aExpectedStatus && 
			aEvent != aExpectedEvent)
		{
		// Request has completed with incorrect result.
		// Maybe there are more notifications to consume before we get
		// the one we want.
	
		// Repost notification until it either times out or we get
		// the right result.
		aStore.NotifyStoreEvent(aRequestStatus, aEvent, aIndex);	
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeMedium);
		}

	ASSERT_EQUALS(aRequestStatus.Int(), aExpectedStatus, _L("RMobileSmsStore::NotifyStoreEvent wrong completion status"));
	ASSERT_BITS_SET(aEvent, aExpectedEvent,0, _L("RMobileSmsStore::NotifyStoreEvent incorrect store event"));
	
	// Cancel request if it is still pending
	if (aRequestStatus.Int() == KRequestPending)
		{
		aRequestStatus.Cancel();
		}
	} // TSmsStoreTsyTestHelper::WaitForSmsStoreNotifyStoreEvent


void TSmsStoreTsyTestHelper::CreateValidSmsPdu(TDes8& aString, TDesC16& aPhoneNumber, TDes8& aPdu, 
		TDataCodingScheme aDataCodingScheme, TBool aStatusReportRequest)
/**
 * Create a Sms PDU
 *
 * @param aString is the text of the message
 * @param aPhoneNumber is the destination phone number
 * @param aPdu is the PDU which will be sent
 * @param aDataCodingScheme is the coding schemem of the text of the message inside the PDU
 * @param aStatusReportRequest will be true if the TP-SRI bit (status request indicator) should be on.
*/
	{
    // If you want to convert user data to 7bit format:
    TBuf8<140> userData;     
    TUint16 byteCount = ( TUint16 )aString.Length();

    GsmLibCharConversion( TKGsmLibConvertToGsmFormat, aString, 
        byteCount );
    GsmLibSmsPackMessage( userData, aString, byteCount );	
    
     /**
     * Holds Sms Gsm Tdpu variable
     */          
    RMobileSmsMessaging::TMobileSmsGsmTpdu gsmPdu;
        
    TUint16 charCnt = ( TUint16 )( ( userData.Length()*8 ) / 7 );
    if ( ( ( ( gsmPdu.Length()*8 ) % 7 ) == 0) && 
        charCnt && charCnt <= gsmPdu.Length() )
        {
        if ( userData[charCnt-1] == 0 ) 
            // if the last char is NULL, Actual charCnt is charCnt-1
            {
            charCnt--;
            }
        else
            {
            // No implementation
            }
        }
        
	// Some code from STIF test
    const TUint smsMsgTypeSubmitOrSubmitR   = 0x01; 
    const TUint smsMsgVPabsolute            = 0x14;     // absolute format    
    TUint msgParameters( 0 );
    msgParameters = 0;
    msgParameters |= smsMsgTypeSubmitOrSubmitR;  
    msgParameters |= smsMsgVPabsolute;               
    if(aStatusReportRequest)
    	{
    	const TUint smsMsgTypeStatusReportRequest = 0x20;
    	msgParameters |= smsMsgTypeStatusReportRequest;
    	}
    

    TBuf8<12> destinationAddress;
    GsmConvUnicodeTo0340Addr(
        2,   //TON national number
        1,   //NPI isdn numbering plan
        destinationAddress,
        aPhoneNumber );     
        	
    aPdu.SetLength( 0 );                 //reset buffer       
    aPdu.Append( msgParameters );        //Message params           
    aPdu.Append( 0x00 );                 //TP-Message-Reference
    aPdu.Append( destinationAddress );   //TP-Destination-Address    
    aPdu.Append( 0x00 );                 //TP-Protocol-Identifier    
    aPdu.Append( (TUint8)aDataCodingScheme );    //TP-Data-Coding-Scheme (8bit)
    aPdu.Append( 0xA7 );                 //TP-Validity-Period       
    aPdu.Append( charCnt );              //TP-User-Data-Length
    aPdu.Append( userData );             //TP-User-Data           
	} // TSmsStoreTsyTestHelper::CreateValidSmsPdu
	
void TSmsStoreTsyTestHelper::GsmLibCharConversion( TUint aDirection, 
												   TDes8& aString, 
												   TUint16 aLength )
/**
 * TSmsStoreTsyTestHelper::GsmLibCharConversion    
 * The character codes supported by the MS
 * The former is 8-bit set and
 * the latter is a 7-bit set. Therefore, the characters must be converted
 * by using this function. However, characters having the same code in
 * both sets are not converted.
 * (other items were commented in a header).
 *
 * @param aDirection this is the format of the conversion, TKGsmLibConvertToGsmFormat or TKGsmLibConvertToNmpFormat 
 * @param aString is the string to be converted
 * @param aLength is the length of the string
*/
   	{
    TUint16 j( 0 );
    TUint16 i( 0 );

    TBuf8<180> aDestination;

    if ( aDirection == TKGsmLibConvertToNmpFormat )
        {
        for ( i = 0; i < aLength; i++ )
            {
            if ( aString[i] == 0xFF ) // This ends the string in SIM card
                {
                aDestination.Append( '\0' );
                break;
                }

            aString[i] &= 0x7F;     // only 7 bits used in GSM character set

            //Conversion made using GSM 7 bit default alphabet extension 
                //table. 
            if ( ( aString[i] > 0x5A) && ( aString[i] < 0x5F ) ||
                    ( aString[i] > 0x7A ) && ( aString[i] < 0x7F ) )
                    {
                    for ( j = 0 ; j < TKGsmLibConvTableSize; j++ )
                        {
                        if ( aString[i] == TKGsmExtensionConvTable[j].iHandsetChar )
                            {
                            aDestination.Append( 0x1B );//extension table flag
                            aDestination.Append( 
                                TKGsmExtensionConvTable[j].iSmsChar );
                            break;
                            }
                        else if ( aString[i] == 0x20A0 ) //Euro
                            {
                            aDestination.Append( 0x65 );
                            }
                        }
                        
                    }   

            // Conversion could be done only taking out the most significant 
            // bit from Unicode characters between 0x25 - 0x5A or 0x61 - 0x7A.
            // We should be used KGsmLibConvTable if it is outside that are 
            // or it is 0x40 ( @ ).
            else if ( ( aString[i] < 0x20 )
                || ( ( aString[i] > 0x5A ) && ( aString[i] < 0x61 ) )
                ||   ( aString[i] > 0x7A )
                ||   ( aString[i] == 0x40 ) )
                {

                for ( j = 0 ; j < TKGsmLibBidirTableSize; j++ )
                    {
                    if ( aString[i] == TKGsmLibConvTable[j].iSmsChar )
                        {
                        aDestination.Append( 
                            TKGsmLibConvTable[j].iHandsetChar );
                        break;
                        }
                    }
                }
            else
                {
                aDestination.Append( aString[i] );
                }
            }
            
        }
    else // aDirection = KGsmLibConvertToGsmFormat
        {
        for ( i = 0; i < aLength; i++ )
            {
            if ( aString[i] == '\0' )   // End of string in NMP format
                {
                aDestination.Append( 0xFF );    // End of string in SIM
                }

            else if ( ( ( aString[i]  < 0x10 )  && ( aString[i] != 0x0A )
                    && ( aString[i] != 0x0D ) ) || 
                    ( ( aString[i] > 0x7F ) && ( aString[i] < 0xC0 ) ) )
                {
                // Convert to SPACE if control character or Japanese character
                //   NOTE! (CR (0x0D) and LF (0x0A) are not converted.
                aDestination.Append( 0x20 );
                }
            else
                {
                //Conversion made using GSM 7 bit default alphabet extension 
                //table. 
                if ( ( aString[i] > 0x5A ) && ( aString[i] < 0x5F ) ||
                    ( aString[i] > 0x7A ) && ( aString[i] < 0x7F ) )

                    {
                    for ( j = 0 ; j < TKGsmLibConvTableSize; j++ )
                        {
                        if ( aString[i] == TKGsmExtensionConvTable[j].
                        iHandsetChar )
                            {
                            aDestination.Append( 0x1B );//extension table flag
                            aDestination.Append( 
                                TKGsmExtensionConvTable[j].iSmsChar );
                                break;
                            }
                        else if ( aString[i] == 0x20A0 ) //Euro
                            {
                            aDestination.Append( 0x65 );
                            }
                        }

                    }   

                // Conversion could be done only taking out the most 
                // significant bit from Unicode characters between 0x25 - 0x5A
                // or 0x61 - 0x7A. We should be used KGsmLibConvTable if it is
                // outside that are or it is 0x40 ( @ ).

                else if ( ( aString[i] < 0x25 )
                    || ( ( aString[i] > 0x5A ) && ( aString[i] < 0x61 ) )
                    || ( aString[i] > 0x7A )
                    || ( aString[i] == 0x40 ) )
                    {

                    for ( j = 0; j < TKGsmLibConvTableSize; j++ )
                        {
                        if ( aString[i] == TKGsmLibConvTable[j].iHandsetChar )
                            {
                            aDestination.Append( 
                                TKGsmLibConvTable[j].iSmsChar );
                            break;
                            }
                        }

                    }
                else
                    {
                    aDestination.Append( aString[i] );
                    }
                }
            }
        }
    aString.Copy( aDestination );
	}// TSmsStoreTsyTestHelper::TSmsStoreTsyTestHelper


TUint16 TSmsStoreTsyTestHelper::GsmLibSmsPackMessage( TDes8& aDest,
    												  TDes8& aSrc,
    												  TUint16 aByteCount )
/**
 * TSmsStoreTsyTestHelper::GsmLibSmsPackMessage
 * This function packs a given text string consisting of 7-bit
 * characters into 8 bit bytes in such a way that a 160-character
 * message takes only 140 bytes of memory after the compression.
 * (other items were commented in a header).
 *
 * @param aDest is the packaged message
 * @param aSrc is the original unpackaged message
 * @param aByteCount is the number of bytes in the src msg
 *
 * @return length of the coded message
*/
	{
    TUint16 si, di;           // Indexes
    TInt tmpModulo;          // Temporary; to improve efficiency
    
    // Reset the indexes.    
    si = 0;
    di = 0;
    tmpModulo = di % 7;

    while ( si < aByteCount )
        {
        // Is this the last character of the message?
        // If it is, just shift it to the right. Otherwise, fill the rest of 
        // the byte with the bits of the next source character.
        
        if ( si == ( aByteCount - 1 ) )
            {
            TUint num2 = (aSrc[si] >> tmpModulo);
            aDest.Append(num2);
            }
        else
            {
            TUint num2 = (aSrc[si] >> tmpModulo) | 
            ( aSrc[si + 1] << ( 7 - tmpModulo ) );
            aDest.Append( num2 );

            }
   
        di++;
        tmpModulo = di % 7;
        
        // Check if the destination byte could take the entire source 
        // character. In that case, the source character does not have to be 
        // divided and the next source character can be taken.
        
        if ( KErrNone == tmpModulo )
            {
            si += 2;
            }
        else
            {
            si++;
            }
        }
    
    // Return the length of the coded message.
    return( di );
   }

void TSmsStoreTsyTestHelper::GsmConvUnicodeTo0340Addr( TUint8 aTon,        
       												   TUint8 aNpi,        
       												   TDes8& aDest,       
       												   TDesC16& aSource )  
/** 
 * TSmsStoreTsyTestHelper::GsmConvUnicodeTo0340Addr
 * Converts type of number, numbering plan identification and
 * phone number given in unicode string
 * (other items were commented in a header).
 *
 * @param aTon is the Type of Number (national, international, etc)TMobileTON
 * @param aNpi is the Number Plan Indicator (TMobileNPI)
 * @param aDest is the converted unicode
 * @param aSource is the descriptor for conversion
*/        
    {
    
    if ( KErrNone != aSource.Length() )
        {
        GsmConvUnicodeToAddr( aTon, aNpi, aDest, aSource, ETrue );  
        }
    else
        {
        // Address length of 0 is a special case
        aDest.Append( 0 );      

        // Type-of-Address: TON = Unknown, NPI = ISDN/telephone num plan
        aDest.Append( 0x81 );  
        }
    }

void TSmsStoreTsyTestHelper::GsmConvUnicodeToAddr( TUint8 aTon,
   												   TUint8 aNpi,
   												   TDes8& aDest,
   												   TDesC16& aSource,   												   
   												   TBool aAddress )
/**
 * TSmsStoreTsyTestHelper::GsmConvUnicodeToAddr
 * Converts type of number, numbering plan identification and
 * phone number given in unicode string
 * (other items were commented in a header).
*/
    { 
    //get address length
    TUint numberLength = aSource.Length();
    TUint destLength = numberLength;
    TUint i( 0 );

    /*  Solve type of address
        Type of Address field format is as follows:

           7      6     5     4      3      2       1       0    bit
        |     |      |     |     |      |       |       |       |
        |  1  | Type of number   | Numbering-plan-identification|
        |     |      |     |     |      |       |       |       |
    */

    //type of number is three bits from left and fourth bit must be '1'
    TUint8 typeOfNumber = TUint8( aTon | 0x08 );
    //move four bit to the left
    typeOfNumber = TUint8( typeOfNumber << 4 );
    //numbering plan is four bits from left
    TUint8 numberingPlan = TUint8( aNpi & 0x0F );
    //set type of number and numbering plan to the typeOfNumber variable
    typeOfNumber = TUint8( typeOfNumber | numberingPlan );

    if ( 5 == aTon )    //Alphanumeric
        {
        TBuf8<KMaxLengthOfAddressData> addressData;
        TUint8 amountOfsemiOctets( 0 );
  
        GsmUnicodeAddrToAlphanumericAddr( aSource, addressData, amountOfsemiOctets );           

        //fill address length field
        if ( aAddress )
            {
            //amount of used semi-octets in address data
            aDest.Append( amountOfsemiOctets );
            }
        else    //0411 address
            {
            //Actually alphanumeric address data for 0411 address is not 
            //supported (see GSM 24.011 and 24.008)...
            //Length of address data + length of address type field
            aDest.Append( addressData.Length() + 1 );
            }

        //add typeOfNumber to the buffer
        aDest.Append( typeOfNumber );
        //aDest.Append( 0x81 );

        //add address data to the buffer
        aDest.Insert( 2, addressData );  
        }
    else
        {
        //number can contain character '+'
        if ( '+' == aSource[i] )
            {
            i++;
            destLength--;

            // Maximum amount of digits is 20 (3GPP TS 23.040)
            if ( destLength > KMaxAmountOfDigits ) 
                {                                                   
                destLength = KMaxAmountOfDigits;
                numberLength = 21;
                }
            }
        else
            {
            // Maximum amount of digits is 20 (3GPP TS 23.040)
            if ( destLength > KMaxAmountOfDigits ) 
                {                                                   
                destLength = KMaxAmountOfDigits;
                numberLength = 20;
                }
            }

        // calculate address length in bytes
        TUint bufferLength = ( destLength + 1 )/2 + 2;

        //set buffer length
        aDest.SetLength( bufferLength );

        //add length to the buffer
        if ( aAddress )
            {
            //amount of used semi-octets in address data
            aDest[0] = ( TUint8 )destLength;   
            }
        else
            {
            //length of whole address data - length of address length field
            aDest[0] = ( TUint8 )( bufferLength - 1 );
            }

        //add typeOfNumber to the buffer
        aDest[1] = typeOfNumber; 

        //needed variables
        TUint8 numLsb( 0 );
        TUint8 numMsb( 0 );
        TUint index( 2 );

        //add numbers to the buffer
        for ( ; ( i+1 ) < numberLength ; i+=2 )
            {
            numLsb = TUint8( aSource[i] - '0' );
            numMsb = TUint8( ( aSource[i+1] - '0' ) << 4 );

            aDest[index++] = TUint8( numLsb | numMsb );
            }

        //if numberLength is odd, last number is fixed with 0xF0
        if ( i < numberLength )
            {
            aDest[index] = TUint8( 0xF0 | ( aSource[i] - '0' ) );
            }
        }  
    }

void TSmsStoreTsyTestHelper::GsmUnicodeAddrToAlphanumericAddr( TDesC16& aSource,
    														   TDes8& aDestination,
    														   TUint8& aAmountOfSemiOctets )
/**
 * TSmsStoreTsyTestHelper::GsmUnicodeAddrToAlphanumericAddr
 * Converts Unicode characters to GSM 7-bit bit Default Alphabet
 * characters. Note that all Unicode characters 
 * are not supported by the GSM 7-bit bit Default Alphabet set. Such 
 * characters are converted to spaces or to the nearest substitute
 * (generally the acute/grave marks are removed). Control characters and 
 * special characters are converted to spaces. This method supports GSM 
 * 7-bit bit Default Alphabet extension table
 * mechanism.

 * After character conversion this method packs address data by calling
 * GsmLibSmsPackMessage method
 * (other items were commented in a header).
 *
 * @param aSource the original address to be converted
 * @param aDestination is the converted address
 * @param aAmountOfSemiOctets is how many semi-octets is used in packed address
*/
	{
	TUint16 j( 0 );
    TUint16 i( 0 );

    TBuf8<RMobilePhone::KMaxMobileTelNumberSize> alphaChars;
    for ( i = 0; i < aSource.Length(); i++ )
        {
        if ( ( aSource[i] > 0x24 ) && ( aSource[i] < 0x5B ) && 
             ( aSource[i] != 0x40 ) || 
             ( aSource[i] > 0x60 ) && ( aSource[i] < 0x7B ) )
            {
            // Conversion could be done only taking out the most significant 
            // bit from Unicode characters between 0x25 - 0x5A or 0x61 - 0x7A
            // excluding '@' (0x40).
            alphaChars.Append(aSource[i]);
            }
        else if ( ( aSource[i] > 0x5A) && ( aSource[i] < 0x5F ) ||
             ( aSource[i] > 0x7A ) && ( aSource[i] < 0x7F ) ||
             ( aSource[i] == 0x20AC ) )
            {
            //Conversion made using GSM 7 bit default alphabet extension 
            //table. Characters: [,\,],^,{,|,},~,&euro
            for ( j = 0 ; j < KGsmLibConvTableSize; j++ )
                {
                if ( aSource[i] == KGsmExtensionConvTable[j].iUnicodeChar )
                    {
                    alphaChars.Append( 0x1B );// extension table flag
                    alphaChars.Append( 
                        KGsmExtensionConvTable[j].iGsmChar );
                        break;
                    }
                }
            }   
        else
            {
            //Try to find character from conversion table.
            TBool notFound( ETrue );
            for ( j = 0; j < KGsmLibConvTableSize; j++ )
                {
                if ( aSource[i] == KGsmLibConvTable[j].iUnicodeChar )
                    {
                    alphaChars.Append( 
                        KGsmLibConvTable[j].iGsmChar );
                    notFound = EFalse;
                    break;
                    }
                } 
            
            if( notFound )
                {
                //Couldn't found similar character. Convert to SPACE.
                alphaChars.Append( 0x20 ); 
                }
            }
        }

    TUint8 packedChars = GsmLibPackAlphanumericAddress( aDestination, alphaChars );

    //How many unused bits packed address contains
    TUint8 unusedBits = (TUint8)( ( aDestination.Length() * 8 ) - 
                                  ( packedChars * 7 ) );

    //How many semi-octets is used in packed address.
    if ( unusedBits >= 4 )
        {
        //Last semi-octet doesn't contain address data
        aAmountOfSemiOctets = (TUint8)( ( aDestination.Length() * 2 ) - 1 );
        }
    else
        {
        //All semi-octets are used
        aAmountOfSemiOctets = (TUint8)( aDestination.Length() * 2 );
        }
    }

TUint8 TSmsStoreTsyTestHelper::GsmLibPackAlphanumericAddress( TDes8& aDest, TDes8& aSource )
/**
 * TSmsStoreTsyTestHelper::GsmLibPackAlphanumericAddress
 * This function packs a given text string consisting of 7-bit
 * characters into 8 bit bytes in such a way that a 11-character
 * address takes only 10 bytes of memory after the compression
 * (other items were commented in a header).
 *
 * @param aDest is the retultant string after packing
 * @param aSource is the 7 bit character string to be packed
 *
 * @return the length of the coded message.
*/
    {
    TUint8 si( 0 );
    TUint8 di( 0 );
    TUint tempModulo;
    
    // Set tempModulo and byteCount
    tempModulo = di % 7;
    TUint8 byteCount = (TUint8)aSource.Length();

    // Maximum length of address data field is 10 bytes. 10 bytes can contain
    // 11 compressed 7-bit characters. Because of this maximum
    // amount of characters in alphanumeric address is 11.
    if ( byteCount > ( KMaxLengthOfAddressData + 1 ) )
        {
        byteCount = KMaxLengthOfAddressData + 1;
        }   

    while ( si < byteCount )
        {
        // Is this the last character of the message?
        // If it is, just shift it to the right. Otherwise,
        // fill the rest of the byte with the bits of the next 
        // source character.
        
        if ( si == (byteCount - 1) )
            {
            TUint num2 = ( aSource[si] >> tempModulo );
            aDest.Append( num2 );
            }
        else
            {
            TUint num2 = ( aSource[si] >> tempModulo ) | 
                ( aSource[si + 1] << ( 7 - tempModulo ) );
            aDest.Append( num2 );
            }
   
        di++;
        tempModulo = di % 7;

        // Check if the destination byte could take the entire source 
        // character. In that case, the source character does not have to be 
        // divided and the next source character can be taken.
        
        if ( KErrNone == tempModulo )
            {
            si += 2;
            }
        else
            {
            si++;
            }
        }
    
    // Return the length of the coded message.
    return byteCount;
    }

	
TBool TSmsStoreTsyTestHelper::ToText(TDes &aRet,const TDesC8 &aOrigin,TBool aIsRecv)
/*
 * Helper function, which will extract the sms messege actual text  from an sms PDU. 
 * The text is decoded in a 7 bits caracters, and this methos convert it them to 8 bit characters and then 16 bit string.
 * 
 * @param aRet The String that will hold the message text.
 * @param aOrigin the actual PDU.
 * @param aIsRecv a boolean argument that is ETrue for a Receive PDU and EFalse if this is a send PDU.
 * 
 * @return ETrue if succesful, EFalse if not.
 *
 * 
 */
    {
    aRet.Zero();
    TUint first;
    if(aIsRecv)
    	{
    	if(aOrigin.Length()<2)
    		{
    		return EFalse;
    		}
    	first = (TUint)aOrigin[1]/2 + 13;
    	}
    else
    	{
    	if(aOrigin.Length()<3)
    		{
    		return EFalse;
    		}
    	first = (TUint)aOrigin[2]/2 + 8;
    	}
	if(aOrigin.Length()<first+1)
		{
		return EFalse;
		}
    
    TUint8 temp1=0;
    TUint shift=0;
    for(TInt i=first;i<aOrigin.Length();++i)
        {
        TUint16 maskLeft=(1<<(8-shift-1))-1;
        TUint16 maskRight=255-maskLeft;
        TUint8 left=aOrigin[i]&maskLeft;
        TUint right=aOrigin[i]&maskRight;
        left=left<<(shift);
        left = left | temp1;
        right=right>>(8-shift-1);
        aRet.Append(left);
        temp1=right;
        shift++;
        if(shift==7)
            {
            aRet.Append(right);
            shift=0;
            temp1=0;
            }
        }
    return ETrue;
    }

TBool TSmsStoreTsyTestHelper::EnsureStoreFullL(RMobileSmsStore& aSmsStore)
/*
 * Helper function, which make check if the sms store is full, and if not, fill all the empty slots in the store. 
 * 
 * @param aSmsStore A reference to the store session
 * 
 * @return ETrue if succesful and the store is full, EFalse if not.
 * 
 */
    {
	// Get SMS store info.
	TExtEtelRequestStatus getInfoStatus (aSmsStore,EMobilePhoneStoreGetInfo);	
	CleanupStack::PushL(getInfoStatus);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 storeInfo;
	RMobileSmsStore::TMobilePhoneStoreInfoV1Pckg storeInfoPckg (storeInfo);
	aSmsStore.GetInfo(getInfoStatus,storeInfoPckg);
	CHECK_EQUALS_L(iTestStep.WaitForRequestWithTimeOut(getInfoStatus, ETimeMedium), KErrNone,_L("RMobileSmsStore::GetInfo timed out"));
    CHECK_EQUALS_L(getInfoStatus.Int(), KErrNone, _L("RMobileSmsStore::GetInfo returned an error"));
    
	// Write a valid entries with into the sms store till it full
    RMobileSmsStore::TMobileGsmSmsEntryV1 entry;
	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg pckgEntry(entry);
 	entry.iServiceCentre.iTypeOfNumber =  RMobilePhone::EUnknownNumber; 
 	entry.iServiceCentre.iNumberPlan = RMobilePhone::EUnknownNumberingPlan; 
 	entry.iServiceCentre.iTelNumber =_L("01234");
	entry.iMsgStatus = RMobileSmsStore::EStoredMessageUnread;
	entry.iMsgData=KValidSmsPdu1;
	TExtEtelRequestStatus writeStatus (aSmsStore,EMobilePhoneStoreWrite);	
	CleanupStack::PushL(writeStatus);
	for (TInt i = storeInfo.iUsedEntries; i < storeInfo.iTotalEntries; i++)
		{
		entry.iIndex = -1; // need to reset this every time
		aSmsStore.Write(writeStatus,pckgEntry);
		CHECK_EQUALS_L(iTestStep.WaitForRequestWithTimeOut(writeStatus, ETimeMedium), KErrNone, _L("RMobileSmsStore::Write timed out"));
		CHECK_EQUALS_L(writeStatus.Int(), KErrNone, _L("RMobileSmsStore::Write returned an error"));
		}
	
	// Get SMS store info.
	aSmsStore.GetInfo(getInfoStatus,storeInfoPckg);
	CHECK_EQUALS_L(iTestStep.WaitForRequestWithTimeOut(getInfoStatus, ETimeMedium), KErrNone, _L("RMobileSmsStore::GetInfo, timed out"));
    CHECK_EQUALS_L(getInfoStatus.Int(), KErrNone, _L("RMobileSmsStore::GetInfo returned an error"));

    // Pop:
	//	writeStatus
	//	getInfoStatus
	CleanupStack::PopAndDestroy(2,&getInfoStatus);
    
	// Validate that the sms store is full
	return (storeInfo.iUsedEntries == storeInfo.iTotalEntries);
    }
