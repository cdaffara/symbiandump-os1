// lbs.cpp
// 
// Copyright (c) 2008 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <fshell/common.mmh>

#include <lbs.h>
#include <lbssatellite.h>
#include <LbsErrors.h>

#if defined (FSHELL_PLATFORM_S60) || defined (FSHELL_PLATFORM_FOUNDATION)
#include <EPos_CPosModules.h>
#include <EPos_CPosModuleUpdate.h>
#include <EPos_CPosModuleIdList.h> 
#endif

#if !defined (FSHELL_PLATFORM_S60) && !defined (FSHELL_PLATFORM_FOUNDATION)
#include <lbs/lbsadmin.h>
#endif

#include <fshell/ioutils.h>
using namespace IoUtils;

class CCmdLbs : public CCommandBase
	{
public:
	void PrintTime(const TTime& aTime, TBool aNewline);	
	static const TDesC* TechnologyTypeToString(TPositionModuleInfo::TTechnologyType aType);
	static const TDesC* DeviceLocationToString(TPositionModuleInfo::TDeviceLocation aLoc);
	static const TDesC* CostIndicatorToString(TPositionQuality::TCostIndicator aCost);
	static const TDesC* PowerConsumptionToString(TPositionQuality::TPowerConsumption aPower);
	static void CapabilitiesToString(TPositionModuleInfo::TCapabilities aCap, TDes &aDes);
	
	static CCommandBase* NewLC();
	~CCmdLbs();
private:
	CCmdLbs();
	void PrintModuleDetail(TPositionModuleInfo& aModInfo);
	void DoListModuleL();
	void DoListModuleS60L();
	void DoLocation();
	void DoEnableModuleL(TBool aEnable = ETrue);	
#if !defined (FSHELL_PLATFORM_S60)
	void DoSynchTimeL();
#endif

private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);

private:
	CActiveSchedulerWait* iActiveWait;
	RPositionServer iPosSrv;
	RPositioner iPos;
	TPositionSatelliteInfo iSatelliteInfo;	//NOT all LBS modules accept satellite info
	TPositionInfo iPosInfo;	
	TPositionInfoBase* ipPosInfo;			//point to appropriate structure 
		
	RArray<TBool> iVerbose;
	TInt iOptModuleIndex;		//which module to use (module index, not id) 
	TInt iOptTimeoutSec;		//timeout in second, default is 60 second
	TInt iOptLoops;				//how many loops (default 1)
	
	enum TLbsCmd
		{
		ECmdLocation,		
		ECmdListModule,
		ECmdListModuleS60,
		ECmdEnableModule,
		ECmdDisableModule,
		ECmdSynchTime		//synchronise system time with GPS timestamp
		};
	
		
	TLbsCmd iCommand;
	};

CCommandBase* CCmdLbs::NewLC()
	{
	CCmdLbs* self = new(ELeave) CCmdLbs();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdLbs::CCmdLbs()
	{
	iOptModuleIndex = -1;
	iOptTimeoutSec = 300;
	iOptLoops = 1;
	iActiveWait = new (ELeave) CActiveSchedulerWait;	
	}


CCmdLbs::~CCmdLbs()
	{
	iPos.Close();
	iPosSrv.Close();
	
	iVerbose.Close();
	delete iActiveWait;
	}

//////////////////////////////////////////////////////


//////////////////////////////////////////////////////

const TDesC& CCmdLbs::Name() const
	{
	_LIT(KName, "lbs");
	return KName;
	}

void CCmdLbs::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArgCommand, "command");
	aArguments.AppendEnumL((TInt&)iCommand, KArgCommand);
	}

void CCmdLbs::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptVerbose, "verbose");
	aOptions.AppendBoolL(iVerbose, KOptVerbose);
	
	_LIT(KOptModuleIndex, "module-index");
	aOptions.AppendUintL((TUint&)iOptModuleIndex, KOptModuleIndex);

	_LIT(KOptLoops, "loops");
	aOptions.AppendUintL((TUint&)iOptLoops, KOptLoops);

	_LIT(KOptTimeout, "timeout");
	aOptions.AppendUintL((TUint&)iOptTimeoutSec, KOptTimeout);
	}

void CCmdLbs::DoRunL()
	{
	Printf(_L("Connecting to RPositionServer...\r\n"));
	TInt err = iPosSrv.Connect();
	LeaveIfErr(err, _L("Cannot connect to position server"));
	
	switch(iCommand)
		{
		case ECmdLocation:
			DoLocation();
			break;
		case ECmdListModule:
			DoListModuleL();
			break;
		case ECmdListModuleS60:
			DoListModuleS60L();
			break;
		case ECmdEnableModule:
			DoEnableModuleL(ETrue);
			break;
		case ECmdDisableModule:
			DoEnableModuleL(EFalse);
			break;
		case ECmdSynchTime:
		//TODO There's no way of actually specifying this command
#if !defined (FSHELL_PLATFORM_S60)
			DoSynchTimeL();
#else
			LeaveIfErr(KErrNotSupported, _L("SynchTime not supported on S60"));
#endif
			break;
		}	
		
	}

void CCmdLbs::DoLocation()
	{
	TInt VerboseLevel = iVerbose.Count();		
	TInt err;
	TBool bSatelliteCapable = EFalse;
	TPositionModuleId modId; //which module to use
	if (iOptModuleIndex != -1) 
		{
		TPositionModuleInfo modInfo;
		err = iPosSrv.GetModuleInfoByIndex(iOptModuleIndex, modInfo);
		LeaveIfErr(err, _L("invalid module index"));
		modId = modInfo.ModuleId();						
		}
	else
		{	//use the default module
		err = iPosSrv.GetDefaultModuleId(modId);
		LeaveIfErr(err, _L("Invalid default module"));
		}		
	
	TPositionModuleInfo modInfo;
	err = iPosSrv.GetModuleInfoById(modId, modInfo);
	TBuf<128> modName;
	modInfo.GetModuleName(modName);	
	
	Printf(_L("Openning RPositioner...(Module ID: 0x%X \"%S\")\r\n"), modId.iUid, &modName);
	err = iPos.Open(iPosSrv, modId);
	LeaveIfErr(err, _L("Cannot open positioner"));
	
   // Set position requestor, it is compulsory on S60 only
    err = iPos.SetRequestor( CRequestor::ERequestorService ,
         CRequestor::EFormatApplication , _L("FSHELL FLBS COMMAND") );
	LeaveIfErr(err, _L("Cannot set requestor"));
			
	TPositionUpdateOptions UpdateOption;
	TTimeIntervalMicroSeconds TimeoutMs = (TInt64)iOptTimeoutSec * (TInt64)1000000;
	UpdateOption.SetUpdateTimeOut(TimeoutMs);
	
	Printf(_L("SetUpdateOptions... Timeout = %d seconds\r\n"), iOptTimeoutSec);
	err = iPos.SetUpdateOptions(UpdateOption);
	LeaveIfErr(err, _L("Cannot set update options"));
	
	//check if the module is capable of satellite info
	{
		TPositionModuleInfo::TCapabilities modCap = modInfo.Capabilities();
		bSatelliteCapable = modCap & TPositionModuleInfo::ECapabilitySatellite;
		if (bSatelliteCapable)
			{
			ipPosInfo = &iSatelliteInfo;
			Printf(_L("This module is capable of receiving satellite information\r\n"));
			}
		else
			ipPosInfo = &iPosInfo;			
	}
	
	for (TInt i=0; i<iOptLoops; i++)
		{
		TRequestStatus status;
		TPosition pos;
		Printf(_L("Calling RPositioner::NotifyPositionUpdate...\r\n"));
		iPos.NotifyPositionUpdate(*ipPosInfo, status);
		User::WaitForRequest(status);
		
		err = status.Int();
		if (err != KErrNone)
			LeaveIfErr(err, _L("NotifyPositionUpdate encountered an error"));
				
		static_cast <TPositionInfo*>(ipPosInfo) -> GetPosition(pos);
		TTime LocTime = pos.Time();
		TReal32 HorizontalAccuracy = pos.HorizontalAccuracy();
		TReal32 VerticalAccuracy = pos.VerticalAccuracy();
		//print current location information
		Printf(_L("Altit:%f Latit:%f Longt:%f HAccu:%.2f VAccu:%.2f\r\n"), 
				pos.Altitude(), pos.Latitude(), pos.Longitude(), 
				HorizontalAccuracy, VerticalAccuracy);		
		
		Printf(_L("Location Signal Time:"));
		PrintTime(LocTime, ETrue);
		
		//if there is satellite information, optionally print it out
		if (bSatelliteCapable && (VerboseLevel>0) )
			{
			TPositionSatelliteInfo* pSateInfo = static_cast <TPositionSatelliteInfo*> (ipPosInfo);
			
			TInt NumSatellitesInView = pSateInfo->NumSatellitesInView();
			TInt NumSatellitesUsed = pSateInfo->NumSatellitesUsed();
			TTime SatelliteTime = pSateInfo->SatelliteTime();
			TReal32 HorizontalDoP = pSateInfo->HorizontalDoP();
			TReal32 VerticalDoP = pSateInfo->VerticalDoP();
			TReal32 TimeDoP = pSateInfo->TimeDoP(); 
			Printf(_L("Satellite In View:%d Used:%d Time:"), 
					NumSatellitesInView, NumSatellitesUsed );
			PrintTime(SatelliteTime, ETrue);
			Printf(_L("HoriDop:%f VertDop:%f TimeDop:%f \r\n"), 
					HorizontalDoP, VerticalDoP, TimeDoP);
			
			//Print each satellite info
			for (TInt i=0; i<NumSatellitesInView; i++ )
				{
				TSatelliteData SatellliteData;
				err = pSateInfo->GetSatelliteData((TUint)i, SatellliteData);
				if (err == KErrNotFound)
					{
					Printf(_L("Satellite #%d Not Found\r\n"), i);
					continue;
					}
				else
					{
					User::LeaveIfError(err);
					}
				
				TInt SatelliteId = SatellliteData.SatelliteId(); 
				TReal32 Azimuth = SatellliteData.Azimuth();
				TReal32 Elevation = SatellliteData.Elevation();
				TBool IsUsed = SatellliteData.IsUsed();
				TInt SignalStrength = SatellliteData.SignalStrength();
				
				Printf(_L("Satellite #%d, ID:%d Azimuth %.2f Elevation %.2f IsUsed:%d Strenth:%d\r\n"), 
						i, SatelliteId, Azimuth, Elevation, IsUsed, SignalStrength);
			
				}
			}			
		}	
	}

void CCmdLbs::DoListModuleL()
	{
	TInt err;
	TUint numModules;
	TPositionModuleInfo modInfo;
	
	err = iPosSrv.GetNumModules(numModules);
	User::LeaveIfError(err);

	Printf(_L("%d Modules found\r\n"), 
			numModules);
	
	for (TUint i=0 ; i < numModules ; i++)
		{
		Printf(_L("============================\r\n")); 		
		err = iPosSrv.GetModuleInfoByIndex(i, modInfo);
		User::LeaveIfError(err);
		
		Printf(_L("Module Index: %d\r\n"), i);
		
		PrintModuleDetail(modInfo);
		}
	
	Printf(_L("=================================\r\n"));
	//print default module ID
	TPositionModuleId DefaultId;
	err = iPosSrv.GetDefaultModuleId(DefaultId);		
	Printf(_L("Default Module ID 0x%X\r\n"), DefaultId.iUid );
	}

//S60 proprietary
//similar to DoListModuleL, but list module based on S60 API,
//and sometimes it gives different results than symbain API
void CCmdLbs::DoListModuleS60L()
	{
#ifdef FSHELL_PLATFORM_S60
	//TInt err;
	TUint numModules;
	CPosModules* pModHand = CPosModules::OpenL();
	CleanupStack::PushL(pModHand);
	
	CPosModuleIdList* pIdList = pModHand->ModuleIdListL();
	CleanupStack::PushL(pIdList);
	
	numModules = pIdList->Count();
	Printf(_L("%d Modules found\r\n"), 
			numModules);
	
	for (TUint i=0 ; i < numModules ; i++)
		{
		Printf(_L("============================\r\n"));
		TPositionModuleId S60modId = (*pIdList) [i];
		TPositionModuleInfo S60modInfo;
		TBuf<128> S60modName;			//to store module name
		
		pModHand->GetModuleInfoL(S60modId, S60modInfo);
		S60modInfo.GetModuleName(S60modName);
				
		Printf(_L("Module Index: %d\r\n"), i);
		
		PrintModuleDetail(S60modInfo);
		}

	Printf(_L("=================================\r\n"));		
	
	CleanupStack::PopAndDestroy(pIdList);
	CleanupStack::PopAndDestroy(pModHand);		
#endif
	}

void CCmdLbs::PrintModuleDetail(TPositionModuleInfo& aModInfo)
	{
	//TInt err;
	TBool bAvailable = aModInfo.IsAvailable();
	
	TPositionModuleId modId;
	modId = aModInfo.ModuleId();
	
	TBuf<128> modName;
	aModInfo.GetModuleName(modName);
	
	//==quality
	TPositionQuality modQuality;
	aModInfo.GetPositionQuality(modQuality);
	TTimeIntervalMicroSeconds firstFix = modQuality.TimeToFirstFix();
	TTimeIntervalMicroSeconds nextFix = modQuality.TimeToNextFix();
	TReal32 horiAccu = modQuality.HorizontalAccuracy();
	TReal32 vertAccu = modQuality.VerticalAccuracy();
	TPositionQuality::TCostIndicator cost = modQuality.CostIndicator();
	TPositionQuality::TPowerConsumption power = modQuality.PowerConsumption();
	////////////////////////////////////////////////////////////////
	TPositionModuleInfo::TTechnologyType modType = aModInfo.TechnologyType();		
	TPositionModuleInfo::TDeviceLocation modLoc = aModInfo.DeviceLocation();		
	TPositionModuleInfo::TCapabilities modCap = aModInfo.Capabilities();
	TVersion modVersion = aModInfo.Version();
	TVersionName VersionName = modVersion.Name(); 
	
	//TPositionClassFamily aClassType;
	//modInfo.ClassesSupported(aClassType);
	
	Printf(_L("Module Id:0x%X, Name:\"%S\" Available:%d \r\n"),
			modId.iUid, &modName, bAvailable);
	Printf(_L("Type:%S Version:%S\r\n"), 
			TechnologyTypeToString(modType), &VersionName );
	Printf(_L("Location:%S\r\n"), DeviceLocationToString (modLoc));

	Printf(_L("Quality first fix:%Ld next fix:%Ld \r\n"),
			firstFix.Int64(), nextFix.Int64());
	Printf(_L("Horizontal Accuracy:%f Vertical Accuray:%f \r\n"),
			horiAccu, vertAccu);
	Printf(_L("Cost Indicator:%S\r\n"), CostIndicatorToString(cost));
	Printf(_L("Power Consumption:%S\r\n"), PowerConsumptionToString(power));		
				
	TBuf<256> CapString;
	CapabilitiesToString(modCap, CapString);
	Printf(_L("Capabilities:%S\r\n"), &CapString);	
	}
	
//it is a S60 proprietary function
//
void CCmdLbs::DoEnableModuleL(TBool aEnable)
	{
	TInt err;
	TBuf<128> modName;			//to store module name
	TPositionModuleInfo modInfo;
	TPositionModuleId modId = TUid::Null(); //which module to use
	TBool bIsCurAvail;
	
	if (iOptModuleIndex != -1) 
		{
		err = iPosSrv.GetModuleInfoByIndex(iOptModuleIndex, modInfo);
		LeaveIfErr(err, _L("invalid module index"));
		modId = modInfo.ModuleId();						
		}
	else 
		{
		LeaveIfErr(KErrArgument, _L("Need to specify a module"));
		}
	
	bIsCurAvail = modInfo.IsAvailable();
	modInfo.GetModuleName(modName);
	
	TPtrC AvailNow = (bIsCurAvail)?_L("Yes"):_L("No");
	Printf(_L("Module Id=0x%08x Name:\"%S\" Current Availability:%S \r\n"), 
			modId.iUid, &modName, &AvailNow);
	
	//modInfo.SetIsAvailable(ETrue);	//Note: this has no effect on S60 platform
	
#ifdef FSHELL_PLATFORM_S60
	CPosModules* pModHand = CPosModules::OpenL();
	CleanupStack::PushL(pModHand);
	
	//diagnosis code: to verify the information given by S60 API matches Symbian API
	{
		CPosModuleIdList* pIdList = pModHand->ModuleIdListL();
		CleanupStack::PushL(pIdList);
		
		TPositionModuleId S60modId = (*pIdList) [iOptModuleIndex];
		TPositionModuleInfo S60modInfo;
		TBuf<128> S60modName;			//to store module name
				
		pModHand->GetModuleInfoL(S60modId, S60modInfo);
		S60modInfo.GetModuleName(S60modName);
		RDebug::Print(_L("S60 LBS module name:%S"), &S60modName);
		
		CleanupStack::PopAndDestroy(pIdList);
	}
	
	
	CPosModuleUpdate* pModUpdate = CPosModuleUpdate::NewLC();	
	TBool bNewAvail = aEnable;
	pModUpdate->SetUpdateAvailability(bNewAvail);	
	TRAP(err, pModHand->UpdateModuleL(modId, *pModUpdate));
	LeaveIfErr(err, _L("Could not update module information") );
		
	CleanupStack::PopAndDestroy(pModUpdate);		
	CleanupStack::PopAndDestroy(pModHand);		
#endif
	
	//verify if the change has take effect
		{
		err = iPosSrv.GetModuleInfoByIndex(iOptModuleIndex, modInfo);
		LeaveIfErr(err, _L("invalid module index"));
		modId = modInfo.ModuleId();						
		bIsCurAvail = modInfo.IsAvailable();
		TPtrC NewAvail = (bIsCurAvail)?_L("Yes"):_L("No");
		Printf(_L("Module Id=0x%08x Name:\"%S\" New Availability:%S \r\n"), 
				modId.iUid, &modName, &NewAvail);		
		}
		
	}
	
#if !defined (FSHELL_PLATFORM_S60)
void CCmdLbs::DoSynchTimeL()
	{
	// Set LBS setting to allow manual clock adjustment
	//This may not be set by default
    CLbsAdmin* admin = CLbsAdmin::NewL();
    CleanupStack::PushL(admin);
    CLbsAdmin::TClockAdjust clockAdjust;   
    User::LeaveIfError(admin->Get(KLbsSettingAllowManualClockAdjust,clockAdjust));
    admin->Set(KLbsSettingAllowManualClockAdjust,CLbsAdmin::EClockAdjustOn);
    CleanupStack::PopAndDestroy(admin);
	}
#endif

EXE_BOILER_PLATE(CCmdLbs)

void CCmdLbs::PrintTime(const TTime& aTime, TBool aNewline)
	{
	
	TTime NullTime = Time::NullTTime();
	if (aTime == NullTime) 
		{
		Printf(_L("(NullTime)"));
		}
	else
		{
		_LIT8(KDateTimeFormat, "%d-%02d-%02d %02d:%02d:%02d");
		TDateTime dt = aTime.DateTime();
		Printf(KDateTimeFormat, dt.Year(), dt.Month()+1, dt.Day()+1, dt.Hour(), dt.Minute(), dt.Second());
		}
	
	if (aNewline) Printf(_L("\r\n"));
	}


#define CASE_LIT(x) case x: { _LIT(KName, #x); return &KName; }

const TDesC* CCmdLbs::TechnologyTypeToString(TPositionModuleInfo::TTechnologyType aType)
	{
	switch(aType)
		{
		CASE_LIT(TPositionModuleInfo::ETechnologyUnknown);
		CASE_LIT(TPositionModuleInfo::ETechnologyTerminal);
		CASE_LIT(TPositionModuleInfo::ETechnologyNetwork);
		CASE_LIT(TPositionModuleInfo::ETechnologyAssisted);
		default:
			{			
			_LIT(KUnknown, "Unknown");
			return &KUnknown;
			}
		}
	}


const TDesC* CCmdLbs::DeviceLocationToString(TPositionModuleInfo::TDeviceLocation aLoc)
	{
	switch(aLoc)
		{
		CASE_LIT(TPositionModuleInfo::EDeviceUnknown);
		CASE_LIT(TPositionModuleInfo::EDeviceInternal);
		CASE_LIT(TPositionModuleInfo::EDeviceExternal);
		default:
			{			
			_LIT(KUnknown, "Unknown");
			return &KUnknown;
			}
		}
	}


const TDesC* CCmdLbs::CostIndicatorToString(TPositionQuality::TCostIndicator aCost)
	{
	switch(aCost)
		{
		CASE_LIT(TPositionQuality::ECostUnknown);
		CASE_LIT(TPositionQuality::ECostZero);
		CASE_LIT(TPositionQuality::ECostPossible);
		CASE_LIT(TPositionQuality::ECostCharge);
		default:
			{			
			_LIT(KUnknown, "Unknown");
			return &KUnknown;
			}
		}
	}

const TDesC* CCmdLbs::PowerConsumptionToString(TPositionQuality::TPowerConsumption aPower)
	{
	switch(aPower)
		{
		CASE_LIT(TPositionQuality::EPowerUnknown);
		CASE_LIT(TPositionQuality::EPowerZero);
		CASE_LIT(TPositionQuality::EPowerLow);
		CASE_LIT(TPositionQuality::EPowerMedium);
		CASE_LIT(TPositionQuality::EPowerHigh);
		default:
			{			
			_LIT(KUnknown, "Unknown");
			return &KUnknown;
			}
		}
	}
#define CAPFLAG(x) if (aCap & TPositionModuleInfo::x) { aDes += _L(#x); aDes += _L("|");}

void CCmdLbs::CapabilitiesToString(TPositionModuleInfo::TCapabilities aCap, TDes &aDes)
	{
	aDes.SetLength(0);
	//if (aCap & TPositionModuleInfo::ECapabilityNone) aDes += _L("ECapabilityNone|");
	CAPFLAG(ECapabilityNone);
	CAPFLAG(ECapabilityHorizontal);
	CAPFLAG(ECapabilityVertical);
	CAPFLAG(ECapabilitySpeed);
	CAPFLAG(ECapabilityDirection);
	CAPFLAG(ECapabilitySatellite);
	CAPFLAG(ECapabilityCompass);
	CAPFLAG(ECapabilityNmea);
	CAPFLAG(ECapabilityAddress);
	CAPFLAG(ECapabilityBuilding);
	CAPFLAG(ECapabilityMedia);
	}
	
