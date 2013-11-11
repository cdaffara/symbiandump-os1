// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file ctlbsconfigreader.h
// This is the header file which contains the ini file configuration reader classes
// 
//

#ifndef __CT_LBS_CONFIG_READER_H__
#define __CT_LBS_CONFIG_READER_H__


// Epoc includes
#include <e32cmn.h>

// LBS includes
#include <lbs.h>
#include <lbscommon.h>
#include <lbspositioninfo.h>
#include <lbs/lbslocdatasourcegpsbase.h>

// LBS test includes
#include <lbs/test/tlbsutils.h>

// Forward Declaration



const TInt KMaxSentence = 200;
const TInt KMaxFields = 20;
const TInt KMaxIniFileBuffer = 100;
const TInt KMaxConfigReaderIniFileName = 100;

class CConfigReaderBase : public CBase
	{
public:
    virtual ~CConfigReaderBase();

	/** Main entry function which reads the ini file, fills out the data items defined in the
		derived classes. Once NewL as been called to construct the object, this should then be
		called. */
	void ProcessL();

protected:
	/** Converts a string to a TInt value. */
	void ExtractValueL(const TPtrC8& aField, TInt& aValue);
	
	/** Converts a string to a TReal32 value. */
	void ExtractValueL(const TPtrC8& aField, TReal32& aValue);
	
	/** Converts a string to a TReal64 value. */
	void ExtractValueL(const TPtrC8& aField, TReal64& aValue);

//	void ExtractDegreesL(const TPtrC8& aField, TReal64& aDegrees); this maybe handy so we can have lat, long in degrees not raw real numbers...
//	void ExtractTimeL(const TPtrC8& aTime, TDateTime& aTheTime);
//	void ExtractDateL(const TPtrC8& aDate, TDateTime& aTheDate);

//	Derived classes provide the implementation.
	/** Parses a full sentence read from the ini file. */
	virtual void HandleSentenceL()=0;
	
	/** Fills the data item (detailed in the derived class) with default values. */
	virtual void DefaultData()=0;
	
	/** Fills the data item (detailed in the derived class) with values parsed from the sentence. */
	virtual void ExtractDataL()=0;
	
	CConfigReaderBase(const TDesC &aConfigFileName);
	
	CConfigReaderBase(const TDesC& aConfigFileName, const TDesC& aConfigSection);
	
private:
	/** Adds valid sentence character to the field. Once the field delimiter is found the field
		is appended to the field array. */
	void AppendFieldChar(TUint aChar);
	
	/** Process sentences read from the ini file buffer. A complete valid sentence is passed
		to HandeSentenceL() to allow the derived class to process the sentence fields into
		it's data items. */
	void ProcessStringL(const TDesC8& aData);

private:	
//	enum TReadState {EWantStart, EWantTerminator};

	enum TReadState {/*	EStart, */
						EStartSection,
						EProcessSection,
						EStartSentence,
						EProcessSentence,
						EHalt};


	//const TDesC iConfigFileName;	// can we try this, but we will not be able to add the path
	/** Configuration ini filename to open and read. */
	TBuf<KMaxConfigReaderIniFileName> iConfigFileName;
	TBuf<KMaxSentence> iConfigSection;
	
	/** State machine variable used while processing a sentence. */
	TReadState iReadState;

	/** The sentence buffer, to store a valid sentence ready for processing. */
	TBuf8<KMaxSentence> iSentence;
	

	TBuf8<KMaxSentence> iReadSection;

protected:
	/** Field array. Each field is delimited by a ',' or '*' and will be converted into the
		member variable of the data item defined in the derived class. */
	TPtrC8 iFields[KMaxFields];
	
	/** Number of fields in the field array. */
	TInt iNumFields;
	
	/** Where the field starts in the sentence string. */
	TInt iFieldStart;
	
	/** Length of the field. */
	TInt iFieldLength;
	};



// Default values for TPosition,
#define DEFAULT_NOTIFY_POS_UPDATE_LATITUDE                 49.2
#define DEFAULT_NOTIFY_POS_UPDATE_LONGITUDE                3.5
#define DEFAULT_NOTIFY_POS_UPDATE_ALTITUDE                 50.0
#define DEFAULT_NOTIFY_POS_UPDATE_HORIZONTAL_ACCURACY      2.0
#define DEFAULT_NOTIFY_POS_UPDATE_VERTICAL_ACCURACY        3.0

// TCourse 
#define DEFAULT_NOTIFY_POS_UPDATE_SPEED                    26.0
#define DEFAULT_NOTIFY_POS_UPDATE_HEADING                  25.0
#define DEFAULT_NOTIFY_POS_UPDATE_VERTICAL_SPEED           20.0
#define DEFAULT_NOTIFY_POS_UPDATE_COURSE                   30.0
#define DEFAULT_NOTIFY_POS_UPDATE_SPEED_ACCURACY           2.0
#define DEFAULT_NOTIFY_POS_UPDATE_HEADING_ACCURACY         10.0
#define DEFAULT_NOTIFY_POS_UPDATE_VERTICAL_SPEED_ACCURACY  3.0
#define DEFAULT_NOTIFY_POS_UPDATE_COURSE_ACCURACY          4.0

// TSatelliteData - first satellite
#define DEFAULT_NOTIFY_POS_UPDATE_SATELLITE_ID1            10
#define DEFAULT_NOTIFY_POS_UPDATE_AZIMUTH1                 50.0
#define DEFAULT_NOTIFY_POS_UPDATE_ELEVATION1               27.0
#define DEFAULT_NOTIFY_POS_UPDATE_IS_USED1                 ETrue
#define DEFAULT_NOTIFY_POS_UPDATE_SIGNAL_STRENGTH1         4

// TSatelliteData - second stellite
#define DEFAULT_NOTIFY_POS_UPDATE_SATELLITE_ID2            11
#define DEFAULT_NOTIFY_POS_UPDATE_AZIMUTH2                 47.0
#define DEFAULT_NOTIFY_POS_UPDATE_ELEVATION2               30.0
#define DEFAULT_NOTIFY_POS_UPDATE_IS_USED2                 EFalse
#define DEFAULT_NOTIFY_POS_UPDATE_SIGNAL_STRENGTH2         3

//TDetailedErrorReport
#define DEFAULT_SD_OF_LONG_ERROR                           5.0
#define DEFAULT_SD_OF_LAT_ERROR                            6.0
#define DEFAULT_SD_OF_ALT_ERROR                            7.0 
#define DEFAULT_SD_OF_SEMI_MAJOR_AXIS_ERROR                8.0
#define DEFAULT_SD_OF_SEMI_MINOR_AXIS_ERROR                9.0
#define DEFAULT_ORIEN_OF_SEMI_MAJOR_AXIS_ERROR             10.0
#define DEFAULT_RMS_VAL_OF_SD_OF_RANGE                     11.0

#define DEFAULT_GEOIDAL_SEPARATION                         12.0
#define DEFAULT_MAGNETIC_VARIATION                         13.0
#define DEFAULT_COURSE_OVER_GROUND_MAGNETIC                14.0
//TGpsTimingMeasurementData
#define DEFAULT_GPS_TIMING_OF_CELL_MsPart                  16383                          
#define DEFAULT_GPS_TIMING_OF_CELL_LsPart                  4294967295UL 
#define DEFAULT_REFERENCE_IDENTITY                         511
#define DEFAULT_SFN                                        4095


class CUpdateConfigReader : public CConfigReaderBase
	{
public:

	enum TUpdateFields
		{
		EUpdateType = 0,	// Must be present
		EUpdateRepeat,
		ENumOfMeasurements,

		// TCoordinate
		EPosLatitude,			/** TCoordinate.iLatitude, TReal64 */
		EPosLongitude,			/** TCoordinate.iLongitude, TReal64 */
		EPosAltitude,			/** TCoordinate.iAltitude, TReal32 */
	
		// TLocalitiy
		EPosHorzAccuracy,		/** TLocality.iHorizontalAccuracy, TReal32 */
		EPosVertAccuracy,		/** TLocality.iVerticalAccuracy, TReal32 */

		// Optional
		EUpdateErr,
		EUpdateDelay			/** Time to delay, can be negative to force early update */
	};


	static CUpdateConfigReader* NewL(const TDesC &aConfigFileName, const TDesC& aConfigSection, RPointerArray<TLbsModuleUpdateItem>& aUpdateArr);

protected:
	void HandleSentenceL();

	void DefaultData();
	void ExtractDataL();
		
private:
	CUpdateConfigReader(const TDesC &aConfigFileName, const TDesC& aConfigSection, RPointerArray<TLbsModuleUpdateItem>& aUpdateArr);
	void ConstructL();  

	RPointerArray<TLbsModuleUpdateItem>& iUpdateArr;
	TLbsModuleUpdateItem* iUpdate;
	};




class CPosInfoConfigReader : public CConfigReaderBase
	{
public:

	enum TPosInfoFields
		{
		EPosType = 0,	// Must be present
		EposRepeat,

		// TCoordinate
		EPosLatitude,			/** TCoordinate.iLatitude, TReal64 */
		EPosLongitude,			/** TCoordinate.iLongitude, TReal64 */
		EPosAltitude,			/** TCoordinate.iAltitude, TReal32 */
		EPosDatum,				/** TCoordinate.iDatum, TUid */
	
		// TLocalitiy
		EPosHorzAccuracy,		/** TLocality.iHorizontalAccuracy, TReal32 */
		EPosVertAccuracy,		/** TLocality.iVerticalAccuracy, TReal32 */
	
		// TPosition
	//	EPosTime,	/** TPosition.iTime, TTime */	// see comment below
	
		// TCourse
		EPosSpeed,				/** TCourse.iSpeed, TReal32 */
		EPosHeading,			/** TCourse.iHeading, TReal32 */
		EPosSpeedAccuracy,		/** TCourse.iSpeedAccuracy, TReal32 */
		EPosHeadingAccuracy,	/** TCourse.iHeadingAccuracy, TReal32 */
	};


	static CPosInfoConfigReader* NewL(const TDesC &aConfigFileName, const TDesC& aConfigSection, RPointerArray<TAny>& aPosInfoArr);

protected:
	void HandleSentenceL();

	void DefaultData();
	void ExtractDataL();
		
private:
	CPosInfoConfigReader(const TDesC &aConfigFileName, const TDesC& aConfigSection, RPointerArray<TAny>& aPosInfoArr);
	void ConstructL();  

	RPointerArray<TAny>& iPosInfoArr;
	TPositionInfoBase* iPosInfo;
	};



/** CModuleStatusConfigReader is a reader class to process module status sentences.
	Once processed the aModuleStatus item will be filled with field values held in
	the sentence.
	
	The supported sentence is "modstatus", which MUST start with '$' and end with '*',
	fields MUST be delimted by ',' and the sentence MUST NOT contain any white space.

	$modstatus,device_field,data_quality_field*

		where device_field is one of
			device_error
			device_disable
			device_inactive
			device_initialising
			device_standby
			device_ready
			device_active

		where data_quality_field is one of 
			data_quality_loss
			data_quality_partial
			data_quality_normal
			
*/
class CModuleStatusConfigReader : public CConfigReaderBase
	{
public:

	enum TModuleStatusFields
		{
		EModuleStatusType = 0,	// Must be present, only 1 type 'modstatus'.

		// TPositionModuleStatus fields.
		EModuleStatusDevice,		/**	TPositionModuleStatus.iDeviceStatus, TDeviceStatus */
		EModuleStatusDataQuality,	/** TPositionModuleStatus.iDataQualityStatus, TDataQualityStatus */
		EModuleStatusDelayUpdate    /** */
	};

	static CModuleStatusConfigReader* NewL(const TDesC &aConfigFileName, const TDesC& aConfigSection, TPositionModuleStatus& aModuleStatus, TBool& aDelayReportingModStatus);

protected:
	/** Converts a string to a TPositionModuleStatus::TDeviceStatus enum value. */
	void ExtractValueDevice(const TPtrC8& aField, TPositionModuleStatus::TDeviceStatus& aValue);
	
	/** Converts a string to a TPositionModuleStatus::TDataQualityStatus enum value. */
	void ExtractValueDataQuality(const TPtrC8& aField, TPositionModuleStatus::TDataQualityStatus& aValue);

	void ExtractValueDelayUpdate(const TPtrC8& aField, TBool& aValue);	
	/** See CConfigReaderBase for comments. */
	void HandleSentenceL();
	void DefaultData();
	void ExtractDataL();
	
private:
	CModuleStatusConfigReader(const TDesC &aConfigFileName, const TDesC& aConfigSection, TPositionModuleStatus& aModuleStatus, TBool& aDelayReportingModStatus);

	/** Reference to the module status data item to be setup. */
	TPositionModuleStatus& iModuleStatus;
	/** */
	TBool& iDelayReportingModStatus;
	};



/* and a class reader for module update options
updateoptions,100,100,100,false
interval,
timeout
maxage
partialupdates
*/

class CUpdateOptsConfigReader : public CConfigReaderBase
	{
public:

	enum TUpdateOptsFields
		{
		EUpdateOptsType = 0,	// Must be present, only 1 type 'updateoptions'

		// TPositionUpdateOptions
		EUpdateOptsInterval,		/**	TPositionUpdateOptions.iUpdateInterval, TTimeIntervalMicroSeconds */
		EUpdateOptsTimeOut,			/** TPositionUpdateOptions.iUpdateTimeOut, TTimeIntervalMicroSeconds */
		EUpdateOptsMaxAge,			/** TPositionUpdateOptions.iMaxUpdateAge, TTimeIntervalMicroSeconds */
		EUpdateOptsPartialUpdates	/** TBool TPositionUpdateOptions.iAcceptPartialUpdates, TBool */
	};


	static CUpdateOptsConfigReader* NewL(const TDesC &aConfigFileName, const TDesC& aConfigSection, TPositionUpdateOptions& aUpdateOpts);

protected:
	/** Converts string to a TTimeIntervalMicroSeconds value. */
	void ExtractValueL(const TPtrC8& aField, TTimeIntervalMicroSeconds& aValue);
	
	/** Converts string to a TBool value. */
	void ExtractValue(const TPtrC8& aField, TBool& aValue);

	void HandleSentenceL();

	void DefaultData();
	void ExtractDataL();
	
private:
	CUpdateOptsConfigReader(const TDesC &aConfigFileName, const TDesC& aConfigSection, TPositionUpdateOptions& aUpdateOpts);

	TPositionUpdateOptions& iUpdateOpts;
	};

/*
 * Class reader for the expected modes in which the module is asked to run
 */
class CAgpsModuleModesConfigReader : public CConfigReaderBase
	{
private:
	enum TModuleModesFields
		{
		EModuleModesType = 0,			// must be present and needs to be 'modulemodes'
		EGpsModeField, 					// the GPS mode
		EBegginingOfGpsOptionsArray		// where the array of gps options items starts
		};
public:
	static CAgpsModuleModesConfigReader* NewL(const TDesC &aConfigFileName, const TDesC& aConfigSection, 
					RPointerArray<TLbsGpsOptions>& aModuleModes);
private:
	CAgpsModuleModesConfigReader(const TDesC &aConfigFileName, const TDesC& aConfigSection, 
					RPointerArray<TLbsGpsOptions>& aModuleModes);
	void HandleSentenceL();
	
	void DefaultData();
	void ExtractDataL();

private:
	RPointerArray<TLbsGpsOptions>& iModuleModes;
	};

/*
 * Class reader for details of all clients making requests in a test
 */
class CClientDetailsConfigReader : public CConfigReaderBase
	{
public:
	static CClientDetailsConfigReader* NewL(const TDesC &aConfigFileName, const TDesC& aConfigSection, 
					RArray<TCTClientDetails>& aClientDetailsArray);
private:
	CClientDetailsConfigReader(const TDesC &aConfigFileName, const TDesC& aConfigSection, 
					RArray<TCTClientDetails>& aClientDetailsArray);
	void HandleSentenceL();
	
	void DefaultData();
	void ExtractDataL();

private:
	RArray<TCTClientDetails>& iClientDetailsArray;
	};
#endif //__CT_LBS_CONFIG_READER_H__
