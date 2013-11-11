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
// INCLUDE FILES
//



#include <e32std.h>
#include <e32math.h>
#include <lbspositioninfo.h> 
#include <lbssatellite.h>            // TPositionSatelliteInfo
#include "BTGPSLogging.h"
#include "BTGPSRequestHandler.h"
#include "BTGPSConstantsManager.h"
#include "BTGPSFix.h"
#include "BTGPSLogging.h"
#include "BTGPSNmeaParser.h"
#include "BTGPSNmeaBuffer.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

//The knots to m/s conversion multiplier
const TReal64 KKnotInMetersPerSecond = 0.5144;

//Hardcoded accuracy
const TReal32 KHorizontalAccuracy = 10.0;
const TReal32 KVerticalAccuracy = 30.0;

//Decimal seperator
const TUint8 KDecimalSeperator = '.';

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CBTGPSFix::CBTGPSFix
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CBTGPSFix::CBTGPSFix()
    {
    Reset();
    }

// -----------------------------------------------------------------------------
// CBTGPSFix::SetNmeaBuffer
// Set NMEA Buffer
// -----------------------------------------------------------------------------
void CBTGPSFix::SetNmeaBuffer(CBTGPSNmeaBuffer* aNmeaBuffer)
    {
    iNmeaBuffer = aNmeaBuffer;
    }

// -----------------------------------------------------------------------------
// CBTGPSFix::Reset
// Reset the fix parameters.
// -----------------------------------------------------------------------------
void CBTGPSFix::Reset()
    {
    TRACESTRING("CBTGPSFix::Reset")
    ResetFix();
    
    ResetGSVTime();
    
    iNumberOfSatellites = 0;
    iSatelliteArray.Reset();
    for(TInt i=0; i<KMaxGSVMessageNumber; i++)
        {
        iNMEAGSVCache[i].Zero();
        }
        
    //Reset NMEA Buffer
    if(iNmeaBuffer!=NULL)
        {
        iNmeaBuffer->ResetBuffer();
        }
    }

// -----------------------------------------------------------------------------
// CBTGPSFix::ResetFix
// -----------------------------------------------------------------------------
void CBTGPSFix::ResetFix()
    {
    TRACESTRING("CBTGPSFix::ResetFix start...")
    TRealX nan;
    nan.SetNaN();
    
    ResetPosition();
    
    iSpeed=nan;
    iHeading=nan;
    
    iUsedSatellitesArray.Reset();
    iSatelliteTime = TTime(0);
    
    iHorizontalDoP = nan;
    iVerticalDoP = nan;
    iPositionDoP = nan;
    iSatellitesUsed = 0;
    
    //Clear the mask
    iReceivedMsgMask = 0;
    
    iGpsMode = EGpsModeSatellite;
    TRACESTRING("CBTGPSFix::ResetFix end")
    }

// -----------------------------------------------------------------------------
// CBTGPSFix::ResetGSVTime
// -----------------------------------------------------------------------------
void CBTGPSFix::ResetGSVTime()
    {
    TRACESTRING("CBTGPSFix::ResetGSVTime start...")
    iGsvTime = TTime(0);
    TRACESTRING("CBTGPSFix::ResetGSVTime end");
    }

// -----------------------------------------------------------------------------
// CBTGPSFix::ResetPosition
// 
// -----------------------------------------------------------------------------
void CBTGPSFix::ResetPosition()
    {
    TRACESTRING("CBTGPSFix::ResetPosition start...")
    TRealX nan;
    nan.SetNaN();
    
    iLatitude = nan;
    iLongitude = nan;
    iAltitude = nan;
    iTime = TTime(0);
    
    iValid = CBTGPSFix::EFixValidityUnkown;
    TRACESTRING("CBTGPSFix::ResetPosition end")
    }

// -----------------------------------------------------------------------------
// CBTGPSFix::FixState
// -----------------------------------------------------------------------------
CBTGPSFix::TState CBTGPSFix::FixState() const
    {
    return iValid;
    }

// -----------------------------------------------------------------------------
// CBTGPSFix::IsFixValid
// 
// -----------------------------------------------------------------------------
TBool CBTGPSFix::IsFixValid() const
    {
    return (iValid == EFixValid2D || iValid == EFixValid3D);
    }

// -----------------------------------------------------------------------------
// CBTGPSFix::IfFullNmeaPatternReceived
// 
// -----------------------------------------------------------------------------
TBool CBTGPSFix::IfFullNmeaPatternReceived() const
    {
    return iReceivedMsgMask == (EReceivedGSA|EReceivedGGA|EReceivedRMC);
    }
        
// -----------------------------------------------------------------------------
// CBTGPSFix::FixTime
// 
// -----------------------------------------------------------------------------
TTime CBTGPSFix::FixTime() const
    {
    return iTime;
    }

// -----------------------------------------------------------------------------
// CBTGPSFix::GsvTime
// 
// -----------------------------------------------------------------------------
TTime CBTGPSFix::GsvTime() const
    {
    return iGsvTime;
    }

// -----------------------------------------------------------------------------
// CBTGPSFix::ParseMessage
// Fills the information available from a command to the fix
// -----------------------------------------------------------------------------
CBTGPSFix::TParsingStatus CBTGPSFix::ParseMessage(const TBTGPSNmeaParser& aParser)
    {
    TRACESTRING("CBTGPSFix::ParseMessage start...")
    TRAPD(err, ParseMessageL(aParser));
    TParsingStatus ret = EUnkown;
    switch(err)
        {
        case KErrNone:
            ret = EInfoUpdated;
            break;
        case KErrNotSupported:
            ret = EUnkown;
            break;
        default:
            ret = EInvalid;
        }
    TRACESTRING("CBTGPSFix::ParseMessage end")
    return ret;
    }

// -----------------------------------------------------------------------------
// CBTGPSFix::ParseMessageL
// Fills the information available from a command to the fix
// -----------------------------------------------------------------------------
void CBTGPSFix::ParseMessageL(const TBTGPSNmeaParser& aParser)
    {
    if(!aParser.IsValid())
        {
        User::Leave(KErrCorrupt);
        }
        
    switch (aParser.MessageId())
        {
        case ENmeaGPGSV: 
            ParseGSVSentenceL(aParser);
            break;
            
        case ENmeaGPRMC:
            ParseRMCSentenceL(aParser);
            break;
            
        case ENmeaGPGGA:
            ParseGGASentenceL(aParser);
            break;
            
        case ENmeaGPGSA:
            ParseGSASentenceL(aParser);
            break;
            
        default:
            User::Leave(KErrNotSupported);
            break;
        }
    }

// -----------------------------------------------------------------------------
// CBTGPSFix::ParseGGASentenceL
// 
// -----------------------------------------------------------------------------
void CBTGPSFix::ParseGGASentenceL(
        const TBTGPSNmeaParser& aParser)
    {
    
    GetLatitudeLongitude(aParser, ENmeaFieldGPGGALatitude);
    
    //Get GPS Quality indicator. 
    //By default, assume GPS works in normal mode
    TInt gpsQualityIndicator(EGpsQualitySPS); 
    if(aParser.GetFieldData(ENmeaFieldGPGGAFix, gpsQualityIndicator)==KErrNone)
        {
        //Get satellite mode
        switch(gpsQualityIndicator)
            {
            case EGpsQualityManualInput:
                iGpsMode = EGpsModeManualInput;
                break;
            case EGpsQualityNotValid:
                iGpsMode = EGpsModeInvalid;
                break;
            case EGpsQualityEstimated:
                iGpsMode = EGpsModeEstimated;
                break;
            case EGpsQualitySimulation:
                iGpsMode = EGpsModeSimulator;
                break;
            case EGpsQualitySPS:
            case EGpsQualityDGPS:
            case EGpsQualityPPS:
            case EGpsQualityRTK:
            case EGpsQualityFloatRTK:
            default:
                //Specify any value other than those defined in NMEA spec as 
                //Satellite mode. This will enable PSY working with mal-functioning
                //GPS and future GPS technology.
                iGpsMode = EGpsModeSatellite;
                break;
            }
        }
        
    //Get number of satellites in use
    aParser.GetFieldData(
        ENmeaFieldGPGGASatellitesUsed,
        iSatellitesUsed);
        
    //Get HDOP
    aParser.GetFieldData(
        ENmeaFieldGPGGAHDOP,
        iHorizontalDoP);
    
    //Acquire altitude over msl
    TReal tempVal32(0); 
    aParser.GetFieldData(
        ENmeaFieldGPGGAMSLAltitude,
        tempVal32);

    //Get Geoidal seperation
    aParser.GetFieldData(
        ENmeaFieldGPGGAGeoidalSeperation,
        iGeoidalSeperation);
        
    //Caculate altitude over WGS84
    iAltitude = tempVal32+iGeoidalSeperation;
    
    //GGA received
    iReceivedMsgMask = iReceivedMsgMask | EReceivedGGA; 
    }

// -----------------------------------------------------------------------------
// CBTGPSFix::ParseGSVSentenceL
// 
// -----------------------------------------------------------------------------
void CBTGPSFix::ParseGSVSentenceL(
        const TBTGPSNmeaParser& aParser)
    {
    //Get message number
    TInt messageNumber(1);
    User::LeaveIfError(aParser.GetFieldData(
        ENmeaFieldGPGSVMessageNumber,
        messageNumber));
        
    //Check validity of message number
    if(messageNumber>KMaxGSVMessageNumber)
        {
        User::Leave(KErrCorrupt);
        }
    
    if(messageNumber == 1)
        {
        //Get total number messages
        TInt totalNumberOfMessages(iGsvTotalNumberOfMessage);
        User::LeaveIfError(aParser.GetFieldData(
            ENmeaFieldGPGSVNumberOfMessage,
            totalNumberOfMessages));
        iGsvTotalNumberOfMessage = totalNumberOfMessages;
        
        //Get satellites in view
        TInt satellitesInView(iGsvSatelliteInView); 
        aParser.GetFieldData(
            ENmeaFieldGPGSVSatellitesInView,
            satellitesInView);
    
        iGsvSatelliteInView = satellitesInView;
        
        //Clear cache
        for(TInt i=0; i<KMaxGSVMessageNumber; i++)
            {
            iNMEAGSVCache[i].Zero();
            }
        }
        
    if(iGsvTotalNumberOfMessage == 0)
        {
        //Wait until we get the message number 1
        return;
        }
        
    //copy NMEA sentence to cache
    SecureCopy(iNMEAGSVCache[messageNumber-1], aParser.NmeaSentence());
    if(messageNumber == iGsvTotalNumberOfMessage)
        {
        //Extract satellite info from all the cached messages.
        iNumberOfSatellites = 0;
        iSatelliteArray.Reset();
        
        //Extract sat info from cache
        for(TInt i=0; i<iGsvTotalNumberOfMessage-1; i++)
            {
            TBTGPSNmeaParser cacheMsgParser;
            cacheMsgParser.SetNmeaSentence(iNMEAGSVCache[i]);
            if(cacheMsgParser.IsValid())
                {
                ExtractSatInfoL(cacheMsgParser);
                }
            }
            
        //Extract sat info from this message
        ExtractSatInfoL(aParser);
        
        
        //Reset total number of message and number of sat in view
        iGsvTotalNumberOfMessage = 0;
        iGsvSatelliteInView = 0;

        //Clear cache
        for(TInt i=0; i<KMaxGSVMessageNumber; i++)
            {
            iNMEAGSVCache[i].Zero();
            }
            
        //Set GSV time
        iGsvTime.UniversalTime();
        TRACESTRING2("CBTGPSFix:: Set GSV Time=%d", iGsvTime.Int64())

        }
    }

// -----------------------------------------------------------------------------
// CBTGPSFix::ExtractSatInfo
// 
// -----------------------------------------------------------------------------
void CBTGPSFix::ExtractSatInfoL(const TBTGPSNmeaParser& aParser)
    {
    //get message number
    TInt messageNumber(1);
    User::LeaveIfError(aParser.GetFieldData(
        ENmeaFieldGPGSVMessageNumber,
        messageNumber));
    

    //Detect the number of satellites in this message
    if( (((messageNumber - 1) * 4) + 1) <= iGsvSatelliteInView )
        {
        ExtractSatelliteDataL(aParser,
            ENmeaFieldGPGSVSatelliteID1,
            ENmeaFieldGPGSVElevation1,
            ENmeaFieldGPGSVAzimuth1,
            ENmeaFieldGPGSVSNR1);
        }            
    
    //Detect the number of satellites in this message
    if ( (((messageNumber - 1) * 4) + 2) <= iGsvSatelliteInView )
        {
        ExtractSatelliteDataL(aParser,
            ENmeaFieldGPGSVSatelliteID2,
            ENmeaFieldGPGSVElevation2,
            ENmeaFieldGPGSVAzimuth2,
            ENmeaFieldGPGSVSNR2);
        }            
    
    //Detect the number of satellites in this message
    if ( (((messageNumber - 1) * 4) + 3) <= iGsvSatelliteInView )
        {
        ExtractSatelliteDataL(aParser,
            ENmeaFieldGPGSVSatelliteID3,
            ENmeaFieldGPGSVElevation3,
            ENmeaFieldGPGSVAzimuth3,
            ENmeaFieldGPGSVSNR3);
        }            
    
    //Detect the number of satellites in this message
    if ( (((messageNumber - 1) * 4) + 4) <= iGsvSatelliteInView )
        {
        ExtractSatelliteDataL(aParser,
            ENmeaFieldGPGSVSatelliteID4,
            ENmeaFieldGPGSVElevation4,
            ENmeaFieldGPGSVAzimuth4,
            ENmeaFieldGPGSVSNR4);
        }            
    }

// -----------------------------------------------------------------------------
// CBTGPSFix::ParseGSASentenceL
// 
// -----------------------------------------------------------------------------
void CBTGPSFix::ParseGSASentenceL(
        const TBTGPSNmeaParser& aParser)
    {
    TInt err = KErrNone;
    
    // Acquire fix status
    TInt validity(CBTGPSFix::EFixValidityUnkown);
    aParser.GetFieldData(
        ENmeaFieldGPGSAMode2,
        validity);

    // 2D fix
    if ( 2 == validity )
        {
        iValid = CBTGPSFix::EFixValid2D;
        }
    // 3D fix
    else if ( 3 == validity )
        {
        iValid = CBTGPSFix::EFixValid3D;
        }
    // No fix
    else
        {
        iValid = CBTGPSFix::EFixNotValid;
        }   
    
    //Get number of used satellites
    iSatellitesUsed = 0;
    
    for ( TInt i = ENmeaFieldGPGSASatellitesUsedCH1; 
            i <= ENmeaFieldGPGSASatellitesUsedCH12; i++ )
        {
        TInt usedSatelliteId=0;
        err = aParser.GetFieldData(
            i,
            usedSatelliteId);
        if(err==KErrNone && usedSatelliteId !=0)
            {
            iUsedSatellitesArray[iSatellitesUsed] = usedSatelliteId;
            iSatellitesUsed++;
            }
        }
    
    //Get HDOP
    aParser.GetFieldData(
        ENmeaFieldGPGSAHDOP,
        iHorizontalDoP);
        
    //Get VDOP
    aParser.GetFieldData(
        ENmeaFieldGPGSAVDOP,
        iVerticalDoP);
    
    //Get PDOP
    aParser.GetFieldData(
        ENmeaFieldGPGSAPDOP,
        iPositionDoP);

    //GSA received
    iReceivedMsgMask = iReceivedMsgMask | EReceivedGSA; 

    }

// -----------------------------------------------------------------------------
// CBTGPSFix::ParseRMCSentenceL
// 
// -----------------------------------------------------------------------------
void CBTGPSFix::ParseRMCSentenceL(
        const TBTGPSNmeaParser& aParser)
    {
    //Do the hairy time conversion and
    //add the 2000 to the year
    
    //The format for Set YYYYMMDD:HHMMSS.SSSSSS
    TBuf16<9+10+3> time;
    
    //Get date and time field
    TPtrC8 dateField;
    aParser.GetFieldBlock(
        ENmeaFieldGPRMCDate,
        dateField);

    //Get time field
    TPtrC8 timeField;
    aParser.GetFieldBlock(
        ENmeaFieldGPRMCUTCTime,
        timeField);
    
    if(dateField.Length()==6 && 
            timeField.Length()>0 && 
            timeField.Length()<=10)
        {
        //Date consist of DDMMYY
        TBuf16<6> date;
        date.Copy(dateField);

        time.Append(_L("20"));
        time.Append(date.Mid(4,2));

        //Take care that the month is indexed from zero
        TInt tempVal(1);
        TLex tempLexer = date.Mid(2,2);
        tempLexer.Val(tempVal); //error ignored
        tempVal = tempVal - 1;
        time.AppendNumFixedWidth(tempVal,EDecimal,2);
    
        //Take care that the date is indexed from zero
        tempLexer = date.Mid(0,2);
        tempVal = 1;
        tempLexer.Val(tempVal); //error ignored
        tempVal = tempVal - 1;
        time.AppendNumFixedWidth(tempVal,EDecimal,2);
        time.Append(_L(":"));           
    
        //Time consist of HHMMSS.SSS
        TBuf16<10> times;
    
        times.Copy(timeField);
        time.Append(times);
    
        iSatelliteTime.Set(time);//error ignored
        }

    //Get latitude and longitude
    GetLatitudeLongitude(aParser, ENmeaFieldGPRMCLatitude);
    
    //Get speed
    TReal32 tempVal32(0);
    if(aParser.GetFieldData(
        ENmeaFieldGPRMCSpeedOverGround, 
        tempVal32)==KErrNone)
        {
        iSpeed = KKnotInMetersPerSecond*tempVal32;
        }
    
    //Get heading
    aParser.GetFieldData(
        ENmeaFieldGPRMCCourseOverGround,
        iHeading);
        
    //Get GPS Mode
    TPtrC8 gpsMode;
    aParser.GetFieldBlock(
        ENmeaFieldGPRMCModeIndicator,
        gpsMode);
    if(gpsMode.Length()!=0)
        {
        switch(gpsMode[0])
            {
            case 'A':
            case 'D':
                iGpsMode = EGpsModeSatellite;
                break;
            case 'E':
                iGpsMode = EGpsModeEstimated;
                break;
            case 'M':
                iGpsMode = EGpsModeManualInput;
                break;
            case 'S':
                iGpsMode = EGpsModeSimulator;
                break;
            case 'N':
            default:
                iGpsMode = EGpsModeInvalid;
                break;
            }
        }

    //RMC received
    iReceivedMsgMask = iReceivedMsgMask | EReceivedRMC;

    }


// -----------------------------------------------------------------------------
// CBTGPSFix::GetLatitudeLongitude
// -----------------------------------------------------------------------------
void CBTGPSFix::GetLatitudeLongitude(
            const TBTGPSNmeaParser& aParser,
            TInt aStartId)
    {
    TPtrC8 nsField;
    TPtrC8 latField;
    TPtrC8 ewField;
    TPtrC8 logField;
    
    aParser.GetFieldBlock(aStartId, latField);
    aParser.GetFieldBlock(aStartId+1, nsField);
    aParser.GetFieldBlock(aStartId+2, logField);
    aParser.GetFieldBlock(aStartId+3, ewField);
    
    //We retrieve position only when all fields are valid
    if(nsField.Length()!=0 && latField.Length()!=0 &&
        ewField.Length()!=0 && logField.Length()!=0)
        {
        TReal latitude, longitude;
        if( GetDegrees(nsField, latField, latitude) == KErrNone &&
            GetDegrees(ewField, logField, longitude) == KErrNone)
            {
            iLatitude = latitude;
            iLongitude = longitude;
            TRACESTRING2("lat=%f", latitude);
            TRACESTRING2("lon=%f", longitude);
            }
        }
    //Set fix time
    iTime.UniversalTime();
    }


// -----------------------------------------------------------------------------
// CBTGPSFix::GetDegrees
// Transforms from degrees and minutes string to degree float
// -----------------------------------------------------------------------------
TInt CBTGPSFix::GetDegrees(
        const TDesC8& aSide,
        const TDesC8& aDegreesMinutes,
        TReal64& aDegrees)
    {
    TInt err = KErrNone;
    
    _LIT8(compNS, "S");
    _LIT8(compEW, "W");
    _LIT8(decimalPoint, ".");
    
    aDegrees = 0.0;
    TReal64 sign = 1.0;
    
    // Create sign multiplier according to N/S field
    if ( 0 == aSide.Compare(compNS) || 0 == aSide.Compare(compEW) )
        {
        sign = -1;
        }
    
    TInt loop = aDegreesMinutes.Find(decimalPoint);
    
    //Check that the decimal point was in right place
    //There could be 1-3 chars in degrees ([[D]D]DMM.MMMM)
    if ( (KErrNotFound != loop) && (loop > 2) && (loop < 6) )
        {
        loop -= 2;
        
        //Get the degrees (MM.MMM)
        TLex8 numberLexer = aDegreesMinutes.Mid(loop);
        err = numberLexer.Val(aDegrees, KDecimalSeperator);
        
        //Get the minutes (DDD)
        numberLexer = aDegreesMinutes.Left(loop);
        TReal64 temp = 0.0;
        err |= numberLexer.Val( temp, KDecimalSeperator );
        
        if(err!=KErrNone)
            {
            return err;
            }
            
        //Degrees are calculate as sign*(DD+(MM.MMM/60))
        aDegrees = (temp) + aDegrees/60;
        aDegrees = aDegrees * sign;
        }
    return err;
    }


// -----------------------------------------------------------------------------
// CBTGPSFix::AppendSatelliteData
// Fills the internal vector with satellite data
// -----------------------------------------------------------------------------
void CBTGPSFix::AppendSatelliteData(TSatelliteInfo& aSatInfo)
    {
    //Prevent over indexing
    if ( KMaxNumberOfSatellites > iNumberOfSatellites ) 
        {
        iSatelliteArray[iNumberOfSatellites] = aSatInfo;
        ++iNumberOfSatellites;
        }
    }

// -----------------------------------------------------------------------------
// CBTGPSFix::ExtractSatelliteDataL
// ------------------------------------------------------------------------------
void CBTGPSFix::ExtractSatelliteDataL (
        const TBTGPSNmeaParser& aParser,
        TInt aField1,
        TInt aField2,
        TInt aField3,
        TInt aField4)
    {
    TSatelliteInfo satInfo;
    
    satInfo.iSatelliteId = -1;
    satInfo.iElevation = -1;
    satInfo.iAzimuth = -1;
    satInfo.iSNR = -1;
    
    TInt err = KErrNone;
    err = aParser.GetFieldData(aField1, satInfo.iSatelliteId);
    
    err |= aParser.GetFieldData(aField2, satInfo.iElevation); 
        
    err |= aParser.GetFieldData(aField3, satInfo.iAzimuth); 
    
    err |= aParser.GetFieldData(aField4, satInfo.iSNR);
    
    if(err==KErrNone)
        {
        AppendSatelliteData(satInfo);
        }
    }

// -----------------------------------------------------------------------------
// CBTGPSFix::SecureCopy
// ------------------------------------------------------------------------------
TBool CBTGPSFix::SecureCopy(TDes8& aTo,const TDesC8& aFrom)
    {
    //Prevent overflow
    if ( aFrom.Length() > aTo.MaxLength() )
        {
        //Cut the message
        aTo.Copy(aFrom.Left(aTo.MaxLength()));
        return EFalse;
        }
    else
        {
        aTo.Copy(aFrom);
        return ETrue;
        }
    }

// -----------------------------------------------------------------------------
// CBTGPSFix::FillPositionInfo
// ------------------------------------------------------------------------------
TInt CBTGPSFix::FillPositionInfo(
            TPositionInfoBase& aPosInfo,
            TInt* aNmeaIndex) const
    {
    TRAPD(err, FillPositionInfoL(aPosInfo,aNmeaIndex));
    return err;
    }

// -----------------------------------------------------------------------------
// CBTGPSFix::FillPositionInfoL
// ------------------------------------------------------------------------------
void CBTGPSFix::FillPositionInfoL(
    TPositionInfoBase& aPosInfo,
    TInt* aNmeaIndex ) const
    {
    TRACESTRING("CBTGPSFix::FillPositionInfo start...")
    //HPositionGenericInfo
    //---------------------------------------------------------------
    if ( aPosInfo.PositionClassType() & EPositionGenericInfoClass )
        {
        HPositionGenericInfo* posInfo =
            static_cast<HPositionGenericInfo*>(&aPosInfo);

        if ( posInfo->IsRequestedField(EPositionFieldHorizontalSpeed) )
            {
            if ( IsFixValid() )
                {
                User::LeaveIfError(posInfo->SetValue(EPositionFieldHorizontalSpeed,
                    static_cast<TReal32>(iSpeed)));
                }
            }

        if ( posInfo->IsRequestedField(EPositionFieldHeading) )
            {
            if ( IsFixValid() )
                {
                User::LeaveIfError(posInfo->SetValue(EPositionFieldHeading,
                    static_cast<TReal32>(iHeading)));
                }
            }

        if ( posInfo->IsRequestedField(EPositionFieldNMEASentences) )
            {
            TUint8 messageNum=0;
            if(aNmeaIndex!=NULL)
                {
				TRACESTRING("CBTGPSFix::FillPositionInfo fill nmea sentences")
				TRACESTRING2("begining = %d", *aNmeaIndex)
				TRACESTRING2("bottom   = %d", iNmeaBuffer->CurrentIndex())
                TInt beginning = *aNmeaIndex;
                TBuf8<KMaxNmeaMessageSize> nmeaBuf;
                while(iNmeaBuffer->ReadSentences(
                    nmeaBuf,
                    beginning) == KErrNone)
                    {
                    User::LeaveIfError(posInfo->SetValue(static_cast<TUint16>(
                        EPositionFieldNMEASentencesStart+messageNum),
                        nmeaBuf));
                    ++messageNum;
                    }
                    
                *aNmeaIndex = beginning;
                }

            User::LeaveIfError(posInfo->SetValue(EPositionFieldNMEASentences,
                static_cast<TUint8>(messageNum)));
            }

        if ( posInfo->IsRequestedField(EPositionFieldSatelliteNumInView) )
            {
            User::LeaveIfError(posInfo->SetValue(EPositionFieldSatelliteNumInView,
                static_cast<TInt8>(iNumberOfSatellites)));
            }

        if ( posInfo->IsRequestedField(EPositionFieldSatelliteNumUsed) )
            {
            User::LeaveIfError(posInfo->SetValue(EPositionFieldSatelliteNumUsed,
                static_cast<TInt8>(iSatellitesUsed)));
            }

        if ( posInfo->IsRequestedField(EPositionFieldSatelliteTime) )
            {
            User::LeaveIfError(
                posInfo->SetValue(
                    EPositionFieldSatelliteTime,
                    iSatelliteTime));
            }

        if ( posInfo->IsRequestedField(EPositionFieldSatelliteHorizontalDoP) )
            {
            User::LeaveIfError(
                posInfo->SetValue(
                    EPositionFieldSatelliteHorizontalDoP,
                    static_cast<TReal32>(iHorizontalDoP)));
            }

        if ( posInfo->IsRequestedField(EPositionFieldSatelliteVerticalDoP) )
            {
            User::LeaveIfError(
                posInfo->SetValue(
                    EPositionFieldSatelliteVerticalDoP,
                    static_cast<TReal32>(iVerticalDoP)));
            }

        if ( posInfo->IsRequestedField(EPositionFieldSatellitePositionDoP) )
            {
            User::LeaveIfError(
                posInfo->SetValue(
                    EPositionFieldSatellitePositionDoP,
                    static_cast<TReal32>(iPositionDoP)));
            }
        }

    
    //TpositionSatelliteInfo
    //---------------------------------------------------------------
    if ( aPosInfo.PositionClassType() & EPositionSatelliteInfoClass )
        {
        TPositionSatelliteInfo* posInfo =
            static_cast<TPositionSatelliteInfo*>(&aPosInfo);
        posInfo->ClearSatellitesInView();
        TSatelliteData satData;

        //Iterate through the satellites
        for ( TInt i = 0; i < iNumberOfSatellites;++i )
            {
            satData.SetSatelliteId(iSatelliteArray[i].iSatelliteId);
            satData.SetAzimuth(iSatelliteArray[i].iAzimuth);
            satData.SetElevation(iSatelliteArray[i].iElevation);
            satData.SetIsUsed(EFalse);

            //Find out if the this satellite was used in fix calculation
            for ( TInt ii = 0; ii < iSatellitesUsed; ii++)
                {
                 if ( iUsedSatellitesArray[ii] == 
                        iSatelliteArray[i].iSatelliteId )
                    {
                    //This satellite is used in solution
                    satData.SetIsUsed(ETrue);
                    ii = iSatellitesUsed; //Break the loop
                    }
                }

            satData.SetSignalStrength(iSatelliteArray[i].iSNR);
            posInfo->AppendSatelliteData(satData);
            }

        posInfo->SetSatelliteTime(iSatelliteTime);
        posInfo->SetVerticalDoP(iVerticalDoP);
        posInfo->SetHorizontalDoP(iHorizontalDoP);
        }


    //TPositionCourseInfo
    //---------------------------------------------------------------
    if ( aPosInfo.PositionClassType() & EPositionCourseInfoClass )
        {
        TPositionCourseInfo* posInfo =
            static_cast<TPositionCourseInfo*>(&aPosInfo);
        TCourse cos;

        if ( IsFixValid() )
            {   
            cos.SetSpeed(iSpeed);
            cos.SetHeading(iHeading);
            }

        posInfo->SetCourse(cos);
        }



    //TPositionInfo
    //---------------------------------------------------------------
    if ( aPosInfo.PositionClassType() & EPositionInfoClass )
        {
        TPositionInfo * posInfo = static_cast<TPositionInfo*>(&aPosInfo);
        TPosition pos;

        if ( IsFixValid() )
            {
            if ( iValid == CBTGPSFix::EFixValid2D ) 
                {
                pos.SetCoordinate(iLatitude,iLongitude);
                }
            else
                {
                pos.SetCoordinate(iLatitude,iLongitude,iAltitude);
                }

            //Do a sanity check for the DOP values. If they are not valid,
            //use hard coded values instead.
            if ( (iHorizontalDoP > 0.0) && (iVerticalDoP > 0.0) )
                {
                pos.SetAccuracy(
                    CBTGPSRequestHandler::ConstantsManager().iUserEquivalentRangeError
                        * iHorizontalDoP,
                    CBTGPSRequestHandler::ConstantsManager().iUserEquivalentRangeError
                        * iVerticalDoP);
                }
            else
                {
                pos.SetAccuracy(KHorizontalAccuracy,KVerticalAccuracy);
                }
            }

        pos.SetTime(iTime);
        pos.SetDatum(KPositionDatumWgs84);
        posInfo->SetPosition(pos);
        }
    TRACESTRING("CBTGPSFix::FillPositionInfo end");
    }

// -----------------------------------------------------------------------------
// CBTGPSFix::ValidateNmeaBufferIndex
// ------------------------------------------------------------------------------
void CBTGPSFix::ValidateNmeaBufferIndex(
            TInt& aNmeaIndex,
            TInt& aBottom) const
    {
    TRACESTRING("CBTGPSFix::ValidateNmeaBufferIndex: start");
    TRACESTRING2("aNmeaIndex: %d", aNmeaIndex);
    TRACESTRING2("aBottom   : %d", aBottom);
    TRACESTRING2("currentBot: %d", iNmeaBuffer->CurrentIndex());
    
    TInt currentBottom = iNmeaBuffer->CurrentIndex();
    
    //when aNmeaIndex==KBTGPSNmeaIndexNotSet, client shall
    //get all NMEA data in the buffer

    //aBottom is the bottom of the NMEA buffer that the client
    //last time seen. 
    //When aBottom==KBTGPSNmeaIndexNotSet, it means the client
    //has just constructed. At this time, we don't need to check
    //the index

    if(aNmeaIndex != KBTGPSNmeaIndexNotSet 
        && aBottom != KBTGPSNmeaIndexNotSet)
        {
        
        //If index falles in the middle of aBottom and currentBottom,
        //we shall invalidate the index

        if( ( aNmeaIndex>aBottom && aNmeaIndex<currentBottom ) ||
            ( aBottom>currentBottom && aNmeaIndex > aBottom ) ||
            ( currentBottom > aNmeaIndex && aBottom > currentBottom ) )
            {
            //Invalidate aNmeaIndex
            aNmeaIndex = KBTGPSNmeaIndexNotSet;
            
            TRACESTRING("CBTGPSFix::ValidateNmeaBufferIndex: Index reset");
            }
        }

    //Update aBottom
    aBottom = currentBottom;
    }

// -----------------------------------------------------------------------------
// CBTGPSFix::operator =
// ------------------------------------------------------------------------------
void CBTGPSFix::operator =(const CBTGPSFix& aFix)
    {
    //This class is flat. We can just copy memory
    Mem::Copy(this, &aFix, sizeof(CBTGPSFix));
    }


//  End of File

