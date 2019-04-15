package inet.examples.ospfv3.multiple_areas2;

import inet.node.ospfv3.Ospfv3Router;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.networklayer.configurator.ipv6.Ipv6FlatNetworkConfigurator;
import inet.common.misc.ThruputMeteringChannel;
import inet.node.ethernet.EtherSwitch;
import inet.node.inet.StandardHost;


network multiple_areas2
{
    @display("bgb=819.0325,484.4675");
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
                //							"<interface among='Router6 Host1' address='192.168.11.x' netmask='255.255.255.0' />"+
                //							"<interface among='Router7 Host2' address='192.168.12.x' netmask='255.255.255.0' />"+
                //							"<interface among='Router8 Host3' address='192.168.13.x' netmask='255.255.255.0' />"+
                //							"<interface among='Router9 Host4' address='192.168.14.x' netmask='255.255.255.0' />"+
                //							"<interface among='Router10 Host5' address='192.168.15.x' netmask='255.255.255.0' />"+
                config = xml("<config>"+
                            "<interface among='Router1 Router2 Router3 Router4 Router5' address='192.168.1.x' netmask='255.255.255.0' />"+
							"<interface among='Router1 Router6' address='192.168.10.x' netmask='255.255.255.0' />"+
							"<interface among='Router2 Router7' address='192.168.20.x' netmask='255.255.255.0' />"+
							"<interface among='Router3 Router8' address='192.168.30.x' netmask='255.255.255.0' />"+
							"<interface among='Router4 Router9' address='192.168.40.x' netmask='255.255.255.0' />"+
							"<interface among='Router5 Router10' address='192.168.50.x' netmask='255.255.255.0' />"+
//							"<interface among='Router6 Host1' address='192.168.11.x' netmask='255.255.255.0' />"+
//							"<interface among='Router7 Host2' address='192.168.12.x' netmask='255.255.255.0' />"+
//							"<interface among='Router8 Host3' address='192.168.13.x' netmask='255.255.255.0' />"+
//							"<interface among='Router9 Host4' address='192.168.14.x' netmask='255.255.255.0' />"+
//							"<interface among='Router10 Host5' address='192.168.15.x' netmask='255.255.255.0' />"+
                            "</config>");
                @display("p=23,24");
        }
        Router1: Ospfv3Router {
            parameters:
                @display("p=308,316");
            gates:
                ethg[2];
        }
        Router2: Ospfv3Router {
            parameters:
                @display("p=410,316");
            gates:
                ethg[2];
        }
        Router4: Ospfv3Router {
            @display("p=474,187");
            gates:
                ethg[2];
        }
        Router3: Ospfv3Router {
            @display("p=510,316");
            gates:
                ethg[2];
        }
        Router5: Ospfv3Router {
            @display("p=347,183");
            gates:
                ethg[2];
        }
        Router6: Ospfv3Router {
            @display("p=165,316");
            gates:
                ethg[1];
        }
        Router7: Ospfv3Router {
            @display("p=410,419");
            gates:
                ethg[1];
        }
        Router8: Ospfv3Router {
            @display("p=654,316");
            gates:
                ethg[1];
        }
        Router9: Ospfv3Router {
            @display("p=579,101");
            gates:
                ethg[1];
        }
        Router10: Ospfv3Router {
            @display("p=234,101");
            gates:
                ethg[1];
        }

        etherSwitch: EtherSwitch {
            @display("p=410,239");
        }

    connections:
        Router1.ethg[0] <--> Channel <--> etherSwitch.ethg++;
        Router2.ethg[0] <--> Channel <--> etherSwitch.ethg++;
        Router3.ethg[0] <--> Channel <--> etherSwitch.ethg++;
        Router4.ethg[0] <--> Channel <--> etherSwitch.ethg++;
        Router5.ethg[0] <--> Channel <--> etherSwitch.ethg++;

        Router1.ethg[1] <--> Channel <--> Router6.ethg[0];
        Router2.ethg[1] <--> Channel <--> Router7.ethg[0];
        Router3.ethg[1] <--> Channel <--> Router8.ethg[0];
        Router4.ethg[1] <--> Channel <--> Router9.ethg[0];
        Router5.ethg[1] <--> Channel <--> Router10.ethg[0];

}
