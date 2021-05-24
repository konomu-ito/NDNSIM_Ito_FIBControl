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

#include "ndn-consumer.hpp"
#include "ns3/ptr.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/packet.h"
#include "ns3/callback.h"
#include "ns3/string.h"
#include "ns3/boolean.h"
#include "ns3/uinteger.h"
#include "ns3/integer.h"
#include "ns3/double.h"
#include "util/random.hpp"
#include "util/crypto.hpp"

#include "utils/ndn-ns3-packet-tag.hpp"
#include "utils/ndn-rtt-mean-deviation.hpp"

#include <ndn-cxx/lp/tags.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/ref.hpp>

#define N 26

NS_LOG_COMPONENT_DEFINE("ndn.Consumer");

namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED(Consumer);

TypeId
Consumer::GetTypeId(void)
{
	static TypeId tid =
			TypeId("ns3::ndn::Consumer")
			.SetGroupName("Ndn")
			.SetParent<App>()
			.AddAttribute("StartSeq", "Initial sequence number", IntegerValue(0),
					MakeIntegerAccessor(&Consumer::m_seq), MakeIntegerChecker<int32_t>())

					.AddAttribute("Prefix", "Name of the Interest", StringValue("/"),
							MakeNameAccessor(&Consumer::m_interestName), MakeNameChecker())
							.AddAttribute("LifeTime", "LifeTime for interest packet", StringValue("2s"),
									MakeTimeAccessor(&Consumer::m_interestLifeTime), MakeTimeChecker())

									.AddAttribute("RetxTimer",
											"Timeout defining how frequent retransmission timeouts should be checked",
											StringValue("50ms"),
											MakeTimeAccessor(&Consumer::GetRetxTimer, &Consumer::SetRetxTimer),
											MakeTimeChecker())

											.AddTraceSource("LastRetransmittedInterestDataDelay",
													"Delay between last retransmitted Interest and received Data",
													MakeTraceSourceAccessor(&Consumer::m_lastRetransmittedInterestDataDelay),
													"ns3::ndn::Consumer::LastRetransmittedInterestDataDelayCallback")

													.AddTraceSource("FirstInterestDataDelay",
															"Delay between first transmitted Interest and received Data",
															MakeTraceSourceAccessor(&Consumer::m_firstInterestDataDelay),
															"ns3::ndn::Consumer::FirstInterestDataDelayCallback");

	return tid;
}

Consumer::Consumer()
: m_rand(CreateObject<UniformRandomVariable>())
, m_seq(0)
, m_seqMax(0) // don't request anything
{
	NS_LOG_FUNCTION_NOARGS();
	for(int i=0;i<2;i++){
		for(int j=1;j<6;j++){
			for(int k=0;k<3;k++){
				table[i][j][k]=0;
			}
		}
	}

	m_rtt = CreateObject<RttMeanDeviation>();
}

void
Consumer::SetRetxTimer(Time retxTimer)
{
	m_retxTimer = retxTimer;
	if (m_retxEvent.IsRunning()) {
		// m_retxEvent.Cancel (); // cancel any scheduled cleanup events
		Simulator::Remove(m_retxEvent); // slower, but better for memory
	}

	// schedule even with new timeout
	m_retxEvent = Simulator::Schedule(m_retxTimer, &Consumer::CheckRetxTimeout, this);
}

Time
Consumer::GetRetxTimer() const
{
	return m_retxTimer;
}

void
Consumer::CheckRetxTimeout()
{
	Time now = Simulator::Now();

	Time rto = m_rtt->RetransmitTimeout();
	// NS_LOG_DEBUG ("Current RTO: " << rto.ToDouble (Time::S) << "s");

	while (!m_seqTimeouts.empty()) {
		SeqTimeoutsContainer::index<i_timestamp>::type::iterator entry =
				m_seqTimeouts.get<i_timestamp>().begin();
		if (entry->time + rto <= now) // timeout expired?
		{
			uint32_t seqNo = entry->seq;
			m_seqTimeouts.get<i_timestamp>().erase(entry);
			OnTimeout(seqNo);
		}
		else
			break; // nothing else to do. All later packets need not be retransmitted
	}

	m_retxEvent = Simulator::Schedule(m_retxTimer, &Consumer::CheckRetxTimeout, this);
}

// Application Methods
void
Consumer::StartApplication() // Called at time specified by Start
{
	NS_LOG_FUNCTION_NOARGS();

	// do base stuff
	App::StartApplication();

	ScheduleNextPacket();
}

void
Consumer::StopApplication() // Called at time specified by Stop
{
	NS_LOG_FUNCTION_NOARGS();

	// cancel periodic packet generation
	Simulator::Cancel(m_sendEvent);

	// cleanup base stuff
	App::StopApplication();
}


double
Consumer::dijkstra(int sp, int dp, int sRoute[N], int functionType, int consumerNode, double weight, int flag){

	double Distance[N][N];
	double fcc1 = ns3::getFunctionCallCount(1) * weight;
	double fcc2 = ns3::getFunctionCallCount(2) * weight;
	double fcc3 = ns3::getFunctionCallCount(3) * weight;
	double fcc4 = ns3::getFunctionCallCount(4) * weight;
	double fcc5 = ns3::getFunctionCallCount(5) * weight;
	double fcc6 = ns3::getFunctionCallCount(6) * weight;
	double fcc7 = ns3::getFunctionCallCount(7) * weight;
	double fcc8 = ns3::getFunctionCallCount(8) * weight;
	double fcc9 = ns3::getFunctionCallCount(9) * weight;
	double fcc10 = ns3::getFunctionCallCount(10) * weight;
	double fcc11 = ns3::getFunctionCallCount(11) * weight;
	double fcc12 = ns3::getFunctionCallCount(12) * weight;
	double fcc13 = ns3::getFunctionCallCount(13) * weight;
	double fcc14 = ns3::getFunctionCallCount(14) * weight;
	double fcc15 = ns3::getFunctionCallCount(15) * weight;

	double fcc1t = ns3::getTotalFcc(1) * weight;
	double fcc2t = ns3::getTotalFcc(2) * weight;
	double fcc3t = ns3::getTotalFcc(3) * weight;
	double fcc4t = ns3::getTotalFcc(4) * weight;
	double fcc5t = ns3::getTotalFcc(5) * weight;
	double fcc6t = ns3::getTotalFcc(6) * weight;
	double fcc7t = ns3::getTotalFcc(7) * weight;
	double fcc8t = ns3::getTotalFcc(8) * weight;
	double fcc9t = ns3::getTotalFcc(9) * weight;
	double fcc10t = ns3::getTotalFcc(10) * weight;
	double fcc11t = ns3::getTotalFcc(11) * weight;
	double fcc12t = ns3::getTotalFcc(12) * weight;
	double fcc13t = ns3::getTotalFcc(13) * weight;
	double fcc14t = ns3::getTotalFcc(14) * weight;
	double fcc15t = ns3::getTotalFcc(15) * weight;

	for(int i = 0; i < N; i++) {
		for(int j = 0; j < N; j++)
			Distance[i][j] = -1;
	}


	for(int i = 0; i < N; i++)
		Distance[i][i] = 0;


	if(flag == 0){
		if (consumerNode == 0){

			//F1->F2->F4
			if (functionType == 1) {
				Distance[0][3] = 2 + fcc1;
				Distance[0][10] = 3 + fcc2;
				Distance[0][21] = 7 + fcc3;
				Distance[3][7] = 1 + fcc4;
				Distance[3][13] = 4 + fcc5;
				Distance[3][23] = 6 + fcc6;
				Distance[10][7] = 2 + fcc4;
				Distance[10][13] = 1 + fcc5;
				Distance[10][23] = 3 + fcc6;
				Distance[21][7] = 5 + fcc4;
				Distance[21][13] = 3 + fcc5;
				Distance[21][23] = 2 + fcc6;
				Distance[7][6] = 1 + fcc10;
				Distance[7][9] = 2 + fcc11;
				Distance[7][15] = 3 + fcc12;
				Distance[13][6] = 3 + fcc10;
				Distance[13][9] = 2 + fcc11;
				Distance[13][15] = 3 + fcc12;
				Distance[23][6] = 5 + fcc10;
				Distance[23][9] = 4 + fcc11;
				Distance[23][15] = 3 + fcc12;
				Distance[6][25] = 2;
				Distance[9][25] = 2;
				Distance[15][25] = 2;
			}

			//F1->F2->F5
			if (functionType == 2) {
				Distance[0][3] = 2 + fcc1;
				Distance[0][10] = 3 + fcc2;
				Distance[0][21] = 7 + fcc3;
				Distance[3][7] = 1 + fcc4;
				Distance[3][13] = 4 + fcc5;
				Distance[3][23] = 6 + fcc6;
				Distance[10][7] = 2 + fcc4;
				Distance[10][13] = 1 + fcc5;
				Distance[10][23] = 3 + fcc6;
				Distance[21][7] = 5 + fcc4;
				Distance[21][13] = 3 + fcc5;
				Distance[21][23] = 2 + fcc6;
				Distance[7][2] = 2 + fcc13;
				Distance[7][11] = 2 + fcc14;
				Distance[7][20] = 4 + fcc15;
				Distance[13][2] = 4 + fcc13;
				Distance[13][11] = 2 + fcc14;
				Distance[13][20] = 4 + fcc15;
				Distance[23][2] = 6 + fcc13;
				Distance[23][11] = 4 + fcc14;
				Distance[23][20] = 3 + fcc15;
				Distance[2][25] = 3;
				Distance[11][25] = 1;
				Distance[20][25] = 1;
			}

			//F2->F1->F4
			if (functionType == 3) {
				Distance[0][7] = 3 + fcc4;
				Distance[0][13] = 4 + fcc5;
				Distance[0][23] = 6 + fcc6;
				Distance[7][3] = 1 + fcc1;
				Distance[7][10] = 2 + fcc2;
				Distance[7][21] = 5 + fcc3;
				Distance[13][3] = 4 + fcc1;
				Distance[13][10] = 1 + fcc2;
				Distance[13][21] = 3 + fcc3;
				Distance[23][3] = 6 + fcc1;
				Distance[23][10] = 3 + fcc2;
				Distance[23][21] = 2 + fcc3;
				Distance[3][6] = 2 + fcc10;
				Distance[3][9] = 3 + fcc11;
				Distance[3][15] = 4 + fcc12;
				Distance[10][6] = 2 + fcc10;
				Distance[10][9] = 1 + fcc11;
				Distance[10][15] = 3 + fcc12;
				Distance[21][6] = 4 + fcc10;
				Distance[21][9] = 3 + fcc11;
				Distance[21][15] = 2 + fcc12;
				Distance[6][25] = 2;
				Distance[9][25] = 2;
				Distance[15][25] = 2;
			}

			//F2->F1->F5
			if (functionType == 4) {
				Distance[0][7] = 3 + fcc4;
				Distance[0][13] = 4 + fcc5;
				Distance[0][23] = 6 + fcc6;
				Distance[7][3] = 1 + fcc1;
				Distance[7][10] = 2 + fcc2;
				Distance[7][21] = 5 + fcc3;
				Distance[13][3] = 4 + fcc1;
				Distance[13][10] = 1 + fcc2;
				Distance[13][21] = 3 + fcc3;
				Distance[23][3] = 6 + fcc1;
				Distance[23][10] = 3 + fcc2;
				Distance[23][21] = 2 + fcc3;
				Distance[3][2] = 1 + fcc13;
				Distance[3][11] = 3 + fcc14;
				Distance[3][20] = 5 + fcc15;
				Distance[10][2] = 3 + fcc13;
				Distance[10][11] = 2 + fcc14;
				Distance[10][20] = 4 + fcc15;
				Distance[21][2] = 5 + fcc13;
				Distance[21][11] = 3 + fcc14;
				Distance[21][20] = 1 + fcc15;
				Distance[2][25] = 3;
				Distance[11][25] = 1;
				Distance[20][25] = 1;
			}

			//F1->F3->F4
			if (functionType == 5) {
				Distance[0][3] = 2 + fcc1;
				Distance[0][10] = 3 + fcc2;
				Distance[0][21] = 7 + fcc3;
				Distance[3][4] = 1 + fcc7;
				Distance[3][12] = 4 + fcc8;
				Distance[3][17] = 5 + fcc9;
				Distance[10][4] = 3 + fcc7;
				Distance[10][12] = 2 + fcc8;
				Distance[10][17] = 2 + fcc9;
				Distance[21][4] = 6 + fcc7;
				Distance[21][12] = 2 + fcc8;
				Distance[21][17] = 2 + fcc9;
				Distance[4][6] = 2 + fcc10;
				Distance[4][9] = 3 + fcc11;
				Distance[4][15] = 4 + fcc12;
				Distance[12][6] = 2 + fcc10;
				Distance[12][9] = 1 + fcc11;
				Distance[12][15] = 2 + fcc12;
				Distance[17][6] = 4 + fcc10;
				Distance[17][9] = 3 + fcc11;
				Distance[17][15] = 2 + fcc12;
				Distance[6][25] = 2;
				Distance[9][25] = 2;
				Distance[15][25] = 2;
			}

			//F1->F3->F5
			if (functionType == 6) {
				Distance[0][3] = 2 + fcc1;
				Distance[0][10] = 3 + fcc2;
				Distance[0][21] = 7 + fcc3;
				Distance[3][4] = 1 + fcc7;
				Distance[3][12] = 4 + fcc8;
				Distance[3][17] = 5 + fcc9;
				Distance[10][4] = 3 + fcc7;
				Distance[10][12] = 2 + fcc8;
				Distance[10][17] = 2 + fcc9;
				Distance[21][4] = 6 + fcc7;
				Distance[21][12] = 2 + fcc8;
				Distance[21][17] = 2 + fcc9;
				Distance[4][2] = 2 + fcc13;
				Distance[4][11] = 3 + fcc14;
				Distance[4][20] = 5 + fcc15;
				Distance[12][2] = 3 + fcc13;
				Distance[12][11] = 1 + fcc14;
				Distance[12][20] = 3 + fcc15;
				Distance[17][2] = 5 + fcc13;
				Distance[17][11] = 3 + fcc14;
				Distance[17][20] = 3 + fcc15;
				Distance[2][25] = 3;
				Distance[11][25] = 1;
				Distance[20][25] = 1;
			}

			//F3->F1->F4
			if (functionType == 7) {
				Distance[0][4] = 2 + fcc7;
				Distance[0][12] = 5 + fcc8;
				Distance[0][17] = 5 + fcc9;
				Distance[4][3] = 1 + fcc1;
				Distance[4][10] = 3 + fcc2;
				Distance[4][21] = 6 + fcc3;
				Distance[12][3] = 4 + fcc1;
				Distance[12][10] = 2 + fcc2;
				Distance[12][21] = 2 + fcc3;
				Distance[17][3] = 5 + fcc1;
				Distance[17][10] = 2 + fcc2;
				Distance[17][21] = 2 + fcc3;
				Distance[3][6] = 2 + fcc10;
				Distance[3][9] = 3 + fcc11;
				Distance[3][15] = 4 + fcc12;
				Distance[10][6] = 2 + fcc10;
				Distance[10][9] = 1 + fcc11;
				Distance[10][15] = 3 + fcc12;
				Distance[21][6] = 4 + fcc10;
				Distance[21][9] = 3 + fcc11;
				Distance[21][15] = 2 + fcc12;
				Distance[6][25] = 2;
				Distance[9][25] = 2;
				Distance[15][25] = 2;
			}

			//F3->F1->F5
			if (functionType == 8) {
				Distance[0][4] = 2 + fcc7;
				Distance[0][12] = 5 + fcc8;
				Distance[0][17] = 5 + fcc9;
				Distance[4][3] = 1 + fcc1;
				Distance[4][10] = 3 + fcc2;
				Distance[4][21] = 6 + fcc3;
				Distance[12][3] = 4 + fcc1;
				Distance[12][10] = 2 + fcc2;
				Distance[12][21] = 2 + fcc3;
				Distance[17][3] = 5 + fcc1;
				Distance[17][10] = 2 + fcc2;
				Distance[17][21] = 2 + fcc3;
				Distance[3][2] = 1 + fcc13;
				Distance[3][11] = 3 + fcc14;
				Distance[3][20] = 5 + fcc15;
				Distance[10][2] = 3 + fcc13;
				Distance[10][11] = 2 + fcc14;
				Distance[10][20] = 4 + fcc15;
				Distance[21][2] = 5 + fcc13;
				Distance[21][11] = 3 + fcc14;
				Distance[21][20] = 1 + fcc15;
				Distance[2][25] = 3;
				Distance[11][25] = 1;
				Distance[20][25] = 1;
			}

			//F2->F3->F4
			if (functionType == 9) {
				Distance[0][7] = 3 + fcc4;
				Distance[0][13] = 4 + fcc5;
				Distance[0][23] = 6 + fcc6;
				Distance[7][4] = 1 + fcc7;
				Distance[7][12] = 3 + fcc8;
				Distance[7][17] = 4 + fcc9;
				Distance[13][4] = 4 + fcc7;
				Distance[13][12] = 1 + fcc8;
				Distance[13][17] = 1 + fcc9;
				Distance[23][4] = 6 + fcc7;
				Distance[23][12] = 3 + fcc8;
				Distance[23][17] = 1 + fcc9;
				Distance[4][6] = 2 + fcc10;
				Distance[4][9] = 3 + fcc11;
				Distance[4][15] = 4 + fcc12;
				Distance[12][6] = 2 + fcc10;
				Distance[12][9] = 1 + fcc11;
				Distance[12][15] = 2 + fcc12;
				Distance[17][6] = 4 + fcc10;
				Distance[17][9] = 3 + fcc11;
				Distance[17][15] = 2 + fcc12;
				Distance[6][25] = 2;
				Distance[9][25] = 2;
				Distance[15][25] = 2;
			}

			//F2->F3->F5
			if (functionType == 10) {
				Distance[0][7] = 3 + fcc4;
				Distance[0][13] = 4 + fcc5;
				Distance[0][23] = 6 + fcc6;
				Distance[7][4] = 1 + fcc7;
				Distance[7][12] = 3 + fcc8;
				Distance[7][17] = 4 + fcc9;
				Distance[13][4] = 4 + fcc7;
				Distance[13][12] = 1 + fcc8;
				Distance[13][17] = 1 + fcc9;
				Distance[23][4] = 6 + fcc7;
				Distance[23][12] = 3 + fcc8;
				Distance[23][17] = 1 + fcc9;
				Distance[4][2] = 2 + fcc13;
				Distance[4][11] = 3 + fcc14;
				Distance[4][20] = 5 + fcc15;
				Distance[12][2] = 3 + fcc13;
				Distance[12][11] = 1 + fcc14;
				Distance[12][20] = 3 + fcc15;
				Distance[17][2] = 5 + fcc13;
				Distance[17][11] = 3 + fcc14;
				Distance[17][20] = 3 + fcc15;
				Distance[2][25] = 3;
				Distance[11][25] = 1;
				Distance[20][25] = 1;
			}

			//F3->F2->F4
			if (functionType == 11) {
				Distance[0][4] = 2 + fcc4;
				Distance[0][12] = 5 + fcc5;
				Distance[0][17] = 5 + fcc6;
				Distance[4][7] = 2 + fcc7;
				Distance[4][13] = 3 + fcc8;
				Distance[4][23] = 4 + fcc9;
				Distance[12][7] = 3 + fcc7;
				Distance[12][13] = 1 + fcc8;
				Distance[12][23] = 3 + fcc9;
				Distance[17][7] = 4 + fcc7;
				Distance[17][13] = 1 + fcc8;
				Distance[17][23] = 1 + fcc9;
				Distance[7][6] = 1 + fcc10;
				Distance[7][9] = 2 + fcc11;
				Distance[7][15] = 3 + fcc12;
				Distance[13][6] = 3 + fcc10;
				Distance[13][9] = 2 + fcc11;
				Distance[13][15] = 3 + fcc12;
				Distance[23][6] = 5 + fcc10;
				Distance[23][9] = 4 + fcc11;
				Distance[23][15] = 3 + fcc12;
				Distance[6][25] = 2;
				Distance[9][25] = 2;
				Distance[15][25] = 2;
			}

			//F3->F2->F5
			if (functionType == 12) {
				Distance[0][4] = 2 + fcc4;
				Distance[0][12] = 5 + fcc5;
				Distance[0][17] = 5 + fcc6;
				Distance[4][7] = 1 + fcc7;
				Distance[4][13] = 4 + fcc8;
				Distance[4][23] = 6 + fcc9;
				Distance[12][7] = 3 + fcc7;
				Distance[12][13] = 1 + fcc8;
				Distance[12][23] = 3 + fcc9;
				Distance[17][7] = 4 + fcc7;
				Distance[17][13] = 1 + fcc8;
				Distance[17][23] = 1 + fcc9;
				Distance[7][2] = 2 + fcc13;
				Distance[7][11] = 2 + fcc14;
				Distance[7][20] = 4 + fcc15;
				Distance[13][2] = 4 + fcc13;
				Distance[13][11] = 2 + fcc14;
				Distance[13][20] = 4 + fcc15;
				Distance[23][2] = 6 + fcc13;
				Distance[23][11] = 4 + fcc14;
				Distance[23][20] = 3 + fcc15;
				Distance[2][25] = 3;
				Distance[11][25] = 1;
				Distance[20][25] = 1;
			}

		}

		if (consumerNode == 41){

			//F1->F2->F4
			if (functionType == 1) {
				Distance[0][3] = 3 + fcc1;
				Distance[0][10] = 2 + fcc2;
				Distance[0][21] = 6 + fcc3;
				Distance[3][7] = 1 + fcc4;
				Distance[3][13] = 4 + fcc5;
				Distance[3][23] = 6 + fcc6;
				Distance[10][7] = 2 + fcc4;
				Distance[10][13] = 1 + fcc5;
				Distance[10][23] = 3 + fcc6;
				Distance[21][7] = 5 + fcc4;
				Distance[21][13] = 3 + fcc5;
				Distance[21][23] = 2 + fcc6;
				Distance[7][6] = 1 + fcc10;
				Distance[7][9] = 2 + fcc11;
				Distance[7][15] = 3 + fcc12;
				Distance[13][6] = 3 + fcc10;
				Distance[13][9] = 2 + fcc11;
				Distance[13][15] = 3 + fcc12;
				Distance[23][6] = 5 + fcc10;
				Distance[23][9] = 4 + fcc11;
				Distance[23][15] = 3 + fcc12;
				Distance[6][25] = 2;
				Distance[9][25] = 2;
				Distance[15][25] = 2;
			}

			//F1->F2->F5
			if (functionType == 2) {
				Distance[0][3] = 3 + fcc1;
				Distance[0][10] = 2 + fcc2;
				Distance[0][21] = 6 + fcc3;
				Distance[3][7] = 1 + fcc4;
				Distance[3][13] = 4 + fcc5;
				Distance[3][23] = 6 + fcc6;
				Distance[10][7] = 2 + fcc4;
				Distance[10][13] = 1 + fcc5;
				Distance[10][23] = 3 + fcc6;
				Distance[21][7] = 5 + fcc4;
				Distance[21][13] = 3 + fcc5;
				Distance[21][23] = 2 + fcc6;
				Distance[7][2] = 2 + fcc13;
				Distance[7][11] = 2 + fcc14;
				Distance[7][20] = 4 + fcc15;
				Distance[13][2] = 4 + fcc13;
				Distance[13][11] = 2 + fcc14;
				Distance[13][20] = 4 + fcc15;
				Distance[23][2] = 6 + fcc13;
				Distance[23][11] = 4 + fcc14;
				Distance[23][20] = 3 + fcc15;
				Distance[2][25] = 3;
				Distance[11][25] = 1;
				Distance[20][25] = 1;
			}

			//F2->F1->F4
			if (functionType == 3) {
				Distance[0][7] = 2 + fcc4;
				Distance[0][13] = 3 + fcc5;
				Distance[0][23] = 5 + fcc6;
				Distance[7][3] = 1 + fcc1;
				Distance[7][10] = 2 + fcc2;
				Distance[7][21] = 5 + fcc3;
				Distance[13][3] = 4 + fcc1;
				Distance[13][10] = 1 + fcc2;
				Distance[13][21] = 3 + fcc3;
				Distance[23][3] = 6 + fcc1;
				Distance[23][10] = 3 + fcc2;
				Distance[23][21] = 2 + fcc3;
				Distance[3][6] = 2 + fcc10;
				Distance[3][9] = 3 + fcc11;
				Distance[3][15] = 4 + fcc12;
				Distance[10][6] = 2 + fcc10;
				Distance[10][9] = 1 + fcc11;
				Distance[10][15] = 3 + fcc12;
				Distance[21][6] = 4 + fcc10;
				Distance[21][9] = 3 + fcc11;
				Distance[21][15] = 2 + fcc12;
				Distance[6][25] = 2;
				Distance[9][25] = 2;
				Distance[15][25] = 2;
			}

			//F2->F1->F5
			if (functionType == 4) {
				Distance[0][7] = 2 + fcc4;
				Distance[0][13] = 3 + fcc5;
				Distance[0][23] = 5 + fcc6;
				Distance[7][3] = 1 + fcc1;
				Distance[7][10] = 2 + fcc2;
				Distance[7][21] = 5 + fcc3;
				Distance[13][3] = 4 + fcc1;
				Distance[13][10] = 1 + fcc2;
				Distance[13][21] = 3 + fcc3;
				Distance[23][3] = 6 + fcc1;
				Distance[23][10] = 3 + fcc2;
				Distance[23][21] = 2 + fcc3;
				Distance[3][2] = 1 + fcc13;
				Distance[3][11] = 3 + fcc14;
				Distance[3][20] = 5 + fcc15;
				Distance[10][2] = 3 + fcc13;
				Distance[10][11] = 2 + fcc14;
				Distance[10][20] = 4 + fcc15;
				Distance[21][2] = 5 + fcc13;
				Distance[21][11] = 3 + fcc14;
				Distance[21][20] = 1 + fcc15;
				Distance[2][25] = 3;
				Distance[11][25] = 1;
				Distance[20][25] = 1;
			}

			//F1->F3->F4
			if (functionType == 5) {
				Distance[0][3] = 3 + fcc1;
				Distance[0][10] = 2 + fcc2;
				Distance[0][21] = 6 + fcc3;
				Distance[3][4] = 1 + fcc7;
				Distance[3][12] = 4 + fcc8;
				Distance[3][17] = 5 + fcc9;
				Distance[10][4] = 3 + fcc7;
				Distance[10][12] = 2 + fcc8;
				Distance[10][17] = 2 + fcc9;
				Distance[21][4] = 6 + fcc7;
				Distance[21][12] = 2 + fcc8;
				Distance[21][17] = 2 + fcc9;
				Distance[4][6] = 2 + fcc10;
				Distance[4][9] = 3 + fcc11;
				Distance[4][15] = 4 + fcc12;
				Distance[12][6] = 2 + fcc10;
				Distance[12][9] = 1 + fcc11;
				Distance[12][15] = 2 + fcc12;
				Distance[17][6] = 4 + fcc10;
				Distance[17][9] = 3 + fcc11;
				Distance[17][15] = 2 + fcc12;
				Distance[6][25] = 2;
				Distance[9][25] = 2;
				Distance[15][25] = 2;
			}

			//F1->F3->F5
			if (functionType == 6) {
				Distance[0][3] = 3 + fcc1;
				Distance[0][10] = 2 + fcc2;
				Distance[0][21] = 6 + fcc3;
				Distance[3][4] = 1 + fcc7;
				Distance[3][12] = 4 + fcc8;
				Distance[3][17] = 5 + fcc9;
				Distance[10][4] = 3 + fcc7;
				Distance[10][12] = 2 + fcc8;
				Distance[10][17] = 2 + fcc9;
				Distance[21][4] = 6 + fcc7;
				Distance[21][12] = 2 + fcc8;
				Distance[21][17] = 2 + fcc9;
				Distance[4][2] = 2 + fcc13;
				Distance[4][11] = 3 + fcc14;
				Distance[4][20] = 5 + fcc15;
				Distance[12][2] = 3 + fcc13;
				Distance[12][11] = 1 + fcc14;
				Distance[12][20] = 3 + fcc15;
				Distance[17][2] = 5 + fcc13;
				Distance[17][11] = 3 + fcc14;
				Distance[17][20] = 3 + fcc15;
				Distance[2][25] = 3;
				Distance[11][25] = 1;
				Distance[20][25] = 1;
			}

			//F3->F1->F4
			if (functionType == 7) {
				Distance[0][4] = 3 + fcc7;
				Distance[0][12] = 4 + fcc8;
				Distance[0][17] = 4 + fcc9;
				Distance[4][3] = 1 + fcc1;
				Distance[4][10] = 3 + fcc2;
				Distance[4][21] = 6 + fcc3;
				Distance[12][3] = 4 + fcc1;
				Distance[12][10] = 2 + fcc2;
				Distance[12][21] = 2 + fcc3;
				Distance[17][3] = 5 + fcc1;
				Distance[17][10] = 2 + fcc2;
				Distance[17][21] = 2 + fcc3;
				Distance[3][6] = 2 + fcc10;
				Distance[3][9] = 3 + fcc11;
				Distance[3][15] = 4 + fcc12;
				Distance[10][6] = 2 + fcc10;
				Distance[10][9] = 1 + fcc11;
				Distance[10][15] = 3 + fcc12;
				Distance[21][6] = 4 + fcc10;
				Distance[21][9] = 3 + fcc11;
				Distance[21][15] = 2 + fcc12;
				Distance[6][25] = 2;
				Distance[9][25] = 2;
				Distance[15][25] = 2;
			}

			//F3->F1->F5
			if (functionType == 8) {
				Distance[0][4] = 3 + fcc7;
				Distance[0][12] = 4 + fcc8;
				Distance[0][17] = 4 + fcc9;
				Distance[4][3] = 1 + fcc1;
				Distance[4][10] = 3 + fcc2;
				Distance[4][21] = 6 + fcc3;
				Distance[12][3] = 4 + fcc1;
				Distance[12][10] = 2 + fcc2;
				Distance[12][21] = 2 + fcc3;
				Distance[17][3] = 5 + fcc1;
				Distance[17][10] = 2 + fcc2;
				Distance[17][21] = 2 + fcc3;
				Distance[3][2] = 1 + fcc13;
				Distance[3][11] = 3 + fcc14;
				Distance[3][20] = 5 + fcc15;
				Distance[10][2] = 3 + fcc13;
				Distance[10][11] = 2 + fcc14;
				Distance[10][20] = 4 + fcc15;
				Distance[21][2] = 5 + fcc13;
				Distance[21][11] = 3 + fcc14;
				Distance[21][20] = 1 + fcc15;
				Distance[2][25] = 3;
				Distance[11][25] = 1;
				Distance[20][25] = 1;
			}

			//F2->F3->F4
			if (functionType == 9) {
				Distance[0][7] = 2 + fcc4;
				Distance[0][13] = 3 + fcc5;
				Distance[0][23] = 5 + fcc6;
				Distance[7][4] = 1 + fcc7;
				Distance[7][12] = 3 + fcc8;
				Distance[7][17] = 4 + fcc9;
				Distance[13][4] = 4 + fcc7;
				Distance[13][12] = 1 + fcc8;
				Distance[13][17] = 1 + fcc9;
				Distance[23][4] = 6 + fcc7;
				Distance[23][12] = 3 + fcc8;
				Distance[23][17] = 1 + fcc9;
				Distance[4][6] = 2 + fcc10;
				Distance[4][9] = 3 + fcc11;
				Distance[4][15] = 4 + fcc12;
				Distance[12][6] = 2 + fcc10;
				Distance[12][9] = 1 + fcc11;
				Distance[12][15] = 2 + fcc12;
				Distance[17][6] = 4 + fcc10;
				Distance[17][9] = 3 + fcc11;
				Distance[17][15] = 2 + fcc12;
				Distance[6][25] = 2;
				Distance[9][25] = 2;
				Distance[15][25] = 2;
			}

			//F2->F3->F5
			if (functionType == 10) {
				Distance[0][7] = 2 + fcc4;
				Distance[0][13] = 3 + fcc5;
				Distance[0][23] = 5 + fcc6;
				Distance[7][4] = 1 + fcc7;
				Distance[7][12] = 3 + fcc8;
				Distance[7][17] = 4 + fcc9;
				Distance[13][4] = 4 + fcc7;
				Distance[13][12] = 1 + fcc8;
				Distance[13][17] = 1 + fcc9;
				Distance[23][4] = 6 + fcc7;
				Distance[23][12] = 3 + fcc8;
				Distance[23][17] = 1 + fcc9;
				Distance[4][2] = 2 + fcc13;
				Distance[4][11] = 3 + fcc14;
				Distance[4][20] = 5 + fcc15;
				Distance[12][2] = 3 + fcc13;
				Distance[12][11] = 1 + fcc14;
				Distance[12][20] = 3 + fcc15;
				Distance[17][2] = 5 + fcc13;
				Distance[17][11] = 3 + fcc14;
				Distance[17][20] = 3 + fcc15;
				Distance[2][25] = 3;
				Distance[11][25] = 1;
				Distance[20][25] = 1;
			}

			//F3->F2->F4
			if (functionType == 11) {
				Distance[0][4] = 3 + fcc4;
				Distance[0][12] = 4 + fcc5;
				Distance[0][17] = 4 + fcc6;
				Distance[4][7] = 2 + fcc7;
				Distance[4][13] = 3 + fcc8;
				Distance[4][23] = 4 + fcc9;
				Distance[12][7] = 3 + fcc7;
				Distance[12][13] = 1 + fcc8;
				Distance[12][23] = 3 + fcc9;
				Distance[17][7] = 4 + fcc7;
				Distance[17][13] = 1 + fcc8;
				Distance[17][23] = 1 + fcc9;
				Distance[7][6] = 1 + fcc10;
				Distance[7][9] = 2 + fcc11;
				Distance[7][15] = 3 + fcc12;
				Distance[13][6] = 3 + fcc10;
				Distance[13][9] = 2 + fcc11;
				Distance[13][15] = 3 + fcc12;
				Distance[23][6] = 5 + fcc10;
				Distance[23][9] = 4 + fcc11;
				Distance[23][15] = 3 + fcc12;
				Distance[6][25] = 2;
				Distance[9][25] = 2;
				Distance[15][25] = 2;
			}

			//F3->F2->F5
			if (functionType == 12) {
				Distance[0][4] = 3 + fcc4;
				Distance[0][12] = 4 + fcc5;
				Distance[0][17] = 4 + fcc6;
				Distance[4][7] = 1 + fcc7;
				Distance[4][13] = 4 + fcc8;
				Distance[4][23] = 6 + fcc9;
				Distance[12][7] = 3 + fcc7;
				Distance[12][13] = 1 + fcc8;
				Distance[12][23] = 3 + fcc9;
				Distance[17][7] = 4 + fcc7;
				Distance[17][13] = 1 + fcc8;
				Distance[17][23] = 1 + fcc9;
				Distance[7][2] = 2 + fcc13;
				Distance[7][11] = 2 + fcc14;
				Distance[7][20] = 4 + fcc15;
				Distance[13][2] = 4 + fcc13;
				Distance[13][11] = 2 + fcc14;
				Distance[13][20] = 4 + fcc15;
				Distance[23][2] = 6 + fcc13;
				Distance[23][11] = 4 + fcc14;
				Distance[23][20] = 3 + fcc15;
				Distance[2][25] = 3;
				Distance[11][25] = 1;
				Distance[20][25] = 1;
			}

		}

		if (consumerNode == 42){

			//F1->F2->F4
			if (functionType == 1) {
				Distance[0][3] = 7 + fcc1;
				Distance[0][10] = 4 + fcc2;
				Distance[0][21] = 4 + fcc3;
				Distance[3][7] = 1 + fcc4;
				Distance[3][13] = 4 + fcc5;
				Distance[3][23] = 6 + fcc6;
				Distance[10][7] = 2 + fcc4;
				Distance[10][13] = 1 + fcc5;
				Distance[10][23] = 3 + fcc6;
				Distance[21][7] = 5 + fcc4;
				Distance[21][13] = 3 + fcc5;
				Distance[21][23] = 2 + fcc6;
				Distance[7][6] = 1 + fcc10;
				Distance[7][9] = 2 + fcc11;
				Distance[7][15] = 3 + fcc12;
				Distance[13][6] = 3 + fcc10;
				Distance[13][9] = 2 + fcc11;
				Distance[13][15] = 3 + fcc12;
				Distance[23][6] = 5 + fcc10;
				Distance[23][9] = 4 + fcc11;
				Distance[23][15] = 3 + fcc12;
				Distance[6][25] = 1;
				Distance[9][25] = 2;
				Distance[15][25] = 3;
			}

			//F1->F2->F5
			if (functionType == 2) {
				Distance[0][3] = 7 + fcc1;
				Distance[0][10] = 4 + fcc2;
				Distance[0][21] = 4 + fcc3;
				Distance[3][7] = 1 + fcc4;
				Distance[3][13] = 4 + fcc5;
				Distance[3][23] = 6 + fcc6;
				Distance[10][7] = 2 + fcc4;
				Distance[10][13] = 1 + fcc5;
				Distance[10][23] = 3 + fcc6;
				Distance[21][7] = 5 + fcc4;
				Distance[21][13] = 3 + fcc5;
				Distance[21][23] = 2 + fcc6;
				Distance[7][2] = 2 + fcc13;
				Distance[7][11] = 2 + fcc14;
				Distance[7][20] = 4 + fcc15;
				Distance[13][2] = 4 + fcc13;
				Distance[13][11] = 2 + fcc14;
				Distance[13][20] = 4 + fcc15;
				Distance[23][2] = 6 + fcc13;
				Distance[23][11] = 4 + fcc14;
				Distance[23][20] = 3 + fcc15;
				Distance[2][25] = 1;
				Distance[11][25] = 2;
				Distance[20][25] = 4;
			}

			//F2->F1->F4
			if (functionType == 3) {
				Distance[0][7] = 6 + fcc4;
				Distance[0][13] = 4 + fcc5;
				Distance[0][23] = 2 + fcc6;
				Distance[7][3] = 1 + fcc1;
				Distance[7][10] = 2 + fcc2;
				Distance[7][21] = 5 + fcc3;
				Distance[13][3] = 4 + fcc1;
				Distance[13][10] = 1 + fcc2;
				Distance[13][21] = 3 + fcc3;
				Distance[23][3] = 6 + fcc1;
				Distance[23][10] = 3 + fcc2;
				Distance[23][21] = 2 + fcc3;
				Distance[3][6] = 2 + fcc10;
				Distance[3][9] = 3 + fcc11;
				Distance[3][15] = 4 + fcc12;
				Distance[10][6] = 2 + fcc10;
				Distance[10][9] = 1 + fcc11;
				Distance[10][15] = 3 + fcc12;
				Distance[21][6] = 4 + fcc10;
				Distance[21][9] = 3 + fcc11;
				Distance[21][15] = 2 + fcc12;
				Distance[6][25] = 1;
				Distance[9][25] = 2;
				Distance[15][25] = 3;
			}

			//F2->F1->F5
			if (functionType == 4) {
				Distance[0][7] = 6 + fcc4;
				Distance[0][13] = 4 + fcc5;
				Distance[0][23] = 2 + fcc6;
				Distance[7][3] = 1 + fcc1;
				Distance[7][10] = 2 + fcc2;
				Distance[7][21] = 5 + fcc3;
				Distance[13][3] = 4 + fcc1;
				Distance[13][10] = 1 + fcc2;
				Distance[13][21] = 3 + fcc3;
				Distance[23][3] = 6 + fcc1;
				Distance[23][10] = 3 + fcc2;
				Distance[23][21] = 2 + fcc3;
				Distance[3][2] = 1 + fcc13;
				Distance[3][11] = 3 + fcc14;
				Distance[3][20] = 5 + fcc15;
				Distance[10][2] = 3 + fcc13;
				Distance[10][11] = 2 + fcc14;
				Distance[10][20] = 4 + fcc15;
				Distance[21][2] = 5 + fcc13;
				Distance[21][11] = 3 + fcc14;
				Distance[21][20] = 1 + fcc15;
				Distance[2][25] = 1;
				Distance[11][25] = 2;
				Distance[20][25] = 4;
			}

			//F1->F3->F4
			if (functionType == 5) {
				Distance[0][3] = 7 + fcc1;
				Distance[0][10] = 4 + fcc2;
				Distance[0][21] = 4 + fcc3;
				Distance[3][4] = 1 + fcc7;
				Distance[3][12] = 4 + fcc8;
				Distance[3][17] = 5 + fcc9;
				Distance[10][4] = 3 + fcc7;
				Distance[10][12] = 2 + fcc8;
				Distance[10][17] = 2 + fcc9;
				Distance[21][4] = 6 + fcc7;
				Distance[21][12] = 2 + fcc8;
				Distance[21][17] = 2 + fcc9;
				Distance[4][6] = 2 + fcc10;
				Distance[4][9] = 3 + fcc11;
				Distance[4][15] = 4 + fcc12;
				Distance[12][6] = 2 + fcc10;
				Distance[12][9] = 1 + fcc11;
				Distance[12][15] = 2 + fcc12;
				Distance[17][6] = 4 + fcc10;
				Distance[17][9] = 3 + fcc11;
				Distance[17][15] = 2 + fcc12;
				Distance[6][25] = 1;
				Distance[9][25] = 2;
				Distance[15][25] = 3;
			}

			//F1->F3->F5
			if (functionType == 6) {
				Distance[0][3] = 7 + fcc1;
				Distance[0][10] = 4 + fcc2;
				Distance[0][21] = 4 + fcc3;
				Distance[3][4] = 1 + fcc7;
				Distance[3][12] = 4 + fcc8;
				Distance[3][17] = 5 + fcc9;
				Distance[10][4] = 3 + fcc7;
				Distance[10][12] = 2 + fcc8;
				Distance[10][17] = 2 + fcc9;
				Distance[21][4] = 6 + fcc7;
				Distance[21][12] = 2 + fcc8;
				Distance[21][17] = 2 + fcc9;
				Distance[4][2] = 2 + fcc13;
				Distance[4][11] = 3 + fcc14;
				Distance[4][20] = 5 + fcc15;
				Distance[12][2] = 3 + fcc13;
				Distance[12][11] = 1 + fcc14;
				Distance[12][20] = 3 + fcc15;
				Distance[17][2] = 5 + fcc13;
				Distance[17][11] = 3 + fcc14;
				Distance[17][20] = 3 + fcc15;
				Distance[2][25] = 1;
				Distance[11][25] = 2;
				Distance[20][25] = 4;
			}

			//F3->F1->F4
			if (functionType == 7) {
				Distance[0][4] = 7 + fcc7;
				Distance[0][12] = 5 + fcc8;
				Distance[0][17] = 3 + fcc9;
				Distance[4][3] = 1 + fcc1;
				Distance[4][10] = 3 + fcc2;
				Distance[4][21] = 6 + fcc3;
				Distance[12][3] = 4 + fcc1;
				Distance[12][10] = 2 + fcc2;
				Distance[12][21] = 2 + fcc3;
				Distance[17][3] = 5 + fcc1;
				Distance[17][10] = 2 + fcc2;
				Distance[17][21] = 2 + fcc3;
				Distance[3][6] = 2 + fcc10;
				Distance[3][9] = 3 + fcc11;
				Distance[3][15] = 4 + fcc12;
				Distance[10][6] = 2 + fcc10;
				Distance[10][9] = 1 + fcc11;
				Distance[10][15] = 3 + fcc12;
				Distance[21][6] = 4 + fcc10;
				Distance[21][9] = 3 + fcc11;
				Distance[21][15] = 2 + fcc12;
				Distance[6][25] = 1;
				Distance[9][25] = 2;
				Distance[15][25] = 3;
			}

			//F3->F1->F5
			if (functionType == 8) {
				Distance[0][4] = 7 + fcc7;
				Distance[0][12] = 5 + fcc8;
				Distance[0][17] = 3 + fcc9;
				Distance[4][3] = 1 + fcc1;
				Distance[4][10] = 3 + fcc2;
				Distance[4][21] = 6 + fcc3;
				Distance[12][3] = 4 + fcc1;
				Distance[12][10] = 2 + fcc2;
				Distance[12][21] = 2 + fcc3;
				Distance[17][3] = 5 + fcc1;
				Distance[17][10] = 2 + fcc2;
				Distance[17][21] = 2 + fcc3;
				Distance[3][2] = 1 + fcc13;
				Distance[3][11] = 3 + fcc14;
				Distance[3][20] = 5 + fcc15;
				Distance[10][2] = 3 + fcc13;
				Distance[10][11] = 2 + fcc14;
				Distance[10][20] = 4 + fcc15;
				Distance[21][2] = 5 + fcc13;
				Distance[21][11] = 3 + fcc14;
				Distance[21][20] = 1 + fcc15;
				Distance[2][25] = 1;
				Distance[11][25] = 2;
				Distance[20][25] = 4;
			}

			//F2->F3->F4
			if (functionType == 9) {
				Distance[0][7] = 6 + fcc4;
				Distance[0][13] = 4 + fcc5;
				Distance[0][23] = 2 + fcc6;
				Distance[7][4] = 1 + fcc7;
				Distance[7][12] = 3 + fcc8;
				Distance[7][17] = 4 + fcc9;
				Distance[13][4] = 4 + fcc7;
				Distance[13][12] = 1 + fcc8;
				Distance[13][17] = 1 + fcc9;
				Distance[23][4] = 6 + fcc7;
				Distance[23][12] = 3 + fcc8;
				Distance[23][17] = 1 + fcc9;
				Distance[4][6] = 2 + fcc10;
				Distance[4][9] = 3 + fcc11;
				Distance[4][15] = 4 + fcc12;
				Distance[12][6] = 2 + fcc10;
				Distance[12][9] = 1 + fcc11;
				Distance[12][15] = 2 + fcc12;
				Distance[17][6] = 4 + fcc10;
				Distance[17][9] = 3 + fcc11;
				Distance[17][15] = 2 + fcc12;
				Distance[6][25] = 1;
				Distance[9][25] = 2;
				Distance[15][25] = 3;
			}

			//F2->F3->F5
			if (functionType == 10) {
				Distance[0][7] = 6 + fcc4;
				Distance[0][13] = 4 + fcc5;
				Distance[0][23] = 2 + fcc6;
				Distance[7][4] = 1 + fcc7;
				Distance[7][12] = 3 + fcc8;
				Distance[7][17] = 4 + fcc9;
				Distance[13][4] = 4 + fcc7;
				Distance[13][12] = 1 + fcc8;
				Distance[13][17] = 1 + fcc9;
				Distance[23][4] = 6 + fcc7;
				Distance[23][12] = 3 + fcc8;
				Distance[23][17] = 1 + fcc9;
				Distance[4][2] = 2 + fcc13;
				Distance[4][11] = 3 + fcc14;
				Distance[4][20] = 5 + fcc15;
				Distance[12][2] = 3 + fcc13;
				Distance[12][11] = 1 + fcc14;
				Distance[12][20] = 3 + fcc15;
				Distance[17][2] = 5 + fcc13;
				Distance[17][11] = 3 + fcc14;
				Distance[17][20] = 3 + fcc15;
				Distance[2][25] = 1;
				Distance[11][25] = 2;
				Distance[20][25] = 4;
			}

			//F3->F2->F4
			if (functionType == 11) {
				Distance[0][4] = 7 + fcc4;
				Distance[0][12] = 5 + fcc5;
				Distance[0][17] = 3 + fcc6;
				Distance[4][7] = 2 + fcc7;
				Distance[4][13] = 3 + fcc8;
				Distance[4][23] = 4 + fcc9;
				Distance[12][7] = 3 + fcc7;
				Distance[12][13] = 1 + fcc8;
				Distance[12][23] = 3 + fcc9;
				Distance[17][7] = 4 + fcc7;
				Distance[17][13] = 1 + fcc8;
				Distance[17][23] = 1 + fcc9;
				Distance[7][6] = 1 + fcc10;
				Distance[7][9] = 2 + fcc11;
				Distance[7][15] = 3 + fcc12;
				Distance[13][6] = 3 + fcc10;
				Distance[13][9] = 2 + fcc11;
				Distance[13][15] = 3 + fcc12;
				Distance[23][6] = 5 + fcc10;
				Distance[23][9] = 4 + fcc11;
				Distance[23][15] = 3 + fcc12;
				Distance[6][25] = 1;
				Distance[9][25] = 2;
				Distance[15][25] = 3;
			}

			//F3->F2->F5
			if (functionType == 12) {
				Distance[0][4] = 7 + fcc4;
				Distance[0][12] = 5 + fcc5;
				Distance[0][17] = 3 + fcc6;
				Distance[4][7] = 1 + fcc7;
				Distance[4][13] = 4 + fcc8;
				Distance[4][23] = 6 + fcc9;
				Distance[12][7] = 3 + fcc7;
				Distance[12][13] = 1 + fcc8;
				Distance[12][23] = 3 + fcc9;
				Distance[17][7] = 4 + fcc7;
				Distance[17][13] = 1 + fcc8;
				Distance[17][23] = 1 + fcc9;
				Distance[7][2] = 2 + fcc13;
				Distance[7][11] = 2 + fcc14;
				Distance[7][20] = 4 + fcc15;
				Distance[13][2] = 4 + fcc13;
				Distance[13][11] = 2 + fcc14;
				Distance[13][20] = 4 + fcc15;
				Distance[23][2] = 6 + fcc13;
				Distance[23][11] = 4 + fcc14;
				Distance[23][20] = 3 + fcc15;
				Distance[2][25] = 1;
				Distance[11][25] = 2;
				Distance[20][25] = 4;
			}

		}

		if (consumerNode == 43){

			//F1->F2->F4
			if (functionType == 1) {
				Distance[0][3] = 6 + fcc1;
				Distance[0][10] = 3 + fcc2;
				Distance[0][21] = 4 + fcc3;
				Distance[3][7] = 1 + fcc4;
				Distance[3][13] = 4 + fcc5;
				Distance[3][23] = 6 + fcc6;
				Distance[10][7] = 2 + fcc4;
				Distance[10][13] = 1 + fcc5;
				Distance[10][23] = 3 + fcc6;
				Distance[21][7] = 5 + fcc4;
				Distance[21][13] = 3 + fcc5;
				Distance[21][23] = 2 + fcc6;
				Distance[7][6] = 1 + fcc10;
				Distance[7][9] = 2 + fcc11;
				Distance[7][15] = 3 + fcc12;
				Distance[13][6] = 3 + fcc10;
				Distance[13][9] = 2 + fcc11;
				Distance[13][15] = 3 + fcc12;
				Distance[23][6] = 5 + fcc10;
				Distance[23][9] = 4 + fcc11;
				Distance[23][15] = 3 + fcc12;
				Distance[6][25] = 1;
				Distance[9][25] = 2;
				Distance[15][25] = 3;
			}

			//F1->F2->F5
			if (functionType == 2) {
				Distance[0][3] = 6 + fcc1;
				Distance[0][10] = 3 + fcc2;
				Distance[0][21] = 4 + fcc3;
				Distance[3][7] = 1 + fcc4;
				Distance[3][13] = 4 + fcc5;
				Distance[3][23] = 6 + fcc6;
				Distance[10][7] = 2 + fcc4;
				Distance[10][13] = 1 + fcc5;
				Distance[10][23] = 3 + fcc6;
				Distance[21][7] = 5 + fcc4;
				Distance[21][13] = 3 + fcc5;
				Distance[21][23] = 2 + fcc6;
				Distance[7][2] = 2 + fcc13;
				Distance[7][11] = 2 + fcc14;
				Distance[7][20] = 4 + fcc15;
				Distance[13][2] = 4 + fcc13;
				Distance[13][11] = 2 + fcc14;
				Distance[13][20] = 4 + fcc15;
				Distance[23][2] = 6 + fcc13;
				Distance[23][11] = 4 + fcc14;
				Distance[23][20] = 3 + fcc15;
				Distance[2][25] = 1;
				Distance[11][25] = 2;
				Distance[20][25] = 4;
			}

			//F2->F1->F4
			if (functionType == 3) {
				Distance[0][7] = 5 + fcc4;
				Distance[0][13] = 3 + fcc5;
				Distance[0][23] = 3 + fcc6;
				Distance[7][3] = 1 + fcc1;
				Distance[7][10] = 2 + fcc2;
				Distance[7][21] = 5 + fcc3;
				Distance[13][3] = 4 + fcc1;
				Distance[13][10] = 1 + fcc2;
				Distance[13][21] = 3 + fcc3;
				Distance[23][3] = 6 + fcc1;
				Distance[23][10] = 3 + fcc2;
				Distance[23][21] = 2 + fcc3;
				Distance[3][6] = 2 + fcc10;
				Distance[3][9] = 3 + fcc11;
				Distance[3][15] = 4 + fcc12;
				Distance[10][6] = 2 + fcc10;
				Distance[10][9] = 1 + fcc11;
				Distance[10][15] = 3 + fcc12;
				Distance[21][6] = 4 + fcc10;
				Distance[21][9] = 3 + fcc11;
				Distance[21][15] = 2 + fcc12;
				Distance[6][25] = 1;
				Distance[9][25] = 2;
				Distance[15][25] = 3;
			}

			//F2->F1->F5
			if (functionType == 4) {
				Distance[0][7] = 5 + fcc4;
				Distance[0][13] = 3 + fcc5;
				Distance[0][23] = 3 + fcc6;
				Distance[7][3] = 1 + fcc1;
				Distance[7][10] = 2 + fcc2;
				Distance[7][21] = 5 + fcc3;
				Distance[13][3] = 4 + fcc1;
				Distance[13][10] = 1 + fcc2;
				Distance[13][21] = 3 + fcc3;
				Distance[23][3] = 6 + fcc1;
				Distance[23][10] = 3 + fcc2;
				Distance[23][21] = 2 + fcc3;
				Distance[3][2] = 1 + fcc13;
				Distance[3][11] = 3 + fcc14;
				Distance[3][20] = 5 + fcc15;
				Distance[10][2] = 3 + fcc13;
				Distance[10][11] = 2 + fcc14;
				Distance[10][20] = 4 + fcc15;
				Distance[21][2] = 5 + fcc13;
				Distance[21][11] = 3 + fcc14;
				Distance[21][20] = 1 + fcc15;
				Distance[2][25] = 1;
				Distance[11][25] = 2;
				Distance[20][25] = 4;
			}

			//F1->F3->F4
			if (functionType == 5) {
				Distance[0][3] = 6 + fcc1;
				Distance[0][10] = 3 + fcc2;
				Distance[0][21] = 4 + fcc3;
				Distance[3][4] = 1 + fcc7;
				Distance[3][12] = 4 + fcc8;
				Distance[3][17] = 5 + fcc9;
				Distance[10][4] = 3 + fcc7;
				Distance[10][12] = 2 + fcc8;
				Distance[10][17] = 2 + fcc9;
				Distance[21][4] = 6 + fcc7;
				Distance[21][12] = 2 + fcc8;
				Distance[21][17] = 2 + fcc9;
				Distance[4][6] = 2 + fcc10;
				Distance[4][9] = 3 + fcc11;
				Distance[4][15] = 4 + fcc12;
				Distance[12][6] = 2 + fcc10;
				Distance[12][9] = 1 + fcc11;
				Distance[12][15] = 2 + fcc12;
				Distance[17][6] = 4 + fcc10;
				Distance[17][9] = 3 + fcc11;
				Distance[17][15] = 2 + fcc12;
				Distance[6][25] = 1;
				Distance[9][25] = 2;
				Distance[15][25] = 3;
			}

			//F1->F3->F5
			if (functionType == 6) {
				Distance[0][3] = 6 + fcc1;
				Distance[0][10] = 3 + fcc2;
				Distance[0][21] = 4 + fcc3;
				Distance[3][4] = 1 + fcc7;
				Distance[3][12] = 4 + fcc8;
				Distance[3][17] = 5 + fcc9;
				Distance[10][4] = 3 + fcc7;
				Distance[10][12] = 2 + fcc8;
				Distance[10][17] = 2 + fcc9;
				Distance[21][4] = 6 + fcc7;
				Distance[21][12] = 2 + fcc8;
				Distance[21][17] = 2 + fcc9;
				Distance[4][2] = 2 + fcc13;
				Distance[4][11] = 3 + fcc14;
				Distance[4][20] = 5 + fcc15;
				Distance[12][2] = 3 + fcc13;
				Distance[12][11] = 1 + fcc14;
				Distance[12][20] = 3 + fcc15;
				Distance[17][2] = 5 + fcc13;
				Distance[17][11] = 3 + fcc14;
				Distance[17][20] = 3 + fcc15;
				Distance[2][25] = 1;
				Distance[11][25] = 2;
				Distance[20][25] = 4;
			}

			//F3->F1->F4
			if (functionType == 7) {
				Distance[0][4] = 6 + fcc7;
				Distance[0][12] = 4 + fcc8;
				Distance[0][17] = 2 + fcc9;
				Distance[4][3] = 1 + fcc1;
				Distance[4][10] = 3 + fcc2;
				Distance[4][21] = 6 + fcc3;
				Distance[12][3] = 4 + fcc1;
				Distance[12][10] = 2 + fcc2;
				Distance[12][21] = 2 + fcc3;
				Distance[17][3] = 5 + fcc1;
				Distance[17][10] = 2 + fcc2;
				Distance[17][21] = 2 + fcc3;
				Distance[3][6] = 2 + fcc10;
				Distance[3][9] = 3 + fcc11;
				Distance[3][15] = 4 + fcc12;
				Distance[10][6] = 2 + fcc10;
				Distance[10][9] = 1 + fcc11;
				Distance[10][15] = 3 + fcc12;
				Distance[21][6] = 4 + fcc10;
				Distance[21][9] = 3 + fcc11;
				Distance[21][15] = 2 + fcc12;
				Distance[6][25] = 1;
				Distance[9][25] = 2;
				Distance[15][25] = 3;
			}

			//F3->F1->F5
			if (functionType == 8) {
				Distance[0][4] = 6 + fcc7;
				Distance[0][12] = 4 + fcc8;
				Distance[0][17] = 2 + fcc9;
				Distance[4][3] = 1 + fcc1;
				Distance[4][10] = 3 + fcc2;
				Distance[4][21] = 6 + fcc3;
				Distance[12][3] = 4 + fcc1;
				Distance[12][10] = 2 + fcc2;
				Distance[12][21] = 2 + fcc3;
				Distance[17][3] = 5 + fcc1;
				Distance[17][10] = 2 + fcc2;
				Distance[17][21] = 2 + fcc3;
				Distance[3][2] = 1 + fcc13;
				Distance[3][11] = 3 + fcc14;
				Distance[3][20] = 5 + fcc15;
				Distance[10][2] = 3 + fcc13;
				Distance[10][11] = 2 + fcc14;
				Distance[10][20] = 4 + fcc15;
				Distance[21][2] = 5 + fcc13;
				Distance[21][11] = 3 + fcc14;
				Distance[21][20] = 1 + fcc15;
				Distance[2][25] = 1;
				Distance[11][25] = 2;
				Distance[20][25] = 4;
			}

			//F2->F3->F4
			if (functionType == 9) {
				Distance[0][7] = 5 + fcc4;
				Distance[0][13] = 3 + fcc5;
				Distance[0][23] = 3 + fcc6;
				Distance[7][4] = 1 + fcc7;
				Distance[7][12] = 3 + fcc8;
				Distance[7][17] = 4 + fcc9;
				Distance[13][4] = 4 + fcc7;
				Distance[13][12] = 1 + fcc8;
				Distance[13][17] = 1 + fcc9;
				Distance[23][4] = 6 + fcc7;
				Distance[23][12] = 3 + fcc8;
				Distance[23][17] = 1 + fcc9;
				Distance[4][6] = 2 + fcc10;
				Distance[4][9] = 3 + fcc11;
				Distance[4][15] = 4 + fcc12;
				Distance[12][6] = 2 + fcc10;
				Distance[12][9] = 1 + fcc11;
				Distance[12][15] = 2 + fcc12;
				Distance[17][6] = 4 + fcc10;
				Distance[17][9] = 3 + fcc11;
				Distance[17][15] = 2 + fcc12;
				Distance[6][25] = 1;
				Distance[9][25] = 2;
				Distance[15][25] = 3;
			}

			//F2->F3->F5
			if (functionType == 10) {
				Distance[0][7] = 5 + fcc4;
				Distance[0][13] = 3 + fcc5;
				Distance[0][23] = 3 + fcc6;
				Distance[7][4] = 1 + fcc7;
				Distance[7][12] = 3 + fcc8;
				Distance[7][17] = 4 + fcc9;
				Distance[13][4] = 4 + fcc7;
				Distance[13][12] = 1 + fcc8;
				Distance[13][17] = 1 + fcc9;
				Distance[23][4] = 6 + fcc7;
				Distance[23][12] = 3 + fcc8;
				Distance[23][17] = 1 + fcc9;
				Distance[4][2] = 2 + fcc13;
				Distance[4][11] = 3 + fcc14;
				Distance[4][20] = 5 + fcc15;
				Distance[12][2] = 3 + fcc13;
				Distance[12][11] = 1 + fcc14;
				Distance[12][20] = 3 + fcc15;
				Distance[17][2] = 5 + fcc13;
				Distance[17][11] = 3 + fcc14;
				Distance[17][20] = 3 + fcc15;
				Distance[2][25] = 1;
				Distance[11][25] = 2;
				Distance[20][25] = 4;
			}

			//F3->F2->F4
			if (functionType == 11) {
				Distance[0][4] = 6 + fcc4;
				Distance[0][12] = 4 + fcc5;
				Distance[0][17] = 2 + fcc6;
				Distance[4][7] = 2 + fcc7;
				Distance[4][13] = 3 + fcc8;
				Distance[4][23] = 4 + fcc9;
				Distance[12][7] = 3 + fcc7;
				Distance[12][13] = 1 + fcc8;
				Distance[12][23] = 3 + fcc9;
				Distance[17][7] = 4 + fcc7;
				Distance[17][13] = 1 + fcc8;
				Distance[17][23] = 1 + fcc9;
				Distance[7][6] = 1 + fcc10;
				Distance[7][9] = 2 + fcc11;
				Distance[7][15] = 3 + fcc12;
				Distance[13][6] = 3 + fcc10;
				Distance[13][9] = 2 + fcc11;
				Distance[13][15] = 3 + fcc12;
				Distance[23][6] = 5 + fcc10;
				Distance[23][9] = 4 + fcc11;
				Distance[23][15] = 3 + fcc12;
				Distance[6][25] = 1;
				Distance[9][25] = 2;
				Distance[15][25] = 3;
			}

			//F3->F2->F5
			if (functionType == 12) {
				Distance[0][4] = 6 + fcc4;
				Distance[0][12] = 4 + fcc5;
				Distance[0][17] = 2 + fcc6;
				Distance[4][7] = 1 + fcc7;
				Distance[4][13] = 4 + fcc8;
				Distance[4][23] = 6 + fcc9;
				Distance[12][7] = 3 + fcc7;
				Distance[12][13] = 1 + fcc8;
				Distance[12][23] = 3 + fcc9;
				Distance[17][7] = 4 + fcc7;
				Distance[17][13] = 1 + fcc8;
				Distance[17][23] = 1 + fcc9;
				Distance[7][2] = 2 + fcc13;
				Distance[7][11] = 2 + fcc14;
				Distance[7][20] = 4 + fcc15;
				Distance[13][2] = 4 + fcc13;
				Distance[13][11] = 2 + fcc14;
				Distance[13][20] = 4 + fcc15;
				Distance[23][2] = 6 + fcc13;
				Distance[23][11] = 4 + fcc14;
				Distance[23][20] = 3 + fcc15;
				Distance[2][25] = 1;
				Distance[11][25] = 2;
				Distance[20][25] = 4;
			}

		}

	}


	/*
  //for us
  if(flag == 0){
    if (consumerNode == 0){

      //F1->F2->F4
      if (functionType == 1) {
        Distance[0][2] = 2 + fcc1;
        Distance[0][11] = 3 + fcc2;
        Distance[0][18] = 6 + fcc3;
        Distance[2][7] = 2 + fcc4;
        Distance[2][10] = 3 + fcc5;
        Distance[2][16] = 4 + fcc6;
        Distance[11][7] = 2 + fcc4;
        Distance[11][10] = 2 + fcc5;
        Distance[11][16] = 2 + fcc6;
        Distance[18][7] = 4 + fcc4;
        Distance[18][10] = 2 + fcc5;
        Distance[18][16] = 2 + fcc6;
        Distance[7][5] = 2 + fcc10;
        Distance[7][12] = 3 + fcc11;
        Distance[7][21] = 5 + fcc12;
        Distance[10][5] = 2 + fcc10;
        Distance[10][12] = 2 + fcc11;
        Distance[10][21] = 4 + fcc12;
        Distance[16][5] = 5 + fcc10;
        Distance[16][12] = 1 + fcc11;
        Distance[16][21] = 1 + fcc12;
        Distance[5][25] = 1;
        Distance[12][25] = 4;
        Distance[21][25] = 6;
      }

      //F1->F2->F5
      if (functionType == 2) {
        Distance[0][2] = 2 + fcc1;
        Distance[0][11] = 3 + fcc2;
        Distance[0][18] = 6 + fcc3;
        Distance[2][7] = 2 + fcc4;
        Distance[2][10] = 3 + fcc5;
        Distance[2][16] = 4 + fcc6;
        Distance[11][7] = 2 + fcc4;
        Distance[11][10] = 2 + fcc5;
        Distance[11][16] = 2 + fcc6;
        Distance[18][7] = 4 + fcc4;
        Distance[18][10] = 2 + fcc5;
        Distance[18][16] = 2 + fcc6;
        Distance[7][3] = 1 + fcc13;
        Distance[7][9] = 2 + fcc14;
        Distance[7][23] = 5 + fcc15;
        Distance[10][3] = 3 + fcc13;
        Distance[10][9] = 1 + fcc14;
        Distance[10][23] = 3 + fcc15;
        Distance[16][3] = 5 + fcc13;
        Distance[16][9] = 2 + fcc14;
        Distance[16][23] = 2 + fcc15;
        Distance[3][25] = 1;
        Distance[9][25] = 3;
        Distance[23][25] = 6;
      }

      //F2->F1->F4
      if (functionType == 3) {
        Distance[0][7] = 3 + fcc4;
        Distance[0][10] = 4 + fcc5;
        Distance[0][16] = 5 + fcc6;
        Distance[7][2] = 2 + fcc1;
        Distance[7][11] = 2 + fcc2;
        Distance[7][18] = 4 + fcc3;
        Distance[10][2] = 3 + fcc1;
        Distance[10][11] = 2 + fcc2;
        Distance[10][18] = 2 + fcc3;
        Distance[16][2] = 4 + fcc1;
        Distance[16][11] = 2 + fcc2;
        Distance[16][18] = 2 + fcc3;
        Distance[2][5] = 2 + fcc10;
        Distance[2][12] = 3 + fcc11;
        Distance[2][21] = 5 + fcc12;
        Distance[11][5] = 4 + fcc10;
        Distance[11][12] = 1 + fcc11;
        Distance[11][21] = 3 + fcc12;
        Distance[18][5] = 4 + fcc10;
        Distance[18][12] = 3 + fcc11;
        Distance[18][21] = 3 + fcc12;
        Distance[5][25] = 1;
        Distance[12][25] = 4;
        Distance[21][25] = 6;
      }

      //F2->F1->F5
      if (functionType == 4) {
        Distance[0][7] = 3 + fcc4;
        Distance[0][10] = 4 + fcc5;
        Distance[0][16] = 5 + fcc6;
        Distance[7][2] = 2 + fcc1;
        Distance[7][11] = 2 + fcc2;
        Distance[7][18] = 4 + fcc3;
        Distance[10][2] = 3 + fcc1;
        Distance[10][11] = 2 + fcc2;
        Distance[10][18] = 2 + fcc3;
        Distance[16][2] = 4 + fcc1;
        Distance[16][11] = 2 + fcc2;
        Distance[16][18] = 2 + fcc3;
        Distance[2][3] = 1 + fcc13;
        Distance[2][9] = 2 + fcc14;
        Distance[2][23] = 6 + fcc15;
        Distance[11][3] = 3 + fcc13;
        Distance[11][9] = 1 + fcc14;
        Distance[11][23] = 4 + fcc15;
        Distance[18][3] = 5 + fcc13;
        Distance[18][9] = 3 + fcc14;
        Distance[18][23] = 2 + fcc15;
        Distance[3][25] = 1;
        Distance[9][25] = 3;
        Distance[23][25] = 6;
      }

      //F1->F3->F4
      if (functionType == 5) {
        Distance[0][2] = 2 + fcc1;
        Distance[0][11] = 3 + fcc2;
        Distance[0][18] = 6 + fcc3;
        Distance[2][8] = 3 + fcc7;
        Distance[2][13] = 4 + fcc8;
        Distance[2][15] = 3 + fcc9;
        Distance[11][8] = 3 + fcc7;
        Distance[11][13] = 2 + fcc8;
        Distance[11][15] = 1 + fcc9;
        Distance[18][8] = 3 + fcc7;
        Distance[18][13] = 2 + fcc8;
        Distance[18][15] = 3 + fcc9;
        Distance[8][5] = 1 + fcc10;
        Distance[8][12] = 3 + fcc11;
        Distance[8][21] = 5 + fcc12;
        Distance[13][5] = 3 + fcc10;
        Distance[13][12] = 1 + fcc11;
        Distance[13][21] = 3 + fcc12;
        Distance[15][5] = 5 + fcc10;
        Distance[15][12] = 2 + fcc11;
        Distance[15][21] = 2 + fcc12;
        Distance[5][25] = 1;
        Distance[12][25] = 4;
        Distance[21][25] = 6;
      }

      //F1->F3->F5
      if (functionType == 6) {
        Distance[0][2] = 2 + fcc1;
        Distance[0][11] = 3 + fcc2;
        Distance[0][18] = 6 + fcc3;
        Distance[2][8] = 3 + fcc7;
        Distance[2][13] = 4 + fcc8;
        Distance[2][15] = 3 + fcc9;
        Distance[11][8] = 3 + fcc7;
        Distance[11][13] = 2 + fcc8;
        Distance[11][15] = 1 + fcc9;
        Distance[18][8] = 3 + fcc7;
        Distance[18][13] = 2 + fcc8;
        Distance[18][15] = 3 + fcc9;
        Distance[8][3] = 2 + fcc13;
        Distance[8][6] = 2 + fcc14;
        Distance[8][23] = 4 + fcc15;
        Distance[13][3] = 4 + fcc13;
        Distance[13][6] = 2 + fcc14;
        Distance[13][23] = 2 + fcc15;
        Distance[15][3] = 4 + fcc13;
        Distance[15][6] = 2 + fcc14;
        Distance[15][23] = 3 + fcc15;
        Distance[3][25] = 1;
        Distance[9][25] = 3;
        Distance[23][25] = 6;
      }

      //F3->F1->F4
      if (functionType == 7) {
        Distance[0][8] = 4 + fcc7;
        Distance[0][13] = 5 + fcc8;
        Distance[0][15] = 4 + fcc9;
        Distance[8][2] = 3 + fcc1;
        Distance[8][11] = 3 + fcc2;
        Distance[8][18] = 3 + fcc3;
        Distance[13][2] = 4 + fcc1;
        Distance[13][11] = 2 + fcc2;
        Distance[13][18] = 2 + fcc3;
        Distance[15][2] = 3 + fcc1;
        Distance[15][11] = 1 + fcc2;
        Distance[15][18] = 3 + fcc3;
        Distance[2][5] = 2 + fcc10;
        Distance[2][12] = 3 + fcc11;
        Distance[2][21] = 5 + fcc12;
        Distance[11][5] = 4 + fcc10;
        Distance[11][12] = 1 + fcc11;
        Distance[11][21] = 3 + fcc12;
        Distance[18][5] = 4 + fcc10;
        Distance[18][12] = 3 + fcc11;
        Distance[18][21] = 3 + fcc12;
        Distance[5][25] = 1;
        Distance[12][25] = 4;
        Distance[21][25] = 6;
      }

      //F3->F1->F5
      if (functionType == 8) {
        Distance[0][8] = 4 + fcc7;
        Distance[0][13] = 5 + fcc8;
        Distance[0][15] = 4 + fcc9;
        Distance[8][2] = 3 + fcc1;
        Distance[8][11] = 3 + fcc2;
        Distance[8][18] = 3 + fcc3;
        Distance[13][2] = 4 + fcc1;
        Distance[13][11] = 2 + fcc2;
        Distance[13][18] = 2 + fcc3;
        Distance[15][2] = 3 + fcc1;
        Distance[15][11] = 1 + fcc2;
        Distance[15][18] = 3 + fcc3;
        Distance[2][3] = 1 + fcc13;
        Distance[2][9] = 2 + fcc14;
        Distance[2][23] = 6 + fcc15;
        Distance[11][3] = 3 + fcc13;
        Distance[11][9] = 1 + fcc14;
        Distance[11][23] = 4 + fcc15;
        Distance[18][3] = 5 + fcc13;
        Distance[18][9] = 3 + fcc14;
        Distance[18][23] = 2 + fcc15;
        Distance[3][25] = 1;
        Distance[9][25] = 3;
        Distance[23][25] = 6;
      }

      //F2->F3->F4
      if (functionType == 9) {
        Distance[0][7] = 3 + fcc4;
        Distance[0][10] = 4 + fcc5;
        Distance[0][16] = 5 + fcc6;
        Distance[7][8] = 1 + fcc7;
        Distance[7][13] = 3 + fcc8;
        Distance[7][15] = 3 + fcc9;
        Distance[10][8] = 1 + fcc7;
        Distance[10][13] = 1 + fcc8;
        Distance[10][15] = 3 + fcc9;
        Distance[16][8] = 4 + fcc7;
        Distance[16][13] = 2 + fcc8;
        Distance[16][15] = 1 + fcc9;
        Distance[8][5] = 1 + fcc10;
        Distance[8][12] = 3 + fcc11;
        Distance[8][21] = 5 + fcc12;
        Distance[13][5] = 3 + fcc10;
        Distance[13][12] = 1 + fcc11;
        Distance[13][21] = 3 + fcc12;
        Distance[15][5] = 5 + fcc10;
        Distance[15][12] = 2 + fcc11;
        Distance[15][21] = 2 + fcc12;
        Distance[5][25] = 1;
        Distance[12][25] = 4;
        Distance[21][25] = 6;
      }

      //F2->F3->F5
      if (functionType == 10) {
        Distance[0][7] = 3 + fcc4;
        Distance[0][10] = 4 + fcc5;
        Distance[0][16] = 5 + fcc6;
        Distance[7][8] = 1 + fcc7;
        Distance[7][13] = 3 + fcc8;
        Distance[7][15] = 3 + fcc9;
        Distance[10][8] = 1 + fcc7;
        Distance[10][13] = 1 + fcc8;
        Distance[10][15] = 3 + fcc9;
        Distance[16][8] = 4 + fcc7;
        Distance[16][13] = 2 + fcc8;
        Distance[16][15] = 1 + fcc9;
        Distance[8][3] = 2 + fcc13;
        Distance[8][9] = 2 + fcc14;
        Distance[8][23] = 4 + fcc15;
        Distance[13][3] = 4 + fcc13;
        Distance[13][9] = 2 + fcc14;
        Distance[13][23] = 2 + fcc15;
        Distance[15][3] = 4 + fcc13;
        Distance[15][9] = 2 + fcc14;
        Distance[15][23] = 3 + fcc15;
        Distance[3][25] = 1;
        Distance[9][25] = 3;
        Distance[23][25] = 6;
      }

      //F3->F2->F4
      if (functionType == 11) {
        Distance[0][8] = 4 + fcc7;
        Distance[0][13] = 5 + fcc8;
        Distance[0][15] = 4 + fcc9;
        Distance[8][7] = 1 + fcc4;
        Distance[8][10] = 1 + fcc5;
        Distance[8][16] = 4 + fcc6;
        Distance[13][7] = 3 + fcc4;
        Distance[13][10] = 1 + fcc5;
        Distance[13][16] = 2 + fcc6;
        Distance[15][7] = 3 + fcc4;
        Distance[15][10] = 3 + fcc5;
        Distance[15][16] = 1 + fcc6;
        Distance[7][5] = 2 + fcc10;
        Distance[7][12] = 3 + fcc11;
        Distance[7][21] = 5 + fcc12;
        Distance[10][5] = 2 + fcc10;
        Distance[10][12] = 2 + fcc11;
        Distance[10][21] = 4 + fcc12;
        Distance[16][5] = 5 + fcc10;
        Distance[16][12] = 1 + fcc11;
        Distance[16][21] = 1 + fcc12;
        Distance[5][25] = 1;
        Distance[12][25] = 4;
        Distance[21][25] = 6;
      }

      //F3->F2->F5
      if (functionType == 12) {
        Distance[0][8] = 4 + fcc7;
        Distance[0][13] = 5 + fcc8;
        Distance[0][15] = 4 + fcc9;
        Distance[8][7] = 1 + fcc4;
        Distance[8][10] = 1 + fcc5;
        Distance[8][16] = 4 + fcc6;
        Distance[13][7] = 3 + fcc4;
        Distance[13][10] = 1 + fcc5;
        Distance[13][16] = 2 + fcc6;
        Distance[15][7] = 3 + fcc4;
        Distance[15][10] = 3 + fcc5;
        Distance[15][16] = 1 + fcc6;
        Distance[7][3] = 1 + fcc13;
        Distance[7][9] = 2 + fcc14;
        Distance[7][23] = 5 + fcc15;
        Distance[10][3] = 3 + fcc13;
        Distance[10][9] = 1 + fcc14;
        Distance[10][23] = 3 + fcc15;
        Distance[16][3] = 5 + fcc13;
        Distance[16][9] = 2 + fcc14;
        Distance[16][23] = 2 + fcc15;
        Distance[3][25] = 1;
        Distance[9][25] = 3;
        Distance[23][25] = 6;
      }

    }

    if (consumerNode == 41){

      //F1->F2->F4
      if (functionType == 1) {
        Distance[0][2] = 4 + fcc1;
        Distance[0][11] = 2 + fcc2;
        Distance[0][18] = 6 + fcc3;
        Distance[2][7] = 2 + fcc4;
        Distance[2][10] = 3 + fcc5;
        Distance[2][16] = 4 + fcc6;
        Distance[11][7] = 2 + fcc4;
        Distance[11][10] = 2 + fcc5;
        Distance[11][16] = 2 + fcc6;
        Distance[18][7] = 4 + fcc4;
        Distance[18][10] = 2 + fcc5;
        Distance[18][16] = 2 + fcc6;
        Distance[7][5] = 2 + fcc10;
        Distance[7][12] = 3 + fcc11;
        Distance[7][21] = 5 + fcc12;
        Distance[10][5] = 2 + fcc10;
        Distance[10][12] = 2 + fcc11;
        Distance[10][21] = 4 + fcc12;
        Distance[16][5] = 5 + fcc10;
        Distance[16][12] = 1 + fcc11;
        Distance[16][21] = 1 + fcc12;
        Distance[5][25] = 5;
        Distance[12][25] = 2;
        Distance[21][25] = 1;
      }

      //F1->F2->F5
      if (functionType == 2) {
        Distance[0][2] = 4 + fcc1;
        Distance[0][11] = 2 + fcc2;
        Distance[0][18] = 6 + fcc3;
        Distance[2][7] = 2 + fcc4;
        Distance[2][10] = 3 + fcc5;
        Distance[2][16] = 4 + fcc6;
        Distance[11][7] = 2 + fcc4;
        Distance[11][10] = 2 + fcc5;
        Distance[11][16] = 2 + fcc6;
        Distance[18][7] = 4 + fcc4;
        Distance[18][10] = 2 + fcc5;
        Distance[18][16] = 2 + fcc6;
        Distance[7][3] = 1 + fcc13;
        Distance[7][9] = 2 + fcc14;
        Distance[7][23] = 5 + fcc15;
        Distance[10][3] = 3 + fcc13;
        Distance[10][9] = 1 + fcc14;
        Distance[10][23] = 3 + fcc15;
        Distance[16][3] = 5 + fcc13;
        Distance[16][9] = 2 + fcc14;
        Distance[16][23] = 2 + fcc15;
        Distance[3][25] = 6;
        Distance[9][25] = 3;
        Distance[23][25] = 1;
      }

      //F2->F1->F4
      if (functionType == 3) {
        Distance[0][7] = 4 + fcc4;
        Distance[0][10] = 4 + fcc5;
        Distance[0][16] = 4 + fcc6;
        Distance[7][2] = 2 + fcc1;
        Distance[7][11] = 2 + fcc2;
        Distance[7][18] = 4 + fcc3;
        Distance[10][2] = 3 + fcc1;
        Distance[10][11] = 2 + fcc2;
        Distance[10][18] = 2 + fcc3;
        Distance[16][2] = 4 + fcc1;
        Distance[16][11] = 2 + fcc2;
        Distance[16][18] = 2 + fcc3;
        Distance[2][5] = 2 + fcc10;
        Distance[2][12] = 3 + fcc11;
        Distance[2][21] = 5 + fcc12;
        Distance[11][5] = 4 + fcc10;
        Distance[11][12] = 1 + fcc11;
        Distance[11][21] = 3 + fcc12;
        Distance[18][5] = 4 + fcc10;
        Distance[18][12] = 3 + fcc11;
        Distance[18][21] = 3 + fcc12;
        Distance[5][25] = 5;
        Distance[12][25] = 2;
        Distance[21][25] = 1;
      }

      //F2->F1->F5
      if (functionType == 4) {
        Distance[0][7] = 4 + fcc4;
        Distance[0][10] = 4 + fcc5;
        Distance[0][16] = 4 + fcc6;
        Distance[7][2] = 2 + fcc1;
        Distance[7][11] = 2 + fcc2;
        Distance[7][18] = 4 + fcc3;
        Distance[10][2] = 3 + fcc1;
        Distance[10][11] = 2 + fcc2;
        Distance[10][18] = 2 + fcc3;
        Distance[16][2] = 4 + fcc1;
        Distance[16][11] = 2 + fcc2;
        Distance[16][18] = 2 + fcc3;
        Distance[2][3] = 1 + fcc13;
        Distance[2][9] = 2 + fcc14;
        Distance[2][23] = 6 + fcc15;
        Distance[11][3] = 3 + fcc13;
        Distance[11][9] = 1 + fcc14;
        Distance[11][23] = 4 + fcc15;
        Distance[18][3] = 5 + fcc13;
        Distance[18][9] = 3 + fcc14;
        Distance[18][23] = 2 + fcc15;
        Distance[3][25] = 6;
        Distance[9][25] = 3;
        Distance[23][25] = 1;
      }

      //F1->F3->F4
      if (functionType == 5) {
        Distance[0][2] = 4 + fcc1;
        Distance[0][11] = 2 + fcc2;
        Distance[0][18] = 6 + fcc3;
        Distance[2][8] = 3 + fcc7;
        Distance[2][13] = 4 + fcc8;
        Distance[2][15] = 3 + fcc9;
        Distance[11][8] = 3 + fcc7;
        Distance[11][13] = 2 + fcc8;
        Distance[11][15] = 1 + fcc9;
        Distance[18][8] = 3 + fcc7;
        Distance[18][13] = 2 + fcc8;
        Distance[18][15] = 3 + fcc9;
        Distance[8][5] = 1 + fcc10;
        Distance[8][12] = 3 + fcc11;
        Distance[8][21] = 5 + fcc12;
        Distance[13][5] = 3 + fcc10;
        Distance[13][12] = 1 + fcc11;
        Distance[13][21] = 3 + fcc12;
        Distance[15][5] = 5 + fcc10;
        Distance[15][12] = 2 + fcc11;
        Distance[15][21] = 2 + fcc12;
        Distance[5][25] = 5;
        Distance[12][25] = 2;
        Distance[21][25] = 1;
      }

      //F1->F3->F5
      if (functionType == 6) {
        Distance[0][2] = 4 + fcc1;
        Distance[0][11] = 2 + fcc2;
        Distance[0][18] = 6 + fcc3;
        Distance[2][8] = 3 + fcc7;
        Distance[2][13] = 4 + fcc8;
        Distance[2][15] = 3 + fcc9;
        Distance[11][8] = 3 + fcc7;
        Distance[11][13] = 2 + fcc8;
        Distance[11][15] = 1 + fcc9;
        Distance[18][8] = 3 + fcc7;
        Distance[18][13] = 2 + fcc8;
        Distance[18][15] = 3 + fcc9;
        Distance[8][3] = 2 + fcc13;
        Distance[8][6] = 2 + fcc14;
        Distance[8][23] = 4 + fcc15;
        Distance[13][3] = 4 + fcc13;
        Distance[13][6] = 2 + fcc14;
        Distance[13][23] = 2 + fcc15;
        Distance[15][3] = 4 + fcc13;
        Distance[15][6] = 2 + fcc14;
        Distance[15][23] = 3 + fcc15;
        Distance[3][25] = 6;
        Distance[9][25] = 3;
        Distance[23][25] = 1;
      }

      //F3->F1->F4
      if (functionType == 7) {
        Distance[0][8] = 5 + fcc7;
        Distance[0][13] = 4 + fcc8;
        Distance[0][15] = 3 + fcc9;
        Distance[8][2] = 3 + fcc1;
        Distance[8][11] = 3 + fcc2;
        Distance[8][18] = 3 + fcc3;
        Distance[13][2] = 4 + fcc1;
        Distance[13][11] = 2 + fcc2;
        Distance[13][18] = 2 + fcc3;
        Distance[15][2] = 3 + fcc1;
        Distance[15][11] = 1 + fcc2;
        Distance[15][18] = 3 + fcc3;
        Distance[2][5] = 2 + fcc10;
        Distance[2][12] = 3 + fcc11;
        Distance[2][21] = 5 + fcc12;
        Distance[11][5] = 4 + fcc10;
        Distance[11][12] = 1 + fcc11;
        Distance[11][21] = 3 + fcc12;
        Distance[18][5] = 4 + fcc10;
        Distance[18][12] = 3 + fcc11;
        Distance[18][21] = 3 + fcc12;
        Distance[5][25] = 5;
        Distance[12][25] = 2;
        Distance[21][25] = 1;
      }

      //F3->F1->F5
      if (functionType == 8) {
        Distance[0][8] = 5 + fcc7;
        Distance[0][13] = 4 + fcc8;
        Distance[0][15] = 3 + fcc9;
        Distance[8][2] = 3 + fcc1;
        Distance[8][11] = 3 + fcc2;
        Distance[8][18] = 3 + fcc3;
        Distance[13][2] = 4 + fcc1;
        Distance[13][11] = 2 + fcc2;
        Distance[13][18] = 2 + fcc3;
        Distance[15][2] = 3 + fcc1;
        Distance[15][11] = 1 + fcc2;
        Distance[15][18] = 3 + fcc3;
        Distance[2][3] = 1 + fcc13;
        Distance[2][9] = 2 + fcc14;
        Distance[2][23] = 6 + fcc15;
        Distance[11][3] = 3 + fcc13;
        Distance[11][9] = 1 + fcc14;
        Distance[11][23] = 4 + fcc15;
        Distance[18][3] = 5 + fcc13;
        Distance[18][9] = 3 + fcc14;
        Distance[18][23] = 2 + fcc15;
        Distance[3][25] = 6;
        Distance[9][25] = 3;
        Distance[23][25] = 1;
      }

      //F2->F3->F4
      if (functionType == 9) {
        Distance[0][7] = 4 + fcc4;
        Distance[0][10] = 4 + fcc5;
        Distance[0][16] = 4 + fcc6;
        Distance[7][8] = 1 + fcc7;
        Distance[7][13] = 3 + fcc8;
        Distance[7][15] = 3 + fcc9;
        Distance[10][8] = 1 + fcc7;
        Distance[10][13] = 1 + fcc8;
        Distance[10][15] = 3 + fcc9;
        Distance[16][8] = 4 + fcc7;
        Distance[16][13] = 2 + fcc8;
        Distance[16][15] = 1 + fcc9;
        Distance[8][5] = 1 + fcc10;
        Distance[8][12] = 3 + fcc11;
        Distance[8][21] = 5 + fcc12;
        Distance[13][5] = 3 + fcc10;
        Distance[13][12] = 1 + fcc11;
        Distance[13][21] = 3 + fcc12;
        Distance[15][5] = 5 + fcc10;
        Distance[15][12] = 2 + fcc11;
        Distance[15][21] = 2 + fcc12;
        Distance[5][25] = 5;
        Distance[12][25] = 2;
        Distance[21][25] = 1;
      }

      //F2->F3->F5
      if (functionType == 10) {
        Distance[0][7] = 4 + fcc4;
        Distance[0][10] = 4 + fcc5;
        Distance[0][16] = 4 + fcc6;
        Distance[7][8] = 1 + fcc7;
        Distance[7][13] = 3 + fcc8;
        Distance[7][15] = 3 + fcc9;
        Distance[10][8] = 1 + fcc7;
        Distance[10][13] = 1 + fcc8;
        Distance[10][15] = 3 + fcc9;
        Distance[16][8] = 4 + fcc7;
        Distance[16][13] = 2 + fcc8;
        Distance[16][15] = 1 + fcc9;
        Distance[8][3] = 2 + fcc13;
        Distance[8][9] = 2 + fcc14;
        Distance[8][23] = 4 + fcc15;
        Distance[13][3] = 4 + fcc13;
        Distance[13][9] = 2 + fcc14;
        Distance[13][23] = 2 + fcc15;
        Distance[15][3] = 4 + fcc13;
        Distance[15][9] = 2 + fcc14;
        Distance[15][23] = 3 + fcc15;
        Distance[3][25] = 6;
        Distance[9][25] = 3;
        Distance[23][25] = 1;
      }

      //F3->F2->F4
      if (functionType == 11) {
        Distance[0][8] = 5 + fcc7;
        Distance[0][13] = 4 + fcc8;
        Distance[0][15] = 3 + fcc9;
        Distance[8][7] = 1 + fcc4;
        Distance[8][10] = 1 + fcc5;
        Distance[8][16] = 4 + fcc6;
        Distance[13][7] = 3 + fcc4;
        Distance[13][10] = 1 + fcc5;
        Distance[13][16] = 2 + fcc6;
        Distance[15][7] = 3 + fcc4;
        Distance[15][10] = 3 + fcc5;
        Distance[15][16] = 1 + fcc6;
        Distance[7][5] = 2 + fcc10;
        Distance[7][12] = 3 + fcc11;
        Distance[7][21] = 5 + fcc12;
        Distance[10][5] = 2 + fcc10;
        Distance[10][12] = 2 + fcc11;
        Distance[10][21] = 4 + fcc12;
        Distance[16][5] = 5 + fcc10;
        Distance[16][12] = 1 + fcc11;
        Distance[16][21] = 1 + fcc12;
        Distance[5][25] = 5;
        Distance[12][25] = 2;
        Distance[21][25] = 1;
      }

      //F3->F2->F5
      if (functionType == 12) {
        Distance[0][8] = 5 + fcc7;
        Distance[0][13] = 4 + fcc8;
        Distance[0][15] = 3 + fcc9;
        Distance[8][7] = 1 + fcc4;
        Distance[8][10] = 1 + fcc5;
        Distance[8][16] = 4 + fcc6;
        Distance[13][7] = 3 + fcc4;
        Distance[13][10] = 1 + fcc5;
        Distance[13][16] = 2 + fcc6;
        Distance[15][7] = 3 + fcc4;
        Distance[15][10] = 3 + fcc5;
        Distance[15][16] = 1 + fcc6;
        Distance[7][3] = 1 + fcc13;
        Distance[7][9] = 2 + fcc14;
        Distance[7][23] = 5 + fcc15;
        Distance[10][3] = 3 + fcc13;
        Distance[10][9] = 1 + fcc14;
        Distance[10][23] = 3 + fcc15;
        Distance[16][3] = 5 + fcc13;
        Distance[16][9] = 2 + fcc14;
        Distance[16][23] = 2 + fcc15;
        Distance[3][25] = 6;
        Distance[9][25] = 3;
        Distance[23][25] = 1;
      }

    }

    if (consumerNode == 42){

      //F1->F2->F4
      if (functionType == 1) {
        Distance[0][2] = 7 + fcc1;
        Distance[0][11] = 6 + fcc2;
        Distance[0][18] = 2 + fcc3;
        Distance[2][7] = 2 + fcc4;
        Distance[2][10] = 3 + fcc5;
        Distance[2][16] = 4 + fcc6;
        Distance[11][7] = 2 + fcc4;
        Distance[11][10] = 2 + fcc5;
        Distance[11][16] = 2 + fcc6;
        Distance[18][7] = 4 + fcc4;
        Distance[18][10] = 2 + fcc5;
        Distance[18][16] = 2 + fcc6;
        Distance[7][5] = 2 + fcc10;
        Distance[7][12] = 3 + fcc11;
        Distance[7][21] = 5 + fcc12;
        Distance[10][5] = 2 + fcc10;
        Distance[10][12] = 2 + fcc11;
        Distance[10][21] = 4 + fcc12;
        Distance[16][5] = 5 + fcc10;
        Distance[16][12] = 1 + fcc11;
        Distance[16][21] = 1 + fcc12;
        Distance[5][25] = 1;
        Distance[12][25] = 4;
        Distance[21][25] = 6;
      }

      //F1->F2->F5
      if (functionType == 2) {
        Distance[0][2] = 7 + fcc1;
        Distance[0][11] = 6 + fcc2;
        Distance[0][18] = 2 + fcc3;
        Distance[2][7] = 2 + fcc4;
        Distance[2][10] = 3 + fcc5;
        Distance[2][16] = 4 + fcc6;
        Distance[11][7] = 2 + fcc4;
        Distance[11][10] = 2 + fcc5;
        Distance[11][16] = 2 + fcc6;
        Distance[18][7] = 4 + fcc4;
        Distance[18][10] = 2 + fcc5;
        Distance[18][16] = 2 + fcc6;
        Distance[7][3] = 1 + fcc13;
        Distance[7][9] = 2 + fcc14;
        Distance[7][23] = 5 + fcc15;
        Distance[10][3] = 3 + fcc13;
        Distance[10][9] = 1 + fcc14;
        Distance[10][23] = 3 + fcc15;
        Distance[16][3] = 5 + fcc13;
        Distance[16][9] = 2 + fcc14;
        Distance[16][23] = 2 + fcc15;
        Distance[3][25] = 1;
        Distance[9][25] = 3;
        Distance[23][25] = 6;
      }

      //F2->F1->F4
      if (functionType == 3) {
        Distance[0][7] = 6 + fcc4;
        Distance[0][10] = 4 + fcc5;
        Distance[0][16] = 4 + fcc6;
        Distance[7][2] = 2 + fcc1;
        Distance[7][11] = 2 + fcc2;
        Distance[7][18] = 4 + fcc3;
        Distance[10][2] = 3 + fcc1;
        Distance[10][11] = 2 + fcc2;
        Distance[10][18] = 2 + fcc3;
        Distance[16][2] = 4 + fcc1;
        Distance[16][11] = 2 + fcc2;
        Distance[16][18] = 2 + fcc3;
        Distance[2][5] = 2 + fcc10;
        Distance[2][12] = 3 + fcc11;
        Distance[2][21] = 5 + fcc12;
        Distance[11][5] = 4 + fcc10;
        Distance[11][12] = 1 + fcc11;
        Distance[11][21] = 3 + fcc12;
        Distance[18][5] = 4 + fcc10;
        Distance[18][12] = 3 + fcc11;
        Distance[18][21] = 3 + fcc12;
        Distance[5][25] = 1;
        Distance[12][25] = 4;
        Distance[21][25] = 6;
      }

      //F2->F1->F5
      if (functionType == 4) {
        Distance[0][7] = 6 + fcc4;
        Distance[0][10] = 4 + fcc5;
        Distance[0][16] = 4 + fcc6;
        Distance[7][2] = 2 + fcc1;
        Distance[7][11] = 2 + fcc2;
        Distance[7][18] = 4 + fcc3;
        Distance[10][2] = 3 + fcc1;
        Distance[10][11] = 2 + fcc2;
        Distance[10][18] = 2 + fcc3;
        Distance[16][2] = 4 + fcc1;
        Distance[16][11] = 2 + fcc2;
        Distance[16][18] = 2 + fcc3;
        Distance[2][3] = 1 + fcc13;
        Distance[2][9] = 2 + fcc14;
        Distance[2][23] = 6 + fcc15;
        Distance[11][3] = 3 + fcc13;
        Distance[11][9] = 1 + fcc14;
        Distance[11][23] = 4 + fcc15;
        Distance[18][3] = 5 + fcc13;
        Distance[18][9] = 3 + fcc14;
        Distance[18][23] = 2 + fcc15;
        Distance[3][25] = 1;
        Distance[9][25] = 3;
        Distance[23][25] = 6;
      }

      //F1->F3->F4
      if (functionType == 5) {
        Distance[0][2] = 7 + fcc1;
        Distance[0][11] = 6 + fcc2;
        Distance[0][18] = 2 + fcc3;
        Distance[2][8] = 3 + fcc7;
        Distance[2][13] = 4 + fcc8;
        Distance[2][15] = 3 + fcc9;
        Distance[11][8] = 3 + fcc7;
        Distance[11][13] = 2 + fcc8;
        Distance[11][15] = 1 + fcc9;
        Distance[18][8] = 3 + fcc7;
        Distance[18][13] = 2 + fcc8;
        Distance[18][15] = 3 + fcc9;
        Distance[8][5] = 1 + fcc10;
        Distance[8][12] = 3 + fcc11;
        Distance[8][21] = 5 + fcc12;
        Distance[13][5] = 3 + fcc10;
        Distance[13][12] = 1 + fcc11;
        Distance[13][21] = 3 + fcc12;
        Distance[15][5] = 5 + fcc10;
        Distance[15][12] = 2 + fcc11;
        Distance[15][21] = 2 + fcc12;
        Distance[5][25] = 1;
        Distance[12][25] = 4;
        Distance[21][25] = 6;
      }

      //F1->F3->F5
      if (functionType == 6) {
        Distance[0][2] = 7 + fcc1;
        Distance[0][11] = 6 + fcc2;
        Distance[0][18] = 2 + fcc3;
        Distance[2][8] = 3 + fcc7;
        Distance[2][13] = 4 + fcc8;
        Distance[2][15] = 3 + fcc9;
        Distance[11][8] = 3 + fcc7;
        Distance[11][13] = 2 + fcc8;
        Distance[11][15] = 1 + fcc9;
        Distance[18][8] = 3 + fcc7;
        Distance[18][13] = 2 + fcc8;
        Distance[18][15] = 3 + fcc9;
        Distance[8][3] = 2 + fcc13;
        Distance[8][6] = 2 + fcc14;
        Distance[8][23] = 4 + fcc15;
        Distance[13][3] = 4 + fcc13;
        Distance[13][6] = 2 + fcc14;
        Distance[13][23] = 2 + fcc15;
        Distance[15][3] = 4 + fcc13;
        Distance[15][6] = 2 + fcc14;
        Distance[15][23] = 3 + fcc15;
        Distance[3][25] = 1;
        Distance[9][25] = 3;
        Distance[23][25] = 6;
      }

      //F3->F1->F4
      if (functionType == 7) {
        Distance[0][8] = 5 + fcc7;
        Distance[0][13] = 4 + fcc8;
        Distance[0][15] = 5 + fcc9;
        Distance[8][2] = 3 + fcc1;
        Distance[8][11] = 3 + fcc2;
        Distance[8][18] = 3 + fcc3;
        Distance[13][2] = 4 + fcc1;
        Distance[13][11] = 2 + fcc2;
        Distance[13][18] = 2 + fcc3;
        Distance[15][2] = 3 + fcc1;
        Distance[15][11] = 1 + fcc2;
        Distance[15][18] = 3 + fcc3;
        Distance[2][5] = 2 + fcc10;
        Distance[2][12] = 3 + fcc11;
        Distance[2][21] = 5 + fcc12;
        Distance[11][5] = 4 + fcc10;
        Distance[11][12] = 1 + fcc11;
        Distance[11][21] = 3 + fcc12;
        Distance[18][5] = 4 + fcc10;
        Distance[18][12] = 3 + fcc11;
        Distance[18][21] = 3 + fcc12;
        Distance[5][25] = 1;
        Distance[12][25] = 4;
        Distance[21][25] = 6;
      }

      //F3->F1->F5
      if (functionType == 8) {
        Distance[0][8] = 5 + fcc7;
        Distance[0][13] = 4 + fcc8;
        Distance[0][15] = 5 + fcc9;
        Distance[8][2] = 3 + fcc1;
        Distance[8][11] = 3 + fcc2;
        Distance[8][18] = 3 + fcc3;
        Distance[13][2] = 4 + fcc1;
        Distance[13][11] = 2 + fcc2;
        Distance[13][18] = 2 + fcc3;
        Distance[15][2] = 3 + fcc1;
        Distance[15][11] = 1 + fcc2;
        Distance[15][18] = 3 + fcc3;
        Distance[2][3] = 1 + fcc13;
        Distance[2][9] = 2 + fcc14;
        Distance[2][23] = 6 + fcc15;
        Distance[11][3] = 3 + fcc13;
        Distance[11][9] = 1 + fcc14;
        Distance[11][23] = 4 + fcc15;
        Distance[18][3] = 5 + fcc13;
        Distance[18][9] = 3 + fcc14;
        Distance[18][23] = 2 + fcc15;
        Distance[3][25] = 1;
        Distance[9][25] = 3;
        Distance[23][25] = 6;
      }

      //F2->F3->F4
      if (functionType == 9) {
        Distance[0][7] = 6 + fcc4;
        Distance[0][10] = 4 + fcc5;
        Distance[0][16] = 4 + fcc6;
        Distance[7][8] = 1 + fcc7;
        Distance[7][13] = 3 + fcc8;
        Distance[7][15] = 3 + fcc9;
        Distance[10][8] = 1 + fcc7;
        Distance[10][13] = 1 + fcc8;
        Distance[10][15] = 3 + fcc9;
        Distance[16][8] = 4 + fcc7;
        Distance[16][13] = 2 + fcc8;
        Distance[16][15] = 1 + fcc9;
        Distance[8][5] = 1 + fcc10;
        Distance[8][12] = 3 + fcc11;
        Distance[8][21] = 5 + fcc12;
        Distance[13][5] = 3 + fcc10;
        Distance[13][12] = 1 + fcc11;
        Distance[13][21] = 3 + fcc12;
        Distance[15][5] = 5 + fcc10;
        Distance[15][12] = 2 + fcc11;
        Distance[15][21] = 2 + fcc12;
        Distance[5][25] = 1;
        Distance[12][25] = 4;
        Distance[21][25] = 6;
      }

      //F2->F3->F5
      if (functionType == 10) {
        Distance[0][7] = 6 + fcc4;
        Distance[0][10] = 4 + fcc5;
        Distance[0][16] = 4 + fcc6;
        Distance[7][8] = 1 + fcc7;
        Distance[7][13] = 3 + fcc8;
        Distance[7][15] = 3 + fcc9;
        Distance[10][8] = 1 + fcc7;
        Distance[10][13] = 1 + fcc8;
        Distance[10][15] = 3 + fcc9;
        Distance[16][8] = 4 + fcc7;
        Distance[16][13] = 2 + fcc8;
        Distance[16][15] = 1 + fcc9;
        Distance[8][3] = 2 + fcc13;
        Distance[8][9] = 2 + fcc14;
        Distance[8][23] = 4 + fcc15;
        Distance[13][3] = 4 + fcc13;
        Distance[13][9] = 2 + fcc14;
        Distance[13][23] = 2 + fcc15;
        Distance[15][3] = 4 + fcc13;
        Distance[15][9] = 2 + fcc14;
        Distance[15][23] = 3 + fcc15;
        Distance[3][25] = 1;
        Distance[9][25] = 3;
        Distance[23][25] = 6;
      }

      //F3->F2->F4
      if (functionType == 11) {
        Distance[0][8] = 5 + fcc7;
        Distance[0][13] = 4 + fcc8;
        Distance[0][15] = 5 + fcc9;
        Distance[8][7] = 1 + fcc4;
        Distance[8][10] = 1 + fcc5;
        Distance[8][16] = 4 + fcc6;
        Distance[13][7] = 3 + fcc4;
        Distance[13][10] = 1 + fcc5;
        Distance[13][16] = 2 + fcc6;
        Distance[15][7] = 3 + fcc4;
        Distance[15][10] = 3 + fcc5;
        Distance[15][16] = 1 + fcc6;
        Distance[7][5] = 2 + fcc10;
        Distance[7][12] = 3 + fcc11;
        Distance[7][21] = 5 + fcc12;
        Distance[10][5] = 2 + fcc10;
        Distance[10][12] = 2 + fcc11;
        Distance[10][21] = 4 + fcc12;
        Distance[16][5] = 5 + fcc10;
        Distance[16][12] = 1 + fcc11;
        Distance[16][21] = 1 + fcc12;
        Distance[5][25] = 1;
        Distance[12][25] = 4;
        Distance[21][25] = 6;
      }

      //F3->F2->F5
      if (functionType == 12) {
        Distance[0][8] = 5 + fcc7;
        Distance[0][13] = 4 + fcc8;
        Distance[0][15] = 5 + fcc9;
        Distance[8][7] = 1 + fcc4;
        Distance[8][10] = 1 + fcc5;
        Distance[8][16] = 4 + fcc6;
        Distance[13][7] = 3 + fcc4;
        Distance[13][10] = 1 + fcc5;
        Distance[13][16] = 2 + fcc6;
        Distance[15][7] = 3 + fcc4;
        Distance[15][10] = 3 + fcc5;
        Distance[15][16] = 1 + fcc6;
        Distance[7][3] = 1 + fcc13;
        Distance[7][9] = 2 + fcc14;
        Distance[7][23] = 5 + fcc15;
        Distance[10][3] = 3 + fcc13;
        Distance[10][9] = 1 + fcc14;
        Distance[10][23] = 3 + fcc15;
        Distance[16][3] = 5 + fcc13;
        Distance[16][9] = 2 + fcc14;
        Distance[16][23] = 2 + fcc15;
        Distance[3][25] = 1;
        Distance[9][25] = 3;
        Distance[23][25] = 6;
      }

    }

    if (consumerNode == 43){

      //F1->F2->F4
      if (functionType == 1) {
        Distance[0][2] = 3 + fcc1;
        Distance[0][11] = 5 + fcc2;
        Distance[0][18] = 5 + fcc3;
        Distance[2][7] = 2 + fcc4;
        Distance[2][10] = 3 + fcc5;
        Distance[2][16] = 4 + fcc6;
        Distance[11][7] = 2 + fcc4;
        Distance[11][10] = 2 + fcc5;
        Distance[11][16] = 2 + fcc6;
        Distance[18][7] = 4 + fcc4;
        Distance[18][10] = 2 + fcc5;
        Distance[18][16] = 2 + fcc6;
        Distance[7][5] = 2 + fcc10;
        Distance[7][12] = 3 + fcc11;
        Distance[7][21] = 5 + fcc12;
        Distance[10][5] = 2 + fcc10;
        Distance[10][12] = 2 + fcc11;
        Distance[10][21] = 4 + fcc12;
        Distance[16][5] = 5 + fcc10;
        Distance[16][12] = 1 + fcc11;
        Distance[16][21] = 1 + fcc12;
        Distance[5][25] = 5;
        Distance[12][25] = 2;
        Distance[21][25] = 1;
      }

      //F1->F2->F5
      if (functionType == 2) {
        Distance[0][2] = 3 + fcc1;
        Distance[0][11] = 5 + fcc2;
        Distance[0][18] = 5 + fcc3;
        Distance[2][7] = 2 + fcc4;
        Distance[2][10] = 3 + fcc5;
        Distance[2][16] = 4 + fcc6;
        Distance[11][7] = 2 + fcc4;
        Distance[11][10] = 2 + fcc5;
        Distance[11][16] = 2 + fcc6;
        Distance[18][7] = 4 + fcc4;
        Distance[18][10] = 2 + fcc5;
        Distance[18][16] = 2 + fcc6;
        Distance[7][3] = 1 + fcc13;
        Distance[7][9] = 2 + fcc14;
        Distance[7][23] = 5 + fcc15;
        Distance[10][3] = 3 + fcc13;
        Distance[10][9] = 1 + fcc14;
        Distance[10][23] = 3 + fcc15;
        Distance[16][3] = 5 + fcc13;
        Distance[16][9] = 2 + fcc14;
        Distance[16][23] = 2 + fcc15;
        Distance[3][25] = 6;
        Distance[9][25] = 3;
        Distance[23][25] = 1;
      }

      //F2->F1->F4
      if (functionType == 3) {
        Distance[0][7] = 3 + fcc4;
        Distance[0][10] = 3 + fcc5;
        Distance[0][16] = 6 + fcc6;
        Distance[7][2] = 2 + fcc1;
        Distance[7][11] = 2 + fcc2;
        Distance[7][18] = 4 + fcc3;
        Distance[10][2] = 3 + fcc1;
        Distance[10][11] = 2 + fcc2;
        Distance[10][18] = 2 + fcc3;
        Distance[16][2] = 4 + fcc1;
        Distance[16][11] = 2 + fcc2;
        Distance[16][18] = 2 + fcc3;
        Distance[2][5] = 2 + fcc10;
        Distance[2][12] = 3 + fcc11;
        Distance[2][21] = 5 + fcc12;
        Distance[11][5] = 4 + fcc10;
        Distance[11][12] = 1 + fcc11;
        Distance[11][21] = 3 + fcc12;
        Distance[18][5] = 4 + fcc10;
        Distance[18][12] = 3 + fcc11;
        Distance[18][21] = 3 + fcc12;
        Distance[5][25] = 5;
        Distance[12][25] = 2;
        Distance[21][25] = 1;
      }

      //F2->F1->F5
      if (functionType == 4) {
        Distance[0][7] = 3 + fcc4;
        Distance[0][10] = 3 + fcc5;
        Distance[0][16] = 6 + fcc6;
        Distance[7][2] = 2 + fcc1;
        Distance[7][11] = 2 + fcc2;
        Distance[7][18] = 4 + fcc3;
        Distance[10][2] = 3 + fcc1;
        Distance[10][11] = 2 + fcc2;
        Distance[10][18] = 2 + fcc3;
        Distance[16][2] = 4 + fcc1;
        Distance[16][11] = 2 + fcc2;
        Distance[16][18] = 2 + fcc3;
        Distance[2][3] = 1 + fcc13;
        Distance[2][9] = 2 + fcc14;
        Distance[2][23] = 6 + fcc15;
        Distance[11][3] = 3 + fcc13;
        Distance[11][9] = 1 + fcc14;
        Distance[11][23] = 4 + fcc15;
        Distance[18][3] = 5 + fcc13;
        Distance[18][9] = 3 + fcc14;
        Distance[18][23] = 2 + fcc15;
        Distance[3][25] = 6;
        Distance[9][25] = 3;
        Distance[23][25] = 1;
      }

      //F1->F3->F4
      if (functionType == 5) {
        Distance[0][2] = 3 + fcc1;
        Distance[0][11] = 5 + fcc2;
        Distance[0][18] = 5 + fcc3;
        Distance[2][8] = 3 + fcc7;
        Distance[2][13] = 4 + fcc8;
        Distance[2][15] = 3 + fcc9;
        Distance[11][8] = 3 + fcc7;
        Distance[11][13] = 2 + fcc8;
        Distance[11][15] = 1 + fcc9;
        Distance[18][8] = 3 + fcc7;
        Distance[18][13] = 2 + fcc8;
        Distance[18][15] = 3 + fcc9;
        Distance[8][5] = 1 + fcc10;
        Distance[8][12] = 3 + fcc11;
        Distance[8][21] = 5 + fcc12;
        Distance[13][5] = 3 + fcc10;
        Distance[13][12] = 1 + fcc11;
        Distance[13][21] = 3 + fcc12;
        Distance[15][5] = 5 + fcc10;
        Distance[15][12] = 2 + fcc11;
        Distance[15][21] = 2 + fcc12;
        Distance[5][25] = 5;
        Distance[12][25] = 2;
        Distance[21][25] = 1;
      }

      //F1->F3->F5
      if (functionType == 6) {
        Distance[0][2] = 3 + fcc1;
        Distance[0][11] = 5 + fcc2;
        Distance[0][18] = 5 + fcc3;
        Distance[2][8] = 3 + fcc7;
        Distance[2][13] = 4 + fcc8;
        Distance[2][15] = 3 + fcc9;
        Distance[11][8] = 3 + fcc7;
        Distance[11][13] = 2 + fcc8;
        Distance[11][15] = 1 + fcc9;
        Distance[18][8] = 3 + fcc7;
        Distance[18][13] = 2 + fcc8;
        Distance[18][15] = 3 + fcc9;
        Distance[8][3] = 2 + fcc13;
        Distance[8][6] = 2 + fcc14;
        Distance[8][23] = 4 + fcc15;
        Distance[13][3] = 4 + fcc13;
        Distance[13][6] = 2 + fcc14;
        Distance[13][23] = 2 + fcc15;
        Distance[15][3] = 4 + fcc13;
        Distance[15][6] = 2 + fcc14;
        Distance[15][23] = 3 + fcc15;
        Distance[3][25] = 6;
        Distance[9][25] = 3;
        Distance[23][25] = 1;
      }

      //F3->F1->F4
      if (functionType == 7) {
        Distance[0][8] = 2 + fcc7;
        Distance[0][13] = 4 + fcc8;
        Distance[0][15] = 6 + fcc9;
        Distance[8][2] = 3 + fcc1;
        Distance[8][11] = 3 + fcc2;
        Distance[8][18] = 3 + fcc3;
        Distance[13][2] = 4 + fcc1;
        Distance[13][11] = 2 + fcc2;
        Distance[13][18] = 2 + fcc3;
        Distance[15][2] = 3 + fcc1;
        Distance[15][11] = 1 + fcc2;
        Distance[15][18] = 3 + fcc3;
        Distance[2][5] = 2 + fcc10;
        Distance[2][12] = 3 + fcc11;
        Distance[2][21] = 5 + fcc12;
        Distance[11][5] = 4 + fcc10;
        Distance[11][12] = 1 + fcc11;
        Distance[11][21] = 3 + fcc12;
        Distance[18][5] = 4 + fcc10;
        Distance[18][12] = 3 + fcc11;
        Distance[18][21] = 3 + fcc12;
        Distance[5][25] = 5;
        Distance[12][25] = 2;
        Distance[21][25] = 1;
      }

      //F3->F1->F5
      if (functionType == 8) {
        Distance[0][8] = 2 + fcc7;
        Distance[0][13] = 4 + fcc8;
        Distance[0][15] = 6 + fcc9;
        Distance[8][2] = 3 + fcc1;
        Distance[8][11] = 3 + fcc2;
        Distance[8][18] = 3 + fcc3;
        Distance[13][2] = 4 + fcc1;
        Distance[13][11] = 2 + fcc2;
        Distance[13][18] = 2 + fcc3;
        Distance[15][2] = 3 + fcc1;
        Distance[15][11] = 1 + fcc2;
        Distance[15][18] = 3 + fcc3;
        Distance[2][3] = 1 + fcc13;
        Distance[2][9] = 2 + fcc14;
        Distance[2][23] = 6 + fcc15;
        Distance[11][3] = 3 + fcc13;
        Distance[11][9] = 1 + fcc14;
        Distance[11][23] = 4 + fcc15;
        Distance[18][3] = 5 + fcc13;
        Distance[18][9] = 3 + fcc14;
        Distance[18][23] = 2 + fcc15;
        Distance[3][25] = 6;
        Distance[9][25] = 3;
        Distance[23][25] = 1;
      }

      //F2->F3->F4
      if (functionType == 9) {
        Distance[0][7] = 3 + fcc4;
        Distance[0][10] = 3 + fcc5;
        Distance[0][16] = 6 + fcc6;
        Distance[7][8] = 1 + fcc7;
        Distance[7][13] = 3 + fcc8;
        Distance[7][15] = 3 + fcc9;
        Distance[10][8] = 1 + fcc7;
        Distance[10][13] = 1 + fcc8;
        Distance[10][15] = 3 + fcc9;
        Distance[16][8] = 4 + fcc7;
        Distance[16][13] = 2 + fcc8;
        Distance[16][15] = 1 + fcc9;
        Distance[8][5] = 1 + fcc10;
        Distance[8][12] = 3 + fcc11;
        Distance[8][21] = 5 + fcc12;
        Distance[13][5] = 3 + fcc10;
        Distance[13][12] = 1 + fcc11;
        Distance[13][21] = 3 + fcc12;
        Distance[15][5] = 5 + fcc10;
        Distance[15][12] = 2 + fcc11;
        Distance[15][21] = 2 + fcc12;
        Distance[5][25] = 5;
        Distance[12][25] = 2;
        Distance[21][25] = 1;
      }

      //F2->F3->F5
      if (functionType == 10) {
        Distance[0][7] = 3 + fcc4;
        Distance[0][10] = 3 + fcc5;
        Distance[0][16] = 6 + fcc6;
        Distance[7][8] = 1 + fcc7;
        Distance[7][13] = 3 + fcc8;
        Distance[7][15] = 3 + fcc9;
        Distance[10][8] = 1 + fcc7;
        Distance[10][13] = 1 + fcc8;
        Distance[10][15] = 3 + fcc9;
        Distance[16][8] = 4 + fcc7;
        Distance[16][13] = 2 + fcc8;
        Distance[16][15] = 1 + fcc9;
        Distance[8][3] = 2 + fcc13;
        Distance[8][9] = 2 + fcc14;
        Distance[8][23] = 4 + fcc15;
        Distance[13][3] = 4 + fcc13;
        Distance[13][9] = 2 + fcc14;
        Distance[13][23] = 2 + fcc15;
        Distance[15][3] = 4 + fcc13;
        Distance[15][9] = 2 + fcc14;
        Distance[15][23] = 3 + fcc15;
        Distance[3][25] = 6;
        Distance[9][25] = 3;
        Distance[23][25] = 1;
      }

      //F3->F2->F4
      if (functionType == 11) {
        Distance[0][8] = 2 + fcc7;
        Distance[0][13] = 4 + fcc8;
        Distance[0][15] = 6 + fcc9;
        Distance[8][7] = 1 + fcc4;
        Distance[8][10] = 1 + fcc5;
        Distance[8][16] = 4 + fcc6;
        Distance[13][7] = 3 + fcc4;
        Distance[13][10] = 1 + fcc5;
        Distance[13][16] = 2 + fcc6;
        Distance[15][7] = 3 + fcc4;
        Distance[15][10] = 3 + fcc5;
        Distance[15][16] = 1 + fcc6;
        Distance[7][5] = 2 + fcc10;
        Distance[7][12] = 3 + fcc11;
        Distance[7][21] = 5 + fcc12;
        Distance[10][5] = 2 + fcc10;
        Distance[10][12] = 2 + fcc11;
        Distance[10][21] = 4 + fcc12;
        Distance[16][5] = 5 + fcc10;
        Distance[16][12] = 1 + fcc11;
        Distance[16][21] = 1 + fcc12;
        Distance[5][25] = 5;
        Distance[12][25] = 2;
        Distance[21][25] = 1;
      }

      //F3->F2->F5
      if (functionType == 12) {
        Distance[0][8] = 2 + fcc7;
        Distance[0][13] = 4 + fcc8;
        Distance[0][15] = 6 + fcc9;
        Distance[8][7] = 1 + fcc4;
        Distance[8][10] = 1 + fcc5;
        Distance[8][16] = 4 + fcc6;
        Distance[13][7] = 3 + fcc4;
        Distance[13][10] = 1 + fcc5;
        Distance[13][16] = 2 + fcc6;
        Distance[15][7] = 3 + fcc4;
        Distance[15][10] = 3 + fcc5;
        Distance[15][16] = 1 + fcc6;
        Distance[7][3] = 1 + fcc13;
        Distance[7][9] = 2 + fcc14;
        Distance[7][23] = 5 + fcc15;
        Distance[10][3] = 3 + fcc13;
        Distance[10][9] = 1 + fcc14;
        Distance[10][23] = 3 + fcc15;
        Distance[16][3] = 5 + fcc13;
        Distance[16][9] = 2 + fcc14;
        Distance[16][23] = 2 + fcc15;
        Distance[3][25] = 6;
        Distance[9][25] = 3;
        Distance[23][25] = 1;
      }

    }
  }
	 */

	/*
  //for us
  if(flag == 1){
    //F1->F2->F4
    if (functionType == 1) {
      Distance[0][2] = 1 + fcc1t;
      Distance[0][11] = 1 + fcc2t;
      Distance[0][18] = 1 + fcc3t;
      Distance[2][7] = 1 + fcc4t;
      Distance[2][10] = 1 + fcc5t;
      Distance[2][16] = 1 + fcc6t;
      Distance[11][7] = 1 + fcc4t;
      Distance[11][10] = 1 + fcc5t;
      Distance[11][16] = 1 + fcc6t;
      Distance[18][7] = 1 + fcc4t;
      Distance[18][10] = 1 + fcc5t;
      Distance[18][16] = 1 + fcc6t;
      Distance[7][5] = 1 + fcc10t;
      Distance[7][12] = 1 + fcc11t;
      Distance[7][21] = 1 + fcc12t;
      Distance[10][5] = 1 + fcc10t;
      Distance[10][12] = 1 + fcc11t;
      Distance[10][21] = 1 + fcc12t;
      Distance[16][5] = 1 + fcc10t;
      Distance[16][12] = 1 + fcc11t;
      Distance[16][21] = 1 + fcc12t;
      Distance[5][25] = 1;
      Distance[12][25] = 1;
      Distance[21][25] = 1;
    }

    //F1->F2->F5
    if (functionType == 2) {
      Distance[0][2] = 1 + fcc1t;
      Distance[0][11] = 1 + fcc2t;
      Distance[0][18] = 1 + fcc3t;
      Distance[2][7] = 1 + fcc4t;
      Distance[2][10] = 1 + fcc5t;
      Distance[2][16] = 1 + fcc6t;
      Distance[11][7] = 1 + fcc4t;
      Distance[11][10] = 1 + fcc5t;
      Distance[11][16] = 1 + fcc6t;
      Distance[18][7] = 1 + fcc4t;
      Distance[18][10] = 1 + fcc5t;
      Distance[18][16] = 1 + fcc6t;
      Distance[7][3] = 1 + fcc13t;
      Distance[7][9] = 1 + fcc14t;
      Distance[7][23] = 1 + fcc15t;
      Distance[10][3] = 1 + fcc13t;
      Distance[10][9] = 1 + fcc14t;
      Distance[10][23] = 1 + fcc15t;
      Distance[16][3] = 1 + fcc13t;
      Distance[16][9] = 1 + fcc14t;
      Distance[16][23] = 1 + fcc15t;
      Distance[3][25] = 1;
      Distance[9][25] = 1;
      Distance[23][25] = 1;
    }

    //F2->F1->F4
    if (functionType == 3) {
      Distance[0][7] = 1 + fcc4t;
      Distance[0][10] = 1 + fcc5t;
      Distance[0][16] = 1 + fcc6t;
      Distance[7][2] = 1 + fcc1t;
      Distance[7][11] = 1 + fcc2t;
      Distance[7][18] = 1 + fcc3t;
      Distance[10][2] = 1 + fcc1t;
      Distance[10][11] = 1 + fcc2t;
      Distance[10][18] = 1 + fcc3t;
      Distance[16][2] = 1 + fcc1t;
      Distance[16][11] = 1 + fcc2t;
      Distance[16][18] = 1 + fcc3t;
      Distance[2][5] = 1 + fcc10t;
      Distance[2][12] = 1 + fcc11t;
      Distance[2][21] = 1 + fcc12t;
      Distance[11][5] = 1 + fcc10t;
      Distance[11][12] = 1 + fcc11t;
      Distance[11][21] = 1 + fcc12t;
      Distance[18][5] = 1 + fcc10t;
      Distance[18][12] = 1 + fcc11t;
      Distance[18][21] = 1 + fcc12t;
      Distance[5][25] = 1;
      Distance[12][25] = 1;
      Distance[21][25] = 1;
    }

    //F2->F1->F5
    if (functionType == 4) {
      Distance[0][7] = 1 + fcc4t;
      Distance[0][10] = 1 + fcc5t;
      Distance[0][16] = 1 + fcc6t;
      Distance[7][2] = 1 + fcc1t;
      Distance[7][11] = 1 + fcc2t;
      Distance[7][18] = 1 + fcc3t;
      Distance[10][2] = 1 + fcc1t;
      Distance[10][11] = 1 + fcc2t;
      Distance[10][18] = 1 + fcc3t;
      Distance[16][2] = 1 + fcc1t;
      Distance[16][11] = 1 + fcc2t;
      Distance[16][18] = 1 + fcc3t;
      Distance[2][3] = 1 + fcc13t;
      Distance[2][9] = 1 + fcc14t;
      Distance[2][23] = 1 + fcc15t;
      Distance[11][3] = 1 + fcc13t;
      Distance[11][9] = 1 + fcc14t;
      Distance[11][23] = 1 + fcc15t;
      Distance[18][3] = 1 + fcc13t;
      Distance[18][9] = 1 + fcc14t;
      Distance[18][23] = 1 + fcc15t;
      Distance[3][25] = 1;
      Distance[9][25] = 1;
      Distance[23][25] = 1;
    }

    //F1->F3->F4
    if (functionType == 5) {
      Distance[0][2] = 1 + fcc1t;
      Distance[0][11] = 1 + fcc2t;
      Distance[0][18] = 1 + fcc3t;
      Distance[2][8] = 1 + fcc7t;
      Distance[2][13] = 1 + fcc8t;
      Distance[2][15] = 1 + fcc9t;
      Distance[11][8] = 1 + fcc7t;
      Distance[11][13] = 1 + fcc8t;
      Distance[11][15] = 1 + fcc9t;
      Distance[18][8] = 1 + fcc7t;
      Distance[18][13] = 1 + fcc8t;
      Distance[18][15] = 1 + fcc9t;
      Distance[8][5] = 1 + fcc10t;
      Distance[8][12] = 1 + fcc11t;
      Distance[8][21] = 1 + fcc12t;
      Distance[13][5] = 1 + fcc10t;
      Distance[13][12] = 1 + fcc11t;
      Distance[13][21] = 1 + fcc12t;
      Distance[15][5] = 1 + fcc10t;
      Distance[15][12] = 1 + fcc11t;
      Distance[15][21] = 1 + fcc12t;
      Distance[5][25] = 1;
      Distance[12][25] = 2;
      Distance[21][25] = 1;
    }

    //F1->F3->F5
    if (functionType == 6) {
      Distance[0][2] = 1 + fcc1t;
      Distance[0][11] = 1 + fcc2t;
      Distance[0][18] = 1 + fcc3t;
      Distance[2][8] = 1 + fcc7t;
      Distance[2][13] = 1 + fcc8t;
      Distance[2][15] = 1 + fcc9t;
      Distance[11][8] = 1 + fcc7t;
      Distance[11][13] = 1 + fcc8t;
      Distance[11][15] = 1 + fcc9t;
      Distance[18][8] = 1 + fcc7t;
      Distance[18][13] = 1 + fcc8t;
      Distance[18][15] = 1 + fcc9t;
      Distance[8][3] = 1 + fcc13t;
      Distance[8][6] = 1 + fcc14t;
      Distance[8][23] = 1 + fcc15t;
      Distance[13][3] = 1 + fcc13t;
      Distance[13][6] = 1 + fcc14t;
      Distance[13][23] = 1 + fcc15t;
      Distance[15][3] = 1 + fcc13t;
      Distance[15][6] = 1 + fcc14t;
      Distance[15][23] = 1 + fcc15t;
      Distance[3][25] = 1;
      Distance[9][25] = 1;
      Distance[23][25] = 1;
    }

    //F3->F1->F4
    if (functionType == 7) {
      Distance[0][8] = 1 + fcc7t;
      Distance[0][13] = 1 + fcc8t;
      Distance[0][15] = 1 + fcc9t;
      Distance[8][2] = 1 + fcc1t;
      Distance[8][11] = 1 + fcc2t;
      Distance[8][18] = 1 + fcc3t;
      Distance[13][2] = 1 + fcc1t;
      Distance[13][11] = 1 + fcc2t;
      Distance[13][18] = 1 + fcc3t;
      Distance[15][2] = 1 + fcc1t;
      Distance[15][11] = 1 + fcc2t;
      Distance[15][18] = 1 + fcc3t;
      Distance[2][5] = 1 + fcc10t;
      Distance[2][12] = 1 + fcc11t;
      Distance[2][21] = 1 + fcc12t;
      Distance[11][5] = 1 + fcc10t;
      Distance[11][12] = 1 + fcc11t;
      Distance[11][21] = 1 + fcc12t;
      Distance[18][5] = 1 + fcc10t;
      Distance[18][12] = 1 + fcc11t;
      Distance[18][21] = 1 + fcc12t;
      Distance[5][25] = 1;
      Distance[12][25] = 2;
      Distance[21][25] = 1;
    }

    //F3->F1->F5
    if (functionType == 8) {
      Distance[0][8] = 1 + fcc7t;
      Distance[0][13] = 1 + fcc8t;
      Distance[0][15] = 1 + fcc9t;
      Distance[8][2] = 1 + fcc1t;
      Distance[8][11] = 1 + fcc2t;
      Distance[8][18] = 1 + fcc3t;
      Distance[13][2] = 1 + fcc1t;
      Distance[13][11] = 1 + fcc2t;
      Distance[13][18] = 1 + fcc3t;
      Distance[15][2] = 1 + fcc1t;
      Distance[15][11] = 1 + fcc2t;
      Distance[15][18] = 1 + fcc3t;
      Distance[2][3] = 1 + fcc13t;
      Distance[2][9] = 1 + fcc14t;
      Distance[2][23] = 1 + fcc15t;
      Distance[11][3] = 1 + fcc13t;
      Distance[11][9] = 1 + fcc14t;
      Distance[11][23] = 1 + fcc15t;
      Distance[18][3] = 1 + fcc13t;
      Distance[18][9] = 1 + fcc14t;
      Distance[18][23] = 1 + fcc15t;
      Distance[3][25] = 1;
      Distance[9][25] = 1;
      Distance[23][25] = 1;
    }

    //F2->F3->F4
    if (functionType == 9) {
      Distance[0][7] = 1 + fcc4t;
      Distance[0][10] = 1 + fcc5t;
      Distance[0][16] = 1 + fcc6t;
      Distance[7][8] = 1 + fcc7t;
      Distance[7][13] = 1 + fcc8t;
      Distance[7][15] = 1 + fcc9t;
      Distance[10][8] = 1 + fcc7t;
      Distance[10][13] = 1 + fcc8t;
      Distance[10][15] = 1 + fcc9t;
      Distance[16][8] = 1 + fcc7t;
      Distance[16][13] = 1 + fcc8t;
      Distance[16][15] = 1 + fcc9t;
      Distance[8][5] = 1 + fcc10t;
      Distance[8][12] = 1 + fcc11t;
      Distance[8][21] = 1 + fcc12t;
      Distance[13][5] = 1 + fcc10t;
      Distance[13][12] = 1 + fcc11t;
      Distance[13][21] = 1 + fcc12t;
      Distance[15][5] = 1 + fcc10t;
      Distance[15][12] = 1 + fcc11t;
      Distance[15][21] = 1 + fcc12t;
      Distance[5][25] = 1;
      Distance[12][25] = 1;
      Distance[21][25] = 1;
    }

    //F2->F3->F5
    if (functionType == 10) {
      Distance[0][7] = 1 + fcc4t;
      Distance[0][10] = 1 + fcc5t;
      Distance[0][16] = 1 + fcc6t;
      Distance[7][8] = 1 + fcc7t;
      Distance[7][13] = 1 + fcc8t;
      Distance[7][15] = 1 + fcc9t;
      Distance[10][8] = 1 + fcc7t;
      Distance[10][13] = 1 + fcc8t;
      Distance[10][15] = 1 + fcc9t;
      Distance[16][8] = 1 + fcc7t;
      Distance[16][13] = 1 + fcc8t;
      Distance[16][15] = 1 + fcc9t;
      Distance[8][3] = 1 + fcc10t;
      Distance[8][9] = 1 + fcc11t;
      Distance[8][23] = 1 + fcc12t;
      Distance[13][3] = 1 + fcc10t;
      Distance[13][9] = 1 + fcc11t;
      Distance[13][23] = 1 + fcc12t;
      Distance[15][3] = 1 + fcc10t;
      Distance[15][9] = 1 + fcc11t;
      Distance[15][23] = 1 + fcc12t;
      Distance[3][25] = 1;
      Distance[9][25] = 1;
      Distance[23][25] = 1;
    }

    //F3->F2->F4
    if (functionType == 11) {
      Distance[0][8] = 1 + fcc4t;
      Distance[0][13] = 1 + fcc5t;
      Distance[0][15] = 1 + fcc6t;
      Distance[8][7] = 1 + fcc7t;
      Distance[8][10] = 1 + fcc8t;
      Distance[8][16] = 1 + fcc9t;
      Distance[13][7] = 1 + fcc7t;
      Distance[13][10] = 1 + fcc8t;
      Distance[13][16] = 1 + fcc9t;
      Distance[15][7] = 1 + fcc7t;
      Distance[15][10] = 1 + fcc8t;
      Distance[15][16] = 1 + fcc9t;
      Distance[7][5] = 1 + fcc10t;
      Distance[7][12] = 1 + fcc11t;
      Distance[7][21] = 1 + fcc12t;
      Distance[10][5] = 1 + fcc10t;
      Distance[10][12] = 1 + fcc11t;
      Distance[10][21] = 1 + fcc12t;
      Distance[16][5] = 1 + fcc10t;
      Distance[16][12] = 1 + fcc11t;
      Distance[16][21] = 1 + fcc12t;
      Distance[5][25] = 1;
      Distance[12][25] = 1;
      Distance[21][25] = 1;
    }

    //F3->F2->F5
    if (functionType == 12) {
      Distance[0][8] = 1 + fcc4t;
      Distance[0][13] = 1 + fcc5t;
      Distance[0][15] = 1 + fcc6t;
      Distance[8][7] = 1 + fcc7t;
      Distance[8][10] = 1 + fcc8t;
      Distance[8][16] = 1 + fcc9t;
      Distance[13][7] = 1 + fcc7t;
      Distance[13][10] = 1 + fcc8t;
      Distance[13][16] = 1 + fcc9t;
      Distance[15][7] = 1 + fcc7t;
      Distance[15][10] = 1 + fcc8t;
      Distance[15][16] = 1 + fcc9t;
      Distance[7][3] = 1 + fcc13t;
      Distance[7][9] = 1 + fcc14t;
      Distance[7][23] = 1 + fcc15t;
      Distance[10][3] = 1 + fcc13t;
      Distance[10][9] = 1 + fcc14t;
      Distance[10][23] = 1 + fcc15t;
      Distance[16][3] = 1 + fcc13t;
      Distance[16][9] = 1 + fcc14t;
      Distance[16][23] = 1 + fcc15t;
      Distance[3][25] = 1;
      Distance[9][25] = 1;
      Distance[23][25] = 1;
    }
  }
	 */


	//for us1
	if(flag == 1){
		//F1->F2->F4
		if (functionType == 1) {
			Distance[0][3] = 1 + fcc1t;
			Distance[0][10] = 1 + fcc2t;
			Distance[0][21] = 1 + fcc3t;
			Distance[3][7] = 1 + fcc4t;
			Distance[3][13] = 1 + fcc5t;
			Distance[3][23] = 1 + fcc6t;
			Distance[10][7] = 1 + fcc4t;
			Distance[10][13] = 1 + fcc5t;
			Distance[10][23] = 1 + fcc6t;
			Distance[21][7] = 1 + fcc4t;
			Distance[21][13] = 1 + fcc5t;
			Distance[21][23] = 1 + fcc6t;
			Distance[7][6] = 1 + fcc10t;
			Distance[7][9] = 1 + fcc11t;
			Distance[7][15] = 1 + fcc12t;
			Distance[13][6] = 1 + fcc10t;
			Distance[13][9] = 1 + fcc11t;
			Distance[13][15] = 1 + fcc12t;
			Distance[23][6] = 1 + fcc10t;
			Distance[23][9] = 1 + fcc11t;
			Distance[23][15] = 1 + fcc12t;
			Distance[6][25] = 1;
			Distance[9][25] = 1;
			Distance[15][25] = 1;
		}

		//F1->F2->F5
		if (functionType == 2) {
			Distance[0][3] = 1 + fcc1t;
			Distance[0][10] = 1 + fcc2t;
			Distance[0][21] = 1 + fcc3t;
			Distance[3][7] = 1 + fcc4t;
			Distance[3][13] = 1 + fcc5t;
			Distance[3][23] = 1 + fcc6t;
			Distance[10][7] = 1 + fcc4t;
			Distance[10][13] = 1 + fcc5t;
			Distance[10][23] = 1 + fcc6t;
			Distance[21][7] = 1 + fcc4t;
			Distance[21][13] = 1 + fcc5t;
			Distance[21][23] = 1 + fcc6t;
			Distance[7][2] = 1 + fcc13t;
			Distance[7][11] = 1 + fcc14t;
			Distance[7][20] = 1 + fcc15t;
			Distance[13][2] = 1 + fcc13t;
			Distance[13][11] = 1 + fcc14t;
			Distance[13][20] = 1 + fcc15t;
			Distance[23][2] = 1 + fcc13t;
			Distance[23][11] = 1 + fcc14t;
			Distance[23][20] = 1 + fcc15t;
			Distance[2][25] = 1;
			Distance[11][25] = 1;
			Distance[20][25] = 1;
		}

		//F2->F1->F4
		if (functionType == 3) {
			Distance[0][7] = 1 + fcc4t;
			Distance[0][13] = 1 + fcc5t;
			Distance[0][23] = 1 + fcc6t;
			Distance[7][3] = 1 + fcc1t;
			Distance[7][10] = 1 + fcc2t;
			Distance[7][21] = 1 + fcc3t;
			Distance[13][3] = 1 + fcc1t;
			Distance[13][10] = 1 + fcc2t;
			Distance[13][21] = 1 + fcc3t;
			Distance[23][3] = 1 + fcc1t;
			Distance[23][10] = 1 + fcc2t;
			Distance[23][21] = 1 + fcc3t;
			Distance[3][6] = 1 + fcc10t;
			Distance[3][9] = 1 + fcc11t;
			Distance[3][15] = 1 + fcc12t;
			Distance[10][6] = 1 + fcc10t;
			Distance[10][9] = 1 + fcc11t;
			Distance[10][15] = 1 + fcc12t;
			Distance[21][6] = 1 + fcc10t;
			Distance[21][9] = 1 + fcc11t;
			Distance[21][15] = 1 + fcc12t;
			Distance[6][25] = 1;
			Distance[9][25] = 1;
			Distance[15][25] = 1;
		}

		//F2->F1->F5
		if (functionType == 4) {
			Distance[0][7] = 1 + fcc4t;
			Distance[0][13] = 1 + fcc5t;
			Distance[0][23] = 1 + fcc6t;
			Distance[7][3] = 1 + fcc1t;
			Distance[7][10] = 1 + fcc2t;
			Distance[7][21] = 1 + fcc3t;
			Distance[13][3] = 1 + fcc1t;
			Distance[13][10] = 1 + fcc2t;
			Distance[13][21] = 1 + fcc3t;
			Distance[23][3] = 1 + fcc1t;
			Distance[23][10] = 1 + fcc2t;
			Distance[23][21] = 1 + fcc3t;
			Distance[3][2] = 1 + fcc13t;
			Distance[3][11] = 1 + fcc14t;
			Distance[3][20] = 1 + fcc15t;
			Distance[10][2] = 1 + fcc13t;
			Distance[10][11] = 1 + fcc14t;
			Distance[10][20] = 1 + fcc15t;
			Distance[21][2] = 1 + fcc13t;
			Distance[21][11] = 1 + fcc14t;
			Distance[21][20] = 1 + fcc15t;
			Distance[2][25] = 1;
			Distance[11][25] = 1;
			Distance[20][25] = 1;
		}

		//F1->F3->F4
		if (functionType == 5) {
			Distance[0][3] = 1 + fcc1t;
			Distance[0][10] = 1 + fcc2t;
			Distance[0][21] = 1 + fcc3t;
			Distance[3][4] = 1 + fcc7t;
			Distance[3][12] = 1 + fcc8t;
			Distance[3][17] = 1 + fcc9t;
			Distance[10][4] = 1 + fcc7t;
			Distance[10][12] = 1 + fcc8t;
			Distance[10][17] = 1 + fcc9t;
			Distance[21][4] = 1 + fcc7t;
			Distance[21][12] = 1 + fcc8t;
			Distance[21][17] = 1 + fcc9t;
			Distance[4][6] = 1 + fcc10t;
			Distance[4][9] = 1 + fcc11t;
			Distance[4][15] = 1 + fcc12t;
			Distance[12][6] = 1 + fcc10t;
			Distance[12][9] = 1 + fcc11t;
			Distance[12][15] = 1 + fcc12t;
			Distance[17][6] = 1 + fcc10t;
			Distance[17][9] = 1 + fcc11t;
			Distance[17][15] = 1 + fcc12t;
			Distance[6][25] = 1;
			Distance[9][25] = 1;
			Distance[15][25] = 1;
		}

		//F1->F3->F5
		if (functionType == 6) {
			Distance[0][3] = 1 + fcc1t;
			Distance[0][10] = 1 + fcc2t;
			Distance[0][21] = 1 + fcc3t;
			Distance[3][4] = 1 + fcc7t;
			Distance[3][12] = 1 + fcc8t;
			Distance[3][17] = 1 + fcc9t;
			Distance[10][4] = 1 + fcc7t;
			Distance[10][12] = 1 + fcc8t;
			Distance[10][17] = 1 + fcc9t;
			Distance[21][4] = 1 + fcc7t;
			Distance[21][12] = 1 + fcc8t;
			Distance[21][17] = 1 + fcc9t;
			Distance[4][2] = 1 + fcc13t;
			Distance[4][11] = 1 + fcc14t;
			Distance[4][20] = 1 + fcc15t;
			Distance[12][2] = 1 + fcc13t;
			Distance[12][11] = 1 + fcc14t;
			Distance[12][20] = 1 + fcc15t;
			Distance[17][2] = 1 + fcc13t;
			Distance[17][11] = 1 + fcc14t;
			Distance[17][20] = 1 + fcc15t;
			Distance[2][25] = 1;
			Distance[11][25] = 1;
			Distance[20][25] = 1;
		}

		//F3->F1->F4
		if (functionType == 7) {
			Distance[0][4] = 1 + fcc7t;
			Distance[0][12] = 1 + fcc8t;
			Distance[0][17] = 1 + fcc9t;
			Distance[4][3] = 1 + fcc1t;
			Distance[4][10] = 1 + fcc2t;
			Distance[4][21] = 1 + fcc3t;
			Distance[12][3] = 1 + fcc1t;
			Distance[12][10] = 1 + fcc2t;
			Distance[12][21] = 1 + fcc3t;
			Distance[17][3] = 1 + fcc1t;
			Distance[17][10] = 1 + fcc2t;
			Distance[17][21] = 1 + fcc3t;
			Distance[3][6] = 1 + fcc10t;
			Distance[3][9] = 1 + fcc11t;
			Distance[3][15] = 1 + fcc12t;
			Distance[10][6] = 1 + fcc10t;
			Distance[10][9] = 1 + fcc11t;
			Distance[10][15] = 1 + fcc12t;
			Distance[21][6] = 1 + fcc10t;
			Distance[21][9] = 1 + fcc11t;
			Distance[21][15] = 1 + fcc12t;
			Distance[6][25] = 1;
			Distance[9][25] = 1;
			Distance[15][25] = 1;
		}

		//F3->F1->F5
		if (functionType == 8) {
			Distance[0][4] = 1 + fcc7t;
			Distance[0][12] = 1 + fcc8t;
			Distance[0][17] = 1 + fcc9t;
			Distance[4][3] = 1 + fcc1t;
			Distance[4][10] = 1 + fcc2t;
			Distance[4][21] = 1 + fcc3t;
			Distance[12][3] = 1 + fcc1t;
			Distance[12][10] = 1 + fcc2t;
			Distance[12][21] = 1 + fcc3t;
			Distance[17][3] = 1 + fcc1t;
			Distance[17][10] = 1 + fcc2t;
			Distance[17][21] = 1 + fcc3t;
			Distance[3][2] = 1 + fcc13t;
			Distance[3][11] = 1 + fcc14t;
			Distance[3][20] = 1 + fcc15t;
			Distance[10][2] = 1 + fcc13t;
			Distance[10][11] = 1 + fcc14t;
			Distance[10][20] = 1 + fcc15t;
			Distance[21][2] = 1 + fcc13t;
			Distance[21][11] = 1 + fcc14t;
			Distance[21][20] = 1 + fcc15t;
			Distance[2][25] = 1;
			Distance[11][25] = 1;
			Distance[20][25] = 1;
		}

		//F2->F3->F4
		if (functionType == 9) {
			Distance[0][7] = 1 + fcc4t;
			Distance[0][13] = 1 + fcc5t;
			Distance[0][23] = 1 + fcc6t;
			Distance[7][4] = 1 + fcc7t;
			Distance[7][12] = 1 + fcc8t;
			Distance[7][17] = 1 + fcc9t;
			Distance[13][4] = 1 + fcc7t;
			Distance[13][12] = 1 + fcc8t;
			Distance[13][17] = 1 + fcc9t;
			Distance[23][4] = 1 + fcc7t;
			Distance[23][12] = 1 + fcc8t;
			Distance[23][17] = 1 + fcc9t;
			Distance[4][6] = 1 + fcc10t;
			Distance[4][9] = 1 + fcc11t;
			Distance[4][15] = 1 + fcc12t;
			Distance[12][6] = 1 + fcc10t;
			Distance[12][9] = 1 + fcc11t;
			Distance[12][15] = 1 + fcc12t;
			Distance[17][6] = 1 + fcc10t;
			Distance[17][9] = 1 + fcc11t;
			Distance[17][15] = 1 + fcc12t;
			Distance[6][25] = 1;
			Distance[9][25] = 1;
			Distance[15][25] = 1;
		}

		//F2->F3->F5
		if (functionType == 10) {
			Distance[0][7] = 1 + fcc4t;
			Distance[0][13] = 1 + fcc5t;
			Distance[0][23] = 1 + fcc6t;
			Distance[7][4] = 1 + fcc7t;
			Distance[7][12] = 1 + fcc8t;
			Distance[7][17] = 1 + fcc9t;
			Distance[13][4] = 1 + fcc7t;
			Distance[13][12] = 1 + fcc8t;
			Distance[13][17] = 1 + fcc9t;
			Distance[23][4] = 1 + fcc7t;
			Distance[23][12] = 1 + fcc8t;
			Distance[23][17] = 1 + fcc9t;
			Distance[4][2] = 1 + fcc13t;
			Distance[4][11] = 1 + fcc14t;
			Distance[4][20] = 1 + fcc15t;
			Distance[12][2] = 1 + fcc13t;
			Distance[12][11] = 1 + fcc14t;
			Distance[12][20] = 1 + fcc15t;
			Distance[17][2] = 1 + fcc13t;
			Distance[17][11] = 1 + fcc14t;
			Distance[17][20] = 1 + fcc15t;
			Distance[2][25] = 1;
			Distance[11][25] = 1;
			Distance[20][25] = 1;
		}

		//F3->F2->F4
		if (functionType == 11) {
			Distance[0][4] = 1 + fcc4t;
			Distance[0][12] = 1 + fcc5t;
			Distance[0][17] = 1 + fcc6t;
			Distance[4][7] = 1 + fcc7t;
			Distance[4][13] = 1 + fcc8t;
			Distance[4][23] = 1 + fcc9t;
			Distance[12][7] = 1 + fcc7t;
			Distance[12][13] = 1 + fcc8t;
			Distance[12][23] = 1 + fcc9t;
			Distance[17][7] = 1 + fcc7t;
			Distance[17][13] = 1 + fcc8t;
			Distance[17][23] = 1 + fcc9t;
			Distance[7][6] = 1 + fcc10t;
			Distance[7][9] = 1 + fcc11t;
			Distance[7][15] = 1 + fcc12t;
			Distance[13][6] = 1 + fcc10t;
			Distance[13][9] = 1 + fcc11t;
			Distance[13][15] = 1 + fcc12t;
			Distance[23][6] = 1 + fcc10t;
			Distance[23][9] = 1 + fcc11t;
			Distance[23][15] = 1 + fcc12t;
			Distance[6][25] = 1;
			Distance[9][25] = 1;
			Distance[15][25] = 1;
		}

		//F3->F2->F5
		if (functionType == 12) {
			Distance[0][4] = 1 + fcc4t;
			Distance[0][12] = 1 + fcc5t;
			Distance[0][17] = 1 + fcc6t;
			Distance[4][7] = 1 + fcc7t;
			Distance[4][13] = 1 + fcc8t;
			Distance[4][23] = 1 + fcc9t;
			Distance[12][7] = 1 + fcc7t;
			Distance[12][13] = 1 + fcc8t;
			Distance[12][23] = 1 + fcc9t;
			Distance[17][7] = 1 + fcc7t;
			Distance[17][13] = 1 + fcc8t;
			Distance[17][23] = 1 + fcc9t;
			Distance[7][2] = 1 + fcc13t;
			Distance[7][11] = 1 + fcc14t;
			Distance[7][20] = 1 + fcc15t;
			Distance[13][2] = 1 + fcc13t;
			Distance[13][11] = 1 + fcc14t;
			Distance[13][20] = 1 + fcc15t;
			Distance[23][2] = 1 + fcc13t;
			Distance[23][11] = 1 + fcc14t;
			Distance[23][20] = 1 + fcc15t;
			Distance[2][25] = 1;
			Distance[11][25] = 1;
			Distance[20][25] = 1;
		}
	}



	int nPoint = N;

	//int sRoute[nPoint];
	double sDist;

	double pDist[nPoint];
	int pRoute[nPoint];
	bool pFixed[nPoint];
	int sPoint, i, j, newDist;

	sDist = 99999;

	for(i = 0; i < nPoint; i++){
		sRoute[i] = -1;
		pDist[i] = 99999;
		pFixed[i] = false;
	}

	pDist[sp] = 0;

	while(true){
		i = 0;
		while(i < nPoint){
			if(pFixed[i] == 0){
				break;
			}
			i = i + 1;
		}

		if(i == nPoint){
			break;
		}

		for(j = i + 1; j < nPoint; j++){
			if((pFixed[j] == 0) && (pDist[j] < pDist[i])){
				i = j;
			}
		}

		sPoint = i;
		pFixed[sPoint] = true;

		for(j = 0; j < nPoint; j++){
			if((Distance[sPoint][j] > 0) && (pFixed[j] == 0)){
				newDist = pDist[sPoint] + Distance[sPoint][j];
				if(newDist < pDist[j]){
					pDist[j] = newDist;
					pRoute[j] = sPoint;
				}
			}
		}
	}

	sDist=pDist[dp];
	j = 0;
	i = dp;
	while(i != sp){
		sRoute[j] = i;
		i = pRoute[i];
		j = j + 1;
	}
	sRoute[j] = sp;

	/*
  //for us
  switch (sRoute[3]) {
    case 2:
      sDist -= fcc1;
      break;
    case 12:
      sDist -= fcc2;
      break;
    case 18:
      sDist -= fcc3;
      break;
    case 14:
      sDist -= fcc4;
      break;
    case 7:
      sDist -= fcc5;
      break;
    case 16:
      sDist -= fcc6;
      break;
    case 23:
      sDist -= fcc7;
      break;
    case 5:
      sDist -= fcc8;
      break;
    case 15:
      sDist -= fcc9;
      break;
    case 1:
      sDist -= fcc10;
      break;
    case 17:
      sDist -= fcc11;
      break;
    case 8:
      sDist -= fcc12;
      break;
    case 6:
      sDist -= fcc13;
      break;
    case 19:
      sDist -= fcc14;
      break;
    case 13:
      sDist -= fcc15;
      break;
    default:
      break;
  }

  switch (sRoute[2]) {
    case 2:
      sDist -= fcc1;
      break;
    case 12:
      sDist -= fcc2;
      break;
    case 18:
      sDist -= fcc3;
      break;
    case 14:
      sDist -= fcc4;
      break;
    case 7:
      sDist -= fcc5;
      break;
    case 16:
      sDist -= fcc6;
      break;
    case 23:
      sDist -= fcc7;
      break;
    case 5:
      sDist -= fcc8;
      break;
    case 15:
      sDist -= fcc9;
      break;
    case 1:
      sDist -= fcc10;
      break;
    case 17:
      sDist -= fcc11;
      break;
    case 8:
      sDist -= fcc12;
      break;
    case 6:
      sDist -= fcc13;
      break;
    case 19:
      sDist -= fcc14;
      break;
    case 13:
      sDist -= fcc15;
      break;
    default:
      break;
  }

  switch (sRoute[1]) {
    case 2:
      sDist -= fcc1;
      break;
    case 12:
      sDist -= fcc2;
      break;
    case 18:
      sDist -= fcc3;
      break;
    case 14:
      sDist -= fcc4;
      break;
    case 7:
      sDist -= fcc5;
      break;
    case 16:
      sDist -= fcc6;
      break;
    case 23:
      sDist -= fcc7;
      break;
    case 5:
      sDist -= fcc8;
      break;
    case 15:
      sDist -= fcc9;
      break;
    case 1:
      sDist -= fcc10;
      break;
    case 17:
      sDist -= fcc11;
      break;
    case 8:
      sDist -= fcc12;
      break;
    case 6:
      sDist -= fcc13;
      break;
    case 19:
      sDist -= fcc14;
      break;
    case 13:
      sDist -= fcc15;
      break;
    default:
      break;
  }
	 */


	//for us1
	switch (sRoute[3]) {
	case 3:
		sDist -= fcc1;
		break;
	case 10:
		sDist -= fcc2;
		break;
	case 21:
		sDist -= fcc3;
		break;
	case 7:
		sDist -= fcc4;
		break;
	case 13:
		sDist -= fcc5;
		break;
	case 23:
		sDist -= fcc6;
		break;
	case 4:
		sDist -= fcc7;
		break;
	case 12:
		sDist -= fcc8;
		break;
	case 17:
		sDist -= fcc9;
		break;
	case 6:
		sDist -= fcc10;
		break;
	case 9:
		sDist -= fcc11;
		break;
	case 15:
		sDist -= fcc12;
		break;
	case 2:
		sDist -= fcc13;
		break;
	case 11:
		sDist -= fcc14;
		break;
	case 20:
		sDist -= fcc15;
		break;
	default:
		break;
	}

	switch (sRoute[2]) {
	case 3:
		sDist -= fcc1;
		break;
	case 10:
		sDist -= fcc2;
		break;
	case 21:
		sDist -= fcc3;
		break;
	case 7:
		sDist -= fcc4;
		break;
	case 13:
		sDist -= fcc5;
		break;
	case 23:
		sDist -= fcc6;
		break;
	case 4:
		sDist -= fcc7;
		break;
	case 12:
		sDist -= fcc8;
		break;
	case 17:
		sDist -= fcc9;
		break;
	case 6:
		sDist -= fcc10;
		break;
	case 9:
		sDist -= fcc11;
		break;
	case 15:
		sDist -= fcc12;
		break;
	case 2:
		sDist -= fcc13;
		break;
	case 11:
		sDist -= fcc14;
		break;
	case 20:
		sDist -= fcc15;
		break;
	default:
		break;
	}

	switch (sRoute[1]) {
	case 3:
		sDist -= fcc1;
		break;
	case 10:
		sDist -= fcc2;
		break;
	case 21:
		sDist -= fcc3;
		break;
	case 7:
		sDist -= fcc4;
		break;
	case 13:
		sDist -= fcc5;
		break;
	case 23:
		sDist -= fcc6;
		break;
	case 4:
		sDist -= fcc7;
		break;
	case 12:
		sDist -= fcc8;
		break;
	case 17:
		sDist -= fcc9;
		break;
	case 6:
		sDist -= fcc10;
		break;
	case 9:
		sDist -= fcc11;
		break;
	case 15:
		sDist -= fcc12;
		break;
	case 2:
		sDist -= fcc13;
		break;
	case 11:
		sDist -= fcc14;
		break;
	case 20:
		sDist -= fcc15;
		break;
	default:
		break;
	}


	return sDist;

}

int rr[6] = {0,1,1,1,1,1};

std::string
Consumer::roundRobin(int func){
	if(func == 1){
		if(rr[func] == 1){
			rr[func] = 2;
			return "/F1a";
		}else if(rr[func] == 2){
			rr[func] = 3;
			return "/F1b";
		}else if(rr[func] == 3){
			rr[func] = 1;
			return "/F1c";
		}
	}
	if(func == 2){
		if(rr[func] == 1){
			rr[func] = 2;
			return "/F2a";
		}else if(rr[func] == 2){
			rr[func] = 3;
			return "/F2b";
		}else if(rr[func] == 3){
			rr[func] = 1;
			return "/F2c";
		}
	}
	if(func == 3){
		if(rr[func] == 1){
			rr[func] = 2;
			return "/F3a";
		}else if(rr[func] == 2){
			rr[func] = 3;
			return "/F3b";
		}else if(rr[func] == 3){
			rr[func] = 1;
			return "/F3c";
		}
	}
	if(func == 4){
		if(rr[func] == 1){
			rr[func] = 2;
			return "/F4a";
		}else if(rr[func] == 2){
			rr[func] = 3;
			return "/F4b";
		}else if(rr[func] == 3){
			rr[func] = 1;
			return "/F4c";
		}
	}
	if(func == 5){
		if(rr[func] == 1){
			rr[func] = 2;
			return "/F5a";
		}else if(rr[func] == 2){
			rr[func] = 3;
			return "/F5b";
		}else if(rr[func] == 3){
			rr[func] = 1;
			return "/F5c";
		}
	}

	return "";
}

//defined by yamaguchi
int
Consumer::funcJudge(int fn){
	int function = 4;
	/*if(table[0][fn][0]+table[1][fn][0] <= table[0][fn][1]+table[1][fn][1]){
		if(table[0][fn][0]+table[1][fn][0] <= table[0][fn][2]+table[1][fn][2]){
			function = 0;
		}
	}else if(table[0][fn][1]+table[1][fn][1] <= table[0][fn][2]+table[1][fn][2]){
		function = 1;
	}else{
		function = 2;
	}*/
	if(table[0][fn][0]+table[1][fn][0] <= table[0][fn][1]+table[1][fn][1] && table[0][fn][0]+table[1][fn][0] <= table[0][fn][2]+table[1][fn][2]){
		function = 0;
	}else if(table[0][fn][1]+table[1][fn][1] <= table[0][fn][0]+table[1][fn][0] && table[0][fn][1]+table[1][fn][1] <= table[0][fn][2]+table[1][fn][2]){
		function = 1;
	}else if(table[0][fn][2]+table[1][fn][2] <= table[0][fn][0]+table[1][fn][0] && table[0][fn][2]+table[1][fn][2] <= table[0][fn][0]+table[1][fn][0]){
		function = 2;
	}else{
		std::cout <<  "error" << std::endl;
	}

	if(function != 4){
		table[1][fn][function]++;
	}else{
		std::cout << "Error: cousumer.cpp @FuncJudge" << std::endl;
	}
	return function;
}

//defined by yamaguchi
shared_ptr<std::string>
Consumer::duration(int f1, int f2, int f3){
	shared_ptr<std::string> ptr;

	int charactor = funcJudge(f1);
	if(f1 == 1){
		if(charactor == 0){
			table[1][1][0] += ns3::getWeight();
			ptr = make_shared<std::string>("/F1a");

		}else if(charactor == 1){
			table[1][1][1] += ns3::getWeight();
			ptr = make_shared<std::string>("/F1b");

		}else if(charactor == 2){
			table[1][1][2] += ns3::getWeight();
			ptr = make_shared<std::string>("/F1c");

		}
	}else if(f1 == 2){
		if(charactor == 0){
			table[1][2][0] += ns3::getWeight();
			ptr = make_shared<std::string>("/F2a");

		}else if(charactor == 1){
			table[1][2][1] += ns3::getWeight();
			ptr = make_shared<std::string>("/F2b");

		}else if(charactor == 2){
			table[1][2][2] += ns3::getWeight();
			ptr = make_shared<std::string>("/F2c");

		}
	}else if(f1 == 3){
		if(charactor == 0){
			table[1][3][0] += ns3::getWeight();
			ptr = make_shared<std::string>("/F3a");

		}else if(charactor == 1){
			table[1][3][1] += ns3::getWeight();
			ptr = make_shared<std::string>("/F3b");

		}else if(charactor == 2){
			table[1][3][2] += ns3::getWeight();
			ptr = make_shared<std::string>("/F3c");

		}
	}

	if(f2 == 1){
		*ptr += "/F1";
	}else if(f2 == 2){
		*ptr += "/F2";
	}else if(f2 == 3){
		*ptr += "/F3";
	}

	if(f3 == 4){
		*ptr += "/F4";
	}else if(f3 == 5){
		*ptr += "/F5";
	}

	return ptr;
}

std::string
Consumer::randChoice(int func){
	uint32_t randNum = ::ndn::random::generateWord32() % 3 + 1;
	if(func == 1){
		if(randNum == 1){
			return "/F1a";
		}else if(randNum == 2){
			return "/F1b";
		}else if(randNum == 3){
			return "/F1c";
		}
	}
	if(func == 2){
		if(randNum == 1){
			return "/F2a";
		}else if(randNum == 2){
			return "/F2b";
		}else if(randNum == 3){
			return "/F2c";
		}
	}
	if(func == 3){
		if(randNum == 1){
			return "/F3a";
		}else if(randNum == 2){
			return "/F3b";
		}else if(randNum == 3){
			return "/F3c";
		}
	}
	if(func == 4){
		if(randNum == 1){
			return "/F4a";
		}else if(randNum == 2){
			return "/F4b";
		}else if(randNum == 3){
			return "/F4c";
		}
	}
	if(func == 5){
		if(randNum == 1){
			return "/F5a";
		}else if(randNum == 2){
			return "/F5b";
		}else if(randNum == 3){
			return "/F5c";
		}
	}

	return "";
}

shared_ptr<std::string>
Consumer::fibControl(int f1, int f2, int f3){
	shared_ptr<std::string> ptr;

	if(f1 == 1){
		ptr = make_shared<std::string>("/F1+");
	}else if(f1 == 2){
		ptr = make_shared<std::string>("/F2+");	
	}else if(f1 == 3){
		ptr = make_shared<std::string>("/F3+");
	}

	if(f2 == 1){
		*ptr += "/F1";
	}else if(f2 == 2){
		*ptr += "/F2";
	}else if(f2 == 3){
		*ptr += "/F3";
	}

	if(f3 == 4){
		*ptr += "/F4";
	}else if(f3 == 5){
		*ptr += "/F5";
	}

	return ptr;
}

// 	shared_ptr<std::string> ptr;

// 	if(f1 == 1){
// 		ptr = make_shared<std::string>("/F1");
// 	}else if(f1 == 2){
// 		ptr = make_shared<std::string>("/F2");
// 	}else if(f1 == 3){
// 		ptr = make_shared<std::string>("/F3");
// 	}

// 	if(f2 == 1){
// 		*ptr += "/F1";
// 	}else if(f2 == 2){
// 		*ptr += "/F2";
// 	}else if(f2 == 3){
// 		*ptr += "/F3";
// 	}

// 	if(f3 == 4){
// 		*ptr += "/F4";
// 	}else if(f3 == 5){
// 		*ptr += "/F5";
// 	}

// 	return ptr;
// }


//defined by yamaguchi
shared_ptr<Name>
Consumer::sourceRouting(uint32_t functionType, int currentNode, int* sRoute, double weight){
	shared_ptr<Name> functionName;
	shared_ptr<std::string> funcName = make_shared<std::string>("");
	int distance;
	switch(getChoiceType()){
	case 0: //siraiwaNDN

		distance = dijkstra(0, 25, sRoute, functionType, currentNode, weight, 0);
		//int distance = dijkstra(0, 25, sRoute, 1, currentNode, weight, 0);
		//std::cout << "distance: " << distance << std::endl;

		/*
			  for(int i = 4; i >= 0; i--)
			    printf("%d,",sRoute[i]);
			  std::cout << "" << std::endl;
		 */


		/*
			  //for us
			  switch (sRoute[3]) {
			    case 2:
		 *funcName = "/F1a";
			      break;
			    case 11:
		 *funcName = "/F1b";
			      break;
			    case 18:
		 *funcName = "/F1c";
			      break;
			    case 7:
		 *funcName = "/F2a";
			      break;
			    case 10:
		 *funcName = "/F2b";
			      break;
			    case 16:
		 *funcName = "/F2c";
			      break;
			    case 8:
		 *funcName = "/F3a";
			      break;
			    case 13:
		 *funcName = "/F3b";
			      break;
			    case 15:
		 *funcName = "/F3c";
			      break;
			    case 5:
		 *funcName = "/F4a";
			      break;
			    case 12:
		 *funcName = "/F4b";
			      break;
			    case 21:
		 *funcName = "/F4c";
			      break;
			    case 3:
		 *funcName = "/F5a";
			      break;
			    case 9:
		 *funcName = "/F5b";
			      break;
			    case 23:
		 *funcName = "/F5c";
			      break;
			    default:
			      break;
			  }

			  switch (sRoute[2]) {
			    case 2:
		 *funcName += "/F1a";
			      break;
			    case 11:
		 *funcName += "/F1b";
			      break;
			    case 18:
		 *funcName += "/F1c";
			      break;
			    case 7:
		 *funcName += "/F2a";
			      break;
			    case 10:
		 *funcName += "/F2b";
			      break;
			    case 16:
		 *funcName += "/F2c";
			      break;
			    case 8:
		 *funcName += "/F3a";
			      break;
			    case 13:
		 *funcName += "/F3b";
			      break;
			    case 15:
		 *funcName += "/F3c";
			      break;
			    case 5:
		 *funcName += "/F4a";
			      break;
			    case 12:
		 *funcName += "/F4b";
			      break;
			    case 21:
		 *funcName += "/F4c";
			      break;
			    case 3:
		 *funcName += "/F5a";
			      break;
			    case 9:
		 *funcName += "/F5b";
			      break;
			    case 23:
		 *funcName += "/F5c";
			      break;
			    default:
			      break;
			  }

			  switch (sRoute[1]) {
			    case 2:
		 *funcName += "/F1a";
			      break;
			    case 11:
		 *funcName += "/F1b";
			      break;
			    case 18:
		 *funcName += "/F1c";
			      break;
			    case 7:
		 *funcName += "/F2a";
			      break;
			    case 10:
		 *funcName += "/F2b";
			      break;
			    case 16:
		 *funcName += "/F2c";
			      break;
			    case 8:
		 *funcName += "/F3a";
			      break;
			    case 13:
		 *funcName += "/F3b";
			      break;
			    case 15:
		 *funcName += "/F3c";
			      break;
			    case 5:
		 *funcName += "/F4a";
			      break;
			    case 12:
		 *funcName += "/F4b";
			      break;
			    case 21:
		 *funcName += "/F4c";
			      break;
			    case 3:
		 *funcName += "/F5a";
			      break;
			    case 9:
		 *funcName += "/F5b";
			      break;
			    case 23:
		 *funcName += "/F5c";
			      break;
			    default:
			      break;
			  }
		 */

		//for us1
		switch (sRoute[3]) {
		case 3:
			*funcName = "/F1a";
			break;
		case 10:
			*funcName = "/F1b";
			break;
		case 21:
			*funcName = "/F1c";
			break;
		case 7:
			*funcName = "/F2a";
			break;
		case 13:
			*funcName = "/F2b";
			break;
		case 23:
			*funcName = "/F2c";
			break;
		case 4:
			*funcName = "/F3a";
			break;
		case 12:
			*funcName = "/F3b";
			break;
		case 17:
			*funcName = "/F3c";
			break;
		case 6:
			*funcName = "/F4a";
			break;
		case 9:
			*funcName = "/F4b";
			break;
		case 15:
			*funcName = "/F4c";
			break;
		case 2:
			*funcName = "/F5a";
			break;
		case 11:
			*funcName = "/F5b";
			break;
		case 20:
			*funcName = "/F5c";
			break;
		default:
			break;
		}

		switch (sRoute[2]) {
		case 3:
			*funcName += "/F1a";
			break;
		case 10:
			*funcName += "/F1b";
			break;
		case 21:
			*funcName += "/F1c";
			break;
		case 7:
			*funcName += "/F2a";
			break;
		case 13:
			*funcName += "/F2b";
			break;
		case 23:
			*funcName += "/F2c";
			break;
		case 4:
			*funcName += "/F3a";
			break;
		case 12:
			*funcName += "/F3b";
			break;
		case 17:
			*funcName += "/F3c";
			break;
		case 6:
			*funcName += "/F4a";
			break;
		case 9:
			*funcName += "/F4b";
			break;
		case 15:
			*funcName += "/F4c";
			break;
		case 2:
			*funcName += "/F5a";
			break;
		case 11:
			*funcName += "/F5b";
			break;
		case 20:
			*funcName += "/F5c";
			break;
		default:
			break;
		}

		switch (sRoute[1]) {
		case 3:
			*funcName += "/F1a";
			break;
		case 10:
			*funcName += "/F1b";
			break;
		case 21:
			*funcName += "/F1c";
			break;
		case 7:
			*funcName += "/F2a";
			break;
		case 13:
			*funcName += "/F2b";
			break;
		case 23:
			*funcName += "/F2c";
			break;
		case 4:
			*funcName += "/F3a";
			break;
		case 12:
			*funcName += "/F3b";
			break;
		case 17:
			*funcName += "/F3c";
			break;
		case 6:
			*funcName += "/F4a";
			break;
		case 9:
			*funcName += "/F4b";
			break;
		case 15:
			*funcName += "/F4c";
			break;
		case 2:
			*funcName += "/F5a";
			break;
		case 11:
			*funcName += "/F5b";
			break;
		case 20:
			*funcName += "/F5c";
			break;
		default:
			break;
		}
		increaseTotalHops(distance);

		break;  // end siraiwaNDN
		case 1: //roundRobin
		{
			switch(functionType){// firstType
			case 1:
			case 2:
			case 5:
			case 6:
				*funcName += roundRobin(1);
				break;
			case 3:
			case 4:
			case 9:
			case 10:
				*funcName += roundRobin(2);
				break;
			case 7:
			case 8:
			case 11:
			case 12:
				*funcName += roundRobin(3);
				break;
			default:
				break;
			}
			switch(functionType){// secondType
			case 1:
			case 2:
			case 11:
			case 12:
				*funcName += roundRobin(2);
				break;
			case 3:
			case 4:
			case 7:
			case 8:
				*funcName += roundRobin(1);
				break;
			case 5:
			case 6:
			case 9:
			case 10:
				*funcName += roundRobin(3);
				break;
			default:
				break;
			}
			switch(functionType){// thirdType
			case 1:
			case 3:
			case 5:
			case 7:
			case 9:
			case 11:
				*funcName += roundRobin(4);
				break;
			case 2:
			case 4:
			case 6:
			case 8:
			case 10:
			case 12:
				*funcName += roundRobin(5);
				break;
			default:
				break;
			}
		}
		break;//end roundRobin
		case 2://duration
		{
			switch(functionType){
			case 1:
				funcName = duration(1,2,4);
				break;
			case 2:
				funcName = duration(1,2,5);
				break;
			case 5:
				funcName = duration(1,3,4);
				break;
			case 6:
				funcName = duration(1,3,5);
				break;
			case 3:
				funcName = duration(2,1,4);
				break;
			case 4:
				funcName = duration(2,1,5);
				break;
			case 9:
				funcName = duration(2,3,4);
				break;
			case 10:
				funcName = duration(2,3,5);
				break;
			case 7:
				funcName = duration(3,1,4);
				break;
			case 8:
				funcName = duration(3,1,5);
				break;
			case 11:
				funcName = duration(3,2,4);
				break;
			case 12:
				funcName = duration(3,2,5);
				break;
			default:
				break;
			}
		}

		break;//end duration

		case 3:
		{
			switch(functionType){// firstType
			case 1:
			case 2:
			case 5:
			case 6:
				*funcName += randChoice(1);
				break;
			case 3:
			case 4:
			case 9:
			case 10:
				*funcName += randChoice(2);
				break;
			case 7:
			case 8:
			case 11:
			case 12:
				*funcName += randChoice(3);
				break;
			default:
				break;
			}
			switch(functionType){// secondType
			case 1:
			case 2:
			case 11:
			case 12:
				*funcName += randChoice(2);
				break;
			case 3:
			case 4:
			case 7:
			case 8:
				*funcName += randChoice(1);
				break;
			case 5:
			case 6:
			case 9:
			case 10:
				*funcName += randChoice(3);
				break;
			default:
				break;
			}
			switch(functionType){// thirdType
			case 1:
			case 3:
			case 5:
			case 7:
			case 9:
			case 11:
				*funcName += randChoice(4);
				break;
			case 2:
			case 4:
			case 6:
			case 8:
			case 10:
			case 12:
				*funcName += randChoice(5);
				break;
			default:
				break;
			}
		}
		break;//end randChoice
		case 4:
		{
			switch(functionType){
			case 1:
				funcName = fibControl(1,2,4);
				break;
			case 2:
				funcName = fibControl(1,2,5);
				break;
			case 5:
				funcName = fibControl(1,3,4);
				break;
			case 6:
				funcName = fibControl(1,3,5);
				break;
			case 3:
				funcName = fibControl(2,1,4);
				break;
			case 4:
				funcName = fibControl(2,1,5);
				break;
			case 9:
				funcName = fibControl(2,3,4);
				break;
			case 10:
				funcName = fibControl(2,3,5);
				break;
			case 7:
				funcName = fibControl(3,1,4);
				break;
			case 8:
				funcName = fibControl(3,1,5);
				break;
			case 11:
				funcName = fibControl(3,2,4);
				break;
			case 12:
				funcName = fibControl(3,2,5);
				break;
			default:
				break;
			}
		}
		break;
		default:
			std::cerr << "choice method not found" << std::endl;
			break;
	}
	std::string str = *funcName;
	functionName = make_shared<Name>(Name(str));
	return functionName;
}

void
Consumer::SendPacket()
{
	if(ns3::getTotalSend() > 299) return;
	if (!m_active)
		return;

	NS_LOG_FUNCTION_NOARGS();

	uint32_t seq = std::numeric_limits<uint32_t>::max(); // invalid

	while (m_retxSeqs.size()) {
		seq = *m_retxSeqs.begin();
		m_retxSeqs.erase(m_retxSeqs.begin());
		break;
	}

	if (seq == std::numeric_limits<uint32_t>::max()) {
		if (m_seqMax != std::numeric_limits<uint32_t>::max()) {
			if (m_seq >= m_seqMax) {
				return; // we are totally done
			}
		}

		seq = m_seq++;
	}

	//
	shared_ptr<Name> nameWithSequence = make_shared<Name>(m_interestName);
	nameWithSequence->appendSequenceNumber(seq);

	ns3::increaseTotalSend();
	//

	//choose Function Type from 1 to 6
	uint32_t functionType = ::ndn::random::generateWord32() % 12 + 1;

	int currentNode = ns3::Simulator::GetContext();
	//std::cout << "Consumer Node: " <<  currentNode << std::endl;
	//std::cout << "function type:"  <<  functionType << std::endl;

	//dijkstra
	int sRoute[N];
	//need to change
	double weight = ns3::getWeight();
	shared_ptr<Name> functionName = sourceRouting(functionType, currentNode, sRoute, weight);

	/* for gid
  std::cout << "AllFC: " << getAllFcc() << std::endl;
  std::cout << "F1a  : " << getFunctionCallCount(1) << std::endl;
  std::cout << "Temp  : " << getTotalFccTemp(1) << std::endl;
  std::cout << "Total: " << getTotalFcc(1) << std::endl;
  std::cout << "F1b  : " << getFunctionCallCount(6) << std::endl;
  std::cout << "Temp  : " << getTotalFccTemp(6) << std::endl;
  std::cout << "Total: " << getTotalFcc(6) << std::endl;
  std::cout << "F1c  : " << getFunctionCallCount(9) << std::endl;
  std::cout << "Temp  : " << getTotalFccTemp(9) << std::endl;
  std::cout << "Total: " << getTotalFcc(9) << std::endl;
  std::cout << "F2a  : " << getFunctionCallCount(2) << std::endl;
  std::cout << "Temp  : " << getTotalFccTemp(2) << std::endl;
  std::cout << "Total: " << getTotalFcc(2) << std::endl;
  std::cout << "F2b  : " << getFunctionCallCount(4) << std::endl;
  std::cout << "Temp  : " << getTotalFccTemp(4) << std::endl;
  std::cout << "Total: " << getTotalFcc(4) << std::endl;
  std::cout << "F2c  : " << getFunctionCallCount(8) << std::endl;
  std::cout << "Temp  : " << getTotalFccTemp(8) << std::endl;
  std::cout << "Total: " << getTotalFcc(8) << std::endl;
  std::cout << "F3a  : " << getFunctionCallCount(3) << std::endl;
  std::cout << "Temp  : " << getTotalFccTemp(3) << std::endl;
  std::cout << "Total: " << getTotalFcc(3) << std::endl;
  std::cout << "F3b  : " << getFunctionCallCount(5) << std::endl;
  std::cout << "Temp  : " << getTotalFccTemp(5) << std::endl;
  std::cout << "Total: " << getTotalFcc(5) << std::endl;
  std::cout << "F3c  : " << getFunctionCallCount(7) << std::endl;
  std::cout << "Temp  : " << getTotalFccTemp(7) << std::endl;
  std::cout << "Total: " << getTotalFcc(7) << std::endl;
	 */

	/*
  // us-24
  std::cout << "AllFC: " << getAllFcc() << std::endl;
  std::cout << "F1a  : " << getFunctionCallCount(1) << std::endl;
  std::cout << "Temp  : " << getTotalFccTemp(1) << std::endl;
  std::cout << "Total: " << getTotalFcc(1) << std::endl;
  std::cout << "F1b  : " << getFunctionCallCount(2) << std::endl;
  std::cout << "Temp  : " << getTotalFccTemp(2) << std::endl;
  std::cout << "Total: " << getTotalFcc(2) << std::endl;
  std::cout << "F1c  : " << getFunctionCallCount(3) << std::endl;
  std::cout << "Temp  : " << getTotalFccTemp(3) << std::endl;
  std::cout << "Total: " << getTotalFcc(3) << std::endl;
  std::cout << "F2a  : " << getFunctionCallCount(4) << std::endl;
  std::cout << "Temp  : " << getTotalFccTemp(4) << std::endl;
  std::cout << "Total: " << getTotalFcc(4) << std::endl;
  std::cout << "F2b  : " << getFunctionCallCount(5) << std::endl;
  std::cout << "Temp  : " << getTotalFccTemp(5) << std::endl;
  std::cout << "Total: " << getTotalFcc(5) << std::endl;
  std::cout << "F2c  : " << getFunctionCallCount(6) << std::endl;
  std::cout << "Temp  : " << getTotalFccTemp(6) << std::endl;
  std::cout << "Total: " << getTotalFcc(6) << std::endl;
  std::cout << "F3a  : " << getFunctionCallCount(7) << std::endl;
  std::cout << "Temp  : " << getTotalFccTemp(7) << std::endl;
  std::cout << "Total: " << getTotalFcc(7) << std::endl;
  std::cout << "F3b  : " << getFunctionCallCount(8) << std::endl;
  std::cout << "Temp  : " << getTotalFccTemp(8) << std::endl;
  std::cout << "Total: " << getTotalFcc(8) << std::endl;
  std::cout << "F3c  : " << getFunctionCallCount(9) << std::endl;
  std::cout << "Temp  : " << getTotalFccTemp(9) << std::endl;
  std::cout << "Total: " << getTotalFcc(9) << std::endl;
  std::cout << "F4a  : " << getFunctionCallCount(10) << std::endl;
  std::cout << "Temp  : " << getTotalFccTemp(10) << std::endl;
  std::cout << "Total: " << getTotalFcc(10) << std::endl;
  std::cout << "F4b  : " << getFunctionCallCount(11) << std::endl;
  std::cout << "Temp  : " << getTotalFccTemp(11) << std::endl;
  std::cout << "Total: " << getTotalFcc(11) << std::endl;
  std::cout << "F4c  : " << getFunctionCallCount(12) << std::endl;
  std::cout << "Temp  : " << getTotalFccTemp(12) << std::endl;
  std::cout << "Total: " << getTotalFcc(12) << std::endl;
  std::cout << "F5a  : " << getFunctionCallCount(13) << std::endl;
  std::cout << "Temp  : " << getTotalFccTemp(13) << std::endl;
  std::cout << "Total: " << getTotalFcc(13) << std::endl;
  std::cout << "F5b  : " << getFunctionCallCount(14) << std::endl;
  std::cout << "Temp  : " << getTotalFccTemp(14) << std::endl;
  std::cout << "Total: " << getTotalFcc(14) << std::endl;
  std::cout << "F5c  : " << getFunctionCallCount(15) << std::endl;
  std::cout << "Temp  : " << getTotalFccTemp(15) << std::endl;
  std::cout << "Total: " << getTotalFcc(15) << std::endl;
	 */

	ns3::increaseInterestNum();
	/*
  std::cout << "InterestNum: " << ns3::getInterestNum() << std::endl;
  if(ns3::getInterestNum() == 301){
    Simulator::Stop();
  }

  std::cout << getAllFcc() << std::endl;
  std::cout << getTotalFcc(1) << std::endl;
  std::cout << getTotalFcc(2) << std::endl;
  std::cout << getTotalFcc(3) << std::endl;
  std::cout << getTotalFcc(4) << std::endl;
  std::cout << getTotalFcc(5) << std::endl;
  std::cout << getTotalFcc(6) << std::endl;
  std::cout << getTotalFcc(7) << std::endl;
  std::cout << getTotalFcc(8) << std::endl;
  std::cout << getTotalFcc(9) << std::endl;
  std::cout << getTotalFcc(10) << std::endl;
  std::cout << getTotalFcc(11) << std::endl;
  std::cout << getTotalFcc(12) << std::endl;
  std::cout << getTotalFcc(13) << std::endl;
  std::cout << getTotalFcc(14) << std::endl;
  std::cout << getTotalFcc(15) << std::endl;
	 */
	// shared_ptr<Interest> interest = make_shared<Interest> ();
	shared_ptr<Interest> interest = make_shared<Interest>();
	interest->setNonce(m_rand->GetValue(0, std::numeric_limits<uint32_t>::max()));
	interest->setName(*nameWithSequence);
	interest->setFunction(*functionName);
	if(getChoiceType() == 2){
		std::string headFunc = functionName->toUri();
		int pos = headFunc.find("/",1);
		headFunc.erase(pos, headFunc.size()-1);
		Name headFuncName(headFunc);
		shared_ptr<Name> headFuncNamePtr = make_shared<Name>(headFuncName);
		interest->setFunctionFullName(*headFuncNamePtr);
	}
	if(getChoiceType() == 4){
		// std::string FibCon = "";
		// Name FibConName(FibCon);
		// shared_ptr<Name> FibConNamePtr = make_shared<Name>(FibConName);
		// interest->setFunctionFullName(*FibConNamePtr);
	}

	time::milliseconds interestLifeTime(m_interestLifeTime.GetMilliSeconds());
	//time::milliseconds interestLifeTime(1000);
	//interest->setInterestLifetime(interestLifeTime);
	time::milliseconds nowTime = time::toUnixTimestamp(time::system_clock::now());
	interest->setServiceTime(nowTime);
	interest->setFunctionFlag(0);

	// NS_LOG_INFO ("Requesting Interest: \n" << *interest);
	NS_LOG_INFO("> Interest for " << seq);

	WillSendOutInterest(seq);

	m_transmittedInterests(interest, this, m_face);
	m_appLink->onReceiveInterest(*interest);

	ScheduleNextPacket();
}



///////////////////////////////////////////////////
//          Process incoming packets             //
///////////////////////////////////////////////////

void
Consumer::OnData(shared_ptr<const Data> data)
{
	if (!m_active)
		return;

	App::OnData(data); // tracing inside

	NS_LOG_FUNCTION(this << data);

	// NS_LOG_INFO ("Received content object: " << boost::cref(*data));

	// This could be a problem......
	uint32_t seq = data->getName().at(-1).toSequenceNumber();
	NS_LOG_INFO("< DATA for " << seq);
	ns3::increaseServiceNum();
	std::cout << "Service Num: " << ns3::getServiceNum() << std::endl;

	//added 2019/11/12
	if(getChoiceType() == 2){
		if(data->getTag<lp::FunctionNameTag>() != nullptr){
			Name PreviousFuncName = *(data->getTag<lp::FunctionNameTag>());
			std::string PFuncName = PreviousFuncName.toUri();

			if(PFuncName.compare("/F1a") == 0){
				table[0][1][0] = *(data->getTag<lp::PartialHopTag>());
				table[1][1][0] = *(data->getTag<lp::CountTag>());
			}else if(PFuncName.compare("/F1b") == 0){
				table[0][1][1] = *(data->getTag<lp::PartialHopTag>());
				table[1][1][1] = *(data->getTag<lp::CountTag>());
			}else if(PFuncName.compare("/F1c") == 0){
				table[0][1][2] = *(data->getTag<lp::PartialHopTag>());
				table[1][1][2] = *(data->getTag<lp::CountTag>());
			}else if(PFuncName.compare("/F2a") == 0){
				table[0][2][0] = *(data->getTag<lp::PartialHopTag>());
				table[1][2][0] = *(data->getTag<lp::CountTag>());
			}else if(PFuncName.compare("/F2b") == 0){
				table[0][2][1] = *(data->getTag<lp::PartialHopTag>());
				table[1][2][1] = *(data->getTag<lp::CountTag>());
			}else if(PFuncName.compare("/F2c") == 0){
				table[0][2][2] = *(data->getTag<lp::PartialHopTag>());
				table[1][2][2] = *(data->getTag<lp::CountTag>());
			}else if(PFuncName.compare("/F3a") == 0){
				table[0][3][0] = *(data->getTag<lp::PartialHopTag>());
				table[1][3][0] = *(data->getTag<lp::CountTag>());
			}else if(PFuncName.compare("/F3b") == 0){
				table[0][3][1] = *(data->getTag<lp::PartialHopTag>());
				table[1][3][1] = *(data->getTag<lp::CountTag>());
			}else if(PFuncName.compare("/F3c") == 0){
				table[0][3][2] = *(data->getTag<lp::PartialHopTag>());
				table[1][3][2] = *(data->getTag<lp::CountTag>());
			}else {

			}
		}
	}

	time::milliseconds nowTime = time::toUnixTimestamp(time::system_clock::now());
	//int serviceTime = nowTime.count() - data->getServiceTime().count() - 180;
	int serviceTime = nowTime.count() - data->getServiceTime().count() + 120;
	increaseTotalServiceTime(serviceTime);
	double averageServiceTime = double(getTotalServiceTime()) / double(ns3::getServiceNum());
	ns3::setAverageServiceTime(averageServiceTime);

	//std::cout << "Service Time: " << data->getServiceTime().count() << std::endl;
	std::cout << "Service Time        : " << serviceTime << std::endl;
	std::cout << "Average Service Time: " << averageServiceTime << std::endl;
	std::cout << "Service End Time    : " << nowTime.count() << std::endl;

	std::cout << "------------------------------------------------" << std::endl;

	/*
  std::cout << "Interest Num:" << getInterestNum() << std::endl;
  std::cout << "Data     Num:" << getDataNum() << std::endl;
  std::cout << "AllFC: " << getAllFcc() << std::endl;
  std::cout << "F1a  : " << getFunctionCallCount(1) << std::endl;
  std::cout << "Temp  : " << getTotalFccTemp(1) << std::endl;
  std::cout << "Total: " << getTotalFcc(1) << std::endl;
  std::cout << "F1b  : " << getFunctionCallCount(2) << std::endl;
  std::cout << "Temp  : " << getTotalFccTemp(2) << std::endl;
  std::cout << "Total: " << getTotalFcc(2) << std::endl;
  std::cout << "F1c  : " << getFunctionCallCount(3) << std::endl;
  std::cout << "Temp  : " << getTotalFccTemp(3) << std::endl;
  std::cout << "Total: " << getTotalFcc(3) << std::endl;
  std::cout << "F2a  : " << getFunctionCallCount(4) << std::endl;
  std::cout << "Temp  : " << getTotalFccTemp(4) << std::endl;
  std::cout << "Total: " << getTotalFcc(4) << std::endl;
  std::cout << "F2b  : " << getFunctionCallCount(5) << std::endl;
  std::cout << "Temp  : " << getTotalFccTemp(5) << std::endl;
  std::cout << "Total: " << getTotalFcc(5) << std::endl;
  std::cout << "F2c  : " << getFunctionCallCount(6) << std::endl;
  std::cout << "Temp  : " << getTotalFccTemp(6) << std::endl;
  std::cout << "Total: " << getTotalFcc(6) << std::endl;
  std::cout << "F3a  : " << getFunctionCallCount(7) << std::endl;
  std::cout << "Temp  : " << getTotalFccTemp(7) << std::endl;
  std::cout << "Total: " << getTotalFcc(7) << std::endl;
  std::cout << "F3b  : " << getFunctionCallCount(8) << std::endl;
  std::cout << "Temp  : " << getTotalFccTemp(8) << std::endl;
  std::cout << "Total: " << getTotalFcc(8) << std::endl;
  std::cout << "F3c  : " << getFunctionCallCount(9) << std::endl;
  std::cout << "Temp  : " << getTotalFccTemp(9) << std::endl;
  std::cout << "Total: " << getTotalFcc(9) << std::endl;
  std::cout << "F4a  : " << getFunctionCallCount(10) << std::endl;
  std::cout << "Temp  : " << getTotalFccTemp(10) << std::endl;
  std::cout << "Total: " << getTotalFcc(10) << std::endl;
  std::cout << "F4b  : " << getFunctionCallCount(11) << std::endl;
  std::cout << "Temp  : " << getTotalFccTemp(11) << std::endl;
  std::cout << "Total: " << getTotalFcc(11) << std::endl;
  std::cout << "F4c  : " << getFunctionCallCount(12) << std::endl;
  std::cout << "Temp  : " << getTotalFccTemp(12) << std::endl;
  std::cout << "Total: " << getTotalFcc(12) << std::endl;
  std::cout << "F5a  : " << getFunctionCallCount(13) << std::endl;
  std::cout << "Temp  : " << getTotalFccTemp(13) << std::endl;
  std::cout << "Total: " << getTotalFcc(13) << std::endl;
  std::cout << "F5b  : " << getFunctionCallCount(14) << std::endl;
  std::cout << "Temp  : " << getTotalFccTemp(14) << std::endl;
  std::cout << "Total: " << getTotalFcc(14) << std::endl;
  std::cout << "F5c  : " << getFunctionCallCount(15) << std::endl;
  std::cout << "Temp  : " << getTotalFccTemp(15) << std::endl;
  std::cout << "Total: " << getTotalFcc(15) << std::endl;
	 */


	int totalFcc = 0;
	for(int i = 1; i < 16; i++){
		totalFcc += getTotalFcc(i);
	}
	std::cout << "TotalFcc: " << totalFcc << std::endl;

	int hopCount = 0;
	auto hopCountTag = data->getTag<lp::HopCountTag>();
	if (hopCountTag != nullptr) { // e.g., packet came from local node's cache
		hopCount = *hopCountTag;
	}
	NS_LOG_DEBUG("Hop count: " << hopCount);

	SeqTimeoutsContainer::iterator entry = m_seqLastDelay.find(seq);
	if (entry != m_seqLastDelay.end()) {
		m_lastRetransmittedInterestDataDelay(this, seq, Simulator::Now() - entry->time, hopCount);
	}

	entry = m_seqFullDelay.find(seq);
	if (entry != m_seqFullDelay.end()) {
		m_firstInterestDataDelay(this, seq, Simulator::Now() - entry->time, m_seqRetxCounts[seq], hopCount);
	}

	m_seqRetxCounts.erase(seq);
	m_seqFullDelay.erase(seq);
	m_seqLastDelay.erase(seq);

	m_seqTimeouts.erase(seq);
	m_retxSeqs.erase(seq);

	m_rtt->AckSeq(SequenceNumber32(seq));

	if(getServiceNum() == 300)
		Simulator::Stop();

}

void
Consumer::OnNack(shared_ptr<const lp::Nack> nack)
{
	/// tracing inside
	App::OnNack(nack);

	NS_LOG_INFO("NACK received for: " << nack->getInterest().getName()
			<< ", reason: " << nack->getReason());
}

void
Consumer::OnTimeout(uint32_t sequenceNumber)
{
	NS_LOG_FUNCTION(sequenceNumber);
	// std::cout << Simulator::Now () << ", TO: " << sequenceNumber << ", current RTO: " <<
	// m_rtt->RetransmitTimeout ().ToDouble (Time::S) << "s\n";

	m_rtt->IncreaseMultiplier(); // Double the next RTO
	m_rtt->SentSeq(SequenceNumber32(sequenceNumber),
			1); // make sure to disable RTT calculation for this sample
	m_retxSeqs.insert(sequenceNumber);
	ScheduleNextPacket();
}

void
Consumer::WillSendOutInterest(uint32_t sequenceNumber)
{
	NS_LOG_DEBUG("Trying to add " << sequenceNumber << " with " << Simulator::Now() << ". already "
			<< m_seqTimeouts.size() << " items");

	m_seqTimeouts.insert(SeqTimeout(sequenceNumber, Simulator::Now()));
	m_seqFullDelay.insert(SeqTimeout(sequenceNumber, Simulator::Now()));

	m_seqLastDelay.erase(sequenceNumber);
	m_seqLastDelay.insert(SeqTimeout(sequenceNumber, Simulator::Now()));

	m_seqRetxCounts[sequenceNumber]++;

	m_rtt->SentSeq(SequenceNumber32(sequenceNumber), 1);
}

} // namespace ndn
} // namespace ns3
