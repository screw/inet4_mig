#ifndef __ANSA_OSPFV3PROCESS_H_
#define __ANSA_OSPFV3PROCESS_H_

#include <omnetpp.h>
#include <string>
#include "inet/common/INETDefs.h"
#include "inet/networklayer/contract/ipv4/Ipv4Address.h"
//#include "inet/networklayer/contract/ipv6/IPv6ControlInfo.h"
#include "inet/common/ModuleAccess.h"
#include "inet/networklayer/contract/IInterfaceTable.h"
#include "inet/networklayer/ipv4/Ipv4InterfaceData.h"
#include "inet/networklayer/ipv4/IIpv4RoutingTable.h"

#include "inet/networklayer/ipv6/Ipv6InterfaceData.h"
#include "inet/networklayer/ipv6/Ipv6RoutingTable.h"
#include "inet/networklayer/ipv6/Ipv6Route.h"

#include "inet/ansa/ospfv3/process/OSPFv3Instance.h"
#include "inet/ansa/ospfv3/process/OSPFv3Area.h"
#include "inet/ansa/ospfv3/OSPFv3Timers.h"
#include "inet/ansa/ospfv3/process/OSPFv3RoutingTableEntry.h"
#include "inet/ansa/ospfv3/OSPFv3Common.h"
#include "inet/ansa/ospfv3/OSPFv3Packet_m.h"


#include "inet/networklayer/contract/ipv6/Ipv6Address.h"
#include "inet/networklayer/ipv6/Ipv6RoutingTable.h"


namespace inet{

class OSPFv3Instance;

class INET_API OSPFv3Process : protected cListener, public cSimpleModule
{
  public:
    OSPFv3Process();
    virtual ~OSPFv3Process();
    int getProcessID() const {return this->processID;};
    Ipv4Address getRouterID(){return this->routerID;};
    bool isActivated(){return this->isActive;};
    void activateProcess();
    void setTimer(cMessage* msg, double delay);
    void clearTimer(cMessage* msg){this->cancelEvent(msg);}
    OSPFv3Instance* getInstanceById(int instanceId);
    void addInstance(OSPFv3Instance* newInstance);
    void sendPacket(OSPFv3Packet *packet, Ipv6Address destination, const char* ifName, short hopLimit = 1);
//    OSPFv3LSA* findLSA(LSAKeyType lsaKey, Ipv4Address areaID, int instanceID);
//    bool floodLSA(OSPFv3LSA* lsa, Ipv4Address areaID=Ipv4Address::UNSPECIFIED_ADDRESS, OSPFv3Interface* intf=nullptr, OSPFv3Neighbor* neighbor=nullptr);
//    bool installLSA(OSPFv3LSA *lsa, int instanceID, Ipv4Address areaID=Ipv4Address::UNSPECIFIED_ADDRESS, OSPFv3Interface* intf=nullptr);
//    void rebuildRoutingTable();
//    void calculateASExternalRoutes(std::vector<OSPFv3RoutingTableEntry* > newTableIPv6, std::vector<OSPFv3IPv4RoutingTableEntry* > newTableIPv4);

//    void ageDatabase();
//    cMessage* getAgeTimer(){return this->ageTimer;}

    /**
     * Returns true if one of the Router's Areas the same address range configured as the
     * input address , false otherwise.
     * @param addressRange [in] The IPv6 address  to look for.
     */
//    bool hasAddressRange(const Ipv6AddressRange& addressRange) const;
//    bool hasAddressRange(const Ipv4AddressRange& addressRange) const;

  public:
    IInterfaceTable* ift = nullptr;
    Ipv6RoutingTable *rt = nullptr;
    IIpv4RoutingTable *rt4 = nullptr;
    cModule* containingModule=nullptr;

  private:
    void handleTimer(cMessage* msg);

  private:
    std::vector<OSPFv3Instance*> instances;
    std::map<int, OSPFv3Instance*> instancesById;
    int processID;
    Ipv4Address routerID;
    bool isActive=false;
    void debugDump();
    std::vector<OSPFv3RoutingTableEntry *> routingTableIPv6;
    std::vector<OSPFv3IPv4RoutingTableEntry *> routingTableIPv4;
    cMessage *ageTimer;

  protected:
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage* msg) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    void parseConfig(cXMLElement* areaConfig);


    //backbone area structure
    //virtual links
    //list of external routes
    //list of as-external routes
    //routing table
};
}//namespace inet

#endif
