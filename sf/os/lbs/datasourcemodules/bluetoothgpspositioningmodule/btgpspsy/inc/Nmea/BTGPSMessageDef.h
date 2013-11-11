/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef BTGPSMESSAGEDEF_H
#define BTGPSMESSAGEDEF_H

//  INCLUDES
// CONSTANTS

/**
* The maximum length of a valid NMEA sentence
*/
const TInt KMaxNmeaMessageSize = 256;

//Defines the message Id increments
const TInt KNmeaMessageIdSkip = 100;


enum TNmeaMessageId
    {
    //0 is reserved for invalid message
    ENmeaNull    = 0,   ///dummy message
    ENmeaUnknown = 1,   ///Unknown message
    ENmeaGPGSV   = 100, ///GPGSV message
    ENmeaGPGLL   = 200, ///GPGLL message
    ENmeaGPRMC   = 300, ///GPRMC message
    ENmeaGPVTG   = 400, ///GPVTG message
    ENmeaGPGGA   = 500, ///GPGGA message
    ENmeaGPGSA   = 600, ///GPGSA message
    //Proprietary message starts here
    ENmeaPropriatary = 10000,
    ENmeaPSRF107 = 10100,
    ENmeaPSRF103 = 10200,
    ENmeaPSRF101 = 10300,
    ENmeaPSRF105 = 10400,
    ENmeaPSRF201 = 10500,
    ENmeaProprietaryNok = 20000
    };


enum TNmeaPNokMessageId
    {
    //Received message
    ENmeaPNokOk             = 20100,
    ENmeaPNokReset          = 20200,
    ENmeaPNokPeriodicMsgs   = 20300,
    ENmeaPNokLowPwrMode     = 20400,
    ENmeaPNokNightMode      = 20500,
    ENmeaPNokVersionInfo    = 20600,
    ENmeaPNokPeriodicEvents = 20700,
    ENmeaPNokReferenceTime  = 20800,
    };

//The NMEA message fields
enum TNmeaFieldID
    {
    ENmeaFieldAddressField              = 0,
    ENmeaFieldGPGSVNumberOfMessage      = ENmeaGPGSV,
    ENmeaFieldGPGSVMessageNumber,
    ENmeaFieldGPGSVSatellitesInView,
    ENmeaFieldGPGSVSatelliteID1,
    ENmeaFieldGPGSVElevation1,
    ENmeaFieldGPGSVAzimuth1,
    ENmeaFieldGPGSVSNR1,
    ENmeaFieldGPGSVSatelliteID2,
    ENmeaFieldGPGSVElevation2,
    ENmeaFieldGPGSVAzimuth2,
    ENmeaFieldGPGSVSNR2,
    ENmeaFieldGPGSVSatelliteID3,
    ENmeaFieldGPGSVElevation3,
    ENmeaFieldGPGSVAzimuth3,
    ENmeaFieldGPGSVSNR3,
    ENmeaFieldGPGSVSatelliteID4,
    ENmeaFieldGPGSVElevation4,
    ENmeaFieldGPGSVAzimuth4,
    ENmeaFieldGPGSVSNR4,
    ENmeaFieldGPGLLLatitude             = ENmeaGPGLL,
    ENmeaFieldGPGLLNSIndicator,
    ENmeaFieldGPGLLLongitude,
    ENmeaFieldGPGLLEWIndicator,
    ENmeaFieldGPGLLUTCTime,
    ENmeaFieldGPGLLFix,
    ENmeaFieldGPRMCUTCTime              = ENmeaGPRMC,
    ENmeaFieldGPRMCFix,
    ENmeaFieldGPRMCLatitude,
    ENmeaFieldGPRMCNSIndicator,
    ENmeaFieldGPRMCLongitude,
    ENmeaFieldGPRMCEWIndicator,
    ENmeaFieldGPRMCSpeedOverGround,
    ENmeaFieldGPRMCCourseOverGround,
    ENmeaFieldGPRMCDate,
    ENmeaFieldGPRMCMagneticVariation,
    ENmeaFieldGPRMCMagneticEW,
    ENmeaFieldGPRMCModeIndicator,
    ENmeaFieldGPVTGCourse1              = ENmeaGPVTG,
    ENmeaFieldGPVTGReference1,
    ENmeaFieldGPVTGCourse2,
    ENmeaFieldGPVTGReference2,
    ENmeaFieldGPVTGSpeed,
    ENmeaFieldGPVTGUnits,
    ENmeaFieldGPVTGSpeed2,
    ENmeaFieldGPVTGUnits2,
    ENmeaFieldGPGGAUTCTime              = ENmeaGPGGA,
    ENmeaFieldGPGGALatitude,
    ENmeaFieldGPGGANSIndicator,
    ENmeaFieldGPGGALongitude,
    ENmeaFieldGPGGAEWIndicator,
    ENmeaFieldGPGGAFix,
    ENmeaFieldGPGGASatellitesUsed,
    ENmeaFieldGPGGAHDOP,
    ENmeaFieldGPGGAMSLAltitude,
    ENmeaFieldGPGGAUnits,
    ENmeaFieldGPGGAGeoidalSeperation,
    ENmeaFieldGPGGAGeoidalSeperationUnits,
    ENmeaFieldGPGGAAgeOfDiffCorr,
    ENmeaFieldGPGGADiffRefStationID,
    ENmeaFieldGPGSAMode1                = ENmeaGPGSA,
    ENmeaFieldGPGSAMode2,
    ENmeaFieldGPGSASatellitesUsedCH1,
    ENmeaFieldGPGSASatellitesUsedCH2,
    ENmeaFieldGPGSASatellitesUsedCH3,
    ENmeaFieldGPGSASatellitesUsedCH4,
    ENmeaFieldGPGSASatellitesUsedCH5,
    ENmeaFieldGPGSASatellitesUsedCH6,
    ENmeaFieldGPGSASatellitesUsedCH7,
    ENmeaFieldGPGSASatellitesUsedCH8,
    ENmeaFieldGPGSASatellitesUsedCH9,
    ENmeaFieldGPGSASatellitesUsedCH10,
    ENmeaFieldGPGSASatellitesUsedCH11,
    ENmeaFieldGPGSASatellitesUsedCH12,
    ENmeaFieldGPGSAPDOP,
    ENmeaFieldGPGSAHDOP,
    ENmeaFieldGPGSAVDOP,
    ENmeaFieldPSRF107PushToFix          = ENmeaPSRF107,
    ENmeaFieldPSRF107OnTimeMs,
    ENmeaFieldPSRF107LPIntervalMs,
    ENmeaFieldPSRF103Msg                = ENmeaPSRF103,
    ENmeaFieldPSRF103Mode,
    ENmeaFieldPSRF103Rate,
    ENmeaFieldPSRF103CksumEnable,
    ENmeaFieldPSRF101EFECX              = ENmeaPSRF101,
    ENmeaFieldPSRF101EFECY,
    ENmeaFieldPSRF101EFECZ,
    ENmeaFieldPSRF101ClkOffset,
    ENmeaFieldPSRF101TimeOfWeek,
    ENmeaFieldPSRF101WeekNo,
    ENmeaFieldPSRF101ChannelCount,
    ENmeaFieldPSRF101ResetCfg,
    ENmeaFieldPSRF105Debug              = ENmeaPSRF105,
    ENmeaFieldPSRF201PollParam          = ENmeaPSRF201,
    
    ENmeaFieldProprietaryCmd            = ENmeaProprietaryNok,
    ENmeaFieldPNOKCmdOk                 = ENmeaPNokOk,
    ENmeaFieldPNOKSupport,
    ENmeaFieldPNOKCmdReset              = ENmeaPNokReset,
    ENmeaFieldPNOKResetType,
    ENmeaFieldPNOKCmdMsgs               = ENmeaPNokPeriodicMsgs,
    ENmeaFieldPNOKGGAMessageStatus,
    ENmeaFieldPNOKGSAMessageStatus,
    ENmeaFieldPNOKGSVMessageStatus,
    ENmeaFieldPNOKRMCMessageStatus,
    ENmeaFieldPNOKVTGMessageStatus,
    ENmeaFieldPNOKGLLMessageStatus,
    ENmeaFieldPNOKGPSStatusMessage,
    ENmeaFieldPNOKCmdLowPwr             = ENmeaPNokLowPwrMode,
    ENmeaFieldPNOKLowPowerMode,
    ENmeaFieldPNOKCmdNightMode          = ENmeaPNokNightMode,
    ENmeaFieldPNOKNightModeStatus,
    ENmeaFieldPNOKCmdVer                = ENmeaPNokVersionInfo,
    ENmeaFieldPNOKProductID,
    ENmeaFieldPNOKBTHWVersionNumber,
    ENmeaFieldPNOKBTSWVersionNumber,
    ENmeaFieldPNOKGPSHWVersionNumber,
    ENmeaFieldPNOKGPSSWVersionNumber,
    ENmeaFieldPNOKCmdStat               = ENmeaPNokPeriodicEvents,
    ENmeaFieldPNOKBatteryLevel,
    ENmeaFieldPNOKBatteryStatus,
    ENmeaFieldPNOKExtPowerStatus,
    ENmeaFieldPNOKExtAntennaStatus,
    ENmeaFieldPNOKCmdTime               = ENmeaPNokReferenceTime,
    ENmeaFieldPNOKReferenceTime,
    ENmeaFieldPNOKReferenceDate
    };

//Constants for NMEA message
//NMEA message beginning charater
const TUint8 KNmeaSentenceLead = '$';

//NMEA message terminator 1
const TUint8 KNmeaSentenceTerminator1 = '\r';

//NMEA message terminator 2
const TUint8 KNmeaSentenceTerminator2 = '\n';


//MessageIds for the NMEA messages
_LIT8(KMessageIdGSV,"$GPGSV");
_LIT8(KMessageIdGLL,"$GPGLL");
_LIT8(KMessageIdRMC,"$GPRMC");
_LIT8(KMessageIdVTG,"$GPVTG");
_LIT8(KMessageIdGGA,"$GPGGA");
_LIT8(KMessageIdGSA,"$GPGSA");
_LIT8(KMessageIdPSRF107,"$PSRF107");
_LIT8(KMessageIdPSRF103,"$PSRF103");
_LIT8(KMessageIdPSRF101,"$PSRF101");
_LIT8(KMessageIdPSRF105,"$PSRF105");
_LIT8(KMessageIdPSRF201,"$PSRF201");

// MessageIds for the PNOK messsages
_LIT8(KPNOKIdMsgSupport, 		"$PNOK,OK");
_LIT8(KPNOKIdResetType, 		"$PNOK,RESET");
_LIT8(KPNOKIdPeriodicMsgs,	"$PNOK,MSGS");
_LIT8(KPNOKIdLowPwrMode, 		"$PNOK,LOWPWR");
_LIT8(KPNOKIdNightMode, 		"$PNOK,NIGHTMODE");
_LIT8(KPNOKIdVersionInfo, 	"$PNOK,VER");
_LIT8(KPNOKIdPeriodicEvts,	"$PNOK,STAT");
_LIT8(KPNOKIdReferenceTime,	"$PNOK,TIME");

// Constants defined for PNOK message
// PNOK STAT message. Battery in normal operation mode
const TInt KPNOKStatBattNormal = 0;

// PNOK STAT message. Battery full
const TInt KPNOKStatBattFull = 1;

// PNOK STAT message. Battery low
const TInt KPNOKStatBattLow = 2;

// PNOK STAT message. Ext power not connected
const TInt KPNOKStatExtPowerNotConnected = 0;

// PNOK STAT message. Ext power connected
const TInt KPNOKStatExtPowerConnected = 1;

// PNOK STAT message. Ext antenna not connected
const TInt KPNOKStatExtAntennaNotConnected = 0;

// PNOK STAT message. Ext antenna connected
const TInt KPNOKStatExtAntennaConnected = 1;

// PNOK NigthMode message. Night mode on
const TInt KPNOKNightNightModeOn = 1;

//PNOK NightMode mssage. Nigth mode off
const TInt KPNOKNightNightModeOff = 0;

//Predefiend message
enum TNmeaPredefiendMessage
    {
    //PNok message
    ENmeaPNokNightModeOff,
    ENmeaPNokNightModeOn,
    ENmeaPNokToPowerSaving1s,
    ENmeaPNok,
    ENmeaPNokSelectMessage,
    //Sirf message
    ENmeaSelectMessage,
    ENmeaToNormalPower,
    ENmeaToPowerSaving,
    ENmeaSirfToNmea
    };



//PNOK message to change Night Mode OFF 
_LIT8(KNMEAPNOKNightModeOff, "$PNOK,NIGHTMODE,0*75\r\n");

//PNOK message to change Night Mode ON 
_LIT8(KNMEAPNOKNightModeOn, "$PNOK,NIGHTMODE,1*74\r\n");

//PNOK message to set power saving ON (lowest saving mode: messages are sent in every 1000 ms)
_LIT8(KNMEAPNOKToPowerSaving1s,"$PNOK,LOWPWR,1*2A\r\n");

//PNOK message to set wanted message(GPGGA, GPRMC, GPGSV, GPGSA, STAT).
//No message is turned off.
_LIT8(KNMEAPNOKSelectMessage, "$PNOK,MSGS,1,1,1,1,,,1*21\r\n");

//Literal constant for identifying PNOK messages
_LIT8(KNMEAPNOK, "$PNOK*1A\r\n");

//NMEA messages to set right messages flowing for BT GPS Devices. Turn on
//GGA, GSA, GSV, RMC. No message is turned off.
_LIT8(KNMEASelectedMessages,"$PSRF103,0,0,1,1*25\r\n$PSRF103,2,0,1,1*27\r\n\
$PSRF103,3,0,1,1*26\r\n$PSRF103,4,0,1,1*21\r\n");

//NMEA message to set power saving off
_LIT8(KNMEAToNormalPower,"$PSRF107,0,1000,1000*3d\r\n");

//NMEA message to set power saving on
_LIT8(KNMEAToPowerSaving,"$PSRF107,0,200,1000*0e\r\n");

//SiRF message to set the GPS in NMEA mode. Supported also in Nokia BT GPS Device.
//Check it this is the $PSRF129, which shall be used for Nokia Device.
_LIT8(KSirfToNMEA,"\xA0\xA2\x00\x18\x81\x02\x01\x01\x00\x01\x05\x01\x05\x01\
\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01\x12\xC0\x01\x6A\xB0\xB3\r\n");

#endif //BTGPSMESSAGEDEF_H

// End of File
