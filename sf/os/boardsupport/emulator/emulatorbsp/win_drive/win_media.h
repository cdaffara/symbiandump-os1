// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Definitions of the media abstractions. 
// 
//

/**
 @file
*/

#ifndef WIN_MEDIA_H
#define WIN_MEDIA_H

#include "common.h"
#include "settings_manager.h"

class CWinMediaDeviceBase;

//-----------------------------------------------------------------------------

/** 
    Media abstraction base class.
    Can be specialised to emulate different types of media (e.g. NAND, NOR FLASH),
    access delays and other features.

    This class talks to the real windows media object via CWinMediaDeviceBase interface.
    Actually, it's a shim between EPOC's CBaseExtProxyDrive and Windows device (file or volume).
*/
class CMediaBase
{
  public:    
    CMediaBase(TInt aEpocDrvNumber);
    virtual ~CMediaBase();

    //-- interface to the owner
    virtual TInt Connect() =0;
    virtual void Disconnect(); 

    virtual TInt Read(TInt64 aPos,TInt aLength, TDes8& aDataDes)=0;
    virtual TInt Write(TInt64 aPos,TInt aLength, const TDesC8& aDataDes)=0;
    virtual TInt GetEpocCaps(TLocalDriveCaps& aCaps);
    
    virtual TInt Format(TFormatInfo& anInfo) = 0;
    virtual TInt Format(TInt64 aPos,TInt aLength) = 0;
    //--

  protected:
    CMediaBase();
    CMediaBase(const CMediaBase&);
    CMediaBase& operator=(const CMediaBase&);

    inline TBool IsReadOnly() const {return iReadOnly;}
    inline TSettingsManager& Settings() const {ASSERT(ipSettingsManager); return *ipSettingsManager;}

  protected:

    TBool                iReadOnly:1;       ///< if not 0, the media is read-only.
    TDriveNumber         iEpocDrvNumber;    ///< EPOC drive number, specifies the drive a:..z:
    CWinMediaDeviceBase* ipDevice;          ///< pointer to the interface to the real Windows device
    TSettingsManager*    ipSettingsManager; ///< interface to the settings in 'ini' file
};


//-----------------------------------------------------------------------------


/**
    Generic block media. 
    This is the simple media that consists of the blocks or sectors. 
    Nothing special about reading/writing data onto it: what you write is what you read.
    No addidtional seek/read/write delays emulation.

*/
class CGenericBlockMedia: public CMediaBase
{
  public:    
    CGenericBlockMedia(TInt aEpocDrvNumber);
   ~CGenericBlockMedia();


    //-- overrides from the base class
    virtual TInt Connect();
    virtual void Disconnect();

    virtual TInt Read(TInt64 aPos,TInt aLength, TDes8& aDataDes);
    virtual TInt Write(TInt64 aPos, TInt aLength, const TDesC8& aDataDes);

    virtual TInt Format(TFormatInfo& anInfo);
    virtual TInt Format(TInt64 aPos,TInt aLength);

  private:
    TUint32 iSectorsFormatted; ///< used for media formatting
};






#endif //WIN_MEDIA_H













