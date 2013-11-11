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
// WS_ADDR.INL
// Publich access to the WapPorts and the WapAddresses - this file is exported
// 
//

/**
 @file
 
 Gets the WAP port number.
 
 @return WAP port number
 @publishedAll
 @released
 
*/
inline TWapPortNumber TWapAddr::WapPort() const
    {
    return TWapPortNumber(Port());
    }


/**
 *  Sets the wap port number.
 *  
 *  @param aPort WAP port number
 *  @publishedAll
 *  @released
 */
inline void TWapAddr::SetWapPort(TWapPortNumber aPort)
    {
    SetPort(TUint(aPort));
    }


/**
 *  Sets the WAP port address.
 *  
 *  @param aTel WAP address.
 *  @publishedAll
 *  @released
 */
inline void TWapAddr::SetWapAddress(const TDesC8& aTel)
    {
    Mem::Copy(UserPtr(),aTel.Ptr(),aTel.Length()<EMaxWapAddressLength ? aTel.Length() : EMaxWapAddressLength);
    SetUserLen(aTel.Length());
    }


/**
 *  Gets the WAP address.
 *  
 *  @return WAP address
 *  @publishedAll
 *  @released
 */
inline TPtrC8 TWapAddr::WapAddress() const
    {
    return TPtrC8(UserPtr(),const_cast<TWapAddr*>(this)->GetUserLen());
    }


/**
 *  Comparison operator.
 *  
 *  @param aAddr Object to compare.
 *  @return True if the object value's are equal.
 *  @publishedAll
 *  @released
 */
inline TBool TWapAddr::operator==(const TWapAddr& aAddr) const
    {
    if (WapPort()==aAddr.WapPort())
        return ETrue;
    return EFalse;
    }
