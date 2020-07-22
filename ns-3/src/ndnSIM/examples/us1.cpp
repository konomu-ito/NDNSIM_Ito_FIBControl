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
#include "ns3/simulator.h"

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
  //choose siraiwaNDN or roundRobin or duration
  const char* type = "duration";
  setChoiceType(type);
  AnnotatedTopologyReader topologyReader("", 25);
  topologyReader.SetFileName("src/ndnSIM/examples/topologies/usa.txt");
  topologyReader.Read();

  // Install NDN stack on all nodes
  ndn::StackHelper ndnHelper;
  ndnHelper.SetOldContentStore("ns3::ndn::cs::Nocache");
  ndnHelper.InstallAll();

  // Set BestRoute strategy
  ndn::StrategyChoiceHelper::InstallAll("/prefix1", "/localhost/nfd/strategy/best-route/%FD%01");
  ndn::StrategyChoiceHelper::InstallAll("/prefix2", "/localhost/nfd/strategy/best-route/%FD%01");
  ndn::StrategyChoiceHelper::InstallAll("/prefix3", "/localhost/nfd/strategy/best-route/%FD%01");
  ndn::StrategyChoiceHelper::InstallAll("/prefix4", "/localhost/nfd/strategy/best-route/%FD%01");

  // Installing global routing interface on all nodes
  ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
  ndnGlobalRoutingHelper.InstallAll();

  //Consumer1
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer1"), "/F1a", Names::Find<Node>("Node5"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer1"), "/F1b", Names::Find<Node>("Node5"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer1"), "/F1c", Names::Find<Node>("Node5"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer1"), "/F2a", Names::Find<Node>("Node5"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer1"), "/F2b", Names::Find<Node>("Node5"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer1"), "/F2c", Names::Find<Node>("Node5"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer1"), "/F3a", Names::Find<Node>("Node5"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer1"), "/F3b", Names::Find<Node>("Node5"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer1"), "/F3c", Names::Find<Node>("Node5"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer1"), "/F4a", Names::Find<Node>("Node5"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer1"), "/F4b", Names::Find<Node>("Node5"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer1"), "/F4c", Names::Find<Node>("Node5"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer1"), "/F5a", Names::Find<Node>("Node5"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer1"), "/F5b", Names::Find<Node>("Node5"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer1"), "/F5c", Names::Find<Node>("Node5"), 0);

  //Consumer2
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer2"), "/F1a", Names::Find<Node>("Node8"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer2"), "/F1b", Names::Find<Node>("Node8"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer2"), "/F1c", Names::Find<Node>("Node8"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer2"), "/F2a", Names::Find<Node>("Node8"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer2"), "/F2b", Names::Find<Node>("Node8"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer2"), "/F2c", Names::Find<Node>("Node8"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer2"), "/F3a", Names::Find<Node>("Node8"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer2"), "/F3b", Names::Find<Node>("Node8"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer2"), "/F3c", Names::Find<Node>("Node8"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer2"), "/F4a", Names::Find<Node>("Node8"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer2"), "/F4b", Names::Find<Node>("Node8"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer2"), "/F4c", Names::Find<Node>("Node8"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer2"), "/F5a", Names::Find<Node>("Node8"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer2"), "/F5b", Names::Find<Node>("Node8"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer2"), "/F5c", Names::Find<Node>("Node8"), 0);

  //Consumer3
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer3"), "/F1a", Names::Find<Node>("Node24"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer3"), "/F1b", Names::Find<Node>("Node24"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer3"), "/F1c", Names::Find<Node>("Node24"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer3"), "/F2a", Names::Find<Node>("Node24"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer3"), "/F2b", Names::Find<Node>("Node24"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer3"), "/F2c", Names::Find<Node>("Node24"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer3"), "/F3a", Names::Find<Node>("Node24"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer3"), "/F3b", Names::Find<Node>("Node24"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer3"), "/F3c", Names::Find<Node>("Node24"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer3"), "/F4a", Names::Find<Node>("Node24"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer3"), "/F4b", Names::Find<Node>("Node24"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer3"), "/F4c", Names::Find<Node>("Node24"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer3"), "/F5a", Names::Find<Node>("Node24"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer3"), "/F5b", Names::Find<Node>("Node24"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer3"), "/F5c", Names::Find<Node>("Node24"), 0);

  //Consumer4
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer4"), "/F1a", Names::Find<Node>("Node18"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer4"), "/F1b", Names::Find<Node>("Node18"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer4"), "/F1c", Names::Find<Node>("Node18"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer4"), "/F2a", Names::Find<Node>("Node18"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer4"), "/F2b", Names::Find<Node>("Node18"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer4"), "/F2c", Names::Find<Node>("Node18"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer4"), "/F3a", Names::Find<Node>("Node18"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer4"), "/F3b", Names::Find<Node>("Node18"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer4"), "/F3c", Names::Find<Node>("Node18"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer4"), "/F4a", Names::Find<Node>("Node18"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer4"), "/F4b", Names::Find<Node>("Node18"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer4"), "/F4c", Names::Find<Node>("Node18"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer4"), "/F5a", Names::Find<Node>("Node18"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer4"), "/F5b", Names::Find<Node>("Node18"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Consumer4"), "/F5c", Names::Find<Node>("Node18"), 0);

  //Node1
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node1"), "/F1a", Names::Find<Node>("Node2"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node1"), "/F1b", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node1"), "/F1c", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node1"), "/F2a", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node1"), "/F2b", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node1"), "/F2c", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node1"), "/F3a", Names::Find<Node>("Node2"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node1"), "/F3b", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node1"), "/F3c", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node1"), "/F4a", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node1"), "/F4b", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node1"), "/F4c", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node1"), "/F5a", Names::Find<Node>("Node2"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node1"), "/F5b", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node1"), "/F5c", Names::Find<Node>("Node6"), 0);

  //Node2
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node2"), "/F1a", Names::Find<Node>("Node3"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node2"), "/F1b", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node2"), "/F1c", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node2"), "/F2a", Names::Find<Node>("Node3"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node2"), "/F2b", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node2"), "/F2c", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node2"), "/F3a", Names::Find<Node>("Node3"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node2"), "/F3b", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node2"), "/F3c", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node2"), "/F4a", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node2"), "/F4b", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node2"), "/F4c", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node2"), "/F5a", Names::Find<Node>("F5a"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node2"), "/F5b", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node2"), "/F5c", Names::Find<Node>("Node6"), 0);

  //F5a
  ndn::FibHelper::AddRoute(Names::Find<Node>("F5a"), "/F1a", Names::Find<Node>("Node2"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F5a"), "/F1b", Names::Find<Node>("Node2"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F5a"), "/F1c", Names::Find<Node>("Node2"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F5a"), "/F2a", Names::Find<Node>("Node2"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F5a"), "/F2b", Names::Find<Node>("Node2"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F5a"), "/F2c", Names::Find<Node>("Node2"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F5a"), "/F3a", Names::Find<Node>("Node2"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F5a"), "/F3b", Names::Find<Node>("Node2"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F5a"), "/F3c", Names::Find<Node>("Node2"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F5a"), "/F4a", Names::Find<Node>("Node2"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F5a"), "/F4b", Names::Find<Node>("Node2"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F5a"), "/F4c", Names::Find<Node>("Node2"), 0);
  //ndn::FibHelper::AddRoute(Names::Find<Node>("Node1"), "/F5a", Names::Find<Node>("Node"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F5a"), "/F5b", Names::Find<Node>("Node2"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F5a"), "/F5c", Names::Find<Node>("Node2"), 0);

  //Node3
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node3"), "/F1a", Names::Find<Node>("F1a"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node3"), "/F1b", Names::Find<Node>("Node5"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node3"), "/F1c", Names::Find<Node>("Node2"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node3"), "/F2a", Names::Find<Node>("Node7"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node3"), "/F2b", Names::Find<Node>("Node5"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node3"), "/F2c", Names::Find<Node>("Node5"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node3"), "/F3a", Names::Find<Node>("Node4"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node3"), "/F3b", Names::Find<Node>("Node2"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node3"), "/F3c", Names::Find<Node>("Node5"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node3"), "/F4a", Names::Find<Node>("Node2"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node3"), "/F4b", Names::Find<Node>("Node2"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node3"), "/F4c", Names::Find<Node>("Node2"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node3"), "/F5a", Names::Find<Node>("Node2"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node3"), "/F5b", Names::Find<Node>("Node2"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node3"), "/F5c", Names::Find<Node>("Node2"), 0);

  //F1a
  //ndn::FibHelper::AddRoute(Names::Find<Node>("Node2"), "/F1a", Names::Find<Node>("Node"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1a"), "/F1b", Names::Find<Node>("Node3"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1a"), "/F1c", Names::Find<Node>("Node3"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1a"), "/F2a", Names::Find<Node>("Node3"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1a"), "/F2b", Names::Find<Node>("Node3"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1a"), "/F2c", Names::Find<Node>("Node3"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1a"), "/F3a", Names::Find<Node>("Node3"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1a"), "/F3b", Names::Find<Node>("Node3"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1a"), "/F3c", Names::Find<Node>("Node3"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1a"), "/F4a", Names::Find<Node>("Node3"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1a"), "/F4b", Names::Find<Node>("Node3"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1a"), "/F4c", Names::Find<Node>("Node3"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1a"), "/F5a", Names::Find<Node>("Node3"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1a"), "/F5b", Names::Find<Node>("Node3"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1a"), "/F5c", Names::Find<Node>("Node3"), 0);

  //Node4
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node4"), "/F1a", Names::Find<Node>("Node3"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node4"), "/F1b", Names::Find<Node>("Node5"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node4"), "/F1c", Names::Find<Node>("Node7"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node4"), "/F2a", Names::Find<Node>("Node7"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node4"), "/F2b", Names::Find<Node>("Node5"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node4"), "/F2c", Names::Find<Node>("Node5"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node4"), "/F3a", Names::Find<Node>("F3a"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node4"), "/F3b", Names::Find<Node>("Node7"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node4"), "/F3c", Names::Find<Node>("Node5"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node4"), "/F4a", Names::Find<Node>("Node7"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node4"), "/F4b", Names::Find<Node>("Node7"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node4"), "/F4c", Names::Find<Node>("Node7"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node4"), "/F5a", Names::Find<Node>("Node3"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node4"), "/F5b", Names::Find<Node>("Node7"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node4"), "/F5c", Names::Find<Node>("Node7"), 0);

  //F3a
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3a"), "/F1a", Names::Find<Node>("Node4"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3a"), "/F1b", Names::Find<Node>("Node4"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3a"), "/F1c", Names::Find<Node>("Node4"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3a"), "/F2a", Names::Find<Node>("Node4"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3a"), "/F2b", Names::Find<Node>("Node4"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3a"), "/F2c", Names::Find<Node>("Node4"), 0);
  //ndn::FibHelper::AddRoute(Names::Find<Node>("Node1"), "/F3a", Names::Find<Node>("Node"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3a"), "/F3b", Names::Find<Node>("Node4"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3a"), "/F3c", Names::Find<Node>("Node4"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3a"), "/F4a", Names::Find<Node>("Node4"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3a"), "/F4b", Names::Find<Node>("Node4"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3a"), "/F4c", Names::Find<Node>("Node4"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3a"), "/F5a", Names::Find<Node>("Node4"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3a"), "/F5b", Names::Find<Node>("Node4"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3a"), "/F5c", Names::Find<Node>("Node4"), 0);

  //Node5
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node5"), "/F1a", Names::Find<Node>("Node3"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node5"), "/F1b", Names::Find<Node>("Node8"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node5"), "/F1c", Names::Find<Node>("Node8"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node5"), "/F2a", Names::Find<Node>("Node3"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node5"), "/F2b", Names::Find<Node>("Node8"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node5"), "/F2c", Names::Find<Node>("Node8"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node5"), "/F3a", Names::Find<Node>("Node4"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node5"), "/F3b", Names::Find<Node>("Node8"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node5"), "/F3c", Names::Find<Node>("Node8"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node5"), "/F4a", Names::Find<Node>("Node3"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node5"), "/F4b", Names::Find<Node>("Node8"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node5"), "/F4c", Names::Find<Node>("Node3"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node5"), "/F5a", Names::Find<Node>("Node3"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node5"), "/F5b", Names::Find<Node>("Node3"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node5"), "/F5c", Names::Find<Node>("Node3"), 0);

  //Node6
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node6"), "/F1a", Names::Find<Node>("Node2"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node6"), "/F1b", Names::Find<Node>("Node9"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node6"), "/F1c", Names::Find<Node>("Node9"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node6"), "/F2a", Names::Find<Node>("Node7"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node6"), "/F2b", Names::Find<Node>("Node9"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node6"), "/F2c", Names::Find<Node>("Node9"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node6"), "/F3a", Names::Find<Node>("Node7"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node6"), "/F3b", Names::Find<Node>("Node9"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node6"), "/F3c", Names::Find<Node>("Node9"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node6"), "/F4a", Names::Find<Node>("F4a"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node6"), "/F4b", Names::Find<Node>("Node9"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node6"), "/F4c", Names::Find<Node>("Node11"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node6"), "/F5a", Names::Find<Node>("Node2"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node6"), "/F5b", Names::Find<Node>("Node11"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node6"), "/F5c", Names::Find<Node>("Node11"), 0);

  //F4a
  ndn::FibHelper::AddRoute(Names::Find<Node>("F4a"), "/F1a", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F4a"), "/F1b", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F4a"), "/F1c", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F4a"), "/F2a", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F4a"), "/F2b", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F4a"), "/F2c", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F4a"), "/F3a", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F4a"), "/F3b", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F4a"), "/F3c", Names::Find<Node>("Node6"), 0);
  //ndn::FibHelper::AddRoute(Names::Find<Node>("Node1"), "/F4a", Names::Find<Node>("Node"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F4a"), "/F4b", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F4a"), "/F4c", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F4a"), "/F5a", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F4a"), "/F5b", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F4a"), "/F5c", Names::Find<Node>("Node6"), 0);

  //Node7
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node7"), "/F1a", Names::Find<Node>("Node3"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node7"), "/F1b", Names::Find<Node>("Node8"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node7"), "/F1c", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node7"), "/F2a", Names::Find<Node>("F2a"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node7"), "/F2b", Names::Find<Node>("Node8"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node7"), "/F2c", Names::Find<Node>("Node8"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node7"), "/F3a", Names::Find<Node>("Node4"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node7"), "/F3b", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node7"), "/F3c", Names::Find<Node>("Node8"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node7"), "/F4a", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node7"), "/F4b", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node7"), "/F4c", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node7"), "/F5a", Names::Find<Node>("Node3"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node7"), "/F5b", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node7"), "/F5c", Names::Find<Node>("Node6"), 0);

  //F2a
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2a"), "/F1a", Names::Find<Node>("Node7"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2a"), "/F1b", Names::Find<Node>("Node7"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2a"), "/F1c", Names::Find<Node>("Node7"), 0);
  //ndn::FibHelper::AddRoute(Names::Find<Node>("Node1"), "/F2a", Names::Find<Node>("Node"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2a"), "/F2b", Names::Find<Node>("Node7"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2a"), "/F2c", Names::Find<Node>("Node7"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2a"), "/F3a", Names::Find<Node>("Node7"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2a"), "/F3b", Names::Find<Node>("Node7"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2a"), "/F3c", Names::Find<Node>("Node7"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2a"), "/F4a", Names::Find<Node>("Node7"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2a"), "/F4b", Names::Find<Node>("Node7"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2a"), "/F4c", Names::Find<Node>("Node7"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2a"), "/F5a", Names::Find<Node>("Node7"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2a"), "/F5b", Names::Find<Node>("Node7"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2a"), "/F5c", Names::Find<Node>("Node7"), 0);

  //Node8
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node8"), "/F1a", Names::Find<Node>("Node5"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node8"), "/F1b", Names::Find<Node>("Node10"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node8"), "/F1c", Names::Find<Node>("Node10"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node8"), "/F2a", Names::Find<Node>("Node7"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node8"), "/F2b", Names::Find<Node>("Node10"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node8"), "/F2c", Names::Find<Node>("Node10"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node8"), "/F3a", Names::Find<Node>("Node5"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node8"), "/F3b", Names::Find<Node>("Node10"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node8"), "/F3c", Names::Find<Node>("Node10"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node8"), "/F4a", Names::Find<Node>("Node7"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node8"), "/F4b", Names::Find<Node>("Node10"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node8"), "/F4c", Names::Find<Node>("Node7"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node8"), "/F5a", Names::Find<Node>("Node5"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node8"), "/F5b", Names::Find<Node>("Node7"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node8"), "/F5c", Names::Find<Node>("Node7"), 0);

  //Node9
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node9"), "/F1a", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node9"), "/F1b", Names::Find<Node>("Node10"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node9"), "/F1c", Names::Find<Node>("Node12"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node9"), "/F2a", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node9"), "/F2b", Names::Find<Node>("Node10"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node9"), "/F2c", Names::Find<Node>("Node10"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node9"), "/F3a", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node9"), "/F3b", Names::Find<Node>("Node12"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node9"), "/F3c", Names::Find<Node>("Node10"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node9"), "/F4a", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node9"), "/F4b", Names::Find<Node>("F4b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node9"), "/F4c", Names::Find<Node>("Node11"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node9"), "/F5a", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node9"), "/F5b", Names::Find<Node>("Node11"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node9"), "/F5c", Names::Find<Node>("Node11"), 0);

  //F4b
  ndn::FibHelper::AddRoute(Names::Find<Node>("F4b"), "/F1a", Names::Find<Node>("Node9"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F4b"), "/F1b", Names::Find<Node>("Node9"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F4b"), "/F1c", Names::Find<Node>("Node9"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F4b"), "/F2a", Names::Find<Node>("Node9"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F4b"), "/F2b", Names::Find<Node>("Node9"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F4b"), "/F2c", Names::Find<Node>("Node9"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F4b"), "/F3a", Names::Find<Node>("Node9"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F4b"), "/F3b", Names::Find<Node>("Node9"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F4b"), "/F3c", Names::Find<Node>("Node9"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F4b"), "/F4a", Names::Find<Node>("Node9"), 0);
  //ndn::FibHelper::AddRoute(Names::Find<Node>("Node1"), "/F4b", Names::Find<Node>("Node"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F4b"), "/F4c", Names::Find<Node>("Node9"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F4b"), "/F5a", Names::Find<Node>("Node9"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F4b"), "/F5b", Names::Find<Node>("Node9"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F4b"), "/F5c", Names::Find<Node>("Node9"), 0);

  //Node10
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node10"), "/F1a", Names::Find<Node>("Node8"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node10"), "/F1b", Names::Find<Node>("F1b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node10"), "/F1c", Names::Find<Node>("Node9"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node10"), "/F2a", Names::Find<Node>("Node8"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node10"), "/F2b", Names::Find<Node>("Node13"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node10"), "/F2c", Names::Find<Node>("Node13"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node10"), "/F3a", Names::Find<Node>("Node8"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node10"), "/F3b", Names::Find<Node>("Node9"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node10"), "/F3c", Names::Find<Node>("Node13"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node10"), "/F4a", Names::Find<Node>("Node9"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node10"), "/F4b", Names::Find<Node>("Node9"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node10"), "/F4c", Names::Find<Node>("Node9"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node10"), "/F5a", Names::Find<Node>("Node9"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node10"), "/F5b", Names::Find<Node>("Node9"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node10"), "/F5c", Names::Find<Node>("Node9"), 0);

  //F1b
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1b"), "/F1a", Names::Find<Node>("Node10"), 0);
  //ndn::FibHelper::AddRoute(Names::Find<Node>("Node1"), "/F1b", Names::Find<Node>("Node"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1b"), "/F1c", Names::Find<Node>("Node10"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1b"), "/F2a", Names::Find<Node>("Node10"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1b"), "/F2b", Names::Find<Node>("Node10"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1b"), "/F2c", Names::Find<Node>("Node10"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1b"), "/F3a", Names::Find<Node>("Node10"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1b"), "/F3b", Names::Find<Node>("Node10"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1b"), "/F3c", Names::Find<Node>("Node10"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1b"), "/F4a", Names::Find<Node>("Node10"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1b"), "/F4b", Names::Find<Node>("Node10"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1b"), "/F4c", Names::Find<Node>("Node10"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1b"), "/F5a", Names::Find<Node>("Node10"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1b"), "/F5b", Names::Find<Node>("Node10"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1b"), "/F5c", Names::Find<Node>("Node10"), 0);

  //Node11
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node11"), "/F1a", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node11"), "/F1b", Names::Find<Node>("Node9"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node11"), "/F1c", Names::Find<Node>("Node12"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node11"), "/F2a", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node11"), "/F2b", Names::Find<Node>("Node12"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node11"), "/F2c", Names::Find<Node>("Node12"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node11"), "/F3a", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node11"), "/F3b", Names::Find<Node>("Node12"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node11"), "/F3c", Names::Find<Node>("Node12"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node11"), "/F4a", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node11"), "/F4b", Names::Find<Node>("Node9"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node11"), "/F4c", Names::Find<Node>("Node15"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node11"), "/F5a", Names::Find<Node>("Node6"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node11"), "/F5b", Names::Find<Node>("F5b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node11"), "/F5c", Names::Find<Node>("Node15"), 0);

  //F5b
  ndn::FibHelper::AddRoute(Names::Find<Node>("F5b"), "/F1a", Names::Find<Node>("Node11"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F5b"), "/F1b", Names::Find<Node>("Node11"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F5b"), "/F1c", Names::Find<Node>("Node11"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F5b"), "/F2a", Names::Find<Node>("Node11"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F5b"), "/F2b", Names::Find<Node>("Node11"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F5b"), "/F2c", Names::Find<Node>("Node11"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F5b"), "/F3a", Names::Find<Node>("Node11"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F5b"), "/F3b", Names::Find<Node>("Node11"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F5b"), "/F3c", Names::Find<Node>("Node11"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F5b"), "/F4a", Names::Find<Node>("Node11"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F5b"), "/F4b", Names::Find<Node>("Node11"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F5b"), "/F4c", Names::Find<Node>("Node11"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F5b"), "/F5a", Names::Find<Node>("Node11"), 0);
  //ndn::FibHelper::AddRoute(Names::Find<Node>("Node1"), "/F5b", Names::Find<Node>("Node"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F5b"), "/F5c", Names::Find<Node>("Node11"), 0);

  //Node12
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node12"), "/F1a", Names::Find<Node>("Node9"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node12"), "/F1b", Names::Find<Node>("Node9"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node12"), "/F1c", Names::Find<Node>("Node16"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node12"), "/F2a", Names::Find<Node>("Node9"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node12"), "/F2b", Names::Find<Node>("Node13"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node12"), "/F2c", Names::Find<Node>("Node13"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node12"), "/F3a", Names::Find<Node>("Node9"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node12"), "/F3b", Names::Find<Node>("F3b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node12"), "/F3c", Names::Find<Node>("Node13"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node12"), "/F4a", Names::Find<Node>("Node9"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node12"), "/F4b", Names::Find<Node>("Node9"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node12"), "/F4c", Names::Find<Node>("Node11"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node12"), "/F5a", Names::Find<Node>("Node9"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node12"), "/F5b", Names::Find<Node>("Node11"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node12"), "/F5c", Names::Find<Node>("Node11"), 0);

  //F3b
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3b"), "/F1a", Names::Find<Node>("Node12"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3b"), "/F1b", Names::Find<Node>("Node12"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3b"), "/F1c", Names::Find<Node>("Node12"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3b"), "/F2a", Names::Find<Node>("Node12"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3b"), "/F2b", Names::Find<Node>("Node12"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3b"), "/F2c", Names::Find<Node>("Node12"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3b"), "/F3a", Names::Find<Node>("Node12"), 0);
  //ndn::FibHelper::AddRoute(Names::Find<Node>("Node1"), "/F3b", Names::Find<Node>("Node"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3b"), "/F3c", Names::Find<Node>("Node12"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3b"), "/F4a", Names::Find<Node>("Node12"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3b"), "/F4b", Names::Find<Node>("Node12"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3b"), "/F4c", Names::Find<Node>("Node12"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3b"), "/F5a", Names::Find<Node>("Node12"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3b"), "/F5b", Names::Find<Node>("Node12"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3b"), "/F5c", Names::Find<Node>("Node12"), 0);

  //Node13
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node13"), "/F1a", Names::Find<Node>("Node10"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node13"), "/F1b", Names::Find<Node>("Node10"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node13"), "/F1c", Names::Find<Node>("Node12"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node13"), "/F2a", Names::Find<Node>("Node10"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node13"), "/F2b", Names::Find<Node>("F2b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node13"), "/F2c", Names::Find<Node>("Node17"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node13"), "/F3a", Names::Find<Node>("Node10"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node13"), "/F3b", Names::Find<Node>("Node12"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node13"), "/F3c", Names::Find<Node>("Node17"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node13"), "/F4a", Names::Find<Node>("Node10"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node13"), "/F4b", Names::Find<Node>("Node10"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node13"), "/F4c", Names::Find<Node>("Node12"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node13"), "/F5a", Names::Find<Node>("Node10"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node13"), "/F5b", Names::Find<Node>("Node12"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node13"), "/F5c", Names::Find<Node>("Node12"), 0);

  //F2b
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2b"), "/F1a", Names::Find<Node>("Node13"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2b"), "/F1b", Names::Find<Node>("Node13"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2b"), "/F1c", Names::Find<Node>("Node13"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2b"), "/F2a", Names::Find<Node>("Node13"), 0);
  //ndn::FibHelper::AddRoute(Names::Find<Node>("F2b"), "/F2b", Names::Find<Node>("F2b"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2b"), "/F2c", Names::Find<Node>("Node13"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2b"), "/F3a", Names::Find<Node>("Node13"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2b"), "/F3b", Names::Find<Node>("Node13"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2b"), "/F3c", Names::Find<Node>("Node13"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2b"), "/F4a", Names::Find<Node>("Node13"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2b"), "/F4b", Names::Find<Node>("Node13"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2b"), "/F4c", Names::Find<Node>("Node13"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2b"), "/F5a", Names::Find<Node>("Node13"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2b"), "/F5b", Names::Find<Node>("Node13"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2b"), "/F5c", Names::Find<Node>("Node13"), 0);

  //Node14
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node14"), "/F1a", Names::Find<Node>("Node10"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node14"), "/F1b", Names::Find<Node>("Node10"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node14"), "/F1c", Names::Find<Node>("Node13"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node14"), "/F2a", Names::Find<Node>("Node10"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node14"), "/F2b", Names::Find<Node>("Node13"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node14"), "/F2c", Names::Find<Node>("Node13"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node14"), "/F3a", Names::Find<Node>("Node10"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node14"), "/F3b", Names::Find<Node>("Node13"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node14"), "/F3c", Names::Find<Node>("Node13"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node14"), "/F4a", Names::Find<Node>("Node10"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node14"), "/F4b", Names::Find<Node>("Node10"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node14"), "/F4c", Names::Find<Node>("Node10"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node14"), "/F5a", Names::Find<Node>("Node10"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node14"), "/F5b", Names::Find<Node>("Node10"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node14"), "/F5c", Names::Find<Node>("Node10"), 0);

  //Node15
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node15"), "/F1a", Names::Find<Node>("Node11"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node15"), "/F1b", Names::Find<Node>("Node11"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node15"), "/F1c", Names::Find<Node>("Node16"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node15"), "/F2a", Names::Find<Node>("Node11"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node15"), "/F2b", Names::Find<Node>("Node11"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node15"), "/F2c", Names::Find<Node>("Node16"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node15"), "/F3a", Names::Find<Node>("Node11"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node15"), "/F3b", Names::Find<Node>("Node11"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node15"), "/F3c", Names::Find<Node>("Node16"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node15"), "/F4a", Names::Find<Node>("Node11"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node15"), "/F4b", Names::Find<Node>("Node11"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node15"), "/F4c", Names::Find<Node>("F4c"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node15"), "/F5a", Names::Find<Node>("Node11"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node15"), "/F5b", Names::Find<Node>("Node11"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node15"), "/F5c", Names::Find<Node>("Node20"), 0);

  //F4c
  ndn::FibHelper::AddRoute(Names::Find<Node>("F4c"), "/F1a", Names::Find<Node>("Node15"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F4c"), "/F1b", Names::Find<Node>("Node15"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F4c"), "/F1c", Names::Find<Node>("Node15"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F4c"), "/F2a", Names::Find<Node>("Node15"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F4c"), "/F2b", Names::Find<Node>("Node15"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F4c"), "/F2c", Names::Find<Node>("Node15"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F4c"), "/F3a", Names::Find<Node>("Node15"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F4c"), "/F3b", Names::Find<Node>("Node15"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F4c"), "/F3c", Names::Find<Node>("Node15"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F4c"), "/F4a", Names::Find<Node>("Node15"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F4c"), "/F4b", Names::Find<Node>("Node15"), 0);
  //ndn::FibHelper::AddRoute(Names::Find<Node>("Node1"), "/F4c", Names::Find<Node>("Node"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F4c"), "/F5a", Names::Find<Node>("Node15"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F4c"), "/F5b", Names::Find<Node>("Node15"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F4c"), "/F5c", Names::Find<Node>("Node15"), 0);

  //Node16
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node16"), "/F1a", Names::Find<Node>("Node12"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node16"), "/F1b", Names::Find<Node>("Node12"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node16"), "/F1c", Names::Find<Node>("Node21"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node16"), "/F2a", Names::Find<Node>("Node12"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node16"), "/F2b", Names::Find<Node>("Node12"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node16"), "/F2c", Names::Find<Node>("Node17"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node16"), "/F3a", Names::Find<Node>("Node12"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node16"), "/F3b", Names::Find<Node>("Node12"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node16"), "/F3c", Names::Find<Node>("Node17"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node16"), "/F4a", Names::Find<Node>("Node12"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node16"), "/F4b", Names::Find<Node>("Node12"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node16"), "/F4c", Names::Find<Node>("Node15"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node16"), "/F5a", Names::Find<Node>("Node12"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node16"), "/F5b", Names::Find<Node>("Node12"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node16"), "/F5c", Names::Find<Node>("Node15"), 0);

  //Node17
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node17"), "/F1a", Names::Find<Node>("Node13"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node17"), "/F1b", Names::Find<Node>("Node13"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node17"), "/F1c", Names::Find<Node>("Node16"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node17"), "/F2a", Names::Find<Node>("Node13"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node17"), "/F2b", Names::Find<Node>("Node13"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node17"), "/F2c", Names::Find<Node>("Node23"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node17"), "/F3a", Names::Find<Node>("Node13"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node17"), "/F3b", Names::Find<Node>("Node13"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node17"), "/F3c", Names::Find<Node>("F3c"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node17"), "/F4a", Names::Find<Node>("Node13"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node17"), "/F4b", Names::Find<Node>("Node13"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node17"), "/F4c", Names::Find<Node>("Node16"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node17"), "/F5a", Names::Find<Node>("Node13"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node17"), "/F5b", Names::Find<Node>("Node13"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node17"), "/F5c", Names::Find<Node>("Node16"), 0);

  //F3c
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3c"), "/F1a", Names::Find<Node>("Node17"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3c"), "/F1b", Names::Find<Node>("Node17"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3c"), "/F1c", Names::Find<Node>("Node17"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3c"), "/F2a", Names::Find<Node>("Node17"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3c"), "/F2b", Names::Find<Node>("Node17"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3c"), "/F2c", Names::Find<Node>("Node17"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3c"), "/F3a", Names::Find<Node>("Node17"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3c"), "/F3b", Names::Find<Node>("Node17"), 0);
  //ndn::FibHelper::AddRoute(Names::Find<Node>("Node1"), "/F3c", Names::Find<Node>("Node"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3c"), "/F4a", Names::Find<Node>("Node17"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3c"), "/F4b", Names::Find<Node>("Node17"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3c"), "/F4c", Names::Find<Node>("Node17"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3c"), "/F5a", Names::Find<Node>("Node17"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3c"), "/F5b", Names::Find<Node>("Node17"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F3c"), "/F5c", Names::Find<Node>("Node17"), 0);


  //Node18
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node18"), "/F1a", Names::Find<Node>("Node14"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node18"), "/F1b", Names::Find<Node>("Node14"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node18"), "/F1c", Names::Find<Node>("Node17"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node18"), "/F2a", Names::Find<Node>("Node14"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node18"), "/F2b", Names::Find<Node>("Node14"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node18"), "/F2c", Names::Find<Node>("Node17"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node18"), "/F3a", Names::Find<Node>("Node14"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node18"), "/F3b", Names::Find<Node>("Node14"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node18"), "/F3c", Names::Find<Node>("Node17"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node18"), "/F4a", Names::Find<Node>("Node14"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node18"), "/F4b", Names::Find<Node>("Node14"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node18"), "/F4c", Names::Find<Node>("Node17"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node18"), "/F5a", Names::Find<Node>("Node14"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node18"), "/F5b", Names::Find<Node>("Node14"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node18"), "/F5c", Names::Find<Node>("Node17"), 0);


  //Node19
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node19"), "/F1a", Names::Find<Node>("Node11"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node19"), "/F1b", Names::Find<Node>("Node11"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node19"), "/F1c", Names::Find<Node>("Node20"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node19"), "/F2a", Names::Find<Node>("Node11"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node19"), "/F2b", Names::Find<Node>("Node11"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node19"), "/F2c", Names::Find<Node>("Node20"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node19"), "/F3a", Names::Find<Node>("Node11"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node19"), "/F3b", Names::Find<Node>("Node11"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node19"), "/F3c", Names::Find<Node>("Node11"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node19"), "/F4a", Names::Find<Node>("Node11"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node19"), "/F4b", Names::Find<Node>("Node11"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node19"), "/F4c", Names::Find<Node>("Node11"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node19"), "/F5a", Names::Find<Node>("Node11"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node19"), "/F5b", Names::Find<Node>("Node11"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node19"), "/F5c", Names::Find<Node>("Node20"), 0);

  //Node20
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node20"), "/F1a", Names::Find<Node>("Node15"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node20"), "/F1b", Names::Find<Node>("Node15"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node20"), "/F1c", Names::Find<Node>("Node21"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node20"), "/F2a", Names::Find<Node>("Node15"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node20"), "/F2b", Names::Find<Node>("Node15"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node20"), "/F2c", Names::Find<Node>("Node21"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node20"), "/F3a", Names::Find<Node>("Node15"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node20"), "/F3b", Names::Find<Node>("Node15"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node20"), "/F3c", Names::Find<Node>("Node15"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node20"), "/F4a", Names::Find<Node>("Node15"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node20"), "/F4b", Names::Find<Node>("Node15"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node20"), "/F4c", Names::Find<Node>("Node15"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node20"), "/F5a", Names::Find<Node>("Node15"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node20"), "/F5b", Names::Find<Node>("Node15"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node20"), "/F5c", Names::Find<Node>("F5c"), 0);

  //F5c
  ndn::FibHelper::AddRoute(Names::Find<Node>("F5c"), "/F1a", Names::Find<Node>("Node20"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F5c"), "/F1b", Names::Find<Node>("Node20"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F5c"), "/F1c", Names::Find<Node>("Node20"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F5c"), "/F2a", Names::Find<Node>("Node20"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F5c"), "/F2b", Names::Find<Node>("Node20"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F5c"), "/F2c", Names::Find<Node>("Node20"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F5c"), "/F3a", Names::Find<Node>("Node20"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F5c"), "/F3b", Names::Find<Node>("Node20"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F5c"), "/F3c", Names::Find<Node>("Node20"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F5c"), "/F4a", Names::Find<Node>("Node20"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F5c"), "/F4b", Names::Find<Node>("Node20"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F5c"), "/F4c", Names::Find<Node>("Node20"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F5c"), "/F5a", Names::Find<Node>("Node20"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F5c"), "/F5b", Names::Find<Node>("Node20"), 0);
  //ndn::FibHelper::AddRoute(Names::Find<Node>("Node1"), "/F5c", Names::Find<Node>("Node"), 0);

  //Node21
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node21"), "/F1a", Names::Find<Node>("Node16"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node21"), "/F1b", Names::Find<Node>("Node16"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node21"), "/F1c", Names::Find<Node>("F1c"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node21"), "/F2a", Names::Find<Node>("Node16"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node21"), "/F2b", Names::Find<Node>("Node16"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node21"), "/F2c", Names::Find<Node>("Node22"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node21"), "/F3a", Names::Find<Node>("Node16"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node21"), "/F3b", Names::Find<Node>("Node16"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node21"), "/F3c", Names::Find<Node>("Node16"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node21"), "/F4a", Names::Find<Node>("Node16"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node21"), "/F4b", Names::Find<Node>("Node16"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node21"), "/F4c", Names::Find<Node>("Node16"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node21"), "/F5a", Names::Find<Node>("Node16"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node21"), "/F5b", Names::Find<Node>("Node16"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node21"), "/F5c", Names::Find<Node>("Node20"), 0);

  //F1c
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1c"), "/F1a", Names::Find<Node>("Node21"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1c"), "/F1b", Names::Find<Node>("Node21"), 0);
  //ndn::FibHelper::AddRoute(Names::Find<Node>("Node1"), "/F1c", Names::Find<Node>("Node"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1c"), "/F2a", Names::Find<Node>("Node21"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1c"), "/F2b", Names::Find<Node>("Node21"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1c"), "/F2c", Names::Find<Node>("Node21"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1c"), "/F3a", Names::Find<Node>("Node21"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1c"), "/F3b", Names::Find<Node>("Node21"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1c"), "/F3c", Names::Find<Node>("Node21"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1c"), "/F4a", Names::Find<Node>("Node21"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1c"), "/F4b", Names::Find<Node>("Node21"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1c"), "/F4c", Names::Find<Node>("Node21"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1c"), "/F5a", Names::Find<Node>("Node21"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1c"), "/F5b", Names::Find<Node>("Node21"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F1c"), "/F5c", Names::Find<Node>("Node21"), 0);

  //Node22
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node22"), "/F1a", Names::Find<Node>("Node16"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node22"), "/F1b", Names::Find<Node>("Node17"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node22"), "/F1c", Names::Find<Node>("Node21"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node22"), "/F2a", Names::Find<Node>("Node16"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node22"), "/F2b", Names::Find<Node>("Node17"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node22"), "/F2c", Names::Find<Node>("Node23"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node22"), "/F3a", Names::Find<Node>("Node17"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node22"), "/F3b", Names::Find<Node>("Node16"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node22"), "/F3c", Names::Find<Node>("Node17"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node22"), "/F4a", Names::Find<Node>("Node16"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node22"), "/F4b", Names::Find<Node>("Node16"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node22"), "/F4c", Names::Find<Node>("Node16"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node22"), "/F5a", Names::Find<Node>("Node16"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node22"), "/F5b", Names::Find<Node>("Node16"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node22"), "/F5c", Names::Find<Node>("Node21"), 0);

  //Node23
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node23"), "/F1a", Names::Find<Node>("Node17"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node23"), "/F1b", Names::Find<Node>("Node17"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node23"), "/F1c", Names::Find<Node>("Node22"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node23"), "/F2a", Names::Find<Node>("Node17"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node23"), "/F2b", Names::Find<Node>("Node17"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node23"), "/F2c", Names::Find<Node>("F2c"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node23"), "/F3a", Names::Find<Node>("Node17"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node23"), "/F3b", Names::Find<Node>("Node17"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node23"), "/F3c", Names::Find<Node>("Node17"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node23"), "/F4a", Names::Find<Node>("Node17"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node23"), "/F4b", Names::Find<Node>("Node17"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node23"), "/F4c", Names::Find<Node>("Node17"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node23"), "/F5a", Names::Find<Node>("Node17"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node23"), "/F5b", Names::Find<Node>("Node17"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node23"), "/F5c", Names::Find<Node>("Node22"), 0);

  //F2c
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2c"), "/F1a", Names::Find<Node>("Node23"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2c"), "/F1b", Names::Find<Node>("Node23"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2c"), "/F1c", Names::Find<Node>("Node23"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2c"), "/F2a", Names::Find<Node>("Node23"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2c"), "/F2b", Names::Find<Node>("Node23"), 0);
  //ndn::FibHelper::AddRoute(Names::Find<Node>("Node1"), "/F2c", Names::Find<Node>("Node"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2c"), "/F3a", Names::Find<Node>("Node23"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2c"), "/F3b", Names::Find<Node>("Node23"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2c"), "/F3c", Names::Find<Node>("Node23"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2c"), "/F4a", Names::Find<Node>("Node23"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2c"), "/F4b", Names::Find<Node>("Node23"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2c"), "/F4c", Names::Find<Node>("Node23"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2c"), "/F5a", Names::Find<Node>("Node23"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2c"), "/F5b", Names::Find<Node>("Node23"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("F2c"), "/F5c", Names::Find<Node>("Node23"), 0);

  //Node24
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node24"), "/F1a", Names::Find<Node>("Node18"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node24"), "/F1b", Names::Find<Node>("Node18"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node24"), "/F1c", Names::Find<Node>("Node23"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node24"), "/F2a", Names::Find<Node>("Node18"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node24"), "/F2b", Names::Find<Node>("Node18"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node24"), "/F2c", Names::Find<Node>("Node23"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node24"), "/F3a", Names::Find<Node>("Node18"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node24"), "/F3b", Names::Find<Node>("Node18"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node24"), "/F3c", Names::Find<Node>("Node18"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node24"), "/F4a", Names::Find<Node>("Node18"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node24"), "/F4b", Names::Find<Node>("Node18"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node24"), "/F4c", Names::Find<Node>("Node18"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node24"), "/F5a", Names::Find<Node>("Node18"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node24"), "/F5b", Names::Find<Node>("Node18"), 0);
  ndn::FibHelper::AddRoute(Names::Find<Node>("Node24"), "/F5c", Names::Find<Node>("Node23"), 0);



  // Getting containers for the consumer/producer
  Ptr<Node> consumer1 = Names::Find<Node>("Consumer1");
  /*Ptr<Node> consumer2 = Names::Find<Node>("Consumer2");
  Ptr<Node> consumer3 = Names::Find<Node>("Consumer3");
  Ptr<Node> consumer4 = Names::Find<Node>("Consumer4");
*/
  Ptr<Node> producer1 = Names::Find<Node>("Producer1");
  Ptr<Node> producer2 = Names::Find<Node>("Producer2");
  Ptr<Node> producer3 = Names::Find<Node>("Producer3");
  Ptr<Node> producer4 = Names::Find<Node>("Producer4");

  //Ptr<Node> consumers[4] = {Names::Find<Node>("Consumer1"), Names::Find<Node>("Consumer2"), Names::Find<Node>("Consumer3"), Names::Find<Node>("Consumer4")};

  // Install NDN applications
  std::string prefix1 = "/prefix1";
  std::string prefix2 = "/prefix2";
  std::string prefix3 = "/prefix3";
  std::string prefix4 = "/prefix4";

  /*
  for (int i = 0; i < 2; i++) {
    ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
    consumerHelper.SetAttribute("Frequency", StringValue("10")); // 100 interests a second

    // Each consumer will express the same data /root/<seq-no>
    consumerHelper.SetPrefix(prefix);
    ApplicationContainer app = consumerHelper.Install(consumers[i]);
    app.Start(Seconds(0.01 * i));
    app.Stop(Seconds(20.0));
  }
  */

  
  ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
  consumerHelper.SetPrefix(prefix3);
  consumerHelper.SetAttribute("Frequency", StringValue("0.1"));
  ApplicationContainer app1 = consumerHelper.Install(consumer1);
  app1.Start(Seconds(0));
  //app1.Stop(Seconds(1.0));
  

  /*
  consumerHelper.SetPrefix(prefix4);
  consumerHelper.SetAttribute("Frequency", StringValue("30"));
  ApplicationContainer app2 = consumerHelper.Install(consumer2);
  app2.Start(Seconds(0.02));
  //app2.Stop(Seconds(1.0));

  consumerHelper.SetPrefix(prefix1);
  consumerHelper.SetAttribute("Frequency", StringValue("30"));
  ApplicationContainer app3 = consumerHelper.Install(consumer3);
  app3.Start(Seconds(0.03));
  //app3.Stop(Seconds(1.0));
  
  consumerHelper.SetPrefix(prefix2);
  consumerHelper.SetAttribute("Frequency", StringValue("30"));
  ApplicationContainer app4 = consumerHelper.Install(consumer4);
  app4.Start(Seconds(0.04));
  //app4.Stop(Seconds(1.0));
  


  */
  ndn::AppHelper producerHelper("ns3::ndn::Producer");
  producerHelper.SetPrefix(prefix1);
  producerHelper.SetAttribute("PayloadSize", StringValue("1200"));
  producerHelper.Install(producer1);

  producerHelper.SetPrefix(prefix2);
  producerHelper.SetAttribute("PayloadSize", StringValue("1200"));
  producerHelper.Install(producer2);

  producerHelper.SetPrefix(prefix3);
  producerHelper.SetAttribute("PayloadSize", StringValue("1200"));
  producerHelper.Install(producer3);

  producerHelper.SetPrefix(prefix4);
  producerHelper.SetAttribute("PayloadSize", StringValue("1200"));
  producerHelper.Install(producer4);

  // Add /prefix origins to ndn::GlobalRouter
  ndnGlobalRoutingHelper.AddOrigins(prefix1, producer1);
  ndnGlobalRoutingHelper.AddOrigins(prefix2, producer2);
  ndnGlobalRoutingHelper.AddOrigins(prefix3, producer3);
  ndnGlobalRoutingHelper.AddOrigins(prefix4, producer4);

  // Calculate and install FIBs
  ndn::GlobalRoutingHelper::CalculateRoutes();

  Simulator::Stop(Seconds(1000.0));

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
