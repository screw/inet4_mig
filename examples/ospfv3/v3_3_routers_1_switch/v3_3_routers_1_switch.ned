package inet.examples.ospfv3.v3_3_routers_1_switch;

import inet.node.ospfv3.Ospfv3Router;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.networklayer.configurator.ipv6.Ipv6FlatNetworkConfigurator;
import inet.common.misc.ThruputMeteringChannel;
import inet.node.ethernet.EtherSwitch;
import inet.node.inet.StandardHost;

network v3_3_routers_1_switch
{
    types:
        channel Channel extends ThruputMeteringChannel
        {
            delay = 0.1us;
            datarate = 100Mbps;
            thruputDisplayFormat = "#N";
        }
    submodules:
        configurator: Ipv4NetworkConfigurator {
            parameters:
                config = xml("<config>"+
                            "<interface among='Router1 Router2 Router3 Router4' address='192.168.1.x' netmask='255.255.255.0' />"+
                            "</config>");
                @display("p=211,62");
        }
        Router1: Ospfv3Router {
            parameters:
                @display("p=86,317");
            gates:
                ethg[1];
        }
        Router2: Ospfv3Router {
            parameters:
                @display("p=333,215");
            gates:
                ethg[1];
        }
        etherSwitch: EtherSwitch {
            @display("p=194,230");
        }
        Router3: Ospfv3Router {
            @display("p=75,133");
            gates:
                ethg[1];
        }

    connections:
        Router1.ethg[0] <--> Channel <--> etherSwitch.ethg++;
        Router2.ethg[0] <--> Channel <--> etherSwitch.ethg++;
        Router3.ethg[0] <--> Channel <--> etherSwitch.ethg++;
}
