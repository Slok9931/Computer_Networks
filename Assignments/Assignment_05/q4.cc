/*
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("AssignmentExample");

int
main(int argc, char* argv[])
{
    CommandLine cmd(__FILE__);
    cmd.Parse(argc, argv);

    Time::SetResolution(Time::NS);

    LogComponentEnable("UdpClient", LOG_LEVEL_INFO);
    LogComponentEnable("UdpServer", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

    // Create Nodes
    NodeContainer nodes;
    nodes.Create(2);

    // Point to Point Link
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

    NetDeviceContainer devices = pointToPoint.Install(nodes);

    // Enable PCAP tracing
    pointToPoint.EnablePcapAll("q5");

    // Install Internet Stack
    InternetStackHelper stack;
    stack.Install(nodes);

    // Assign IP Address
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = address.Assign(devices);

    /*
     * FIRST APPLICATION (UDP CLIENT-SERVER)
     */
    uint16_t port1 = 4000;

    UdpServerHelper server1(port1);
    ApplicationContainer serverApps1 = server1.Install(nodes.Get(1));
    serverApps1.Start(Seconds(1.0));
    serverApps1.Stop(Seconds(30.0));

    UdpClientHelper client1(interfaces.GetAddress(1), port1);
    client1.SetAttribute("MaxPackets", UintegerValue(0)); // Unlimited
    client1.SetAttribute("Interval", TimeValue(MilliSeconds(10)));
    client1.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApps1 = client1.Install(nodes.Get(0));
    clientApps1.Start(Seconds(2.0));
    clientApps1.Stop(Seconds(30.0));

    /*
     * SECOND APPLICATION (UDP ECHO CLIENT-SERVER)
     */
    uint16_t port2 = 5000;

    UdpEchoServerHelper echoServer(port2);
    ApplicationContainer serverApps2 = echoServer.Install(nodes.Get(1));
    serverApps2.Start(Seconds(1.0));
    serverApps2.Stop(Seconds(30.0));

    UdpEchoClientHelper echoClient(interfaces.GetAddress(1), port2);
    echoClient.SetAttribute("MaxPackets", UintegerValue(10));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(512));

    ApplicationContainer clientApps2 = echoClient.Install(nodes.Get(0));
    clientApps2.Start(Seconds(3.0));
    clientApps2.Stop(Seconds(30.0));

    /*
     * NetAnim XML
     */
    AnimationInterface anim("topology.xml");

    Simulator::Stop(Seconds(30.0));
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}
