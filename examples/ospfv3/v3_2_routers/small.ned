package inet.examples.ospfv3.v3_2_routers;

//import ansa.node.ANSA_Host;
import inet.node.ospfv3.Ospfv3Router;
//import ansa.node.ANSA_Router;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
//import inet.networklayer.configurator.ipv6.FlatNetworkConfigurator6;
import inet.networklayer.configurator.ipv6.Ipv6FlatNetworkConfigurator;
import inet.common.misc.ThruputMeteringChannel;
import inet.node.ethernet.EtherSwitch;


network v3_2_routers
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
                            "<interface among='Router1 Router2' address='10.10.10.x' netmask='255.255.255.0' />"+
                            "</config>");
                @display("p=147,54");
        }
        Router1: Ospfv3Router {
            parameters:
                @display("p=90,154");
            gates:
                ethg[1];
        }
        Router2: Ospfv3Router {
            parameters:
                @display("p=244,97");
            gates:
                ethg[1];
        }

    connections:
        Router1.ethg[0] <--> Channel <--> Router2.ethg[0];
}
