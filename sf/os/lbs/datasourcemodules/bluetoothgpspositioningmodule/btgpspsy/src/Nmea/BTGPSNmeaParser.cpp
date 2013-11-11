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
#include <e32std.h>

#include "BTGPSNmeaParser.h"
#include "BTGPSMessageDef.h"
#include "BTGPSPanic.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
//NMEA sentence field delimiter
const TUint8 KNmeaFieldDelimiter = ',';

//NMEA sentecne checksum delimiter
const TUint8 KNmeaChecksumDelimiter = '*';

//Data start postion of NMEA sentence
const TInt KNmeaDataStartPos = 1; 

//Minimum length of NMEA sentence(starter and check sum)
const TInt KNmeaMininumLength = 4;

//Checksum field length
const TInt KNmeaChecksumFieldLength = 2;

//Maximum length of address field length
const TInt KNmeaMaxAddressFieldLength = 32;

//Decimal seperator
const TUint8 KDecimalSeperator = '.';

/** 
* Structure for address field Id map
*/
struct TAddressFieldMessageIdMap
    {
    TText8 iField[KNmeaMaxAddressFieldLength];  //Field text
    TInt iId;                                   //Field id
    };

/**
* NMEA and Sirf specific NMEA sentence sId field text and 
* id mapping defination
*/
const struct TAddressFieldMessageIdMap KNmeaAddressIdMap[]=
    {
        {"GPGSV", ENmeaGPGSV},
        {"GPGLL", ENmeaGPGLL},
        {"GPRMC", ENmeaGPRMC},
        {"GPVTG", ENmeaGPVTG},
        {"GPGGA", ENmeaGPGGA},
        {"GPGSA", ENmeaGPGSA},
        {"PSRF107", ENmeaPSRF107},
        {"PSRF103", ENmeaPSRF103},
        {"PSRF101", ENmeaPSRF101},
        {"PSRF105", ENmeaPSRF105},
        {"PSRF201", ENmeaPSRF201},
        {"PNOK",    ENmeaProprietaryNok}
    };

/**
* PNOK message id field text and id mapping defination
*/
const struct TAddressFieldMessageIdMap KNmeaPNokAddressIdMap[]=
    {
        {"OK",          ENmeaPNokOk},
        {"RESET",       ENmeaPNokReset},
        {"MSGS",        ENmeaPNokPeriodicMsgs},
        {"LOWPWR",      ENmeaPNokLowPwrMode},
        {"NIGHTMODE",   ENmeaPNokNightMode},
        {"VER",         ENmeaPNokVersionInfo},
        {"STAT",        ENmeaPNokPeriodicEvents},
        {"TIME",        ENmeaPNokReferenceTime}
     };



// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// TBTGPSNmeaParser::TBTGPSNmeaParser
// -----------------------------------------------------------------------------
TBTGPSNmeaParser::TBTGPSNmeaParser() : TDelimitedParserBase8()
    {
    SetDelimiter(KNmeaFieldDelimiter);
    }

// -----------------------------------------------------------------------------
// TBTGPSNmeaParser::SetNmeaSentence
// -----------------------------------------------------------------------------
void TBTGPSNmeaParser::SetNmeaSentence(const TDesC8& aNmea)
    {
    iNmea.Set(aNmea);
    iId = ENmeaNull;
    if(CheckValidity())
        {
        Parse(TrimSentence());
        GetMessageId();
        }
    }

// -----------------------------------------------------------------------------
// TBTGPSNmeaParser::NmeaSentence
// -----------------------------------------------------------------------------
const TDesC8& TBTGPSNmeaParser::NmeaSentence() const
    {
    return iNmea;
    }

// -----------------------------------------------------------------------------
// TBTGPSNmeaParser::IsValid
// -----------------------------------------------------------------------------
TBool TBTGPSNmeaParser::IsValid() const
    {
    return iId!=ENmeaNull;
    }

// -----------------------------------------------------------------------------
// TBTGPSNmeaParser::MessageId
// -----------------------------------------------------------------------------
TInt TBTGPSNmeaParser::MessageId() const
    {
    return iId;
    }

// -----------------------------------------------------------------------------
// TBTGPSNmeaParser::GetMessageId
// -----------------------------------------------------------------------------
void TBTGPSNmeaParser::GetMessageId()
    {
    iId = ENmeaUnknown;
    Reset();
    TPtrC8 address;
    if(GetFieldBlock(ENmeaFieldAddressField,address)==KErrNone)
        {
        for(TInt i=0; 
            i<sizeof(KNmeaAddressIdMap)/sizeof(TAddressFieldMessageIdMap);i++)
            {
            TPtrC8 knownAddress(KNmeaAddressIdMap[i].iField);
            if(address.Compare(knownAddress)==KErrNone)
                {
                TInt messageId = KNmeaAddressIdMap[i].iId;
                //If message if Nokia proprietary message, we have to check next field
                if(messageId == ENmeaProprietaryNok)
                    {
                    Reset();
                    Inc();Inc();
                    if(GetNext(address)==KErrNone)
                        {
                        for(TInt j=0; 
                            j<sizeof(KNmeaPNokAddressIdMap)/sizeof(TAddressFieldMessageIdMap); 
                            j++)
                            {
                            TPtrC8 nokAddress(KNmeaPNokAddressIdMap[j].iField);
                            if(address.Compare(nokAddress)==KErrNone)
                                {
                                iId = KNmeaPNokAddressIdMap[j].iId;
                                return;
                                }
                            }
                        }
                    }
                iId = messageId;
                return;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// TBTGPSNmeaParser::GetFieldBlock
// -----------------------------------------------------------------------------
TInt TBTGPSNmeaParser::GetFieldBlock(TInt aFieldId, TPtrC8& aField) const
    {
    Reset();
    if(aFieldId != ENmeaFieldAddressField)
        {
        TInt messageId = MessageId();
        //Check fieldId matches message Id
        if(aFieldId<messageId || aFieldId>=messageId+KNmeaMessageIdSkip)
            {
            return KErrNotFound;
            }

        //Skip address field and other fields
        if(messageId != ENmeaNull)
            {
            for(TInt i=0; i<aFieldId-messageId+2; i++)
                {
                Inc();
                }
            }
        }
    return GetNext(aField);
    }

// -----------------------------------------------------------------------------
// TBTGPSNmeaParser::IsNullField
// -----------------------------------------------------------------------------
TBool TBTGPSNmeaParser::IsNullField(TInt aFieldId) const
    {
    TPtrC8 fieldBlock;
    if(GetFieldBlock(aFieldId, fieldBlock)==KErrNone)
        {
        return fieldBlock.Length()==0;
        }
    //error case
    return ETrue;
    }


// -----------------------------------------------------------------------------
// TBTGPSNmeaParser::GetFieldData
// -----------------------------------------------------------------------------
template  <>
TInt TBTGPSNmeaParser::GetFieldData<TReal32>(TInt aFieldId, TReal32& aData) const
    {
    TPtrC8 fieldBlock;
    TInt err = GetFieldBlock(aFieldId, fieldBlock);
    if(err==KErrNone && fieldBlock.Length()>0)
        {
        TLex8 lex(fieldBlock);
        return lex.Val(aData, KDecimalSeperator);
        }
    return err;
    }

// -----------------------------------------------------------------------------
// TBTGPSNmeaParser::GetFieldData
// -----------------------------------------------------------------------------
template  <>
TInt TBTGPSNmeaParser::GetFieldData<TReal>(TInt aFieldId, TReal& aData) const
    {
    TPtrC8 fieldBlock;
    TInt err = GetFieldBlock(aFieldId, fieldBlock);
    if(err==KErrNone && fieldBlock.Length()>0)
        {
        TLex8 lex(fieldBlock);
        return lex.Val(aData, KDecimalSeperator);
        }
    return err;
    }

// -----------------------------------------------------------------------------
// TBTGPSNmeaParser::GetFieldData
// -----------------------------------------------------------------------------
template  <>
TInt TBTGPSNmeaParser::GetFieldData<TInt>(TInt aFieldId, TInt& aData) const
    {
    TPtrC8 fieldBlock;
    TInt err = GetFieldBlock(aFieldId, fieldBlock);
    if(err==KErrNone && fieldBlock.Length()>0)
        {
        TLex8 lex(fieldBlock);
        return lex.Val(aData);
        }
    return err;
    }

// -----------------------------------------------------------------------------
// TBTGPSNmeaParser::GetFieldData<TInt>
// -----------------------------------------------------------------------------
#define GetFieldDataImplementation(s) \
template <> TInt TBTGPSNmeaParser::GetFieldData<s>(TInt aFieldId, s& aData) const \
    { \
    TPtrC8 fieldBlock; \
    TInt err = GetFieldBlock(aFieldId, fieldBlock); \
    if(err==KErrNone && fieldBlock.Length()>0) \
        { \
        TLex8 lex(fieldBlock); \
        return lex.Val(aData); \
        } \
    return err; \
    }

// -----------------------------------------------------------------------------
// TBTGPSNmeaParser::NextFieldData
// -----------------------------------------------------------------------------
template <class T>
TInt TBTGPSNmeaParser::NextFieldData(T& aData) const
    {
    //This function must be called when the sentence is valid
    __ASSERT_DEBUG(IsValid(), Panic(EPanicInvalidNmeaSentence));

    TBuf8<KNmeaMaxAddressFieldLength> fieldBuf;
    TPtrC8 fieldBlock(fieldBuf);
    TInt err = GetNext(fieldBlock);
    if(err==KErrNone)
        {
        TLex8 lex(fieldBlock);
        return lex.Val(aData);
        }
    return err;
    }

// -----------------------------------------------------------------------------
// TBTGPSNmeaParser::CheckValidity
// -----------------------------------------------------------------------------
TBool TBTGPSNmeaParser::CheckValidity() const
    {
    TInt length = iNmea.Length();
    
    //Check the length of the NMEA sentence
    if(length<KNmeaMininumLength)
        {
        return EFalse;
        }

    //Check checksum
    TInt checksumDelimiterPos = iNmea.LocateReverse(KNmeaChecksumDelimiter);
    if((checksumDelimiterPos == KErrNotFound) || (checksumDelimiterPos + KNmeaChecksumFieldLength > length))
        {
        return EFalse;
        }

    //Get checsum field
    TLex8 lex(iNmea.Mid(checksumDelimiterPos, KNmeaChecksumFieldLength));

    //Check checksum 
    return VerifyChecksum(iNmea);
    }

// -----------------------------------------------------------------------------
// TBTGPSNmeaParser::TrimSentence
// -----------------------------------------------------------------------------
const TPtrC8 TBTGPSNmeaParser::TrimSentence() const
    {
    //Check checksum
    TInt checksumDelimiterPos = iNmea.LocateReverse(KNmeaChecksumDelimiter);

    return iNmea.Mid(
        KNmeaDataStartPos,
        checksumDelimiterPos-KNmeaDataStartPos);
    }


// ----------------------------------------------------------------------------
// TBTGPSNmeaParser::VerifyChecksum
// ----------------------------------------------------------------------------
//
TBool TBTGPSNmeaParser::VerifyChecksum(const TDesC8& aSentence) const
    {
    TUint8 checksum = 0;
    TInt i;
    TInt length = aSentence.Length();
    for (i = KNmeaDataStartPos; 
        i < length && aSentence[i] != KNmeaChecksumDelimiter; i++)
        {
        checksum ^= aSentence[i];
        }

    if (++i + KNmeaDataStartPos < aSentence.Length())
        {
        TUint8 sum = TUint8((CharToNibble(aSentence[i]) << 4) +
            CharToNibble(aSentence[i+1]));
        if (sum == checksum)
            {
            return ETrue;
            }
        }

    return EFalse;
    }

// ----------------------------------------------------------------------------
// TBTGPSNmeaParser::CharToNibble
// ----------------------------------------------------------------------------
//
TUint8 TBTGPSNmeaParser::CharToNibble(const TUint8 aChar) const
    {
    if (aChar <= '9')
        {
        return TUint8(aChar - '0');
        }
    else
        {
        return TUint8(aChar - 'A' + 10);
        }
    }

//  End of File



