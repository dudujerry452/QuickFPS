#ifndef _NETWORKAPI_H_
#define _NETWORKAPI_H_


#include <stdint.h>
#include <vector>

class Network; 

typedef Network* NetworkHandle;

NetworkHandle NcreateNetworkHandle();
void NdestroyNetworkHandle(NetworkHandle handle);

void Nstart(NetworkHandle handle, char* serverAddress, uint16_t port);
void Nsend(NetworkHandle handle, char* message);
void NsendBytes(NetworkHandle handle, std::vector<uint8_t> data); 

void Nstop(NetworkHandle handle);




#endif