// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains the header file of the etel functions that have to be adapted later
// 
//

/**
 @file
 *  @publishedAll
*/

#ifndef __GSMUETEL_H__
#define __GSMUETEL_H__

#include <e32base.h>
#include <gsmunmspacemobmsg.h>
#include <gsmuelem.h>


/**
 *  Basic SMS class that contains some of the Etel mulit-mode SMS types.
 *  @publishedAll
 *  @released 
 */
class TGsmSms
	{
public:
	inline TGsmSms();
	inline const TDesC8& Pdu() const;
	inline void SetPdu(const TDesC8& aPdu);
	inline const TGsmSmsTelNumber& Sca() const;
	inline void SetSca(const TGsmSmsTelNumber& aSca);

private:
	NMobileSmsMessaging::TMobileSmsGsmTpdu iPdu;	//< Storage for the PDU
	TGsmSmsTelNumber iSca;							//< Storage for the SCA
	};

/** Responsibility for submit/delivery reports. */
	enum TSmsReportResponsibility
		{
	/** ME has report resposibility. */
		ESmsReportME,
	/** Client has report resposibility. */
		ESmsReportClient
		};
	struct TSmsReportSetting
/**
 *  Flags for report responsibility.
 *  @publishedAll
 */
		{
	/** Responsibility for delivery reports. */
		TSmsReportResponsibility iDeliverReports;
	/** Responsibility for submit reports. */
		TSmsReportResponsibility iSubmitReports;
		};

#include <gsmuetel.inl>

#endif // !defined __GSMUETEL_H__
