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
// @file ctlbsconfigreader.cpp
// This is the Cpp file which contains the ini file configuration reader classes
// 
//

// User includes
#include "ctlbsconfigreader.h"

// Epoc includes
#include <f32file.h>

// Lbs includes
#include <lbsclasstypes.h>
#include <lbs/lbsextendedsatellite.h>

// Literals Used
_LIT8(KNotANumber, "nan");


CConfigReaderBase::CConfigReaderBase(const TDesC& aConfigFileName, const TDesC& aConfigSection) : iConfigFileName(aConfigFileName), iConfigSection(aConfigSection)
	{	
	}


CConfigReaderBase::~CConfigReaderBase()
	{
	}


void CConfigReaderBase::AppendFieldChar(TUint aChar)
	{
	if (iSentence.Length() >= iSentence.MaxLength())
		{
		return;
		}
	
	iSentence.Append(aChar);

	// If aChar is not a field delimiter then just add it to buffer and return
	if (aChar != ',' && aChar != '*')
		{
		iFieldLength++;
		return;
		}

	// Got a field delimiter increase the number of fields
	// aChar == ',' || aChar == '*'

	TPtrC8 thisField = iSentence.Mid(iFieldStart, iFieldLength);
	iFields[iNumFields].Set(thisField);
	iNumFields++;

	// Next field starts here
	iFieldStart  = iSentence.Length();
	iFieldLength = 0;
	}


void CConfigReaderBase::ProcessStringL(const TDesC8& aData)
	{
	TInt length = aData.Length();
	
	for (TInt index = 0; index < length; index++)
		{
		TUint ch = aData[index];

		switch(iReadState)
			{
			// Keep reading until a section is found.
			case EStartSection:
				{
				if (ch == '[')
					{
					iReadSection.SetLength(0);
					iReadState = EProcessSection;
					}
				break;
				}

			// Found a section, keep reading until end of section found.
			case EProcessSection:
				{
				// End of section, check if it's ours.
				if (ch == ']')
					{
					// Need to convert from TBuf8 to TBuf for the comparision.
					TBuf<KMaxSentence> cmpSection;
					cmpSection.Copy(iReadSection);
					
					// Not our section.
					if (iConfigSection.Compare(cmpSection) != 0)
						{
						iReadState = EStartSection;
						}

					// Is our section.
					else
						{					
						iReadState = EStartSentence;
						}
					}
				else
					{
					iReadSection.Append(ch);
					}
				break;
				}
				
			case EStartSentence:
				{
				//
				if (ch == '$')
					{
					iReadState = EProcessSentence;
					iSentence.SetLength(0);
					iNumFields   = 0;
					iFieldStart  = 0;
					iFieldLength = 0;
					for (TInt i = 0; i < KMaxFields; i++)
						{
						iFields[i].Set(KNullDesC8);
						}
					}
				
				// If we find a section we know to stop reading sentences for our section
				// and we let the other section be handled, but it will not match ours.
				else if (ch == '[')
					{
					iReadSection.SetLength(0);
					iReadState = EProcessSection;
					}
				
				break;
				}

			case EProcessSentence:
				{
				AppendFieldChar(ch); // calling this don't you end up with a * in the field, does it matter
				
				if (ch == '*')
					{
					iReadState = EStartSentence;
					HandleSentenceL();
					}			
				break;
				}
			}
		}
	}


void CConfigReaderBase::ExtractValueL(const TPtrC8& aField, TInt& aValue)
	{
	if (aField.Length() != 0)
		{
		TLex8 lex(aField);
		TInt err = lex.Val(aValue);
		User::LeaveIfError(err);
		}
	}


void CConfigReaderBase::ExtractValueL(const TPtrC8& aField, TReal32& aValue)
	{
	TRealX nan;
	nan.SetNaN();
	
	aValue = nan;
	
	if (aField.Length() != 0)
		{
		if (aField.CompareF(KNotANumber))
			{
			TLex8 lex(aField);
			TInt err = lex.Val(aValue);
			User::LeaveIfError(err);
			}
		}
	}


void CConfigReaderBase::ExtractValueL(const TPtrC8& aField, TReal64& aValue)
	{
	TRealX nan;
	nan.SetNaN();
	
	aValue = nan;
	
	if (aField.Length() != 0)
		{
		if (aField.CompareF(KNotANumber))
			{
			TLex8 lex(aField);
			TInt err = lex.Val(aValue);
			User::LeaveIfError(err);
			}
		}
	}

/*
void CConfigReaderBase::ExtractDegreesL(const TPtrC8& aField, TReal64& aDegrees)
	{
	TRealX nan;
	nan.SetNaN();

	aDegrees = nan;

	if (aField.Length() != 0)
		{
		TLex8 lex(aField);
		TInt err = lex.Val(aDegrees);
		User::LeaveIfError(err);
		
		ConvertDecimalMinutesToDecimalDegrees(aDegrees);
		}
	}
*/


void CConfigReaderBase::ProcessL()
	{
	if (iConfigFileName.Length() == 0)
		{
		User::LeaveIfError(KErrArgument);
		}
	
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);	
	
	RFile iniFile;

	User::LeaveIfError(iniFile.Open(fs, iConfigFileName, EFileRead|EFileShareReadersOnly));

	CleanupClosePushL(iniFile);
	
	TBuf8<KMaxIniFileBuffer> buf;
	
	// Loop until EOF.
	User::LeaveIfError(iniFile.Read(buf));
	while (buf.Length() > 0)
		{
		// Process data read from file.
		ProcessStringL(buf);
		
		// Refresh buffer from file.
		User::LeaveIfError(iniFile.Read(buf));
		}

	CleanupStack::PopAndDestroy(2, &fs);	// iniFile, fs.
	}







/*********************** Update Info Reader ************************/

CUpdateConfigReader* CUpdateConfigReader::NewL(const TDesC &aConfigFileName, const TDesC& aConfigSection, RPointerArray<TLbsModuleUpdateItem>& aUpdateArr)
	{
	return new (ELeave) CUpdateConfigReader(aConfigFileName, aConfigSection, aUpdateArr);
	}


CUpdateConfigReader::CUpdateConfigReader(const TDesC &aConfigFileName, const TDesC& aConfigSection, RPointerArray<TLbsModuleUpdateItem>& aUpdateArr) : CConfigReaderBase(aConfigFileName, aConfigSection), iUpdateArr(aUpdateArr)
	{
	}


void CUpdateConfigReader::HandleSentenceL()
	{
	// Not a update sentence, ignore all other valid sentences.
	if (	(iNumFields == EUpdateType) || (iFields[EUpdateType].Compare(_L8("update")) != 0) )
		{
		return;
		}

	// Determine the number of times to repeat the update.
	TInt repeat = 1;

	if (iFields[EUpdateRepeat].Length() != 0)
		{
		ExtractValueL(iFields[1], repeat);
		}

	// Determine the number of measurements for each update.
	TInt numOfMeasurements = 0;
	
	if (iFields[ENumOfMeasurements].Length() != 0)
		{
		ExtractValueL(iFields[ENumOfMeasurements], numOfMeasurements);	// make this member, then set up this + error in the default + extract maybe
		}

	// Add repeat items to the pos info array.
	for (TInt i = 0; i < repeat; i++)
	{
		TPositionGpsMeasurementInfo measureInfo;
		TPositionGpsMeasurementData measureData;


		// Alloc a update item.
		iUpdate = new(ELeave) TLbsModuleUpdateItem();


		// Set measurement info.
		for (TInt j = 0; j < numOfMeasurements; j++)
			{
			measureData.SetSatelliteId(j + 1);
			measureData.SetCarrierNoiseRatio((j+1) + 1);
			measureData.SetDoppler((j+1) + 2);
			measureData.SetWholeGpsChips((j+1) + 3);
			measureData.SetFractionalGpsChips((j+1) + 4);
			measureData.SetMultiPathIndicator(TPositionGpsMeasurementData::EMultiPathLow);
			measureData.SetPseudoRangeRmsError((j+1) + 5);

			User::LeaveIfError(measureInfo.AppendMeasurementData(measureData));
			}
		measureInfo.SetGpsTimeOfWeek(i + 1000);
		iUpdate->SetMeasurement(measureInfo);

		
		// Set position update.		
		// Optional fields are present fill out pos info based on them.
		if (iFields[3].Length() != 0)
			{
			DefaultData();	// This will ensure the pos info items are set to a default value for any incomplete sentences.
			ExtractDataL();
			}

		// Otherwise use default values.			
		else
			{
			DefaultData();
			}


		// Optional error value is present. Set update error.
		TInt updateErr = KErrNone;
		if (iFields[EUpdateErr].Length() != 0)
			{
			ExtractValueL(iFields[EUpdateErr], updateErr);		
			}
		iUpdate->SetError(updateErr);
		
		// Optional time delay(could be negative)
		TInt updateDelay = 0;
		if (iFields[EUpdateDelay].Length() != 0)
			{
			ExtractValueL(iFields[EUpdateDelay], updateDelay);
			}
		iUpdate->SetDelay(updateDelay);

				
		// Add to array, and reset pointer.
		iUpdateArr.AppendL(iUpdate);
		iUpdate = NULL;
		}	
	}


void CUpdateConfigReader::ExtractDataL()
	{
	// Access the position info from the update structure.
	TPositionExtendedSatelliteInfo	posInfo = iUpdate->Position();
	TPosition position;

	// Process fields.
	TReal64 latitude;
	TReal64 longitude;
	TReal32 altitude;
	TReal32 horzAccuracy;
	TReal32 vertAccuracy;

	ExtractValueL(iFields[EPosLatitude], latitude);
	ExtractValueL(iFields[EPosLongitude], longitude);
	ExtractValueL(iFields[EPosAltitude], altitude);
	ExtractValueL(iFields[EPosHorzAccuracy], horzAccuracy);
	ExtractValueL(iFields[EPosVertAccuracy], vertAccuracy);

	// Set values.
	position.SetCoordinate(latitude, longitude, altitude);
	position.SetAccuracy(horzAccuracy, vertAccuracy);
	position.SetCurrentTime();
		
	posInfo.SetPosition(position);

	iUpdate->SetPosition(posInfo);
	}


void CUpdateConfigReader::DefaultData()
	{
	// Access the position info from the update structure.
	TPositionExtendedSatelliteInfo	posSatalliteInfo = iUpdate->Position();


	// Fill out default position data.
	TPosition position;

	position.SetCoordinate(DEFAULT_NOTIFY_POS_UPDATE_LATITUDE, DEFAULT_NOTIFY_POS_UPDATE_LONGITUDE, DEFAULT_NOTIFY_POS_UPDATE_ALTITUDE);
	position.SetAccuracy(DEFAULT_NOTIFY_POS_UPDATE_HORIZONTAL_ACCURACY, DEFAULT_NOTIFY_POS_UPDATE_VERTICAL_ACCURACY);
	position.SetCurrentTime();
		
	posSatalliteInfo.SetPosition(position);


	// Fill out default course data.
	TCourse course;

	course.SetSpeed(DEFAULT_NOTIFY_POS_UPDATE_SPEED);
	course.SetHeading(DEFAULT_NOTIFY_POS_UPDATE_HEADING);
	course.SetSpeedAccuracy(DEFAULT_NOTIFY_POS_UPDATE_SPEED_ACCURACY);
	course.SetHeadingAccuracy(DEFAULT_NOTIFY_POS_UPDATE_HEADING_ACCURACY);
	course.SetVerticalSpeed(DEFAULT_NOTIFY_POS_UPDATE_VERTICAL_SPEED);
	course.SetVerticalSpeedAccuracy(DEFAULT_NOTIFY_POS_UPDATE_VERTICAL_SPEED_ACCURACY);  
	course.SetCourse(DEFAULT_NOTIFY_POS_UPDATE_COURSE);
	course.SetCourseAccuracy(DEFAULT_NOTIFY_POS_UPDATE_COURSE_ACCURACY);
	
	posSatalliteInfo.SetCourse(course);


	// Fill out default satalliteInfo.
	TSatelliteData satellite;

	satellite.SetSatelliteId(DEFAULT_NOTIFY_POS_UPDATE_SATELLITE_ID1);
	satellite.SetAzimuth(DEFAULT_NOTIFY_POS_UPDATE_AZIMUTH1);
	satellite.SetElevation(DEFAULT_NOTIFY_POS_UPDATE_ELEVATION1);
	satellite.SetIsUsed(DEFAULT_NOTIFY_POS_UPDATE_IS_USED1);
	satellite.SetSignalStrength(DEFAULT_NOTIFY_POS_UPDATE_SIGNAL_STRENGTH1);
	
	posSatalliteInfo.AppendSatelliteData(satellite);

	satellite.SetSatelliteId(DEFAULT_NOTIFY_POS_UPDATE_SATELLITE_ID2);
	satellite.SetAzimuth(DEFAULT_NOTIFY_POS_UPDATE_AZIMUTH2);
	satellite.SetElevation(DEFAULT_NOTIFY_POS_UPDATE_ELEVATION2);
	satellite.SetIsUsed(DEFAULT_NOTIFY_POS_UPDATE_IS_USED2);
	satellite.SetSignalStrength(DEFAULT_NOTIFY_POS_UPDATE_SIGNAL_STRENGTH2);
		
	posSatalliteInfo.AppendSatelliteData(satellite);

    //Fill out default extendedsatelliteinfo
	TGpsTimingMeasurementData gpsTimingData;
	// Set timing assistance measurement data
	gpsTimingData.SetDataType(TGpsTimingMeasurementData::EGpsTimingDataTypeUtran);
	gpsTimingData.SetNetworkMode(TGpsTimingMeasurementData::ENetworkModeFdd);
	gpsTimingData.SetGPSTimingOfCellMsPart(DEFAULT_GPS_TIMING_OF_CELL_MsPart);
	gpsTimingData.SetGPSTimingOfCellLsPart(DEFAULT_GPS_TIMING_OF_CELL_LsPart);
	gpsTimingData.SetReferenceIdentity(DEFAULT_REFERENCE_IDENTITY );
	gpsTimingData.SetSfn(DEFAULT_SFN);
	       
	posSatalliteInfo.SetGpsTimingData(gpsTimingData);
		
	TDetailedErrorReport detailedErrorReport;
	//Set GNSS Pseudorange Error Statistics 
    detailedErrorReport.SetStanDeviOfLongitudeError(DEFAULT_SD_OF_LONG_ERROR);
    detailedErrorReport.SetStanDeviOfLatiitudeError(DEFAULT_SD_OF_LAT_ERROR );
    detailedErrorReport.SetStanDeviOfAltitudeError(DEFAULT_SD_OF_ALT_ERROR );
    detailedErrorReport.SetStanDeviOfSemiMajorAxisError(DEFAULT_SD_OF_SEMI_MAJOR_AXIS_ERROR);
    detailedErrorReport.SetStanDeviOfSemiMinorAxisError(DEFAULT_SD_OF_SEMI_MINOR_AXIS_ERROR);
    detailedErrorReport.SetOrientationOfSemiMajorAxisError(DEFAULT_ORIEN_OF_SEMI_MAJOR_AXIS_ERROR);
    detailedErrorReport.SetRmsValOfStanDeviOfRange(DEFAULT_RMS_VAL_OF_SD_OF_RANGE);
    posSatalliteInfo.SetDetailedErrorReport(detailedErrorReport);
    // Set Geoidal separation
    posSatalliteInfo.SetGeoidalSeparation(DEFAULT_GEOIDAL_SEPARATION);
    //Set Magnetic variation
    posSatalliteInfo.SetMagneticVariation(DEFAULT_MAGNETIC_VARIATION);
    //Set Course over ground
    posSatalliteInfo.SetCourseOverGroundMagnetic(DEFAULT_COURSE_OVER_GROUND_MAGNETIC);
	    
    iUpdate->SetPosition(posSatalliteInfo);
	}


/*********************** Position Info Reader ************************/

CPosInfoConfigReader* CPosInfoConfigReader::NewL(const TDesC &aConfigFileName, const TDesC& aConfigSection, RPointerArray<TAny>& aPosInfoArr)
	{
	return new (ELeave) CPosInfoConfigReader(aConfigFileName, aConfigSection, aPosInfoArr);
	}


CPosInfoConfigReader::CPosInfoConfigReader(const TDesC &aConfigFileName, const TDesC& aConfigSection, RPointerArray<TAny>& aPosInfoArr) : CConfigReaderBase(aConfigFileName, aConfigSection), iPosInfoArr(aPosInfoArr)
	{
	}


void CPosInfoConfigReader::HandleSentenceL()
	{
	// Not a position info sentence, ignore all other valid sentences.
	if (	(iNumFields == 0) || (
			(iFields[0].Compare(_L8("position")) != 0) &&	
			(iFields[0].Compare(_L8("course")) != 0) &&
			(iFields[0].Compare(_L8("satellite")) != 0) ))
		{
		return;
		}

	// Determine the number of times to repeat the pos info.
	TInt repeat = 1;

	if (iFields[1].Length() != 0)
		{
		ExtractValueL(iFields[1], repeat);
		}

	// Add repeat items to the pos info array.
	for (TInt i = 0; i < repeat; i++)
	{
		// Alloc a Satellite type info.
		iPosInfo = new(ELeave) TPositionSatelliteInfo();
				
		// Optional fields are present fill out pos info based on them.
		if (iFields[2].Length() != 0)
			{
			DefaultData();	// This will ensure the pos info items are set to a default value for any incomplete sentences.
			ExtractDataL();
			}

		// Otherwise use default values.			
		else
			{
			DefaultData();
			}
			
		// Add to array, and reset pointer.
		User::LeaveIfError(iPosInfoArr.Append(iPosInfo));
		iPosInfo = NULL;
		}	
	}

// i think we will always have to call default to ensure we set things for missing fields
void CPosInfoConfigReader::ExtractDataL()
	{
	// TODO: we need to support something like this, ie $position,1,10,2.3* which is lat, long only
	// I think it will because extract value deals with empty strings
	if (iPosInfo->PositionClassType() & EPositionInfoClass)
		{
		TPosition position;
		TPositionInfo& posInfo = static_cast<TPositionInfo&>(*iPosInfo);
	
		TReal64 latitude;
		TReal64 longitude;
		TReal32 altitude;
		// TODO: TUid datum;
		TReal32 horzAccuracy;
		TReal32 vertAccuracy;

		ExtractValueL(iFields[EPosLatitude], latitude);
		ExtractValueL(iFields[EPosLongitude], longitude);
		ExtractValueL(iFields[EPosAltitude], altitude);
		// TODO: ExtractUidL() not sure what datum is
		ExtractValueL(iFields[EPosHorzAccuracy], horzAccuracy);
		ExtractValueL(iFields[EPosVertAccuracy], vertAccuracy);

		position.SetCoordinate(latitude, longitude, altitude);
		position.SetAccuracy(horzAccuracy, vertAccuracy);
		position.SetCurrentTime();
		
		posInfo.SetPosition(position);
		}

	if (iPosInfo->PositionClassType() & EPositionCourseInfoClass)
		{
		TCourse course;
		TPositionCourseInfo& posCourseInfo = static_cast<TPositionCourseInfo&>(*iPosInfo);

		TReal32 speed;
		TReal32 heading;
		TReal32 speedAccuracy;
		TReal32 headingAccuracy;

		ExtractValueL(iFields[EPosSpeed], speed);
		ExtractValueL(iFields[EPosHeading], heading);
		ExtractValueL(iFields[EPosSpeedAccuracy], speedAccuracy);
		ExtractValueL(iFields[EPosHeadingAccuracy], headingAccuracy);
						
		course.SetSpeed(speed);
		course.SetHeading(heading);
		course.SetSpeedAccuracy(speedAccuracy);
		course.SetHeadingAccuracy(headingAccuracy);
		
		posCourseInfo.SetCourse(course);		
		}

	if (iPosInfo->PositionClassType() & EPositionSatelliteInfoClass)
		{
		// TODO satellite info
		}	
	}


void CPosInfoConfigReader::DefaultData()
	{

	if (iPosInfo->PositionClassType() & EPositionInfoClass)
		{
		TPosition position;
		TPositionInfo& posInfo = static_cast<TPositionInfo&>(*iPosInfo);

		position.SetCoordinate(DEFAULT_NOTIFY_POS_UPDATE_LATITUDE, DEFAULT_NOTIFY_POS_UPDATE_LONGITUDE, DEFAULT_NOTIFY_POS_UPDATE_ALTITUDE);
		position.SetAccuracy(DEFAULT_NOTIFY_POS_UPDATE_HORIZONTAL_ACCURACY, DEFAULT_NOTIFY_POS_UPDATE_VERTICAL_ACCURACY);
		position.SetCurrentTime();
		
		posInfo.SetPosition(position);
		}

	if (iPosInfo->PositionClassType() & EPositionCourseInfoClass)
		{
		TCourse course;
		TPositionCourseInfo& posCourseInfo = static_cast<TPositionCourseInfo&>(*iPosInfo);

		course.SetSpeed(DEFAULT_NOTIFY_POS_UPDATE_SPEED);
		course.SetHeading(DEFAULT_NOTIFY_POS_UPDATE_HEADING);
		course.SetSpeedAccuracy(DEFAULT_NOTIFY_POS_UPDATE_SPEED_ACCURACY);
		course.SetHeadingAccuracy(DEFAULT_NOTIFY_POS_UPDATE_HEADING_ACCURACY);
		
		posCourseInfo.SetCourse(course);
		}

	if (iPosInfo->PositionClassType() & EPositionSatelliteInfoClass)
		{
		TSatelliteData satellite;
		TPositionSatelliteInfo& posSatalliteInfo = static_cast<TPositionSatelliteInfo&>(*iPosInfo);

		satellite.SetSatelliteId(DEFAULT_NOTIFY_POS_UPDATE_SATELLITE_ID1);
		satellite.SetAzimuth(DEFAULT_NOTIFY_POS_UPDATE_AZIMUTH1);
		satellite.SetElevation(DEFAULT_NOTIFY_POS_UPDATE_ELEVATION1);
		satellite.SetIsUsed(DEFAULT_NOTIFY_POS_UPDATE_IS_USED1);
		satellite.SetSignalStrength(DEFAULT_NOTIFY_POS_UPDATE_SIGNAL_STRENGTH1);
		
		posSatalliteInfo.AppendSatelliteData(satellite);

		satellite.SetSatelliteId(DEFAULT_NOTIFY_POS_UPDATE_SATELLITE_ID2);
		satellite.SetAzimuth(DEFAULT_NOTIFY_POS_UPDATE_AZIMUTH2);
		satellite.SetElevation(DEFAULT_NOTIFY_POS_UPDATE_ELEVATION2);
		satellite.SetIsUsed(DEFAULT_NOTIFY_POS_UPDATE_IS_USED2);
		satellite.SetSignalStrength(DEFAULT_NOTIFY_POS_UPDATE_SIGNAL_STRENGTH2);
		
		posSatalliteInfo.AppendSatelliteData(satellite);
		}

/* For extended tests when/if we have them.
	if (infoBase.PositionClassType() & EPositionClassTestExtension)
		{
		TExtPosInfo* genInfo = reinterpret_cast<TExtPosInfo*>(buffer);

		genInfo->iGalaxy = DEFAULT_NOTIFY_POS_UPDATE_EXT_GALAXY;
		genInfo->iSolarSystem = DEFAULT_NOTIFY_POS_UPDATE_EXT_SOLARSYS;
		genInfo->iStarDate = DEFAULT_NOTIFY_POS_UPDATE_EXT_STARDATE;
		}	
*/
	}


/*********************** Module Status Reader ************************/

CModuleStatusConfigReader* CModuleStatusConfigReader::NewL(const TDesC &aConfigFileName, const TDesC& aConfigSection, TPositionModuleStatus& aModuleStatus, TBool& aDelayReportingModStatus)
	{
	return new (ELeave) CModuleStatusConfigReader(aConfigFileName, aConfigSection, aModuleStatus, aDelayReportingModStatus);
	}


CModuleStatusConfigReader::CModuleStatusConfigReader(const TDesC &aConfigFileName, const TDesC& aConfigSection, TPositionModuleStatus& aModuleStatus, TBool& aDelayReportingModStatus) : CConfigReaderBase(aConfigFileName, aConfigSection), iModuleStatus(aModuleStatus), iDelayReportingModStatus(aDelayReportingModStatus)
	{
	}


void CModuleStatusConfigReader::ExtractValueDevice(const TPtrC8& aField, TPositionModuleStatus::TDeviceStatus& aValue)
	{
	// Set device status.
	if (aField.Length() != 0)
		{
		if (aField.Compare(_L8("device_error")) == 0)
			{
			aValue = TPositionModuleStatus::EDeviceError;
			}
			
		else if (aField.Compare(_L8("device_disable")) == 0)
			{
			aValue = TPositionModuleStatus::EDeviceDisabled;
			}
			
		else if (aField.Compare(_L8("device_inactive")) == 0)
			{
			aValue = TPositionModuleStatus::EDeviceInactive;
			}
			
		else if (aField.Compare(_L8("device_initalising")) == 0)
			{
			aValue = TPositionModuleStatus::EDeviceInitialising;
			}
			
		else if (aField.Compare(_L8("device_standby")) == 0)
			{
			aValue = TPositionModuleStatus::EDeviceStandBy;
			}
			
		else if (aField.Compare(_L8("device_ready")) == 0)
			{
			aValue = TPositionModuleStatus::EDeviceReady;
			}
			
		else if (aField.Compare(_L8("device_active")) == 0)
			{
			aValue = TPositionModuleStatus::EDeviceActive;
			}		
		}
	}


void CModuleStatusConfigReader::ExtractValueDataQuality(const TPtrC8& aField, TPositionModuleStatus::TDataQualityStatus& aValue)
	{
	// Set data quality.
	if (aField.Length() != 0)
		{
		if (aField.Compare(_L8("data_quality_loss")) == 0)
			{
			aValue = TPositionModuleStatus::EDataQualityLoss;
			}

		else if (aField.Compare(_L8("data_quality_partial")) == 0)
			{
			aValue = TPositionModuleStatus::EDataQualityPartial;
			}

		else if (aField.Compare(_L8("data_quality_normal")) == 0)
			{
			aValue = TPositionModuleStatus::EDataQualityNormal;
			}
		
		else if (aField.Compare(_L8("data_quality_unknown")) == 0)
			{
			aValue = TPositionModuleStatus::EDataQualityUnknown;
			}		
		}
	}

void CModuleStatusConfigReader::ExtractValueDelayUpdate(const TPtrC8& aField, TBool& aValue)
	{
	aValue = EFalse;	
	if (aField.Length() != 0)
		{
		if (aField.Compare(_L8("true")) == 0)
			{
			aValue = ETrue;
			}
		}	
	}


void CModuleStatusConfigReader::HandleSentenceL()
	{
	// Not a 'modstatus' sentence, ignore all other valid sentences.
	if ( (iNumFields == 0) || (iFields[EModuleStatusType].Compare(_L8("modstatus")) != 0) )
		{
		return;	
		}

	// Set default values for any fields not present in the sentence.
	DefaultData();

	// Set values from set fields in the sentence.
	if (iNumFields >= 3)
		{
		ExtractDataL();
		}	
	}
	
	
void CModuleStatusConfigReader::ExtractDataL()
	{
	
	TPositionModuleStatus::TDeviceStatus device;
	TPositionModuleStatus::TDataQualityStatus dataQuality;			
		
	ExtractValueDevice(iFields[EModuleStatusDevice], device);
	ExtractValueDataQuality(iFields[EModuleStatusDataQuality], dataQuality);

	iModuleStatus.SetDeviceStatus(device);
	iModuleStatus.SetDataQualityStatus(dataQuality);
	
	// is 'delay?' set?
	if(iNumFields == 4)
		{
		ExtractValueDelayUpdate(iFields[EModuleStatusDelayUpdate], iDelayReportingModStatus);
		}
	}


void CModuleStatusConfigReader::DefaultData()
	{
	iModuleStatus.SetDeviceStatus(TPositionModuleStatus::EDeviceReady);
	iModuleStatus.SetDataQualityStatus(TPositionModuleStatus::EDataQualityNormal);
	iDelayReportingModStatus = EFalse;
	}



	
/*********************** Update Options Reader ************************/

CUpdateOptsConfigReader* CUpdateOptsConfigReader::NewL(const TDesC &aConfigFileName, const TDesC& aConfigSection, TPositionUpdateOptions& aUpdateOpts)
	{
	return new (ELeave) CUpdateOptsConfigReader(aConfigFileName, aConfigSection, aUpdateOpts);
	}


CUpdateOptsConfigReader::CUpdateOptsConfigReader(const TDesC &aConfigFileName, const TDesC& aConfigSection, TPositionUpdateOptions& aUpdateOpts) : CConfigReaderBase(aConfigFileName, aConfigSection), iUpdateOpts(aUpdateOpts)
	{
	
	}


void CUpdateOptsConfigReader::ExtractValueL(const TPtrC8& aField, TTimeIntervalMicroSeconds& aValue)
	{
	if (aField.Length() != 0)
		{
		TLex8 lex(aField);
		TInt64 value;
		TInt err = lex.Val(value);
		User::LeaveIfError(err);
		aValue = value;
		}
	}


void CUpdateOptsConfigReader::ExtractValue(const TPtrC8& aField, TBool& aValue)
	{
	aValue = EFalse;	
	if (aField.Length() != 0)
		{
		if (aField.Compare(_L8("true")) == 0)
			{
			aValue = ETrue;
			}
		}
	}


void CUpdateOptsConfigReader::HandleSentenceL()
	{
	// Not a 'updateoptions' sentence, ignore all other valid sentences.
	if ( (iNumFields == 0) || (iFields[EUpdateOptsType].Compare(_L8("updateoptions")) != 0) )
		{
		return;	
		}

	// Set default values for any fields not present in the sentence.
	DefaultData();

	// Set values from set fields in the sentence.
	if (iNumFields == 5)
		{
		ExtractDataL();
		}	
	}
	
	
void CUpdateOptsConfigReader::ExtractDataL()
	{
	TTimeIntervalMicroSeconds interval;
	TTimeIntervalMicroSeconds timeout;
	TTimeIntervalMicroSeconds maxage;
	TBool partialUpdates;

	ExtractValueL(iFields[EUpdateOptsInterval], interval);
	ExtractValueL(iFields[EUpdateOptsTimeOut], timeout);
	ExtractValueL(iFields[EUpdateOptsMaxAge], maxage);
	ExtractValue(iFields[EUpdateOptsPartialUpdates], partialUpdates);

	iUpdateOpts.SetUpdateInterval(interval);
	iUpdateOpts.SetUpdateTimeOut(timeout);
	iUpdateOpts.SetMaxUpdateAge(maxage);
	iUpdateOpts.SetAcceptPartialUpdates(partialUpdates);
	}


void CUpdateOptsConfigReader::DefaultData()
	{
	iUpdateOpts.SetUpdateInterval(0);
	iUpdateOpts.SetUpdateTimeOut(0);
	iUpdateOpts.SetMaxUpdateAge(0);
	iUpdateOpts.SetAcceptPartialUpdates(EFalse);
	}




CAgpsModuleModesConfigReader* CAgpsModuleModesConfigReader::NewL(const TDesC &aConfigFileName, const TDesC& aConfigSection, 
					RPointerArray<TLbsGpsOptions>& aModuleModes)
	{
	return new (ELeave) CAgpsModuleModesConfigReader(aConfigFileName, aConfigSection, aModuleModes);
	}
CAgpsModuleModesConfigReader::CAgpsModuleModesConfigReader(const TDesC &aConfigFileName, const TDesC& aConfigSection, 
					RPointerArray<TLbsGpsOptions>& aModuleModes)
	: CConfigReaderBase(aConfigFileName, aConfigSection), iModuleModes(aModuleModes)
	{
	}

void CAgpsModuleModesConfigReader::HandleSentenceL()
	{
	// Not a 'modulemodes' sentence, ignore all other valid sentences.
	if ( (iNumFields == 0) || (iFields[EModuleModesType].Compare(_L8("modulemodes")) != 0) )
		{
		return;	
		}

	// Set default values for any fields not present in the sentence.
	DefaultData();

	// Set values from set fields in the sentence.
	if (iNumFields > 1)
		{
		ExtractDataL();
		}		
	}
	
void CAgpsModuleModesConfigReader::DefaultData()
/**
 * Does nothing since there is no default data to be set for the expected gps mode
 */
	{
	}

void CAgpsModuleModesConfigReader::ExtractDataL()
	{
	TInt gpsModeInt;
	ExtractValueL(iFields[EGpsModeField], gpsModeInt);
	CLbsAdmin::TGpsMode gpsMode(static_cast<CLbsAdmin::TGpsMode>(gpsModeInt));

	if(iNumFields == EGpsModeField+1)
		{ // there is no options array
		TLbsGpsOptions* gpsOptions = new(ELeave) TLbsGpsOptions;
		gpsOptions->SetGpsMode(gpsMode);
		iModuleModes.AppendL(gpsOptions);
		}
	else
		{
		TLbsGpsOptionsArray* gpsOptionsArray = new(ELeave) TLbsGpsOptionsArray;
		gpsOptionsArray->SetGpsMode(gpsMode);
		
		for(TInt index = EBegginingOfGpsOptionsArray; index < iNumFields; ++index)
			{
			TLbsGpsOptionsItem optionsItem;
			TInt posUpdateTypeInt;
			ExtractValueL(iFields[index], posUpdateTypeInt);
			TLbsGpsOptionsItem::TPosUpdateType posUpdateType(static_cast<TLbsGpsOptionsItem::TPosUpdateType>(posUpdateTypeInt));
			optionsItem.SetLocUpdateType(posUpdateType);
			gpsOptionsArray->AppendOptionItem(optionsItem);
			}
		
		iModuleModes.AppendL(gpsOptionsArray);
		}
	}


CClientDetailsConfigReader* CClientDetailsConfigReader::NewL(const TDesC &aConfigFileName, const TDesC& aConfigSection, 
					RArray<TCTClientDetails>& aClientDetailsArray)
	{
	return new (ELeave) CClientDetailsConfigReader(aConfigFileName, aConfigSection, aClientDetailsArray);
	}
CClientDetailsConfigReader::CClientDetailsConfigReader(const TDesC &aConfigFileName, const TDesC& aConfigSection, 
					RArray<TCTClientDetails>& aClientDetailsArray)
	: CConfigReaderBase(aConfigFileName, aConfigSection), 
		iClientDetailsArray(aClientDetailsArray)
	{
	}

void CClientDetailsConfigReader::HandleSentenceL()
	{
	// Not a 'clientdetails' sentence, ignore all other valid sentences.
	if ( (iNumFields == 0) || (iFields[0].Compare(_L8("clientdetails")) != 0) )
		{
		return;	
		}

	ExtractDataL();
	}
	
void CClientDetailsConfigReader::DefaultData()
/**
 * Does nothing since there is no default data to be set for the expected gps mode
 */
	{
	}

void CClientDetailsConfigReader::ExtractDataL()
	{
	TCTClientDetails clientDetails;
	TInt value;
	
	if(iNumFields > 1)
		{ // initial delay - after which session ID it should start - default is 0
		ExtractValueL(iFields[1], value);
		clientDetails.iSessionIdDelay = value;
		}
	
	if(iNumFields > 2)
		{ // initial delay - after which step in the session it should start - default is 0
		ExtractValueL(iFields[2], value);
		clientDetails.iSessionStepDelay = value;
		}

	if(iNumFields > 3)
		{ // number of NPUDs issued by the client - default is 1 
		ExtractValueL(iFields[3], value);
		clientDetails.iNumberOfNPUDs = value > 0 ? value : 1;
		}
	
	if(iNumFields > 4)
		{ // the update interval - default is 0 (no tracking)
		ExtractValueL(iFields[4], value);
		clientDetails.iUpdateOptions.SetUpdateInterval(value);
		}
	
	if(clientDetails.iNumberOfNPUDs == 1 || clientDetails.iUpdateOptions.UpdateInterval() == 0)
		{ // if one of the params indicates no tracking then set the others to say the same
		clientDetails.iUpdateOptions.SetUpdateInterval(0);
		}

	if(iNumFields > 5)
		{ // the max fix time - default is 0 (no timeout)
		ExtractValueL(iFields[5], value);
		clientDetails.iUpdateOptions.SetUpdateTimeOut(value);
		}

	if(iNumFields > 6)
		{ // the max update age - default is 0
		ExtractValueL(iFields[6], value);
		clientDetails.iUpdateOptions.SetMaxUpdateAge(value);
		}
	
	if(iNumFields > 7)
		{ // if partial updates should be accepted - default is EFalse
		if (iFields[7].Compare(_L8("true")) == 0)
			{
			clientDetails.iUpdateOptions.SetAcceptPartialUpdates(ETrue);
			}
		}

	if(iNumFields > 8)
		{
			{ // if partial updates should be accepted - default is EFalse
			if (iFields[8].Compare(_L8("cancel")) == 0)
				{
				clientDetails.iCancelRequest = ETrue;
				}
			}		
		}
	
	if(iNumFields > 9)
		{ // delay after which step in the session the last request should be cancelled - default is 0
		ExtractValueL(iFields[9], value);
		clientDetails.iSessionIdCancel = value;
		}	
	
	if(iNumFields > 10)
		{ // delay after which session ID the last request should be cancelled - default is 0
		ExtractValueL(iFields[10], value);
		clientDetails.iSessionStepCancel = value;
		}	
	
	if(iNumFields > 11)
		{ // id of psy to be used
		ExtractValueL(iFields[11], value);
		clientDetails.iPsyId = value;
		}
	
	if(iNumFields > 12)
		{ // expected error
		ExtractValueL(iFields[12], value);
		clientDetails.iExpectedError = value;
		}
	
	iClientDetailsArray.AppendL(clientDetails);
	}
