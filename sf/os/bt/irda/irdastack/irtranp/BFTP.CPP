// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32svr.h>
#include <e32cons.h>
#include <e32base.h>
#include "debug.h"
#include "GLOBAL.H"
#include "comreadwrite.h"
#include "SCEP.H"
#include "tranp.h"
#include "BFTP.H"
#include "tranpprot.h"

BFTP::BFTP() : iPicture(NULL,0), iUPFPicture(NULL,0)
	{
	}

 BFTP* BFTP::NewL(CTranpProtocol* aTranpProtocol, const MTranpNotification* aNotifier)
	{
	DEBUG_OPEN();
	BFTPPRINT(_L("BFTP::NewL()\n"));
	BFTP* self = new(ELeave) BFTP();
	CleanupStack::PushL(self);
	self->ConstructL(aTranpProtocol, aNotifier);
	CleanupStack::Pop();
	return self;
	}

 void BFTP::ConstructL(CTranpProtocol* aTranpProtocol, const MTranpNotification* aNotifier)
	{
	BFTPPRINT(_L("BFTP::ConstructL()\n"));
	iTranpProtocol = aTranpProtocol;
	m_oSCEP = SCEP::NewL(aTranpProtocol,this,aNotifier);
	}

 BFTP::~BFTP()
	{
	delete m_oSCEP;
	}

/*
* Method description:	Called from SCEP with a buffer containing a Request PDU
*
* Parameter:			a_Header - TPtr pointing the command buffer
* Parameter:			a_offset - the offset into the buffer where we should start parsing
*/

void BFTP::ReqPDU(const TDesC8& /*a_Header*/, TInt /*a_offset*/)
/** No Longer used.

irda client libraries left in to avoid breaks */
	{
	}

/*
* Method description:	Called from the application with a picture to send
*
* Parameter:			a_picture - TPtr pointing to the picture
* Parameter:			a_fileName - filename of the picture
*/

void BFTP::Put(TPtr8* /*a_picture*/, TPtr /*a_fileName*/)
	{
	// Not fully implemented as of now
	}

/*
* Method description:	Called from the application, returns a TPtr with length>0 when picture is ready.
*						Use Pump() (returnvalue = progress from 0 to 100) to run the module!
*
* Returns:				Pointer to a TPtr pointing to the picture
*/

// TPtr8* BFTP::Get()
//	{
//	return m_oPicture;	// Null if no picture has been received yet
//	}

	/*
	* Method description:	Called from the application, returns a pointer to the filename
	*
	* Returns:				Pointer to the filename
*/

// TPtr8 BFTP::GetFileName()
void BFTP::GetFileName()
	{
	// Deprecated Function - No Longer used
	return ;//m_FileName.Des();
	}

/*
* Method description:	Runs the module, call at intervals like 200 ms
*
* Returns:				Integer containing the progress from 0 to 100
*/

TInt BFTP::Pump(TLatticeSize aLatticeSize)
	{
	iLatticeSize = aLatticeSize;
	return m_oSCEP->Pump();
	}

void BFTP::Abort()
	{
	m_oSCEP->Abort();
	}

/*
* Method description:	Passing through initialization data to SCEP
*
* Parameter:			a_oPMID - Primary Machine ID
*/

void BFTP::SetPMID(const TDesC8& aPMID)
	{
	m_oSCEP->SetPMID(aPMID);
	}

/*
* Method description:	Passing through initialization data to SCEP
*
* Parameter:			a_oSMID - Secondary Machine ID
*/

void BFTP::SetSMID(const TDesC8& aSMID)
	{
	m_oSCEP->SetSMID(aSMID);
	}

/*
* Method description:	Passing through initialization data to SCEP
*
* Parameter:			a_iPDU - Maximum receivable PDU size
*/

void BFTP::SetPDU(TUint8 a_iPDU)
	{
	m_oSCEP->SetPDU(a_iPDU);
	}

