// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file sblpextn.h
// Header file for Service Based Local Policy support (SBLP) for QoS API
// @internalTechnology
// @prototype
//


#ifndef __SBLPEXTN_H__
#define __SBLPEXTN_H__

#include <e32std.h>
#include <in_sock.h>
#include <es_mbuf.h>
#include <networking/qos_extension.h>


const TUint KAuthorizationTokenSize	= 255;

typedef TBuf8<KAuthorizationTokenSize> TAuthorizationToken;
class CSblpParameters : public CBase
{
public:
    struct TFlowIdentifier
        {
        TUint16 iMediaComponentNumber;
        TUint16 iIPFlowNumber;
        };

    IMPORT_C CSblpParameters();
    IMPORT_C virtual ~CSblpParameters();
    IMPORT_C TInt GetMAT (TAuthorizationToken & aAuthToken) const;
	IMPORT_C void SetMAT (const TAuthorizationToken & aAuthToken);
    IMPORT_C TFlowIdentifier& GetFlowId(TInt aIndex);
    IMPORT_C TInt GetFlowIds(RArray<TFlowIdentifier>& aFlowIds);
    IMPORT_C void SetFlowIds(const RArray<TFlowIdentifier> &aFlowIds);
    IMPORT_C TInt NrOfFlowIds();
    IMPORT_C CSblpParameters& operator= (const CSblpParameters& f);

protected:
    TAuthorizationToken iAuthorizationToken;
    RArray<TFlowIdentifier>	iFlowIds;
};

class CSblpPolicy : public CExtensionBase
{

public:
	IMPORT_C static CSblpPolicy* NewL();
	IMPORT_C virtual ~CSblpPolicy();
	IMPORT_C TDesC8& Data();
	IMPORT_C TInt ParseMessage(const TDesC8& aData);
	IMPORT_C CExtensionBase* CreateL();
	IMPORT_C TInt Copy(const CExtensionBase& aExtension);
	IMPORT_C void SetSblpParameters(const CSblpParameters& aSblp);
	IMPORT_C void GetSblpParameters(CSblpParameters& aSblp) const;

protected:
	CSblpPolicy();
	void ConstructL();

private:
	CSblpParameters* iSblp;	// SBLP QoS parameter set
};

#endif // __SBLPEXTN_H__
