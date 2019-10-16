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

#include "forwarder.hpp"
#include "algorithm.hpp"
#include "core/logger.hpp"
#include "strategy.hpp"
#include "table/cleanup.hpp"
#include <ndn-cxx/lp/tags.hpp>
#include "face/null-face.hpp"
#include <boost/random/uniform_int_distribution.hpp>
#include <chrono>
#include <thread>

namespace nfd {

NFD_LOG_INIT("Forwarder");

Forwarder::Forwarder()
  : m_unsolicitedDataPolicy(new fw::DefaultUnsolicitedDataPolicy())
  , m_fib(m_nameTree)
  , m_pit(m_nameTree)
  , m_measurements(m_nameTree)
  , m_strategyChoice(m_nameTree, fw::makeDefaultStrategy(*this))
  , m_csFace(face::makeNullFace(FaceUri("contentstore://")))
{
  fw::installStrategies(*this);
  getFaceTable().addReserved(m_csFace, face::FACEID_CONTENT_STORE);

  m_faceTable.afterAdd.connect([this] (Face& face) {
    face.afterReceiveInterest.connect(
      [this, &face] (const Interest& interest) {
        this->startProcessInterest(face, interest);
      });
    face.afterReceiveData.connect(
      [this, &face] (const Data& data) {
        this->startProcessData(face, data);
      });
    face.afterReceiveNack.connect(
      [this, &face] (const lp::Nack& nack) {
        this->startProcessNack(face, nack);
      });
  });

  m_faceTable.beforeRemove.connect([this] (Face& face) {
    cleanupOnFaceRemoval(m_nameTree, m_fib, m_pit, face);
  });
}

Forwarder::~Forwarder() = default;

void
Forwarder::startProcessInterest(Face& face, const Interest& interest)
{
  // check fields used by forwarding are well-formed
  try {
    if (interest.hasLink()) {
      interest.getLink();
    }
  }
  catch (const tlv::Error&) {
    NFD_LOG_DEBUG("startProcessInterest face=" << face.getId() <<
                  " interest=" << interest.getName() << " malformed");
    // It's safe to call interest.getName() because Name has been fully parsed
    return;
  }

  this->onIncomingInterest(face, interest);
}

void
Forwarder::startProcessData(Face& face, const Data& data)
{
  // check fields used by forwarding are well-formed
  // (none needed)

  this->onIncomingData(face, data);
}

void
Forwarder::startProcessNack(Face& face, const lp::Nack& nack)
{
  // check fields used by forwarding are well-formed
  try {
    if (nack.getInterest().hasLink()) {
      nack.getInterest().getLink();
    }
  }
  catch (const tlv::Error&) {
    NFD_LOG_DEBUG("startProcessNack face=" << face.getId() <<
                  " nack=" << nack.getInterest().getName() <<
                  "~" << nack.getReason() << " malformed");
    return;
  }

  this->onIncomingNack(face, nack);
}

void
Forwarder::onIncomingInterest(Face& inFace, const Interest& interest)
{
  // receive Interest
  NFD_LOG_DEBUG("onIncomingInterest face=" << inFace.getId() <<
                " interest=" << interest.getName());
  interest.setTag(make_shared<lp::IncomingFaceIdTag>(inFace.getId()));
  ++m_counters.nInInterests;

  // /localhost scope control
  bool isViolatingLocalhost = inFace.getScope() == ndn::nfd::FACE_SCOPE_NON_LOCAL &&
                              scope_prefix::LOCALHOST.isPrefixOf(interest.getName());
  if (isViolatingLocalhost) {
    NFD_LOG_DEBUG("onIncomingInterest face=" << inFace.getId() <<
                  " interest=" << interest.getName() << " violates /localhost");
    // (drop)
    return;
  }

  // detect duplicate Nonce with Dead Nonce List
  bool hasDuplicateNonceInDnl = m_deadNonceList.has(interest.getName(), interest.getNonce());
  
  /*
  if (hasDuplicateNonceInDnl) {
    // goto Interest loop pipeline
    this->onInterestLoop(inFace, interest);
    return;
  }
  */

  // PIT insert
  shared_ptr<pit::Entry> pitEntry = m_pit.insert(interest).first;

  // detect duplicate Nonce in PIT entry
  bool hasDuplicateNonceInPit = fw::findDuplicateNonce(*pitEntry, interest.getNonce(), inFace) !=
                                fw::DUPLICATE_NONCE_NONE;
  
  /*
  if (hasDuplicateNonceInPit) {
    // goto Interest loop pipeline
    this->onInterestLoop(inFace, interest);
    return;
  }
  */

  // cancel unsatisfy & straggler timer
  this->cancelUnsatisfyAndStragglerTimer(*pitEntry);

  const pit::InRecordCollection& inRecords = pitEntry->getInRecords();
  bool isPending = inRecords.begin() != inRecords.end();
  if (!isPending) {
    if (m_csFromNdnSim == nullptr) {
      m_cs.find(interest,
                bind(&Forwarder::onContentStoreHit, this, ref(inFace), pitEntry, _1, _2),
                bind(&Forwarder::onContentStoreMiss, this, ref(inFace), pitEntry, _1));
    }
    else {
      shared_ptr<Data> match = m_csFromNdnSim->Lookup(interest.shared_from_this());
      if (match != nullptr) {
        this->onContentStoreHit(inFace, pitEntry, interest, *match);
      }
      else {
        this->onContentStoreMiss(inFace, pitEntry, interest);
      }
    }
  }
  else {
    this->onContentStoreMiss(inFace, pitEntry, interest);
  }
}

void
Forwarder::onInterestLoop(Face& inFace, const Interest& interest)
{
  // if multi-access face, drop
  if (inFace.getLinkType() == ndn::nfd::LINK_TYPE_MULTI_ACCESS) {
    NFD_LOG_DEBUG("onInterestLoop face=" << inFace.getId() <<
                  " interest=" << interest.getName() <<
                  " drop");
    return;
  }

  NFD_LOG_DEBUG("onInterestLoop face=" << inFace.getId() <<
                " interest=" << interest.getName() <<
                " send-Nack-duplicate");

  // send Nack with reason=DUPLICATE
  // note: Don't enter outgoing Nack pipeline because it needs an in-record.
  lp::Nack nack(interest);
  nack.setReason(lp::NackReason::DUPLICATE);
  inFace.sendNack(nack);
}

void
Forwarder::onContentStoreMiss(const Face& inFace, const shared_ptr<pit::Entry>& pitEntry,
                              const Interest& interest)
{
  NFD_LOG_DEBUG("onContentStoreMiss interest=" << interest.getName());

  // insert in-record
  pitEntry->insertOrUpdateInRecord(const_cast<Face&>(inFace), interest);

 //ADDED Print Interest functionName
 //std::cout << "Name: " << interest.getName() << std::endl;
 /*
 if(interest.getName().toUri().find("localhost") == std::string::npos){
   interest.removeHeadFunction(interest);
   std::cout << "Function Name:" << interest.getFunction() << std::endl;
 }
 */

  // set PIT unsatisfy timer
  this->setUnsatisfyTimer(pitEntry);

//ADDED Find Fib Entry for Function Chaining
 //std::cout<<"Node "<<getNode()->GetId()<<std::endl;
 //std::cout<<"NONCE: "<<interest.getNonce()<<std::endl;

  Name functionName = interest.getFunction();

  auto string = functionName.toUri();
  //std::cout << "Function Name:" << string << std::endl;
  auto separator = std::string("/");
  auto separator_length = separator.length();

  auto list = std::vector<std::string>();

  if (separator_length == 0) {
    list.push_back(string);
  } else {
    auto offset = std::string::size_type(0);
    while (1) {
      auto pos = string.find(separator, offset);
      if (pos == std::string::npos) {
        list.push_back(string.substr(offset));
        break;
      }
      list.push_back(string.substr(offset, pos - offset));
      offset = pos + separator_length;
    }
  }

  //std::cout << list[0] << std::endl;
  //std::cout << list[1] << std::endl;

  int currentNode = getNode()->GetId();
  std::string currentNodeName;
  int funcNum = 0;

  /*
  //for us
  switch(currentNode) {
    case 0:
      currentNodeName = "Consumer";
      break;
    case 1:
      currentNodeName = "Node1";
      break;
    case 2:
      currentNodeName = "Node2";
      break;
    case 3:
      currentNodeName = "Node3";
      break;
    case 4:
      currentNodeName = "Node4";
      break;
    case 5:
      currentNodeName = "Node5";
      break;
    case 6:
      currentNodeName = "Node6";
      break;
    case 7:
      currentNodeName = "Node7";
      break;
    case 8:
      currentNodeName = "Node8";
      break;
    case 9:
      currentNodeName = "Node9";
      break;
    case 10:
      currentNodeName = "Node10";
      break;
    case 11:
      currentNodeName = "Node11";
      break;
    case 12:
      currentNodeName = "Node12";
      break;
    case 13:
      currentNodeName = "Node13";
      break;
    case 14:
      currentNodeName = "Node14";
      break;
    case 15:
      currentNodeName = "Node15";
      break;
    case 16:
      currentNodeName = "Node16";
      break;
    case 17:
      currentNodeName = "Node17";
      break;
    case 18:
      currentNodeName = "Node18";
      break;
    case 19:
      currentNodeName = "Node19";
      break;
    case 20:
      currentNodeName = "Node20";
      break;
    case 21:
      currentNodeName = "Node21";
      break;
    case 22:
      currentNodeName = "Node22";
      break;
    case 23:
      currentNodeName = "Node23";
      break;
    case 24:
      currentNodeName = "Node24";
      break;
    case 25:
      currentNodeName = "Producer";
      break;
    case 26:
      currentNodeName = "F1a";
      funcNum = 1;
      break;
    case 27:
      currentNodeName = "F1b";
      funcNum = 2;
      break;
    case 28:
      currentNodeName = "F1c";
      funcNum = 3;
      break;
    case 29:
      currentNodeName = "F2a";
      funcNum = 4;
      break;
    case 30:
      currentNodeName = "F2b";
      funcNum = 5;
      break;
    case 31:
      currentNodeName = "F2c";
      funcNum = 6;
      break;
    case 32:
      currentNodeName = "F3a";
      funcNum = 7;
      break;
    case 33:
      currentNodeName = "F3b";
      funcNum = 8;
      break;
    case 34:
      currentNodeName = "F3c";
      funcNum = 9;
      break;
    case 35:
      currentNodeName = "F4a";
      funcNum = 10;
      break;
    case 36:
      currentNodeName = "F4b";
      funcNum = 11;
      break;
    case 37:
      currentNodeName = "F4c";
      funcNum = 12;
      break;
    case 38:
      currentNodeName = "F5a";
      funcNum = 13;
      break;
    case 39:
      currentNodeName = "F5b";
      funcNum = 14;
      break;
    case 40:
      currentNodeName = "F5c";
      funcNum = 15;
      break;
    case 41:
      currentNodeName = "Consumer2";
      break;
    case 42:
      currentNodeName = "Consumer3";
      break;
    case 43:
      currentNodeName = "Consumer4";
      break;
    case 44:
      currentNodeName = "Producer2";
      break;
    case 45:
      currentNodeName = "Producer3";
      break;
    case 46:
      currentNodeName = "Producer4";
      break;
  }
  */

  
  //for us1
  switch(currentNode) {
    case 0:
      currentNodeName = "Consumer1";
      break;
    case 1:
      currentNodeName = "Node1";
      break;
    case 2:
      currentNodeName = "Node2";
      break;
    case 3:
      currentNodeName = "Node3";
      break;
    case 4:
      currentNodeName = "Node4";
      break;
    case 5:
      currentNodeName = "Node5";
      break;
    case 6:
      currentNodeName = "Node6";
      break;
    case 7:
      currentNodeName = "Node7";
      break;
    case 8:
      currentNodeName = "Node8";
      break;
    case 9:
      currentNodeName = "Node9";
      break;
    case 10:
      currentNodeName = "Node10";
      break;
    case 11:
      currentNodeName = "Node11";
      break;
    case 12:
      currentNodeName = "Node12";
      break;
    case 13:
      currentNodeName = "Node13";
      break;
    case 14:
      currentNodeName = "Node14";
      break;
    case 15:
      currentNodeName = "Node15";
      break;
    case 16:
      currentNodeName = "Node16";
      break;
    case 17:
      currentNodeName = "Node17";
      break;
    case 18:
      currentNodeName = "Node18";
      break;
    case 19:
      currentNodeName = "Node19";
      break;
    case 20:
      currentNodeName = "Node20";
      break;
    case 21:
      currentNodeName = "Node21";
      break;
    case 22:
      currentNodeName = "Node22";
      break;
    case 23:
      currentNodeName = "Node23";
      break;
    case 24:
      currentNodeName = "Node24";
      break;
    case 25:
      currentNodeName = "Producer1";
      break;
    case 26:
      currentNodeName = "F1a";
      funcNum = 1;
      break;
    case 27:
      currentNodeName = "F1b";
      funcNum = 2;
      break;
    case 28:
      currentNodeName = "F1c";
      funcNum = 3;
      break;
    case 29:
      currentNodeName = "F2a";
      funcNum = 4;
      break;
    case 30:
      currentNodeName = "F2b";
      funcNum = 5;
      break;
    case 31:
      currentNodeName = "F2c";
      funcNum = 6;
      break;
    case 32:
      currentNodeName = "F3a";
      funcNum = 7;
      break;
    case 33:
      currentNodeName = "F3b";
      funcNum = 8;
      break;
    case 34:
      currentNodeName = "F3c";
      funcNum = 9;
      break;
    case 35:
      currentNodeName = "F4a";
      funcNum = 10;
      break;
    case 36:
      currentNodeName = "F4b";
      funcNum = 11;
      break;
    case 37:
      currentNodeName = "F4c";
      funcNum = 12;
      break;
    case 38:
      currentNodeName = "F5a";
      funcNum = 13;
      break;
    case 39:
      currentNodeName = "F5b";
      funcNum = 14;
      break;
    case 40:
      currentNodeName = "F5c";
      funcNum = 15;
      break;
    case 41:
      currentNodeName = "Consumer2";
      break;
    case 42:
      currentNodeName = "Consumer3";
      break;
    case 43:
      currentNodeName = "Consumer4";
      break;
    case 44:
      currentNodeName = "Producer2";
      break;
    case 45:
      currentNodeName = "Producer3";
      break;
    case 46:
      currentNodeName = "Producer4";
      break;
  }
  

  std::cout << "Node          : " << currentNodeName << std::endl;
  std::cout << "Function Name : " << interest.getFunction() << std::endl;
  std::cout << "Content  Name : " << interest.getName() << std::endl;

  if (interest.getFunctionFlag() == 1){
    interest.setFunctionFlag(0);
    time::milliseconds nowTime = time::toUnixTimestamp(time::system_clock::now());
    time::milliseconds functionTime = nowTime - interest.getFunctionTime();
    //std::cout << "functionTime: " << functionTime.count() << std::endl;
    time::milliseconds serviceTime = interest.getServiceTime() + functionTime;
    interest.setServiceTime(serviceTime);
    //std::cout << "ServiceTime: " << interest.getServiceTime().count() << std::endl;
  }

  if (list[1] == currentNodeName){
    interest.removeHeadFunction(interest);
    interest.setFunctionFlag(1);
    if(ns3::getChoiceType() == 0){
    	ns3::increaseTotalFcc(funcNum);
    	ns3::increaseAllFcc();
    	if(ns3::getAllFcc() == 30){
    		ns3::resetFcc();
    	}
    }else if(ns3::getChoiceType() == 2){
    	std::string funcStr;
    	/*table index
    	 * 1st hop or count
    	 *  0:hop
    	 *  1:count
    	 * 2nd function number 1 to 5
    	 * 3rd function character 0 to 2
    	 */
    	if(list[2].compare("F1")==0){
    		if(table[0][1][0]+table[1][1][0]<=table[0][1][1]+table[1][1][1] && table[0][1][0]+table[1][1][0]<=table[0][1][2]+table[1][1][2]){
    			funcStr = "/F1a";
    		}else if(table[0][1][1]+table[1][1][1]<=table[0][1][2]+table[1][1][2]){
    			funcStr = "/F1b";
    		}else{
    			funcStr = "/F1c";
    		}
    	}else if(list[2].compare("F2")==0){
    		if(table[0][2][0]+table[1][2][0]<=table[0][2][1]+table[1][2][1] && table[0][2][0]+table[1][2][0]<=table[0][2][2]+table[1][2][2]){
    		    funcStr = "/F2a";
    		}else if(table[0][2][1]+table[1][2][1]<=table[0][2][2]+table[1][2][2]){
    			funcStr = "/F2b";
    		}else{
    			funcStr = "/F2c";
    		}
    	}else if(list[2].compare("F3")==0){
    		if(table[0][3][0]+table[1][3][0]<=table[0][3][1]+table[1][3][1] && table[0][3][0]+table[1][3][0]<=table[0][3][2]+table[1][3][2]){
    			if(table[0][3][0]+table[1][3][0]<=table[0][3][2]+table[1][3][2]){
    				funcStr = "/F3a";
    			}
    		}else if(table[0][3][1]+table[1][3][1]<=table[0][3][2]+table[1][3][2]){
    			funcStr = "/F3b";
    		}else{
    			funcStr = "/F3c";
    		}
    	}else if(list[2].compare("F4")==0){
    		if(table[0][4][0]+table[1][4][0]<table[0][4][1]+table[1][4][1] && table[0][4][0]+table[1][4][0]<=table[0][4][2]+table[1][4][2]){
    			funcStr = "/F4a";
    		}else if(table[0][4][1]+table[1][4][1]<table[0][4][2]+table[1][4][2]){
    			funcStr = "/F4b";
    		}else{
    			funcStr = "/F4c";
    		}
    	}else if(list[2].compare("F5")==0){
    		if(table[0][5][0]+table[1][5][0]<table[0][5][1]+table[1][5][1] && table[0][5][0]+table[1][5][0]<=table[0][5][2]+table[1][5][2]){
    			funcStr = "/F5a";
    		}else if(table[0][5][1]+table[1][5][1]<table[0][5][2]+table[1][5][2]){
    			funcStr = "/F5b";
    		}else{
    			funcStr = "/F5c";
    		}
    	}
    	interest.replaceHeadFunction(interest,make_shared<std::string>(funcStr));
    	interest.addFunctionFullName(Name(funcStr));
    	//std::cout << "funcName: " << interest.getFunction() << ", const: " << interest.getFunctionFullName() << std::endl;
    }
  }

  functionName = interest.getFunction();
  fib::Entry* fibEntry;
  Face* nextHopFaceFunction;
  if(functionName.toUri() != "/"){ //When Function Field is not Empty
    //std::cout << "function routing" << std::endl;
    fibEntry = m_fib.findLongestPrefixMatchFunction(functionName);
    //std::cout << "FIB : " << fibEntry->getPrefix().toUri() << std::endl;
    //std::cout << "Node: " << currentNode << std::endl;

    time::milliseconds nowTime = time::toUnixTimestamp(time::system_clock::now());

    /*
    // for us
    if(currentNode == 2 && fibEntry->getPrefix().toUri() == "/F1a"){
      interest.setFunctionTime(nowTime);
    } else if(currentNode == 11 && fibEntry->getPrefix().toUri() == "/F1b"){
      interest.setFunctionTime(nowTime);
    } else if(currentNode == 18 && fibEntry->getPrefix().toUri() == "/F1c"){
      interest.setFunctionTime(nowTime);
    } else if(currentNode == 7 && fibEntry->getPrefix().toUri() == "/F2a"){
      interest.setFunctionTime(nowTime);
    } else if(currentNode == 10 && fibEntry->getPrefix().toUri() == "/F2b"){
      interest.setFunctionTime(nowTime);
    } else if(currentNode == 16 && fibEntry->getPrefix().toUri() == "/F2c"){
      interest.setFunctionTime(nowTime);
    } else if(currentNode == 8 && fibEntry->getPrefix().toUri() == "/F3a"){
      interest.setFunctionTime(nowTime);
    } else if(currentNode == 13 && fibEntry->getPrefix().toUri() == "/F3b"){
      interest.setFunctionTime(nowTime);
    } else if(currentNode == 15 && fibEntry->getPrefix().toUri() == "/F3c"){
      interest.setFunctionTime(nowTime);
    } else if(currentNode == 5 && fibEntry->getPrefix().toUri() == "/F4a"){
      interest.setFunctionTime(nowTime);
    } else if(currentNode == 12 && fibEntry->getPrefix().toUri() == "/F4b"){
      interest.setFunctionTime(nowTime);
    } else if(currentNode == 21 && fibEntry->getPrefix().toUri() == "/F4c"){
      interest.setFunctionTime(nowTime);
    } else if(currentNode == 3 && fibEntry->getPrefix().toUri() == "/F5a"){
      interest.setFunctionTime(nowTime);
    } else if(currentNode == 9 && fibEntry->getPrefix().toUri() == "/F5b"){
      interest.setFunctionTime(nowTime);
    } else if(currentNode == 23 && fibEntry->getPrefix().toUri() == "/F5c"){
      interest.setFunctionTime(nowTime);
    }
    */
    
    // for us1
    if(currentNode == 3 && fibEntry->getPrefix().toUri() == "/F1a"){
      interest.setFunctionTime(nowTime);
    } else if(currentNode == 10 && fibEntry->getPrefix().toUri() == "/F1b"){
      interest.setFunctionTime(nowTime);
    } else if(currentNode == 21 && fibEntry->getPrefix().toUri() == "/F1c"){
      interest.setFunctionTime(nowTime);
    } else if(currentNode == 7 && fibEntry->getPrefix().toUri() == "/F2a"){
      interest.setFunctionTime(nowTime);
    } else if(currentNode == 13 && fibEntry->getPrefix().toUri() == "/F2b"){
      interest.setFunctionTime(nowTime);
    } else if(currentNode == 23 && fibEntry->getPrefix().toUri() == "/F2c"){
      interest.setFunctionTime(nowTime);
    } else if(currentNode == 4 && fibEntry->getPrefix().toUri() == "/F3a"){
      interest.setFunctionTime(nowTime);
    } else if(currentNode == 12 && fibEntry->getPrefix().toUri() == "/F3b"){
      interest.setFunctionTime(nowTime);
    } else if(currentNode == 17 && fibEntry->getPrefix().toUri() == "/F3c"){
      interest.setFunctionTime(nowTime);
    } else if(currentNode == 6 && fibEntry->getPrefix().toUri() == "/F4a"){
      interest.setFunctionTime(nowTime);
    } else if(currentNode == 9 && fibEntry->getPrefix().toUri() == "/F4b"){
      interest.setFunctionTime(nowTime);
    } else if(currentNode == 15 && fibEntry->getPrefix().toUri() == "/F4c"){
      interest.setFunctionTime(nowTime);
    } else if(currentNode == 2 && fibEntry->getPrefix().toUri() == "/F5a"){
      interest.setFunctionTime(nowTime);
    } else if(currentNode == 11 && fibEntry->getPrefix().toUri() == "/F5b"){
      interest.setFunctionTime(nowTime);
    } else if(currentNode == 20 && fibEntry->getPrefix().toUri() == "/F5c"){
      interest.setFunctionTime(nowTime);
    }


    std::cout << "--------------------------------------------" << std::endl;
    

    if(fibEntry != nullptr){ //When there is matching FIB route
     //std::cout<<"LPM FIB Route: "<<fibEntry->getPrefix()<<std::endl;
      for(auto it : fibEntry->getNextHops()){
        nextHopFaceFunction = &it.getFace();
       //std::cout<< "Passed: "<<interest.getNonce()<<std::endl;

       //interest.removeHeadFunction(interest);
       //Go to OutGoingInterest Pipeline
       this->onOutgoingInterest(pitEntry, *nextHopFaceFunction, interest);
     }
     return;

    }
  }

  // has NextHopFaceId?
  shared_ptr<lp::NextHopFaceIdTag> nextHopTag = interest.getTag<lp::NextHopFaceIdTag>();
  //std::cout << nextHopTag << std::endl;
  if (nextHopTag != nullptr) {
    // chosen NextHop face exists?
    Face* nextHopFace = m_faceTable.get(*nextHopTag);
    if (nextHopFace != nullptr) {
      NFD_LOG_DEBUG("onContentStoreMiss interest=" << interest.getName() << " nexthop-faceid=" << nextHopFace->getId());
      // go to outgoing Interest pipeline
      // scope control is unnecessary, because privileged app explicitly wants to forward
      this->onOutgoingInterest(pitEntry, *nextHopFace, interest);
    }
    return;
  }

  std::cout << "default routing" << std::endl;
  std::cout << "--------------------------------------------" << std::endl;
  // dispatch to strategy: after incoming Interest
  this->dispatchToStrategy(*pitEntry,
    [&] (fw::Strategy& strategy) { strategy.afterReceiveInterest(inFace, interest, pitEntry); });
}

void
Forwarder::onContentStoreHit(const Face& inFace, const shared_ptr<pit::Entry>& pitEntry,
                             const Interest& interest, const Data& data)
{
  NFD_LOG_DEBUG("onContentStoreHit interest=" << interest.getName());

  beforeSatisfyInterest(*pitEntry, *m_csFace, data);
  this->dispatchToStrategy(*pitEntry,
    [&] (fw::Strategy& strategy) { strategy.beforeSatisfyInterest(pitEntry, *m_csFace, data); });

  data.setTag(make_shared<lp::IncomingFaceIdTag>(face::FACEID_CONTENT_STORE));
  //should we lookup PIT for other Interests that also match csMatch?

  // set PIT straggler timer
  this->setStragglerTimer(pitEntry, true, data.getFreshnessPeriod());

  // goto outgoing Data pipeline
  this->onOutgoingData(data, *const_pointer_cast<Face>(inFace.shared_from_this()));
}

void
Forwarder::onOutgoingInterest(const shared_ptr<pit::Entry>& pitEntry, Face& outFace, const Interest& interest)
{
  NFD_LOG_DEBUG("onOutgoingInterest face=" << outFace.getId() <<
                " interest=" << pitEntry->getName());

  // insert out-record
  pitEntry->insertOrUpdateOutRecord(outFace, interest);

  /*
  int currentNode = getNode()->GetId();

  if (interest.getFunctionFlag() == 1){
    interest.setFunctionFlag(0);
    ns3::decreaseFunctionCallCount(currentNode + 10);
  }
  */

  // send Interest
  outFace.sendInterest(interest);
  ++m_counters.nOutInterests;
}

void
Forwarder::onInterestReject(const shared_ptr<pit::Entry>& pitEntry)
{
  if (fw::hasPendingOutRecords(*pitEntry)) {
    NFD_LOG_ERROR("onInterestReject interest=" << pitEntry->getName() <<
                  " cannot reject forwarded Interest");
    return;
  }
  NFD_LOG_DEBUG("onInterestReject interest=" << pitEntry->getName());

  // cancel unsatisfy & straggler timer
  this->cancelUnsatisfyAndStragglerTimer(*pitEntry);

  // set PIT straggler timer
  this->setStragglerTimer(pitEntry, false);
}

void
Forwarder::onInterestUnsatisfied(const shared_ptr<pit::Entry>& pitEntry)
{
  NFD_LOG_DEBUG("onInterestUnsatisfied interest=" << pitEntry->getName());

  // invoke PIT unsatisfied callback
  beforeExpirePendingInterest(*pitEntry);
  this->dispatchToStrategy(*pitEntry,
    [&] (fw::Strategy& strategy) { strategy.beforeExpirePendingInterest(pitEntry); });

  // goto Interest Finalize pipeline
  this->onInterestFinalize(pitEntry, false);
}

void
Forwarder::onInterestFinalize(const shared_ptr<pit::Entry>& pitEntry, bool isSatisfied,
                              time::milliseconds dataFreshnessPeriod)
{
  NFD_LOG_DEBUG("onInterestFinalize interest=" << pitEntry->getName() <<
                (isSatisfied ? " satisfied" : " unsatisfied"));

  // Dead Nonce List insert if necessary
  this->insertDeadNonceList(*pitEntry, isSatisfied, dataFreshnessPeriod, 0);

  // PIT delete
  this->cancelUnsatisfyAndStragglerTimer(*pitEntry);
  m_pit.erase(pitEntry.get());
}

void
Forwarder::onIncomingData(Face& inFace, const Data& data)
{

  // receive Data
  NFD_LOG_DEBUG("onIncomingData face=" << inFace.getId() << " data=" << data.getName());
  data.setTag(make_shared<lp::IncomingFaceIdTag>(inFace.getId()));
  ++m_counters.nInData;

  // /localhost scope control
  bool isViolatingLocalhost = inFace.getScope() == ndn::nfd::FACE_SCOPE_NON_LOCAL &&
                              scope_prefix::LOCALHOST.isPrefixOf(data.getName());
  if (isViolatingLocalhost) {
    NFD_LOG_DEBUG("onIncomingData face=" << inFace.getId() <<
                  " data=" << data.getName() << " violates /localhost");
    // (drop)
    return;
  }

  int currentNode = getNode()->GetId();
  std::string currentNodeName;

  Name functionName = data.getFunction();

  auto string = functionName.toUri();
  std::cout << "Function Name:" << string << std::endl;
  auto separator = std::string("/");
  auto separator_length = separator.length();

  auto list = std::vector<std::string>();

  if (separator_length == 0) {
	  list.push_back(string);
  } else {
	  auto offset = std::string::size_type(0);
	  while (1) {
		  auto pos = string.find(separator, offset);
		  if (pos == std::string::npos) {
			  list.push_back(string.substr(offset));
			  break;
		  }
		  list.push_back(string.substr(offset, pos - offset));
		  offset = pos + separator_length;
	  }
  }

  // for us
  switch(currentNode) {
    case 0:
      currentNodeName = "Consumer1";
      break;
    case 1:
      currentNodeName = "Node1";
      break;
    case 2:
      currentNodeName = "Node2";
      break;
    case 3:
      currentNodeName = "Node3";
      break;
    case 4:
      currentNodeName = "Node4";
      break;
    case 5:
      currentNodeName = "Node5";
      break;
    case 6:
      currentNodeName = "Node6";
      break;
    case 7:
      currentNodeName = "Node7";
      break;
    case 8:
      currentNodeName = "Node8";
      break;
    case 9:
      currentNodeName = "Node9";
      break;
    case 10:
      currentNodeName = "Node10";
      break;
    case 11:
      currentNodeName = "Node11";
      break;
    case 12:
      currentNodeName = "Node12";
      break;
    case 13:
      currentNodeName = "Node13";
      break;
    case 14:
      currentNodeName = "Node14";
      break;
    case 15:
      currentNodeName = "Node15";
      break;
    case 16:
      currentNodeName = "Node16";
      break;
    case 17:
      currentNodeName = "Node17";
      break;
    case 18:
      currentNodeName = "Node18";
      break;
    case 19:
      currentNodeName = "Node19";
      break;
    case 20:
      currentNodeName = "Node20";
      break;
    case 21:
      currentNodeName = "Node21";
      break;
    case 22:
      currentNodeName = "Node22";
      break;
    case 23:
      currentNodeName = "Node23";
      break;
    case 24:
      currentNodeName = "Node24";
      break;
    case 25:
      currentNodeName = "Producer1";
      break;
    case 26:
      currentNodeName = "F1a";
      break;
    case 27:
      currentNodeName = "F1b";
      break;
    case 28:
      currentNodeName = "F1c";
      break;
    case 29:
      currentNodeName = "F2a";
      break;
    case 30:
      currentNodeName = "F2b";
      break;
    case 31:
      currentNodeName = "F2c";
      break;
    case 32:
      currentNodeName = "F3a";
      break;
    case 33:
      currentNodeName = "F3b";
      break;
    case 34:
      currentNodeName = "F3c";
      break;
    case 35:
      currentNodeName = "F4a";
      break;
    case 36:
      currentNodeName = "F4b";
      break;
    case 37:
      currentNodeName = "F4c";
      break;
    case 38:
      currentNodeName = "F5a";
      break;
    case 39:
      currentNodeName = "F5b";
      break;
    case 40:
      currentNodeName = "F5c";
      break;
    case 41:
      currentNodeName = "Consumer2";
      break;
    case 42:
      currentNodeName = "Consumer3";
      break;
    case 43:
      currentNodeName = "Consumer4";
      break;
    case 44:
      currentNodeName = "Producer2";
      break;
    case 45:
      currentNodeName = "Producer3";
      break;
    case 46:
      currentNodeName = "Producer4";
      break;
  }
  

  /*
  switch(currentNode) {
    case 0:
      currentNodeName = "Consumer";
      break;
    case 1:
      currentNodeName = "Node0";
      break;
    case 2:
      currentNodeName = "Node1";
      break;
    case 3:
      currentNodeName = "Node2";
      break;
    case 4:
      currentNodeName = "Node3";
      break;
    case 5:
      currentNodeName = "Node4";
      break;
    case 6:
      currentNodeName = "Node5";
      break;
    case 7:
      currentNodeName = "Node6";
      break;
    case 8:
      currentNodeName = "Node7";
      break;
    case 9:
      currentNodeName = "Node8";
      break;
    case 10:
      currentNodeName = "Producer";
      break;
    case 11:
      currentNodeName = "F1a";
      break;
    case 12:
      currentNodeName = "F2a";
      break;
    case 13:
      currentNodeName = "F3a";
      break;
    case 14:
      currentNodeName = "F2b";
      break;
    case 15:
      currentNodeName = "F3b";
      break;
    case 16:
      currentNodeName = "F1b";
      break;
    case 17:
      currentNodeName = "F3c";
      break;
    case 18:
      currentNodeName = "F2c";
      break;
    case 19:
      currentNodeName = "F1c";
      break;
    case 20:
      currentNodeName = "Consumer2";
      break;
    case 21:
      currentNodeName = "Consumer3";
      break;
  }
  */

  std::cout << "Node          : " << currentNodeName << std::endl;
  std::cout << "Content  Name : " << data.getName() << std::endl;

  std::cout << "Time          : " << time::toUnixTimestamp(time::system_clock::now()).count() << std::endl;

  if(list[3] == currentNodeName){
	  if(currentNodeName.compare("F1a") == 0){
		  table[1][1][0]++;
		  data.resetHop();
		  data.setCount(table[1][1][0]);
	  }else if(currentNodeName.compare("F1b") == 0){
		  table[1][1][1]++;
		  data.resetHop();
		  data.setCount(table[1][1][1]);
	  }else if(currentNodeName.compare("F1c") == 0){
		  table[1][1][2]++;
		  data.resetHop();
		  data.setCount(table[1][1][2]);
	  }else if(currentNodeName.compare("F2a") == 0){
		  table[1][2][0]++;
		  data.resetHop();
		  data.setCount(table[1][2][0]);
	  }else if(currentNodeName.compare("F2b") == 0){
		  table[1][2][1]++;
		  data.resetHop();
		  data.setCount(table[1][2][1]);
	  }else if(currentNodeName.compare("F2c") == 0){
		  table[1][2][2]++;
		  data.resetHop();
		  data.setCount(table[1][2][2]);
	  }else if(currentNodeName.compare("F3a") == 0){
		  table[1][3][0]++;
		  data.resetHop();
		  data.setCount(table[1][3][0]);
	  }else if(currentNodeName.compare("F3b") == 0){
		  table[1][3][1]++;
		  data.resetHop();
		  data.setCount(table[1][3][1]);
	  }else if(currentNodeName.compare("F3c") == 0){
		  table[1][3][2]++;
		  data.resetHop();
		  data.setCount(table[1][3][2]);
	  }else if(currentNodeName.compare("F4a") == 0){
		  table[1][4][0]++;
		  data.resetHop();
		  data.setCount(table[1][4][0]);
	  }else if(currentNodeName.compare("F4b") == 0){
		  table[1][4][1]++;
		  data.resetHop();
		  data.setCount(table[1][4][1]);
	  }else if(currentNodeName.compare("F4c") == 0){
		  table[1][4][2]++;
		  data.resetHop();
		  data.setCount(table[1][4][2]);
	  }else if(currentNodeName.compare("F5a") == 0){
		  table[1][5][0]++;
		  data.resetHop();
		  data.setCount(table[1][5][0]);
	  }else if(currentNodeName.compare("F5b") == 0){
		  table[1][5][1]++;
		  data.resetHop();
		  data.setCount(table[1][5][1]);
	  }else if(currentNodeName.compare("F5c") == 0){
		  table[1][5][2]++;
		  data.resetHop();
		  data.setCount(table[1][5][2]);
	  }
  }else if(list[2] == currentNodeName){
	  int number;
	  int character;
	  if(currentNodeName.compare("F1a") == 0){
		  table[1][1][0]++;
		  number = 1;
		  character = 0;
	  }else if(currentNodeName.compare("F1b") == 0){
		  table[1][1][1]++;
		  number = 1;
		  character = 1;
	  }else if(currentNodeName.compare("F1c") == 0){
		  table[1][1][2]++;
		  number = 1;
		  character = 2;
	  }else if(currentNodeName.compare("F2a") == 0){
		  table[1][2][0]++;
		  number = 2;
		  character = 0;
	  }else if(currentNodeName.compare("F2b") == 0){
		  table[1][2][1]++;
		  number = 2;
		  character = 1;
	  }else if(currentNodeName.compare("F2c") == 0){
		  table[1][2][2]++;
		  number = 2;
		  character = 2;
	  }else if(currentNodeName.compare("F3a") == 0){
		  table[1][3][0]++;
		  number = 3;
		  character = 0;
	  }else if(currentNodeName.compare("F3b") == 0){
		  table[1][3][1]++;
		  number = 3;
		  character = 1;
	  }else if(currentNodeName.compare("F3c") == 0){
		  table[1][3][2]++;
		  number = 3;
		  character = 2;
	  }else if(currentNodeName.compare("F4a") == 0){
		  table[1][4][0]++;
		  number = 4;
		  character = 0;
	  }else if(currentNodeName.compare("F4b") == 0){
		  table[1][4][1]++;
		  number = 4;
		  character = 1;
	  }else if(currentNodeName.compare("F4c") == 0){
		  table[1][4][2]++;
		  number = 4;
		  character = 2;
	  }else if(currentNodeName.compare("F5a") == 0){
		  table[1][5][0]++;
		  number = 5;
		  character = 0;
	  }else if(currentNodeName.compare("F5b") == 0){
		  table[1][5][1]++;
		  number = 5;
		  character = 1;
	  }else if(currentNodeName.compare("F5c") == 0){
		  table[1][5][2]++;
		  number = 5;
		  character = 2;
	  }
	  if(list[3].compare("F4a") == 0){
		  table[0][4][0] = data.getHop();
		  table[1][4][0] = data.getCount();
	  }else if(list[3].compare("F4b") == 0){
		  table[0][4][1] = data.getHop();
		  table[1][4][1] = data.getCount();
	  }else if(list[3].compare("F4c") == 0){
		  table[0][4][2] = data.getHop();
		  table[1][4][2] = data.getCount();
	  }else if(list[3].compare("F5a") == 0){
		  table[0][5][0] = data.getHop();
		  table[1][5][0] = data.getCount();
	  }else if(list[3].compare("F5b") == 0){
		  table[0][5][1] = data.getHop();
		  table[1][5][1] = data.getCount();
	  }else if(list[3].compare("F5c") == 0){
		  table[0][5][2] = data.getHop();
		  table[1][5][2] = data.getCount();
	  }else{
		  std::cout << "Error:forwarder::onIncomingInterest()" << std::endl;
	  }
	  data.resetHop();
	  data.setCount(table[1][number][character]);
  }else if(list[1] == currentNodeName){
	  int number;
	  int character;
	  if(currentNodeName.compare("F1a") == 0){
		  table[1][1][0]++;
		  number = 1;
		  character = 0;
	  }else if(currentNodeName.compare("F1b") == 0){
		  table[1][1][1]++;
		  number = 1;
		  character = 1;
	  }else if(currentNodeName.compare("F1c") == 0){
		  table[1][1][2]++;
		  number = 1;
		  character = 2;
	  }else if(currentNodeName.compare("F2a") == 0){
		  table[1][2][0]++;
		  number = 2;
		  character = 0;
	  }else if(currentNodeName.compare("F2b") == 0){
		  table[1][2][1]++;
		  number = 2;
		  character = 1;
	  }else if(currentNodeName.compare("F2c") == 0){
		  table[1][2][2]++;
		  number = 2;
		  character = 2;
	  }else if(currentNodeName.compare("F3a") == 0){
		  table[1][3][0]++;
		  number = 3;
		  character = 0;
	  }else if(currentNodeName.compare("F3b") == 0){
		  table[1][3][1]++;
		  number = 3;
		  character = 1;
	  }else if(currentNodeName.compare("F3c") == 0){
		  table[1][3][2]++;
		  number = 3;
		  character = 2;
	  }else if(currentNodeName.compare("F4a") == 0){
		  table[1][4][0]++;
		  number = 4;
		  character = 0;
	  }else if(currentNodeName.compare("F4b") == 0){
		  table[1][4][1]++;
		  number = 4;
		  character = 1;
	  }else if(currentNodeName.compare("F4c") == 0){
		  table[1][4][2]++;
		  number = 4;
		  character = 2;
	  }else if(currentNodeName.compare("F5a") == 0){
		  table[1][5][0]++;
		  number = 5;
		  character = 0;
	  }else if(currentNodeName.compare("F5b") == 0){
		  table[1][5][1]++;
		  number = 5;
		  character = 1;
	  }else if(currentNodeName.compare("F5c") == 0){
		  table[1][5][2]++;
		  number = 5;
		  character = 2;
	  }
	  if(list[2].compare("F1a") == 0){
		  table[0][1][0] = data.getHop();
		  table[1][1][0] = data.getCount();
	  }else if(list[2].compare("F1b") == 0){
		  table[0][1][1] = data.getHop();
		  table[1][1][1] = data.getCount();
	  }else if(list[2].compare("F1c") == 0){
		  table[0][1][2] = data.getHop();
		  table[1][1][2] = data.getCount();
	  }else if(list[2].compare("F2a") == 0){
		  table[0][2][0] = data.getHop();
		  table[1][2][0] = data.getCount();
	  }else if(list[2].compare("F2b") == 0){
		  table[0][2][1] = data.getHop();
		  table[1][2][1] = data.getCount();
	  }else if(list[2].compare("F2c") == 0){
		  table[0][2][2] = data.getHop();
		  table[1][2][2] = data.getCount();
	  }else if(list[2].compare("F3a") == 0){
		  table[0][3][0] = data.getHop();
		  table[1][3][0] = data.getCount();
	  }else if(list[2].compare("F3b") == 0){
		  table[0][3][1] = data.getHop();
		  table[1][3][1] = data.getCount();
	  }else if(list[2].compare("F3c") == 0){
		  table[0][3][2] = data.getHop();
		  table[1][3][2] = data.getCount();
	  }else{
		  std::cout << "Error:forwarder::onIncomingInterest()" << std::endl;
	  }
	  data.resetHop();
	  data.setCount(table[1][number][character]);
  }else if(m_csFromNdnSim != nullptr){
	  data.increaseHop();
  }

  //std::cout << currentNodeName << ": hop " << data.getHop() << ": count " << data.getCount() <<std::endl;
  // PIT match
  pit::DataMatchResult pitMatches = m_pit.findAllDataMatches(data);
  if (pitMatches.begin() == pitMatches.end()) {
    // goto Data unsolicited pipeline
    this->onDataUnsolicited(inFace, data);
    return;
  }

  //std::cout << "PIT Matches: " << pitMatches.size() << std::endl;

  shared_ptr<Data> dataCopyWithoutTag = make_shared<Data>(data);
  dataCopyWithoutTag->removeTag<lp::HopCountTag>();

  // CS insert
  if (m_csFromNdnSim == nullptr)
    m_cs.insert(*dataCopyWithoutTag);
  else
    m_csFromNdnSim->Add(dataCopyWithoutTag);

  std::set<Face*> pendingDownstreams;
  bool pitSatisfyFlag = true;
  // foreach PitEntry
  auto now = time::steady_clock::now();
  for (const shared_ptr<pit::Entry>& pitEntry : pitMatches) {
    NFD_LOG_DEBUG("onIncomingData matching=" << pitEntry->getName());
    //std::cout << "Pit Entry: " << pitEntry->getName() << std::endl;

    // cancel unsatisfy & straggler timer
    this->cancelUnsatisfyAndStragglerTimer(*pitEntry);

    uint32_t max = 1;
    Face* savedFace;
    for(const pit::InRecord& inRecord : pitEntry->getInRecords()) {
      std::cout << inRecord.getSequenceNumber() << std::endl;
      if(inRecord.getSequenceNumber() > max){
        max = inRecord.getSequenceNumber();
        savedFace = &inRecord.getFace();
      }
    }
      
    if(max > 1){
      pendingDownstreams.insert(savedFace);
      pitEntry->deleteInRecord(*savedFace);
      pitSatisfyFlag = false;
    } else {
      // remember pending downstreams
      for (const pit::InRecord& inRecord : pitEntry->getInRecords()) {
        if (inRecord.getExpiry() > now) {
          pendingDownstreams.insert(&inRecord.getFace());
        }
      }
    }

    // invoke PIT satisfy callback
    beforeSatisfyInterest(*pitEntry, inFace, data);
    this->dispatchToStrategy(*pitEntry,
      [&] (fw::Strategy& strategy) { strategy.beforeSatisfyInterest(pitEntry, inFace, data); });

    // Dead Nonce List insert if necessary (for out-record of inFace)
    this->insertDeadNonceList(*pitEntry, true, data.getFreshnessPeriod(), &inFace);

    /*
    // mark PIT satisfied
    pitEntry->clearInRecords();
    pitEntry->deleteOutRecord(inFace);
    */

    if(pitSatisfyFlag){
      pitEntry->clearInRecords();
      pitEntry->deleteOutRecord(inFace);
    }

    // set PIT straggler timer
    this->setStragglerTimer(pitEntry, true, data.getFreshnessPeriod());
  }

  // foreach pending downstream
  for (Face* pendingDownstream : pendingDownstreams) {
    /*
    if (pendingDownstream == &inFace) {
      std::cout << "bitch" << std::endl;
      continue;
    }
    */
    // goto outgoing Data pipeline
    this->onOutgoingData(data, *pendingDownstream);
    //std::cout << "bitch" << std::endl;
  }

  std::cout << "--------------------------------------------" << std::endl;

}

void
Forwarder::onDataUnsolicited(Face& inFace, const Data& data)
{
  // accept to cache?
  fw::UnsolicitedDataDecision decision = m_unsolicitedDataPolicy->decide(inFace, data);
  if (decision == fw::UnsolicitedDataDecision::CACHE) {
    // CS insert
    if (m_csFromNdnSim == nullptr)
      m_cs.insert(data, true);
    else
      m_csFromNdnSim->Add(data.shared_from_this());
  }

  NFD_LOG_DEBUG("onDataUnsolicited face=" << inFace.getId() <<
                " data=" << data.getName() <<
                " decision=" << decision);
}

void
Forwarder::onOutgoingData(const Data& data, Face& outFace)
{
  if (outFace.getId() == face::INVALID_FACEID) {
    NFD_LOG_WARN("onOutgoingData face=invalid data=" << data.getName());
    return;
  }
  NFD_LOG_DEBUG("onOutgoingData face=" << outFace.getId() << " data=" << data.getName());

  // /localhost scope control
  bool isViolatingLocalhost = outFace.getScope() == ndn::nfd::FACE_SCOPE_NON_LOCAL &&
                              scope_prefix::LOCALHOST.isPrefixOf(data.getName());
  if (isViolatingLocalhost) {
    NFD_LOG_DEBUG("onOutgoingData face=" << outFace.getId() <<
                  " data=" << data.getName() << " violates /localhost");
    // (drop)
    return;
  }

  // TODO traffic manager

  // send Data
  outFace.sendData(data);
  ++m_counters.nOutData;
}

void
Forwarder::onIncomingNack(Face& inFace, const lp::Nack& nack)
{
  // receive Nack
  nack.setTag(make_shared<lp::IncomingFaceIdTag>(inFace.getId()));
  ++m_counters.nInNacks;

  // if multi-access face, drop
  if (inFace.getLinkType() == ndn::nfd::LINK_TYPE_MULTI_ACCESS) {
    NFD_LOG_DEBUG("onIncomingNack face=" << inFace.getId() <<
                  " nack=" << nack.getInterest().getName() <<
                  "~" << nack.getReason() << " face-is-multi-access");
    return;
  }

  // PIT match
  shared_ptr<pit::Entry> pitEntry = m_pit.find(nack.getInterest());
  // if no PIT entry found, drop
  if (pitEntry == nullptr) {
    NFD_LOG_DEBUG("onIncomingNack face=" << inFace.getId() <<
                  " nack=" << nack.getInterest().getName() <<
                  "~" << nack.getReason() << " no-PIT-entry");
    return;
  }

  // has out-record?
  pit::OutRecordCollection::iterator outRecord = pitEntry->getOutRecord(inFace);
  // if no out-record found, drop
  if (outRecord == pitEntry->out_end()) {
    NFD_LOG_DEBUG("onIncomingNack face=" << inFace.getId() <<
                  " nack=" << nack.getInterest().getName() <<
                  "~" << nack.getReason() << " no-out-record");
    return;
  }

  // if out-record has different Nonce, drop
  if (nack.getInterest().getNonce() != outRecord->getLastNonce()) {
    NFD_LOG_DEBUG("onIncomingNack face=" << inFace.getId() <<
                  " nack=" << nack.getInterest().getName() <<
                  "~" << nack.getReason() << " wrong-Nonce " <<
                  nack.getInterest().getNonce() << "!=" << outRecord->getLastNonce());
    return;
  }

  NFD_LOG_DEBUG("onIncomingNack face=" << inFace.getId() <<
                " nack=" << nack.getInterest().getName() <<
                "~" << nack.getReason() << " OK");

  // record Nack on out-record
  outRecord->setIncomingNack(nack);

  // trigger strategy: after receive NACK
  this->dispatchToStrategy(*pitEntry,
    [&] (fw::Strategy& strategy) { strategy.afterReceiveNack(inFace, nack, pitEntry); });
}

void
Forwarder::onOutgoingNack(const shared_ptr<pit::Entry>& pitEntry, const Face& outFace,
                          const lp::NackHeader& nack)
{
  if (outFace.getId() == face::INVALID_FACEID) {
    NFD_LOG_WARN("onOutgoingNack face=invalid" <<
                  " nack=" << pitEntry->getInterest().getName() <<
                  "~" << nack.getReason() << " no-in-record");
    return;
  }

  // has in-record?
  pit::InRecordCollection::iterator inRecord = pitEntry->getInRecord(outFace);

  // if no in-record found, drop
  if (inRecord == pitEntry->in_end()) {
    NFD_LOG_DEBUG("onOutgoingNack face=" << outFace.getId() <<
                  " nack=" << pitEntry->getInterest().getName() <<
                  "~" << nack.getReason() << " no-in-record");
    return;
  }

  // if multi-access face, drop
  if (outFace.getLinkType() == ndn::nfd::LINK_TYPE_MULTI_ACCESS) {
    NFD_LOG_DEBUG("onOutgoingNack face=" << outFace.getId() <<
                  " nack=" << pitEntry->getInterest().getName() <<
                  "~" << nack.getReason() << " face-is-multi-access");
    return;
  }

  NFD_LOG_DEBUG("onOutgoingNack face=" << outFace.getId() <<
                " nack=" << pitEntry->getInterest().getName() <<
                "~" << nack.getReason() << " OK");

  // create Nack packet with the Interest from in-record
  lp::Nack nackPkt(inRecord->getInterest());
  nackPkt.setHeader(nack);

  // erase in-record
  pitEntry->deleteInRecord(outFace);

  // send Nack on face
  const_cast<Face&>(outFace).sendNack(nackPkt);
  ++m_counters.nOutNacks;
}

static inline bool
compare_InRecord_expiry(const pit::InRecord& a, const pit::InRecord& b)
{
  return a.getExpiry() < b.getExpiry();
}

void
Forwarder::setUnsatisfyTimer(const shared_ptr<pit::Entry>& pitEntry)
{
  pit::InRecordCollection::iterator lastExpiring =
    std::max_element(pitEntry->in_begin(), pitEntry->in_end(), &compare_InRecord_expiry);

  time::steady_clock::TimePoint lastExpiry = lastExpiring->getExpiry();
  time::nanoseconds lastExpiryFromNow = lastExpiry - time::steady_clock::now();
  if (lastExpiryFromNow <= time::seconds::zero()) {
    // TODO all in-records are already expired; will this happen?
  }

  scheduler::cancel(pitEntry->m_unsatisfyTimer);
  pitEntry->m_unsatisfyTimer = scheduler::schedule(lastExpiryFromNow,
    bind(&Forwarder::onInterestUnsatisfied, this, pitEntry));
}

void
Forwarder::setStragglerTimer(const shared_ptr<pit::Entry>& pitEntry, bool isSatisfied,
                             time::milliseconds dataFreshnessPeriod)
{
  time::nanoseconds stragglerTime = time::milliseconds(1000);

  scheduler::cancel(pitEntry->m_stragglerTimer);
  pitEntry->m_stragglerTimer = scheduler::schedule(stragglerTime,
    bind(&Forwarder::onInterestFinalize, this, pitEntry, isSatisfied, dataFreshnessPeriod));
}

void
Forwarder::cancelUnsatisfyAndStragglerTimer(pit::Entry& pitEntry)
{
  scheduler::cancel(pitEntry.m_unsatisfyTimer);
  scheduler::cancel(pitEntry.m_stragglerTimer);
}

static inline void
insertNonceToDnl(DeadNonceList& dnl, const pit::Entry& pitEntry,
                 const pit::OutRecord& outRecord)
{
  dnl.add(pitEntry.getName(), outRecord.getLastNonce());
}

void
Forwarder::insertDeadNonceList(pit::Entry& pitEntry, bool isSatisfied,
                               time::milliseconds dataFreshnessPeriod, Face* upstream)
{
  // need Dead Nonce List insert?
  bool needDnl = false;
  if (isSatisfied) {
    bool hasFreshnessPeriod = dataFreshnessPeriod >= time::milliseconds::zero();
    // Data never becomes stale if it doesn't have FreshnessPeriod field
    needDnl = static_cast<bool>(pitEntry.getInterest().getMustBeFresh()) &&
              (hasFreshnessPeriod && dataFreshnessPeriod < m_deadNonceList.getLifetime());
  }
  else {
    needDnl = true;
  }

  if (!needDnl) {
    return;
  }

  // Dead Nonce List insert
  if (upstream == 0) {
    // insert all outgoing Nonces
    const pit::OutRecordCollection& outRecords = pitEntry.getOutRecords();
    std::for_each(outRecords.begin(), outRecords.end(),
                  bind(&insertNonceToDnl, ref(m_deadNonceList), cref(pitEntry), _1));
  }
  else {
    // insert outgoing Nonce of a specific face
    pit::OutRecordCollection::iterator outRecord = pitEntry.getOutRecord(*upstream);
    if (outRecord != pitEntry.getOutRecords().end()) {
      m_deadNonceList.add(pitEntry.getName(), outRecord->getLastNonce());
    }
  }
}

} // namespace nfd
