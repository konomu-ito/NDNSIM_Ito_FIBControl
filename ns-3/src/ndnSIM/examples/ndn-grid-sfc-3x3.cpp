/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2011-2015  Regents of the University of California.
 *
 * This file is part of ndnSIM. See AUTHORS for complete list of ndnSIM authors and
 * contributors.
 *
 * ndnSIM is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * ndnSIM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ndnSIM, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 **/

// ndn-grid-topo-plugin.cpp

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"

namespace ns3 {

/**
 * This scenario simulates a grid topology (using topology reader module)
 *
 * (consumer) -- ( ) ----- ( )
 *     |          |         |
 *    ( ) ------ ( ) ----- ( )
 *     |          |         |
 *    ( ) ------ ( ) -- (producer)
 *
 * All links are 1Mbps with propagation 10ms delay.
 *
 * FIB is populated using NdnGlobalRoutingHelper.
 *
 * Consumer requests data from producer with frequency 10 interests per second
 * (interests contain constantly increasing sequence number).
 *
 * For every received interest, producer replies with a data packet, containing
 * 1024 bytes of virtual payload.
 *
 * To run scenario and see what is happening, use the following command:
 *
 *     NS_LOG=ndn.Consumer:ndn.Producer ./waf --run=ndn-grid-topo-plugin
 */

int
main(int argc, char* argv[])
{
  CommandLine cmd;
  cmd.Parse(argc, argv);

  AnnotatedTopologyReader topologyReader("", 25);
  topologyReader.SetFileName("src/ndnSIM/examples/topologies/topo-grid-sfc-3x3.txt");
  topologyReader.Read();

  // Install NDN stack on all nodes
  ndn::StackHelper ndnHelper;
  ndnHelper.InstallAll();

  // Set BestRoute strategy
  ndn::StrategyChoiceHelper::InstallAll("/prefix", "/localhost/nfd/strategy/best-route/%FD%01");

  // Installing global routing interface on all nodes
  ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
  ndnGlobalRoutingHelper.InstallAll();


  //FIB
  //Consumer
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer"), "/F1a", Names::Find<Node>("F1a"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer"), "/F2a", Names::Find<Node>("F1a"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer"), "/F3a", Names::Find<Node>("F1a"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer"), "/F1b", Names::Find<Node>("F1a"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer"), "/F2b", Names::Find<Node>("F1a"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer"), "/F3b", Names::Find<Node>("F1a"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer"), "/F1c", Names::Find<Node>("F1a"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer"), "/F2c", Names::Find<Node>("F1a"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer"), "/F3c", Names::Find<Node>("F1a"), 0);

  //F1a
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1a"), "/F2a", Names::Find<Node>("F2a"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1a"), "/F3a", Names::Find<Node>("F2a"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1a"), "/F2b", Names::Find<Node>("F2b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1a"), "/F3c", Names::Find<Node>("F2b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1a"), "/F3b", Names::Find<Node>("F2a"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1a"), "/F1b", Names::Find<Node>("F2a"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1a"), "/F2c", Names::Find<Node>("F2b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1a"), "/F3c", Names::Find<Node>("F2b"), 0);

  //F2a
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2a"), "/F1a", Names::Find<Node>("F1a"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2a"), "/F3a", Names::Find<Node>("F3a"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2a"), "/F2b", Names::Find<Node>("F3b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2a"), "/F1c", Names::Find<Node>("F3a"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2a"), "/F3b", Names::Find<Node>("F3b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2a"), "/F1b", Names::Find<Node>("F3a"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2a"), "/F2c", Names::Find<Node>("F3b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2a"), "/F3c", Names::Find<Node>("F3b"), 0);

  //F3a
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3a"), "/F1a", Names::Find<Node>("F2a"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3a"), "/F2a", Names::Find<Node>("F2a"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3a"), "/F2b", Names::Find<Node>("F1b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3a"), "/F1c", Names::Find<Node>("F1b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3a"), "/F3b", Names::Find<Node>("F1b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3a"), "/F1b", Names::Find<Node>("F1b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3a"), "/F2c", Names::Find<Node>("F1b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3a"), "/F3c", Names::Find<Node>("F1b"), 0);

  //F2b
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2b"), "/F1a", Names::Find<Node>("F1a"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2b"), "/F3a", Names::Find<Node>("F3b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2b"), "/F2a", Names::Find<Node>("F3b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2b"), "/F3c", Names::Find<Node>("F3c"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2b"), "/F3b", Names::Find<Node>("F3b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2b"), "/F1b", Names::Find<Node>("F3b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2b"), "/F2c", Names::Find<Node>("F3c"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2b"), "/F3c", Names::Find<Node>("F3c"), 0);

  //F3b
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3b"), "/F1a", Names::Find<Node>("F2b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3b"), "/F2a", Names::Find<Node>("F2a"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3b"), "/F3a", Names::Find<Node>("F1b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3b"), "/F2b", Names::Find<Node>("F2b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3b"), "/F1b", Names::Find<Node>("F1b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3b"), "/F3c", Names::Find<Node>("F2b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3b"), "/F2c", Names::Find<Node>("F2c"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3b"), "/F1c", Names::Find<Node>("F2c"), 0);

  //F1b
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1b"), "/F1a", Names::Find<Node>("F3b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1b"), "/F2a", Names::Find<Node>("F3b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1b"), "/F3a", Names::Find<Node>("F3a"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1b"), "/F2b", Names::Find<Node>("F3b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1b"), "/F3b", Names::Find<Node>("F3b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1b"), "/F3c", Names::Find<Node>("F1c"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1b"), "/F2c", Names::Find<Node>("F1c"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1b"), "/F1c", Names::Find<Node>("F1c"), 0);

  //F3c
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3c"), "/F1a", Names::Find<Node>("F2b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3c"), "/F2a", Names::Find<Node>("F2b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3c"), "/F3a", Names::Find<Node>("F2b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3c"), "/F2b", Names::Find<Node>("F2b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3c"), "/F3b", Names::Find<Node>("F2b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3c"), "/F1b", Names::Find<Node>("F2c"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3c"), "/F2c", Names::Find<Node>("F2c"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3c"), "/F1c", Names::Find<Node>("F2c"), 0);

  //F2c
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2c"), "/F1a", Names::Find<Node>("F3b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2c"), "/F2a", Names::Find<Node>("F3b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2c"), "/F3a", Names::Find<Node>("F3b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2c"), "/F2b", Names::Find<Node>("F3b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2c"), "/F3b", Names::Find<Node>("F3b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2c"), "/F1b", Names::Find<Node>("F3b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2c"), "/F3c", Names::Find<Node>("F3c"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2c"), "/F1c", Names::Find<Node>("F3c"), 0);

  //F1c
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1c"), "/F1a", Names::Find<Node>("F1b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1c"), "/F2a", Names::Find<Node>("F1b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1c"), "/F3a", Names::Find<Node>("F1b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1c"), "/F2b", Names::Find<Node>("F1b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1c"), "/F3b", Names::Find<Node>("F1b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1c"), "/F1b", Names::Find<Node>("F1b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1c"), "/F2c", Names::Find<Node>("F2c"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1c"), "/F3c", Names::Find<Node>("F2c"), 0);

  // Getting containers for the consumer/producer
  Ptr<Node> producer = Names::Find<Node>("Producer");
  NodeContainer consumerNodes;
  consumerNodes.Add(Names::Find<Node>("Consumer"));

  // Install NDN applications
  std::string prefix = "/prefix";

  ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
  consumerHelper.SetPrefix(prefix);
  consumerHelper.SetAttribute("Frequency", StringValue("1")); // 100 interests a second
  consumerHelper.Install(consumerNodes);

  ndn::AppHelper producerHelper("ns3::ndn::Producer");
  producerHelper.SetPrefix(prefix);
  producerHelper.SetAttribute("PayloadSize", StringValue("1024"));
  producerHelper.Install(producer);

  // Add /prefix origins to ndn::GlobalRouter
  ndnGlobalRoutingHelper.AddOrigins(prefix, producer);

  // Calculate and install FIBs
  ndn::GlobalRoutingHelper::CalculateRoutes();

  Simulator::Stop(Seconds(20.0));

  Simulator::Run();
  Simulator::Destroy();

  return 0;
}

} // namespace ns3

int
main(int argc, char* argv[])
{
  return ns3::main(argc, argv);
}