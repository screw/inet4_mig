//
// Copyright (C) 2006 Andras Babos and Andras Varga
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//


#include "inet/routing/ospfv3/neighbor/OSPFv3NeighborState.h"

#include "inet/routing/ospfv3/neighbor/OSPFv3Neighbor.h"

namespace inet {

void OSPFv3NeighborState::changeState(OSPFv3Neighbor *neighbor, OSPFv3NeighborState *newState, OSPFv3NeighborState *currentState)
{
    OSPFv3Neighbor::OSPFv3NeighborStateType oldState = currentState->getState();
    OSPFv3Neighbor::OSPFv3NeighborStateType nextState = newState->getState();
    bool shouldRebuildRoutingTable = false;

    EV_DEBUG << "Changing neighbor state from " << currentState->getNeighborStateString() << " to " << newState->getNeighborStateString() << "\n";

    neighbor->changeState(newState, currentState);

    if ((oldState == OSPFv3Neighbor::FULL_STATE) || (nextState == OSPFv3Neighbor::FULL_STATE)) {
        Ipv4Address routerID = neighbor->getInterface()->getArea()->getInstance()->getProcess()->getRouterID();
        RouterLSA *routerLSA = neighbor->getInterface()->getArea()->findRouterLSA(routerID);

        if (routerLSA != nullptr) {
            long sequenceNumber = routerLSA->getHeader().getLsaSequenceNumber();
            if (sequenceNumber == MAX_SEQUENCE_NUMBER) {
                routerLSA->getHeaderForUpdate().setLsaAge(MAX_AGE);
                neighbor->getInterface()->getArea()->floodLSA(routerLSA);
                routerLSA->incrementInstallTime();
            }
            else {
                RouterLSA *newLSA = neighbor->getInterface()->getArea()->originateRouterLSA();

                newLSA->getHeaderForUpdate().setLsaSequenceNumber(sequenceNumber + 1);
                shouldRebuildRoutingTable |= neighbor->getInterface()->getArea()->updateRouterLSA(routerLSA, newLSA);
                if (shouldRebuildRoutingTable)
                    neighbor->getInterface()->getArea()->setSpfTreeRoot(routerLSA);
                delete newLSA;

                neighbor->getInterface()->getArea()->floodLSA(routerLSA);
            }
        }

        if (neighbor->getInterface()->getState() == OSPFv3Interface::INTERFACE_STATE_DESIGNATED) {
            NetworkLSA *networkLSA = neighbor->getInterface()->getArea()->findNetworkLSAByLSID(
                    Ipv4Address(neighbor->getInterface()->getInterfaceId()));

            if (networkLSA != nullptr) {
                long sequenceNumber = networkLSA->getHeader().getLsaSequenceNumber();
                if (sequenceNumber == MAX_SEQUENCE_NUMBER) {
                    networkLSA->getHeaderForUpdate().setLsaAge(MAX_AGE);
                    neighbor->getInterface()->getArea()->floodLSA(networkLSA);
                    networkLSA->incrementInstallTime();
                }
                else {
                    NetworkLSA *newLSA = neighbor->getInterface()->getArea()->originateNetworkLSA(neighbor->getInterface());

                    if (newLSA != nullptr) {
                        newLSA->getHeaderForUpdate().setLsaSequenceNumber(sequenceNumber + 1);
                        shouldRebuildRoutingTable |= neighbor->getInterface()->getArea()->updateNetworkLSA(networkLSA, newLSA);
                        delete newLSA;
                    }
                    else {    // no neighbors on the network -> old NetworkLSA must be flushed
                        networkLSA->getHeaderForUpdate().setLsaAge(MAX_AGE);
                        networkLSA->incrementInstallTime();
                    }
                    std::cout << "neighbor router ID = " << neighbor->getInterface()->getArea()->getInstance()->getProcess()->getRouterID() << endl;
                    std::cout << "neighbor NetworkLSA getCount = " << neighbor->getInterface()->getArea()->getNetworkLSACount() << endl;

                    neighbor->getInterface()->getArea()->floodLSA(networkLSA);
                }
            }
        }
    }

    OSPFv3Area* thisArea = neighbor->getInterface()->getArea();
    if (nextState == OSPFv3Neighbor::DOWN_STATE) //this neigbor was shuted down
    {
        // invalidate all LSA type 3, which I know from this neighbor
        // n�jdi mi v�etky intra-arey, ktor� maj� adv Router ID tento neighbor ID
        // v druh�ch are�ch, n�jdi v�etky inter-area , ktor� maj� prefix t�chto najden�ch intra area
        // set MAX_AGE
        if (thisArea->getInstance()->getAreaCount() > 1) // this is ABR
        {
            for(int ar = 0; ar < thisArea->getInstance()->getAreaCount(); ar++)
            {
                OSPFv3Area* area = thisArea->getInstance()->getArea(ar);
                if(area->getAreaID() == thisArea->getAreaID()) //skip my Area
                    continue;

                // in all other Areas invalidate all Inter-Area-Prefix LSAs with same prefix IP as all
                // known Intra-Area-Prefix LSAs from this neighbor
                for (int i = 0; i < thisArea->getIntraAreaPrefixLSACount(); i++)
                {
                    IntraAreaPrefixLSA *iapLSA = thisArea->getIntraAreaPrefixLSA(i);
                    if (neighbor->getNeighborID() == iapLSA->getHeader().getAdvertisingRouter())
                    {
                        for(int k = 0; k <  iapLSA->getPrefixesArraySize(); k++)
                        {
                            // go through all Inter-Area-Prefix LSA of other Area
                            for (int j = 0; j <  area->getInterAreaPrefixLSACount(); j++)
                            {
                                InterAreaPrefixLSA *interLSA = area->getInterAreaPrefixLSA(j);
                                if ((interLSA->getHeader().getAdvertisingRouter() == thisArea->getInstance()->getProcess()->getRouterID()) &&
                                        (interLSA->getPrefix() == iapLSA->getPrefixes(k).addressPrefix) &&
                                        (interLSA->getPrefixLen() == iapLSA->getPrefixes(k).prefixLen))
                                {
                                    interLSA->getHeaderForUpdate().setLsaAge(MAX_AGE);
                                    area->floodLSA(interLSA);
                                }
                            }
                        }
                        // invalidate INTRA LSA too
                        iapLSA->getHeaderForUpdate().setLsaAge(MAX_AGE);
//                      neighbor->getInterface()->getArea()->floodLSA(iapLSA);
                    }
                }
            }
        }
        else // invalidate only INTRA LSA
        {
            for (int i = 0; i < thisArea->getIntraAreaPrefixLSACount(); i++)
            {
                IntraAreaPrefixLSA *iapLSA = thisArea->getIntraAreaPrefixLSA(i);
                if (neighbor->getNeighborID() == iapLSA->getHeader().getAdvertisingRouter())
                {
                    // invalidate INTRA LSA too
                    iapLSA->getHeaderForUpdate().setLsaAge(MAX_AGE);
                    thisArea->floodLSA(iapLSA);
                }
            }
        }
    }

    if (shouldRebuildRoutingTable) {
        neighbor->getInterface()->getArea()->getInstance()->getProcess()->rebuildRoutingTable();
    }
}

} // namespace inet
