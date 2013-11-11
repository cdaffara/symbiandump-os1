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
//

#include <bluetooth/hci/hcitypes.h>
#include <bluetooth/hci/hciconsts.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COREHCI_BASE);
#endif

// THCIConnectionHandle

EXPORT_C THCIConnectionHandle::THCIConnectionHandle()
	: iConnHandle(KInvalidConnectionHandle)
	{
    }

EXPORT_C THCIConnectionHandle::THCIConnectionHandle(THCIConnHandle aConnHandle)
	: iConnHandle(aConnHandle)
	{
    }

EXPORT_C THCIConnectionHandle& THCIConnectionHandle::operator=(THCIConnHandle aConnHandle)
	{
	iConnHandle = aConnHandle;
	return *this;
	}

EXPORT_C THCIConnHandle THCIConnectionHandle::ConnHandle() const
	{
	return iConnHandle;
	}

EXPORT_C THCIConnectionHandle::operator THCIConnHandle() const
	{
	return iConnHandle;
	}

// THCINumOfCompletedPackets

EXPORT_C THCINumOfCompletedPackets::THCINumOfCompletedPackets()
	: iNumOfCompletedPackets(0)
	{
	}

EXPORT_C THCINumOfCompletedPackets::THCINumOfCompletedPackets(TUint16 aNumOfCompletedPackets)
	: iNumOfCompletedPackets(aNumOfCompletedPackets)
	{
	}

EXPORT_C THCINumOfCompletedPackets& THCINumOfCompletedPackets::operator=(TUint16 aNumOfCompletedPackets)
	{
    iNumOfCompletedPackets = aNumOfCompletedPackets;
    return *this;
	}

EXPORT_C TUint16 THCINumOfCompletedPackets::NumOfCompletedPackets() const
	{
	return iNumOfCompletedPackets;
	}

EXPORT_C THCINumOfCompletedPackets::operator TUint16() const
	{
	return iNumOfCompletedPackets;
	}

// THCIClockOffset

EXPORT_C THCIClockOffset::THCIClockOffset()
	: iClockOffset(0)
	{
	}

EXPORT_C THCIClockOffset::THCIClockOffset(TUint16 aClockOffset)
	: iClockOffset(aClockOffset)
	{
	}

EXPORT_C THCIClockOffset& THCIClockOffset::operator=(TUint16 aClockOffset)
	{
    iClockOffset = aClockOffset;
    return *this;
	}

EXPORT_C TUint16 THCIClockOffset::ClockOffset() const
	{
	return iClockOffset;
	}

EXPORT_C THCIClockOffset::operator TUint16() const
	{
	return iClockOffset;
	}

// THCIPageScanRepetitionMode

EXPORT_C THCIPageScanRepetitionMode::THCIPageScanRepetitionMode()
	: iPageScanRepetitionMode(0)
	{
	}

EXPORT_C THCIPageScanRepetitionMode::THCIPageScanRepetitionMode(TUint8 aPageScanRepetitionMode)
	: iPageScanRepetitionMode(aPageScanRepetitionMode)
	{
	}

EXPORT_C THCIPageScanRepetitionMode& THCIPageScanRepetitionMode::operator=(TUint8 aPageScanRepetitionMode)
	{
    iPageScanRepetitionMode = aPageScanRepetitionMode;
    return *this;
	}

EXPORT_C TUint8 THCIPageScanRepetitionMode::PageScanRepetitionMode() const
	{
	return iPageScanRepetitionMode;
	}

EXPORT_C THCIPageScanRepetitionMode::operator TUint8() const
	{
	return iPageScanRepetitionMode;
	}

// THCIReservedOctet

EXPORT_C THCIReservedOctet::THCIReservedOctet()
	: iReservedOctet(0)
	{
	}

EXPORT_C THCIReservedOctet::THCIReservedOctet(TUint8 aReservedOctet)
	: iReservedOctet(aReservedOctet)
	{
	}

EXPORT_C THCIReservedOctet& THCIReservedOctet::operator=(TUint8 aReservedOctet)
	{
    iReservedOctet = aReservedOctet;
    return *this;
	}

EXPORT_C TUint8 THCIReservedOctet::ReservedOctet() const
	{
	return iReservedOctet;
	}

EXPORT_C THCIReservedOctet::operator TUint8() const
	{
	return iReservedOctet;
	}

// THCIRssi

EXPORT_C THCIRssi::THCIRssi()
	: iRssi(0)
	{
	}

EXPORT_C THCIRssi::THCIRssi(TUint8 aRssi)
	: iRssi(static_cast<TInt8>(aRssi))
	{
	}

EXPORT_C THCIRssi& THCIRssi::operator=(TUint8 aRssi)
	{
    iRssi = static_cast<TInt8>(aRssi);
    return *this;
	}

EXPORT_C TUint8 THCIRssi::Rssi() const
	{
	return static_cast<TUint8>(iRssi);
	}

EXPORT_C THCIRssi::operator TUint8() const
	{
	return static_cast<TUint8>(iRssi);
	}

EXPORT_C THCIRssi::THCIRssi(TInt8 aRssi)
	: iRssi(aRssi)
	{
	}

EXPORT_C THCIRssi& THCIRssi::operator=(TInt8 aRssi)
	{
	iRssi = aRssi;
	return *this;
	}

EXPORT_C TInt8 THCIRssi::RSSI() const
	{
	return iRssi;
	}
