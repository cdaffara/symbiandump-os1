/*
 * rawip_const.h
 *
 *  Created on: Aug 20, 2010
 *      Author: traynsfo
 */

#ifndef RAWIP_CONST_H_
#define RAWIP_CONST_H_

#ifdef __EABI__
namespace RawIPConst {
    // Patch data is used and KMtuIPv4 and KRMtuIPv4 can be modified to a different value in RawIpNif.iby file
    extern const TInt KMtuIPv4;
    extern const TInt KRMtuIPv4;
    
    // Patch data is used and KMtuIPv6 and KRMtuIPv6 can be modified to a different value in RawIpNif.iby file
    extern const TInt KMtuIPv6;
    extern const TInt KRMtuIPv6;
    
    extern const TUint KDefaultSendQueueSize;
    
    // Patch data is used and KMaxTxIPPacketSize and KMaxRxIPPacketSize can be modified to a different value in RawIpNif.iby file
    extern const TInt KMaxSendQueueLen;
    extern const TInt KMaxTxIPPacketSize;
    extern const TInt KMaxRxIPPacketSize;
};

#endif

#endif /* RAWIP_CONST_H_ */
