// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <bluetooth/logger.h>
#include "debug.h"
#include "linkutil.h"
#include "bredrcontrollerconfiguration.h"
#include <bt_subscribe.h>
#include <bt_subscribe_partner.h>
#include <bluetooth/hcicommandqueue.h>
#include <bluetooth/hci/readpagescanactivitycommand.h>
#include <bluetooth/hci/readpagescanactivitycompleteevent.h>
#include <bluetooth/hci/readpagescantypecommand.h>
#include <bluetooth/hci/readpagescantypecompleteevent.h>
#include <bluetooth/hci/writepagescanactivitycommand.h>
#include <bluetooth/hci/writepagescantypecommand.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_LINKMGR);
#endif

const TInt KUnitializedParameters	= -1;

const TUint16 KDefaultPageScanInterval	= 0x0800;
const TUint16 KDefaultPageScanWindow	= 0x0012;
const TUint8 KDefaultPageScanType		= 0x00;

const TUint16 KFastConnectionPageScanInterval	= 0x0024;
const TUint16 KFastConnectionPageScanWindow		= 0x0012;
const TUint8 KFastConnectionPageScanType		= 0x01;


CBrEdrControllerConfiguration::CBrEdrControllerConfiguration(MHCICommandQueue& aCommandQueue,
															 CLinkMgrProtocol& aLinkMgrProtocol)
	:	iOriginalPageScanInterval(KDefaultPageScanInterval),
		iOriginalPageScanWindow(KDefaultPageScanWindow),
		iOriginalPageScanType(KDefaultPageScanType),
		iPageScanParameters(KUnitializedParameters),
		iCommandQueue(aCommandQueue),
		iLinkMgrProtocol(aLinkMgrProtocol)
	{
	LOG_FUNC
	}

CBrEdrControllerConfiguration::~CBrEdrControllerConfiguration()
	{
	LOG_FUNC
	}

CBrEdrControllerConfiguration* CBrEdrControllerConfiguration::NewL(MHCICommandQueue& aCommandQueue,
																   CLinkMgrProtocol& aLinkMgrProtocol)
	{
	LOG_STATIC_FUNC
	CBrEdrControllerConfiguration* self
		= new (ELeave) CBrEdrControllerConfiguration(aCommandQueue, aLinkMgrProtocol);
	return self;
	}

void CBrEdrControllerConfiguration::SetPageScanParametersL(TPageScanParameterSettings aPageScanParameters)
	{
	LOG_FUNC
	if (iPageScanParameters == KUnitializedParameters)
		{
		// obtain the original Controller parameters
		LOG(_L("obtaining the original Controller parameters"));
		
		CReadPageScanActivityCommand* readPageScanActCmd = CReadPageScanActivityCommand::NewL();
		// ownership of cmd transfered even if MhcqAddCommandL leaves
		iCommandQueue.MhcqAddCommandL(readPageScanActCmd, *this);

		CReadPageScanTypeCommand* readPageScanTypeCmd = CReadPageScanTypeCommand::NewL();
		// ownership of cmd transfered even if MhcqAddCommandL leaves
		iCommandQueue.MhcqAddCommandL(readPageScanTypeCmd, *this);

		iPageScanParameters = EStandardPageScanParameters;

		// if the original parameters are requested we don't do anything
		if (aPageScanParameters == EStandardPageScanParameters)
			return;
		}

	// store the settings to publish them later
	iPageScanParameters			= EStandardPageScanParameters;

	TUint16 pageScanInterval	= iOriginalPageScanInterval;
	TUint16 pageScanWindow		= iOriginalPageScanWindow;
	TUint8 pageScanType			= iOriginalPageScanType;

	if (aPageScanParameters == EFastConnectionPageScanParameters)
		{
		// store the settings to publish them later
		iPageScanParameters	= EFastConnectionPageScanParameters;
		pageScanInterval	= KFastConnectionPageScanInterval;
		pageScanWindow		= KFastConnectionPageScanWindow;
		pageScanType		= KFastConnectionPageScanType;
		}

	LOG1(_L("Applying page scan parameter settings %d"), iPageScanParameters);

	// apply the requested page scan parameters
	CWritePageScanActivityCommand* writePageScanActCmd
		= CWritePageScanActivityCommand::NewL(pageScanInterval, pageScanWindow);
	// ownership of cmd transfered even if MhcqAddCommandL leaves
	iCommandQueue.MhcqAddCommandL(writePageScanActCmd, *this);

	CWritePageScanTypeCommand* writePageScanTypeCmd = CWritePageScanTypeCommand::NewL(pageScanType);
	// ownership of cmd transfered even if MhcqAddCommandL leaves
	iCommandQueue.MhcqAddCommandL(writePageScanTypeCmd, *this);
	}

void CBrEdrControllerConfiguration::MhcqcCommandEventReceived(const THCIEventBase& aEvent,
															  const CHCICommandBase* aRelatedCommand)
	{
	LOG_FUNC
	LOG3(_L("Event 0x%02x (errorcode=%d) received for Command 0x%04x"), aEvent.EventCode(),
			aEvent.ErrorCode(), aRelatedCommand->Opcode());
	if (aEvent.EventCode() == ECommandCompleteEvent && aEvent.ErrorCode() == KErrNone)
		{
		switch (aRelatedCommand->Opcode())
			{
		case KReadPageScanActivityOpcode:
			{
			const TReadPageScanActivityCompleteEvent& readPageScanActivityCompleteEvent
				= TReadPageScanActivityCompleteEvent::Cast(aEvent);
			iOriginalPageScanInterval	= readPageScanActivityCompleteEvent.PageScanInterval();
			iOriginalPageScanWindow		= readPageScanActivityCompleteEvent.PageScanwindow();
			LOG2(_L("page scan activity parameters received (interval=0x%04x, window=0x%04x)"),
					iOriginalPageScanInterval, iOriginalPageScanWindow);
			break;
			}
		case KReadPageScanTypeOpcode:
			{
			const TReadPageScanTypeCompleteEvent& readPageScanTypeCompleteEvent
				= TReadPageScanTypeCompleteEvent::Cast(aEvent);
			iOriginalPageScanType = readPageScanTypeCompleteEvent.PageScanType();
			LOG1(_L("page scan type received (=0x%02)"), iOriginalPageScanType);
			break;
			}
		case KWritePageScanActivityOpcode:
			// provide an indication that the requested parameters have been enabled
			// as there is no way to indicate halfway success (one command sent succefully, another failed)
			// page scan activity command status is the criteria for the whole operation
			(void) RProperty::Set(KPropertyUidBluetoothCategory,
								  KPropertyKeyBluetoothGetPageScanParameters, iPageScanParameters);
			LOG1(_L("Write Page Scan Activity command confirmed, page scan parameter settings %d applied"),
					iPageScanParameters);
			break;
		case KWritePageScanTypeOpcode:
			LOG(_L("Write Page Scan Type command confirmed"));
			break;
		default:
			LOG1(_L("Warning!! Upexpected Command Complete Event Received (command opcode:0x%04x)"),
					aRelatedCommand->Opcode());
			__ASSERT_DEBUG(EFalse, Panic(EHCIUnexpectedEvent));
			break;
			}
		}
	}

void CBrEdrControllerConfiguration::SetPageScanParameters(TPageScanParameterSettings aPageScanParameters)
	{
	LOG_FUNC
	// errors handled by upper level component
	// not updated in time the KPropertyKeyBluetoothGetPageScanParameters P&K key indicates an error situation
	TRAPD(err, SetPageScanParametersL(aPageScanParameters));
	if (err)
		{
		LOG1(_L("Warning!! SetPageScanParameters left with error %d"), err);
		}
	}

void CBrEdrControllerConfiguration::MhcqcCommandErrored(TInt IF_FLOGGING(aErrorCode),
														const CHCICommandBase* IF_FLOGGING(aCommand))
	{
	LOG_FUNC
	// errors handled by upper level component
	// not updated in time the KPropertyKeyBluetoothGetPageScanParameters P&K key indicates an error situation
	LOG2(_L("Warning!! Command 0x%04x failed with error %d"), aCommand->Opcode(), aErrorCode);
	}

