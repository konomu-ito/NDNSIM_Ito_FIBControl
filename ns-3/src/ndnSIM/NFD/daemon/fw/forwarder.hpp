/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014-2016,  Regents of the University of California,
 *                           Arizona Board of Regents,
 *                           Colorado State University,
 *                           University Pierre & Marie Curie, Sorbonne University,
 *                           Washington University in St. Louis,
 *                           Beijing Institute of Technology,
 *                           The University of Memphis.
 *
 * This file is part of NFD (Named Data Networking Forwarding Daemon).
 * See AUTHORS.md for complete list of NFD authors and contributors.
 *
 * NFD is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * NFD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * NFD, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NFD_DAEMON_FW_FORWARDER_HPP
#define NFD_DAEMON_FW_FORWARDER_HPP

#include "core/common.hpp"
#include "core/scheduler.hpp"
#include "forwarder-counters.hpp"
#include "face-table.hpp"
#include "unsolicited-data-policy.hpp"
#include "table/fib.hpp"
#include "table/pit.hpp"
#include "table/cs.hpp"
#include "table/measurements.hpp"
#include "table/strategy-choice.hpp"
#include "table/dead-nonce-list.hpp"
#include "table/network-region-table.hpp"
#include "ns3/node.h"
#include "ns3/ptr.h"
#include "ns3/ndnSIM/model/ndn-l3-protocol.hpp"
#include "ns3/ndnSIM/model/cs/ndn-content-store.hpp"

namespace nfd {

namespace fw {
class Strategy;
} // namespace fw

/** \brief main class of NFD
 *
 *  Forwarder owns all faces and tables, and implements forwarding pipelines.
 */
class Forwarder
{
public:
	Forwarder();

	VIRTUAL_WITH_TESTS
	~Forwarder();

	void
	setNode(ns3::Ptr<ns3::Node> node) {
		m_node = node;
	}

	ns3::Ptr<ns3::Node>
	getNode()
	{
		return m_node;
	}

	const ForwarderCounters&
	getCounters() const
	{
		return m_counters;
	}

public: // faces and policies
	FaceTable&
	getFaceTable()
	{
		return m_faceTable;
	}

	/** \brief get existing Face
	 *
	 *  shortcut to .getFaceTable().get(face)
	 */
	Face*
	getFace(FaceId id) const
	{
		return m_faceTable.get(id);
	}

	/** \brief add new Face
	 *
	 *  shortcut to .getFaceTable().add(face)
	 */
	void
	addFace(shared_ptr<Face> face)
	{
		m_faceTable.add(face);
	}

	fw::UnsolicitedDataPolicy&
	getUnsolicitedDataPolicy() const
	{
		return *m_unsolicitedDataPolicy;
	}

	void
	setUnsolicitedDataPolicy(unique_ptr<fw::UnsolicitedDataPolicy> policy)
	{
		BOOST_ASSERT(policy != nullptr);
		m_unsolicitedDataPolicy = std::move(policy);
	}

public: // forwarding entrypoints and tables
	/** \brief start incoming Interest processing
	 *  \param face face on which Interest is received
	 *  \param interest the incoming Interest, must be created with make_shared
	 */
	void
	startProcessInterest(Face& face, const Interest& interest);

	/** \brief start incoming Data processing
	 *  \param face face on which Data is received
	 *  \param data the incoming Data, must be created with make_shared
	 */
	void
	startProcessData(Face& face, const Data& data);

	/** \brief start incoming Nack processing
	 *  \param face face on which Nack is received
	 *  \param nack the incoming Nack, must be created with make_shared
	 */
	void
	startProcessNack(Face& face, const lp::Nack& nack);

	NameTree&
	getNameTree()
	{
		return m_nameTree;
	}

	Fib&
	getFib()
	{
		return m_fib;
	}

	Pit&
	getPit()
	{
		return m_pit;
	}

	Cs&
	getCs()
	{
		return m_cs;
	}

	Measurements&
	getMeasurements()
	{
		return m_measurements;
	}

	StrategyChoice&
	getStrategyChoice()
	{
		return m_strategyChoice;
	}

	DeadNonceList&
	getDeadNonceList()
	{
		return m_deadNonceList;
	}

	NetworkRegionTable&
	getNetworkRegionTable()
	{
		return m_networkRegionTable;
	}

public: // allow enabling ndnSIM content store (will be removed in the future)
	void
	setCsFromNdnSim(ns3::Ptr<ns3::ndn::ContentStore> cs)
	{
		m_csFromNdnSim = cs;
	}

public:
	/** \brief trigger before PIT entry is satisfied
	 *  \sa Strategy::beforeSatisfyInterest
	 */
	signal::Signal<Forwarder, pit::Entry, Face, Data> beforeSatisfyInterest;

	/** \brief trigger before PIT entry expires
	 *  \sa Strategy::beforeExpirePendingInterest
	 */
	signal::Signal<Forwarder, pit::Entry> beforeExpirePendingInterest;

	PUBLIC_WITH_TESTS_ELSE_PRIVATE: // pipelines
	/** \brief incoming Interest pipeline
	 */
	VIRTUAL_WITH_TESTS void
	onIncomingInterest(Face& inFace, const Interest& interest);

	/** \brief Interest loop pipeline
	 */
	VIRTUAL_WITH_TESTS void
	onInterestLoop(Face& inFace, const Interest& interest);

	/** \brief Content Store miss pipeline
	 */
	VIRTUAL_WITH_TESTS void
	onContentStoreMiss(const Face& inFace, const shared_ptr<pit::Entry> pitEntry, const Interest& interest);

	/** \brief Content Store hit pipeline
	 */
	VIRTUAL_WITH_TESTS void
	onContentStoreHit(const Face& inFace, const shared_ptr<pit::Entry>& pitEntry,
			const Interest& interest, const Data& data);

	/** \brief outgoing Interest pipeline
	 */
	VIRTUAL_WITH_TESTS void
	onOutgoingInterest(const shared_ptr<pit::Entry>& pitEntry, Face& outFace, const Interest& interest);

	/** \brief Interest reject pipeline
	 */
	VIRTUAL_WITH_TESTS void
	onInterestReject(const shared_ptr<pit::Entry>& pitEntry);

	/** \brief Interest unsatisfied pipeline
	 */
	VIRTUAL_WITH_TESTS void
	onInterestUnsatisfied(const shared_ptr<pit::Entry>& pitEntry);

	/** \brief Interest finalize pipeline
	 *  \param isSatisfied whether the Interest has been satisfied
	 *  \param dataFreshnessPeriod FreshnessPeriod of satisfying Data
	 */
	VIRTUAL_WITH_TESTS void
	onInterestFinalize(const shared_ptr<pit::Entry>& pitEntry, bool isSatisfied,
			time::milliseconds dataFreshnessPeriod = time::milliseconds(-1));

	/** \brief incoming Data pipeline
	 */
	VIRTUAL_WITH_TESTS void
	onIncomingData(Face& inFace, const Data& data);

	/** \brief Data unsolicited pipeline
	 */
	VIRTUAL_WITH_TESTS void
	onDataUnsolicited(Face& inFace, const Data& data);

	/** \brief outgoing Data pipeline
	 */
	VIRTUAL_WITH_TESTS void
	onOutgoingData(const Data& data, Face& outFace);

	/** \brief incoming Nack pipeline
	 */
	VIRTUAL_WITH_TESTS void
	onIncomingNack(Face& inFace, const lp::Nack& nack);

	/** \brief outgoing Nack pipeline
	 */
	VIRTUAL_WITH_TESTS void
	onOutgoingNack(const shared_ptr<pit::Entry>& pitEntry, const Face& outFace, const lp::NackHeader& nack);

	PROTECTED_WITH_TESTS_ELSE_PRIVATE:
	VIRTUAL_WITH_TESTS void
	setUnsatisfyTimer(const shared_ptr<pit::Entry>& pitEntry);

	VIRTUAL_WITH_TESTS void
	setStragglerTimer(const shared_ptr<pit::Entry>& pitEntry, bool isSatisfied,
			time::milliseconds dataFreshnessPeriod = time::milliseconds(-1));

	VIRTUAL_WITH_TESTS void
	cancelUnsatisfyAndStragglerTimer(pit::Entry& pitEntry);

	/** \brief insert Nonce to Dead Nonce List if necessary
	 *  \param upstream if null, insert Nonces from all out-records;
	 *                  if not null, insert Nonce only on the out-records of this face
	 */
	VIRTUAL_WITH_TESTS void
	insertDeadNonceList(pit::Entry& pitEntry, bool isSatisfied,
			time::milliseconds dataFreshnessPeriod, Face* upstream);

	/** \brief call trigger (method) on the effective strategy of pitEntry
	 */
#ifdef WITH_TESTS
	virtual void
	dispatchToStrategy(pit::Entry& pitEntry, function<void(fw::Strategy&)> trigger)
#else
	template<class Function>
	void
	dispatchToStrategy(pit::Entry& pitEntry, Function trigger)
#endif
	{
		trigger(m_strategyChoice.findEffectiveStrategy(pitEntry));
	}

private:
	ForwarderCounters m_counters;

	ns3::Ptr<ns3::Node> m_node;

	FaceTable m_faceTable;
	unique_ptr<fw::UnsolicitedDataPolicy> m_unsolicitedDataPolicy;

	NameTree           m_nameTree;
	Fib                m_fib;
	Pit                m_pit;
	Cs                 m_cs;
	Measurements       m_measurements;
	StrategyChoice     m_strategyChoice;
	DeadNonceList      m_deadNonceList;
	NetworkRegionTable m_networkRegionTable;
	shared_ptr<Face>   m_csFace;

	ns3::Ptr<ns3::ndn::ContentStore> m_csFromNdnSim;
	int table[2][6][3]={
			{{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
			{{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
	};
	/*table index
	 * 1st hop or count
	 *  0:hop
	 *  1:count
	 * 2nd function number 1 to 5
	 * 3rd function character 0 to 2 = a to c
	 *
	 * ex) table[1][1][0] means f1a count
	 */

	time::milliseconds m_resetTime;
	/*
  int fcc1a =0;
  int fcc1b =0;
  int fcc1c =0;
  int fcc2a =0;
  int fcc2b =0;
  int fcc2c =0;
  int fcc3a =0;
  int fcc3b =0;
  int fcc3c =0;
  int fcc4a =0;
  int fcc4b =0;
  int fcc4c =0;
  int fcc5a =0;
  int fcc5b =0;
  int fcc5c =0;
  int hop1a =1;
  int hop1b =2;
  int hop1c =3;
  int hop2a =1;
  int hop2b =2;
  int hop2c =3;
  int hop3a =1;
  int hop3b =2;
  int hop3c =3;
  int hop4a =1;
  int hop4b =2;
  int hop4c =3;
  int hop5a =1;
  int hop5b =2;
  int hop5c =3;
	 */
	// allow Strategy (base class) to enter pipelines
	friend class fw::Strategy;
};

} // namespace nfd

#endif // NFD_DAEMON_FW_FORWARDER_HPP
