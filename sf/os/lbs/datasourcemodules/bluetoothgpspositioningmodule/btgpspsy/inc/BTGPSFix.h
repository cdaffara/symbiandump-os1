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




#ifndef BTGPSFIX_H
#define BTGPSFIX_H

//  INCLUDES

#include <lbsposition.h>
#include <lbs.h>
#include <e32std.h>
#include "BTGPSMessageDef.h"

// CONSTANTS

//The maximum number of satellites the GPS can track at once
static const TInt KMaxNumberOfSatellites = 12;

//The maximum number of GSV sentence that specified in NMEA spec
const TInt KMaxGSVMessageNumber = 9;

//The maximum length of GSV sentence
const TInt KMaxGSVLength = 82;

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class TBTGPSNmeaParser;
class CBTGPSNmeaBuffer;

// CLASS DECLARATION

/**
*  Class for parsing NMEA sentences and storing the information about the fix.
*  Parsing:
*       - If lexition parsing failed, the whole NMEA sentence is ignored
*       - NULL field is acceptable for any field, even explicitly specified in
*         NMEA spec that can't be NULL
*       - Only all RMC, GGA, GSA sentence are received, then the fix information
*         is valid. 
*       - GSV recieved time is stored for checking GSV validity
*
*/
class CBTGPSFix : public CBase
    {

    public:

        /**
        *Enumerate the status of fix parsing. The value is used when a NMEA sentence
		*is parsed by this class.
        */
        enum TParsingStatus
            {
            EInvalid,       ///The NMEA sentence is invalid
            EInfoUpdated,   ///The NMEA sentence is valid and information updated
            EUnkown         ///The NMEA sentence is not supported by parser
            };
        
        /**
        *Enumerate the different qualities of fix. This information is specified
        *in GSA sentence
        */
        enum TState
            {
            EFixValidityUnkown, ///No GSA sentence received.
            EFixNotValid,       ///Not a valid fix.
            EFixValid2D,        ///2D fix made
            EFixValid3D         ///3D fix made
            };

    public:  // Constructors and destructor
        /**
        * Constructor.
        */
        CBTGPSFix();
        
        /**
        * Set NMEA Buffer
        */
        void SetNmeaBuffer(CBTGPSNmeaBuffer* aNmeaBuffer);

    public:  // New functions

        /**
        * Resets the all fix parameters.
        */
        void Reset();
        
        /**
        * Reset fix parameter except setellites in view information(GSV information)
        */
        void ResetFix();
        
        /**
        * Reset GSV time
        */
        void ResetGSVTime();

        /**
        * Parse message.
		* @param aMsg Received message.
		* @return Parsing status as specified in CBTGPSFix::TParsingStatus.
        */
        TParsingStatus ParseMessage(const TBTGPSNmeaParser& aParser);
        
        /**
        * Get fix state(not valid, 2d fix or 3d fix)
		* @return Fix state as specified in CBTGPSFix::TState
        */
        TState FixState() const;
        
        /**
        * Check if fix is valid
		* @return Return ETrue if fix is valid
        */
        TBool IsFixValid() const;
                
        /**
        * Check if full NMEA message pattern received. 
		* @return Return ETrue if GSA, RMC and GGA are received.
        */
        TBool IfFullNmeaPatternReceived() const;
        
        /**
        * Get fix time. Fix time is updated when new latitude and
		* longitude are received.
		* @return Fix received time.
        */
        TTime FixTime() const;
        
        /**
        * Get GSV time
		* @return Return GSV sentence received time.
        */
        TTime GsvTime() const;
        
        /**
        * Fill fix into to TPositionInfo. Non-leave function.
        */
        TInt FillPositionInfo(
            TPositionInfoBase& aPosInfo,
            TInt* aNmeaIndex = NULL) const;
                    
        /**
        * Fill fix info to TPositionInfo.
        * If fix is not available, then aPosInfo will be emptied.
		* @param aPosInfo The TPositionInfo object to fill the fix info
		* @param aNmeaIndex The pointer to the index where last time
		* NMEA message is retruned to client. On return this value
		* is updated to the current bottom of the NMEA buffer;
        */
        void FillPositionInfoL(
            TPositionInfoBase& aPosInfo,
            TInt* aNmeaIndex = NULL) const;
            
            
        /**
        * Validate the NMEA buffer index. Client shall keep the 
        * index in the FIFO circulate NMEA buffer, which indicate
        * the position when last time. This function will validate
        * the index. If the buffer round back, then the index is 
        * invalidated. Next time client will receive the whole 
        * buffer. Note: This function shall be called before
        * FillPositionInfoL(). 
        * @param aNmeaIndex The NMEA buffer index.
        * @param aBottom Last time the client saw the bottom.
        */
        void ValidateNmeaBufferIndex(
            TInt& aNmeaIndex,
            TInt& aBottom) const;
        
        /**
        * Copy content of this
		* @param aFix the fix info that will be copied.
        */
        void operator =(const CBTGPSFix& aFix);

    private:  // Enumeration

		/**
        * Internal data class for satellite info
        */
        struct TSatelliteInfo
            {
            TInt iSatelliteId;
            TReal32 iElevation;
            TReal32 iAzimuth;
            TInt iSNR;
            };
    
        /**
        * GPS working mode
        */
        enum TGPSMode
            {
            EGpsModeSatellite=0, //GSP is using satellite to get fix
            EGpsModeEstimated,   //dead reckoning
            EGpsModeManualInput,
            EGpsModeSimulator,
            EGpsModeInvalid      //The fix is not valid
            };
            
        /**
        * GPS Quality Indicator
        */
        enum TGPSQuality
            {
            EGpsQualityNotValid = 0,
            EGpsQualitySPS      = 1,
            EGpsQualityDGPS     = 2,
            EGpsQualityPPS      = 3,
            EGpsQualityRTK      = 4,
            EGpsQualityFloatRTK = 5,
            EGpsQualityEstimated    = 6,
            EGpsQualityManualInput  = 7,
            EGpsQualitySimulation   = 8
            };
            
    private:  // New functions
        /**
        * Reset position parameters
        */
        void ResetPosition();
        
        /**
        * Parse message
        */
        void ParseMessageL(const TBTGPSNmeaParser& aParser);


        /**
        * Parse NMEA GGA Sentence
        */
        void ParseGGASentenceL(
            const TBTGPSNmeaParser& aParser);
        
        /**
        * Parse NMEA GSV Sentence
        */
        void ParseGSVSentenceL(
            const TBTGPSNmeaParser& aParser);
        
        /**
        * Parse NMEA GSA Sentence
        */
        void ParseGSASentenceL(
            const TBTGPSNmeaParser& aParser);
        
        /**
        * Parse NMEA RMC Sentence
        */
        void ParseRMCSentenceL(
            const TBTGPSNmeaParser& aParser);

        /**
        * Get latitude and longitude
        */
        void GetLatitudeLongitude(
            const TBTGPSNmeaParser& aParser,
            TInt aStartId);
            
        /**
        * Parses the degree information from the NMEA command 
        * (degrees and minutes) to
        * true degrees.
        * @return Error code
        */
        TInt GetDegrees(
            const TDesC8& aSide,
            const TDesC8& aDegreesMinutes,
            TReal64& aDegrees);

        /**
        * Appends the satellite data to the fix and quarantees 
        * that only KMaxNumberOfSatellites
        * are added.
        */
        void AppendSatelliteData(TSatelliteInfo& aSatInfo);
        
        /**
        * Extract satellite info from message parser
        */
        void ExtractSatInfoL(const TBTGPSNmeaParser& aParser);
        
        /**
        * Extracts the satellite data from a NMEA message and 
        * updates internal data variables.
        */
        void ExtractSatelliteDataL(
            const TBTGPSNmeaParser& aMessage,
            TInt aField1,
            TInt aField2,
            TInt aField3,
            TInt aField4);

        /**
        * Is used to copy the NMEA message and prevent panic if the 
        * NMEA message is too long.
        */
        TBool SecureCopy(TDes8& aTo,const TDesC8& aFrom);

    private: 
        /**
        * Defines the mask of received message
        */
        enum TReceivedMessage
            {
            EReceivedGSA = 0x01,
            EReceivedGGA = 0x02,
            EReceivedRMC = 0x04
            };
        
    private:    // Data
        //Current Fix data
        
        //Latitude, defaults to WGS-84 format.
        TReal64 iLatitude;

        //Longitude, defaults to WGS-84 format.
        TReal64 iLongitude;

        //Altitude, defaults to WGS-84 format.
        TReal32 iAltitude;

        //The timestamp of the fix
        TTime iTime;

        //Speed, defaults to WGS-84 format
        TReal32 iSpeed;

        //Heading, defaults to WGS-84 format
        TReal32 iHeading;

        //Number of satellites seen
        TInt iNumberOfSatellites;

        //Array of satellites
        TFixedArray<TSatelliteInfo,KMaxNumberOfSatellites> iSatelliteArray;

        //Satellites used in location calculation
        TInt iSatellitesUsed;

        //Array of used satellites
        TFixedArray<TInt,KMaxNumberOfSatellites> iUsedSatellitesArray;

        //Time received from satellites
       	TTime iSatelliteTime;

        //Horizontal dilution of precision
        TReal32 iHorizontalDoP;

        //Vertical dilution of precision
        TReal32 iVerticalDoP;
        
        //2D dilution of precision
        TReal32 iPositionDoP;
        
        //Geoidal Seperation
        TReal32 iGeoidalSeperation;
        
        // The fix validity
        TState iValid;

        //GPS Quality Indicator specified in GGA
        TGPSMode iGpsMode;

        //Received message mask
        TUint32 iReceivedMsgMask;
        
        //GSV sentence cache
        TBuf8<KMaxGSVLength> iNMEAGSVCache[KMaxGSVMessageNumber];
        
        //GSV total number of message
        TInt iGsvTotalNumberOfMessage;
        
        //GSV satellites in view
        TInt iGsvSatelliteInView;
        
        //The time that GSV sentences are fully retrieved.
        TTime iGsvTime;
        
        //NMEA Buffer. Not owned by this class
        CBTGPSNmeaBuffer* iNmeaBuffer;
        
    };
    
#endif 
// End of File
