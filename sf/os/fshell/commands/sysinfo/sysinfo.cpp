// sysinfo.cpp
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

//
// CCmdSysInfo.
//
#include "u32std.h"
#include <ETELMM.H>
#include "sysinfo.h"
#include <fshell/common.mmh>
#include <fshell/ltkhal.h>
#include <fshell/ltkutils.h>
#ifdef FSHELL_CORE_SUPPORT_SYSINFO_WLAN
#include <wlansdkpskeys.h>	//for WLAN info
#endif
#include <e32rom.h>

	
CCommandBase* CCmdSysInfo::NewLC()
	{
	CCmdSysInfo* self = new (ELeave) CCmdSysInfo();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdSysInfo::~CCmdSysInfo()
	{
	}

CCmdSysInfo::CCmdSysInfo()
	: CCommandBase(EReportAllErrors)
	{
	}

void CCmdSysInfo::PrintHalL(HALData::TAttribute aHalAttribute)
	{
	LtkUtils::CHalAttribute* attrib = LtkUtils::GetHalInfoL(aHalAttribute);
	if (attrib->iError)
		{
		PrintWarning(_L("Unable to read %S: %S (%d)"), &attrib->iAttributeName, Stringify::Error(attrib->iError), attrib->iError);
		}
	else
		{
		Printf(_L("%S: %S\r\n"), &attrib->iAttributeName, attrib->iDescription);
		}
	delete attrib;
	}

void CCmdSysInfo::PrintWlanL()
	{
#ifdef FSHELL_CORE_SUPPORT_SYSINFO_WLAN
    //result format 00:18:0f:1e:96:a2.
    TBuf8<KPSWlanMacAddressLength> DesC8AddrText;
    TBuf<50> formatedMacAddress;
    LeaveIfErr(RProperty::Get(KPSUidWlan,KPSWlanMacAddress,DesC8AddrText), _L("Unable to get MAC address"));
    for ( TInt i=0; i < (TInt)KPSWlanMacAddressLength; i++ )
        {
        // Set separator
        if( i > 0 )
            formatedMacAddress.Append(':');
 
        // Set data
        TBuf<10> FormatText;
        FormatText.Format(_L("%02x"), DesC8AddrText[i]);
        formatedMacAddress.Append(FormatText);
        }
    Printf(_L("WLAN MAC Address: %S\r\n"), &formatedMacAddress);
#endif
	}

void CCmdSysInfo::PrintPhoneIdL()
	{
#ifdef FSHELL_TELEPHONY_SUPPORT
	TRequestStatus status;

	RTelServer telSvr;
	LeaveIfErr(telSvr.Connect(), _L("Unable to connect to telephony server"));
	CleanupClosePushL(telSvr);

	RMobilePhone mobilePhone;
	LeaveIfErr(mobilePhone.Open(telSvr, _L("DefaultPhone")), _L("Unable to open default mobile phone"));
	CleanupClosePushL(mobilePhone);
		
	TUint32 identCap;
	mobilePhone.GetIdentityCaps(identCap);
		
	RMobilePhone::TMobilePhoneIdentityV1 Id;
	mobilePhone.GetPhoneId(status, Id);
	User::WaitForRequest(status);

	LeaveIfErr(status.Int(), _L("Unable to retrieve phone identity"));
	if (identCap & RMobilePhone::KCapsGetManufacturer)
		Printf(_L("Manufacturer (from TSY): %S\r\n"), &(Id.iManufacturer) );

	if (identCap & RMobilePhone::KCapsGetModel)
		Printf(_L("Model (from TSY): %S\r\n"), &(Id.iModel) );
		
	if (identCap & RMobilePhone::KCapsGetRevision)
		Printf(_L("Revision (from TSY): %S\r\n"), &(Id.iRevision) );
		
	if (identCap & RMobilePhone::KCapsGetSerialNumber)
		Printf(_L("SerialNumber (IMEI): %S\r\n"), &(Id.iSerialNumber) );
	
	//query subscriber id
	if (identCap & RMobilePhone::KCapsGetSubscriberId)
		{
		RMobilePhone::TMobilePhoneSubscriberId subsId;
		mobilePhone.GetSubscriberId(status, subsId);
		User::WaitForRequest(status);

		LeaveIfErr(status.Int(), _L("Unable to query subscriber id"));
		Printf(_L("SubscriberId (IMSI): %S\r\n"), &(subsId) );
		}

	TPckgBuf<RMobilePhone::TMobilePhoneNetworkInfoV1> info;
	RMobilePhone::TMobilePhoneLocationAreaV1 loc;
	mobilePhone.GetCurrentNetwork(status, info, loc);
	User::WaitForRequest(status);
	LeaveIfErr(status.Int(), _L("Unable to get current network info"));
	Printf(_L8("Network id: %S "), &info().iNetworkId);
	Printf(_L("shortName: %S longName: %S displayTag: %S"), &info().iShortName, &info().iLongName, &info().iDisplayTag);

	CleanupStack::PopAndDestroy();	//mobilePhone
	CleanupStack::PopAndDestroy();	//telSvr
#endif	
	}	

void CCmdSysInfo::DoRunL()
	{
	if (iMachineUIDOnly)
		{
		// sysinfo command-line configured to return only the machine uid value & nothing else
		TInt value;
		TInt err = HAL::Get(HALData::EMachineUid, value);
		if (err)
			{
			PrintWarning(_L("Unable to read Machine UID %S (%d)"), Stringify::Error(err), err);
			}
		else
			{
			TBuf<10> buf;
			buf.Format(_L("%d"), value);
			Write(buf);
			}
		}
	else
		{
		// sys info command-line configured to return all information, including HAL field description
		PrintHalL(HALData::EManufacturer);
		PrintHalL(HALData::EManufacturerHardwareRev);
		PrintHalL(HALData::EManufacturerSoftwareRev);
		PrintHalL(HALData::EManufacturerSoftwareBuild);
		PrintHalL(HALData::EModel);
		PrintHalL(HALData::EMachineUid);
		PrintHalL(HALData::ECPUSpeed);
		PrintHalL(HALData::ESystemStartupReason);
		PrintHalL(HALData::ESystemTickPeriod);
		PrintHalL(HALData::ENanoTickPeriod);
		PrintHalL(HALData::EFastCounterFrequency);
		PrintHalL(HALData::EFastCounterCountsUp);
		PrintHalL(HALData::ESystemException);
		PrintHalL(HALData::EDisplayXPixels);
		PrintHalL(HALData::EDisplayYPixels);
		PrintHalL(HALData::EDisplayXTwips);
		PrintHalL(HALData::EDisplayYTwips);
		PrintHalL(HALData::EDisplayColors);
		PrintKernelHalStuff();
		PrintRomVersion();
		TRAP_IGNORE(PrintWlanL());
		TRAP_IGNORE(PrintPhoneIdL());
		}
	}
	
const TDesC& CCmdSysInfo::Name() const
	{
	_LIT(KName, "sysinfo");
	return KName;
	}

void CCmdSysInfo::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptUid, "machine-uid");
	aOptions.AppendBoolL(iMachineUIDOnly, KOptUid);
	}

EXE_BOILER_PLATE(CCmdSysInfo)

void CCmdSysInfo::PrintKernelHalStuff()
	{
	enum TKernelConfigFlagsNotIn91
		{
		EKernelConfigPlatSecLocked = 1<<7,
		EKernelConfigCrazyScheduling = 1<<8,

		EKernelConfigPagingPolicyNoPaging = 0,
		EKernelConfigPagingPolicyAlwaysPage = 1,
		EKernelConfigPagingPolicyDefaultUnpaged = 2,
		EKernelConfigPagingPolicyDefaultPaged = 3,

		EKernelConfigCodePagingPolicyShift			= 5,
		EKernelConfigCodePagingPolicyMask			= 3<<5,
		EKernelConfigCodePagingPolicyNoPaging		= EKernelConfigPagingPolicyNoPaging<<5,
		EKernelConfigCodePagingPolicyAlwaysPage		= EKernelConfigPagingPolicyAlwaysPage<<5,
		EKernelConfigCodePagingPolicyDefaultUnpaged	= EKernelConfigPagingPolicyDefaultUnpaged<<5,
		EKernelConfigCodePagingPolicyDefaultPaged	= EKernelConfigPagingPolicyDefaultPaged<<5,

		EKernelConfigDataPagingPolicyShift			= 9,
		EKernelConfigDataPagingPolicyMask			= 3<<9,
		};

	enum THalFunctionsNotIn91
		{
		EKernelHalSmpSupported = 15,
		EKernelHalNumLogicalCpus = 16,
		EKernelHalConfigFlags = 20,
		};

	// Memory model
	TUint memModelInfo = UserSvr::HalFunction(EHalGroupKernel, EKernelHalMemModelInfo, 0, 0);
	Printf(_L("Memory model: "));
	const TInt EMemModelTypeFlexible = 4;
	switch (memModelInfo & EMemModelTypeMask)
		{
		case EMemModelTypeDirect:
			Printf(_L("Direct")); break;
		case EMemModelTypeMoving:
			Printf(_L("Moving")); break;
		case EMemModelTypeMultiple:
			Printf(_L("Multiple")); break;
		case EMemModelTypeEmul:
			Printf(_L("Emulator")); break;
		case EMemModelTypeFlexible:
			Printf(_L("Flexible")); break;
		default:
			Printf(_L("Unknown (%d)"), memModelInfo & EMemModelTypeMask);
			break;
		}
	Printf(_L("\r\n"));

	// Hal config
	TInt flags = UserSvr::HalFunction(EHalGroupKernel, EKernelHalConfigFlags, 0, 0); // Not the most documented of APIs but has been around since at least v9.1
	if (flags < 0)
		{
#ifdef __EPOC32__
		// The only things that are dynamically calculated are things that aren't supported on platforms that don't support the HalFunction, so this is a good compromise
		flags = ((const TRomHeader*)UserSvr::RomHeaderAddress())->iKernelConfigFlags;
#else
		return;
#endif
		}

	_LIT(KOn, "On");
	_LIT(KOff, "Off");
	Printf(_L("PlatSec Enforcement: %S\r\n"), (flags & EKernelConfigPlatSecEnforcement) ? &KOn : &KOff);
	Printf(_L("PlatSec Locked: %S\r\n"), (flags & EKernelConfigPlatSecLocked) ? &KOn : &KOff);
	Printf(_L("Crazy scheduling: %S\r\n"), (flags & EKernelConfigCrazyScheduling) ? &KOn : &KOff);

	Printf(_L("Code paging: "));
	PagingFormat((flags & EKernelConfigCodePagingPolicyMask) >> EKernelConfigCodePagingPolicyShift);
	Printf(_L("\r\nData paging: "));
	PagingFormat((flags & EKernelConfigDataPagingPolicyMask) >> EKernelConfigDataPagingPolicyShift);
	Printf(_L("\r\n"));

	// Num CPUs
	TBool smpEnabled = (UserSvr::HalFunction(EHalGroupKernel, EKernelHalSmpSupported, 0, 0) == KErrNone);
	if (smpEnabled)
		{
		TInt cpus = UserSvr::HalFunction(EHalGroupKernel, EKernelHalNumLogicalCpus, 0, 0);
		Printf(_L("Num CPUs: %d\r\n"), cpus);
		}
	else
		{
		Printf(_L("Num CPUs: 1 (no SMP)\r\n"));
		}
	}

void CCmdSysInfo::PagingFormat(TUint aFlags)
	{
	enum TMoreFlagsNotIn91
		{
		EKernelConfigPagingPolicyNoPaging = 0,
		EKernelConfigPagingPolicyAlwaysPage = 1,
		EKernelConfigPagingPolicyDefaultUnpaged = 2,
		EKernelConfigPagingPolicyDefaultPaged = 3,
		};

	switch (aFlags)
		{
		case EKernelConfigPagingPolicyNoPaging:
			Printf(_L("No paging")); break;
		case EKernelConfigPagingPolicyAlwaysPage:
			Printf(_L("Always page")); break;
		case EKernelConfigPagingPolicyDefaultUnpaged:
			Printf(_L("Default unpaged")); break;
		case EKernelConfigPagingPolicyDefaultPaged:
			Printf(_L("Default paged")); break;
		default:
			break;
		}
	}

typedef TInt(*TSysUtilGetSWVersionFunction)(TDes&);
#ifdef __EPOC32__
const TInt KSysUtilOrdinal = 9;
#else
const TInt KSysUtilOrdinal = 6;
#endif

void CCmdSysInfo::PrintRomVersion()
	{
#ifdef __EPOC32__
	// Get checksum 
	TRomHeader* romHeader = (TRomHeader*)UserSvr::RomHeaderAddress();
	if  (romHeader)
		{
		Printf(_L("Rom checksum: 0x%08x\r\n"), romHeader->iCheckSum);
		}
#endif

	// Platform version
	_LIT(KS60VersionDllName, "SysUtil.dll");
	RLibrary lib;
	if (lib.Load(KS60VersionDllName) == KErrNone)
		{
		// Get exported version function
		TLibraryFunction fn = lib.Lookup(KSysUtilOrdinal);
		if (fn != NULL)
			{
			TSysUtilGetSWVersionFunction sysUtilGetSWVersion = (TSysUtilGetSWVersionFunction) fn;
			TBuf<128> buf;
			TInt err = (*sysUtilGetSWVersion)(buf);
			if (err)
				{
				Printf(_L("Error returned from SysUtilGetSWVersionFunction: %d\r\n"), err);
				}
			else
				{
				LtkUtils::ReplaceText(buf, _L("\n"), _L(" "));
				Printf(_L("ROM version: %S\r\n"), &buf);
				}
			}
		lib.Close();
		}
	}
