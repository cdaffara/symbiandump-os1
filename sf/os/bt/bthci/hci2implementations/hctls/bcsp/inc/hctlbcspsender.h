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

/**
 @file
 @internalComponent
*/


#ifndef HCTLBCSPSENDER_H
#define HCTLBCSPSENDER_H

#include <e32base.h>

class CHCTLBcspSequencer;
class RBusDevComm;

NONSHARABLE_CLASS(CHCTLBcspSender) : public CActive
/**
CHCTLBcspSender
Implements BCSP specific implementation of UART CHCTLUartSender Active Object
  	
This class is not intended for derivation.
*/
    {
public:
    static CHCTLBcspSender* NewL(CHCTLBcspSequencer& aSequencer, RBusDevComm& aPort);
    ~CHCTLBcspSender();         
    TInt Write(const TDesC8& aData);

private:    
	// from CActive
    virtual void RunL();     
    virtual void DoCancel();
    
    CHCTLBcspSender(CHCTLBcspSequencer& aSequencer, RBusDevComm& aPort);

private: 
	// unowned
	CHCTLBcspSequencer&	iSequencer;
	RBusDevComm& iPort;
    };
#endif //HCTLBCSPSENDER_H

