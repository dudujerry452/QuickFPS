#ifndef _NETWORKAPI_H_
#define _NETWORKAPI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef void* NetworkHandle;

NetworkHandle NcreateNetworkHandle();
void NdestroyNetworkHandle(NetworkHandle handle);

void Nstart(NetworkHandle handle, char* serverAddress, uint16_t port);
void Nsend(NetworkHandle handle, char* message);

void Nstop(NetworkHandle handle); 


#ifdef __cplusplus
}
#endif

#endif