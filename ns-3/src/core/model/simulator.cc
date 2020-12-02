/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2005,2006 INRIA
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */
#include "ns3/core-config.h"
#include "simulator.h"
#include "simulator-impl.h"
#include "scheduler.h"
#include "map-scheduler.h"
#include "event-impl.h"

#include "ptr.h"
#include "string.h"
#include "object-factory.h"
#include "global-value.h"
#include "assert.h"
#include "log.h"

#include <cmath>
#include <fstream>
#include <list>
#include <vector>
#include <iostream>

/**
 * \file
 * \ingroup simulator
 * ns3::Simulator implementation, as well as implementation pointer,
 * global scheduler implementation, and default ns3::NodePrinter
 * and ns3::TimePrinter.
 */

namespace ns3 {

// Note:  Logging in this file is largely avoided due to the
// number of calls that are made to these functions and the possibility
// of causing recursions leading to stack overflow
NS_LOG_COMPONENT_DEFINE ("Simulator");

/**
 * \ingroup simulator
 * The specific simulator implementation to use.
 *
 * Must be derived from SimulatorImpl.
 */
static GlobalValue g_simTypeImpl = GlobalValue
  ("SimulatorImplementationType",
   "The object class to use as the simulator implementation",
   StringValue ("ns3::DefaultSimulatorImpl"),
   MakeStringChecker ());


/**
 * \ingroup scheduler
 * The specific event scheduler implementation to use.
 *
 * Must be derived from Scheduler.
 */
static GlobalValue g_schedTypeImpl = GlobalValue ("SchedulerType",
                                                  "The object class to use as the scheduler implementation",
                                                  TypeIdValue (MapScheduler::GetTypeId ()),
                                                  MakeTypeIdChecker ());

/**
 * \ingroup logging
 * Default TimePrinter implementation.
 *
 * \param [in,out] os The output stream to print the time on.
 */
static void
TimePrinter (std::ostream &os)
{
  os << Simulator::Now ().GetSeconds () << "s";
}

/**
 * \ingroup logging
 * Default node id printer implementation.
 * 
 * \param [in,out] os The output stream to print the node id on.
 */
static void
NodePrinter (std::ostream &os)
{
  if (Simulator::GetContext () == 0xffffffff)
    {
      os << "-1";
    }
  else
    {
      os << Simulator::GetContext ();
    }
}

/**
 * \ingroup simulator
 * \brief Get the static SimulatorImpl instance.
 * \return The SimulatorImpl instance pointer.
 */
static SimulatorImpl **PeekImpl (void)
{
  static SimulatorImpl *impl = 0;
  return &impl;
}

/**
 * \ingroup simulator
 * \brief Get the SimulatorImpl singleton.
 * \return The singleton pointer.
 * \see Simulator::GetImplementation()
 */
static SimulatorImpl * GetImpl (void)
{
  SimulatorImpl **pimpl = PeekImpl ();
  /* Please, don't include any calls to logging macros in this function
   * or pay the price, that is, stack explosions.
   */
  if (*pimpl == 0)
    {
      {
        ObjectFactory factory;
        StringValue s;

        g_simTypeImpl.GetValue (s);
        factory.SetTypeId (s.Get ());
        *pimpl = GetPointer (factory.Create<SimulatorImpl> ());
      }
      {
        ObjectFactory factory;
        StringValue s;
        g_schedTypeImpl.GetValue (s);
        factory.SetTypeId (s.Get ());
        (*pimpl)->SetScheduler (factory);
      }

//
// Note: we call LogSetTimePrinter _after_ creating the implementation
// object because the act of creation can trigger calls to the logging 
// framework which would call the TimePrinter function which would call 
// Simulator::Now which would call Simulator::GetImpl, and, thus, get us 
// in an infinite recursion until the stack explodes.
//
      LogSetTimePrinter (&TimePrinter);
      LogSetNodePrinter (&NodePrinter);
    }
  return *pimpl;
}

void
Simulator::Destroy (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  SimulatorImpl **pimpl = PeekImpl (); 
  if (*pimpl == 0)
    {
      return;
    }
  /* Note: we have to call LogSetTimePrinter (0) below because if we do not do
   * this, and restart a simulation after this call to Destroy, (which is 
   * legal), Simulator::GetImpl will trigger again an infinite recursion until
   * the stack explodes.
   */
  LogSetTimePrinter (0);
  LogSetNodePrinter (0);
  (*pimpl)->Destroy ();
  (*pimpl)->Unref ();
  *pimpl = 0;
}

void
Simulator::SetScheduler (ObjectFactory schedulerFactory)
{
  NS_LOG_FUNCTION (schedulerFactory);
  GetImpl ()->SetScheduler (schedulerFactory);
}

bool 
Simulator::IsFinished (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  return GetImpl ()->IsFinished ();
}

void 
Simulator::Run (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  Time::ClearMarkedTimes ();
  GetImpl ()->Run ();
}

void 
Simulator::Stop (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  NS_LOG_LOGIC ("stop");
  GetImpl ()->Stop ();
}

void 
Simulator::Stop (Time const &delay)
{
  NS_LOG_FUNCTION (delay);
  GetImpl ()->Stop (delay);
}

Time
Simulator::Now (void)
{
  /* Please, don't include any calls to logging macros in this function
   * or pay the price, that is, stack explosions.
   */
  return GetImpl ()->Now ();
}

Time
Simulator::GetDelayLeft (const EventId &id)
{
  NS_LOG_FUNCTION (&id);
  return GetImpl ()->GetDelayLeft (id);
}

EventId
Simulator::Schedule (Time const &delay, const Ptr<EventImpl> &event)
{
  return DoSchedule (delay, GetPointer (event));
}

EventId
Simulator::ScheduleNow (const Ptr<EventImpl> &ev)
{
  return DoScheduleNow (GetPointer (ev));
}
void
Simulator::ScheduleWithContext (uint32_t context, const Time &delay, EventImpl *impl)
{
  return GetImpl ()->ScheduleWithContext (context, delay, impl);
}
EventId
Simulator::ScheduleDestroy (const Ptr<EventImpl> &ev)
{
  return DoScheduleDestroy (GetPointer (ev));
}
EventId 
Simulator::DoSchedule (Time const &time, EventImpl *impl)
{
  return GetImpl ()->Schedule (time, impl);
}
EventId 
Simulator::DoScheduleNow (EventImpl *impl)
{
  return GetImpl ()->ScheduleNow (impl);
}
EventId 
Simulator::DoScheduleDestroy (EventImpl *impl)
{
  return GetImpl ()->ScheduleDestroy (impl);
}


EventId
Simulator::Schedule (Time const &delay, void (*f)(void))
{
  return DoSchedule (delay, MakeEvent (f));
}

void
Simulator::ScheduleWithContext (uint32_t context, Time const &delay, void (*f)(void))
{
  return ScheduleWithContext (context, delay, MakeEvent (f));
}

EventId
Simulator::ScheduleNow (void (*f)(void))
{
  return DoScheduleNow (MakeEvent (f));
}

EventId
Simulator::ScheduleDestroy (void (*f)(void))
{
  return DoScheduleDestroy (MakeEvent (f));
}

void
Simulator::Remove (const EventId &id)
{
  if (*PeekImpl () == 0)
    {
      return;
    }
  return GetImpl ()->Remove (id);
}

void
Simulator::Cancel (const EventId &id)
{
  if (*PeekImpl () == 0)
    {
      return;
    }
  return GetImpl ()->Cancel (id);
}

bool 
Simulator::IsExpired (const EventId &id)
{
  if (*PeekImpl () == 0)
    {
      return true;
    }
  return GetImpl ()->IsExpired (id);
}

Time Now (void)
{
  return Time (Simulator::Now ());
}

Time 
Simulator::GetMaximumSimulationTime (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  return GetImpl ()->GetMaximumSimulationTime ();
}

uint32_t
Simulator::GetContext (void)
{
  return GetImpl ()->GetContext ();
}

uint32_t
Simulator::GetSystemId (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  if (*PeekImpl () != 0)
    {
      return GetImpl ()->GetSystemId ();
    }
  else
    {
      return 0;
    }
}

void
Simulator::SetImplementation (Ptr<SimulatorImpl> impl)
{
  NS_LOG_FUNCTION (impl);
  if (*PeekImpl () != 0)
    {
      NS_FATAL_ERROR ("It is not possible to set the implementation after calling any Simulator:: function. Call Simulator::SetImplementation earlier or after Simulator::Destroy.");
    }
  *PeekImpl () = GetPointer (impl);
  // Set the default scheduler
  ObjectFactory factory;
  StringValue s;
  g_schedTypeImpl.GetValue (s);
  factory.SetTypeId (s.Get ());
  impl->SetScheduler (factory);
//
// Note: we call LogSetTimePrinter _after_ creating the implementation
// object because the act of creation can trigger calls to the logging 
// framework which would call the TimePrinter function which would call 
// Simulator::Now which would call Simulator::GetImpl, and, thus, get us 
// in an infinite recursion until the stack explodes.
//
  LogSetTimePrinter (&TimePrinter);
  LogSetNodePrinter (&NodePrinter);
}

Ptr<SimulatorImpl>
Simulator::GetImplementation (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  return GetImpl ();
}

int functionCallCount1 = 0;
int functionCallCount2 = 0;
int functionCallCount3 = 0;
int functionCallCount4 = 0;
int functionCallCount5 = 0;
int functionCallCount6 = 0;
int functionCallCount7 = 0;
int functionCallCount8 = 0;
int functionCallCount9 = 0;
int functionCallCount10 = 0;
int functionCallCount11 = 0;
int functionCallCount12 = 0;
int functionCallCount13 = 0;
int functionCallCount14 = 0;
int functionCallCount15 = 0;

int totalFcc1 = 0;
int totalFcc2 = 0;
int totalFcc3 = 0;
int totalFcc4 = 0;
int totalFcc5 = 0;
int totalFcc6 = 0;
int totalFcc7 = 0;
int totalFcc8 = 0;
int totalFcc9 = 0;
int totalFcc10 = 0;
int totalFcc11 = 0;
int totalFcc12 = 0;
int totalFcc13 = 0;
int totalFcc14 = 0;
int totalFcc15 = 0;

int totalFcc1temp = 0;
int totalFcc2temp = 0;
int totalFcc3temp = 0;
int totalFcc4temp = 0;
int totalFcc5temp = 0;
int totalFcc6temp = 0;
int totalFcc7temp = 0;
int totalFcc8temp = 0;
int totalFcc9temp = 0;
int totalFcc10temp = 0;
int totalFcc11temp = 0;
int totalFcc12temp = 0;
int totalFcc13temp = 0;
int totalFcc14temp = 0;
int totalFcc15temp = 0;

int currentFcc1 = 0;
int currentFcc2 = 0;
int currentFcc3 = 0;
int currentFcc4 = 0;
int currentFcc5 = 0;
int currentFcc6 = 0;
int currentFcc7 = 0;
int currentFcc8 = 0;
int currentFcc9 = 0;
int currentFcc10 = 0;
int currentFcc11 = 0;
int currentFcc12 = 0;
int currentFcc13 = 0;
int currentFcc14 = 0;
int currentFcc15 = 0;


int allFcc = 0;

int totalHops = 0;

int interestNum = 0;
int dataNum = 0;
int serviceNum = 0;

int totalServiceTime = 0;

static int choiceType = 0;

int totalSend = 0;

double averageServiceTime;


int getFunctionCallCount(int i){
  switch(i) {
    case 1:
      return functionCallCount1;
      break;
    case 2:
      return functionCallCount2;
      break;
    case 3:
      return functionCallCount3;
      break;
    case 4:
      return functionCallCount4;
      break;
    case 5:
      return functionCallCount5;
      break;
    case 6:
      return functionCallCount6;
      break;
    case 7:
      return functionCallCount7;
      break;
    case 8:
      return functionCallCount8;
      break;
    case 9:
      return functionCallCount9;
      break;
    case 10:
      return functionCallCount10;
      break;
    case 11:
      return functionCallCount11;
      break;
    case 12:
      return functionCallCount12;
      break;
    case 13:
      return functionCallCount13;
      break;
    case 14:
      return functionCallCount14;
      break;
    case 15:
      return functionCallCount15;
      break;
    default:
      break;
  }
}


void increaseFunctionCallCount(int i){
  switch(i) {
    case 1:
      currentFcc1++;
      break;
    case 2:
      currentFcc2++;
      break;
    case 3:
      currentFcc3++;
      break;
    case 4:
      currentFcc4++;
      break;
    case 5:
      currentFcc5++;
      break;
    case 6:
      currentFcc6++;
      break;
    case 7:
      currentFcc7++;
      break;
    case 8:
      currentFcc8++;
      break;
    case 9:
      currentFcc9++;
      break;
    case 10:
      currentFcc10++;
      break;
    case 11:
      currentFcc11++;
      break;
    case 12:
      currentFcc12++;
      break;
    case 13:
      currentFcc13++;
      break;
    case 14:
      currentFcc14++;
      break;
    case 15:
      currentFcc15++;
      break;
    default:
      break;
  }
}


void decreaseFunctionCallCount(int i){
  switch(i) {
    case 1:
      currentFcc1--;
      break;
    case 2:
      currentFcc2--;
      break;
    case 3:
      currentFcc3--;
      break;
    case 4:
      currentFcc4--;
      break;
    case 5:
      currentFcc5--;
      break;
    case 6:
      currentFcc6--;
      break;
    case 7:
      currentFcc7--;
      break;
    case 8:
      currentFcc8--;
      break;
    case 9:
      currentFcc9--;
      break;
    case 10:
      currentFcc10--;
      break;
    case 11:
      currentFcc11--;
      break;
    case 12:
      currentFcc12--;
      break;
    case 13:
      currentFcc13--;
      break;
    case 14:
      currentFcc14--;
      break;
    case 15:
      currentFcc15--;
      break;
    default:
      break;
  }
}

int getCurrentFcc(int i){
  switch(i) {
    case 1:
      return currentFcc1;
      break;
    case 2:
      return currentFcc2;
      break;
    case 3:
      return currentFcc3;
      break;
    case 4:
      return currentFcc4;
      break;
    case 5:
      return currentFcc5;
      break;
    case 6:
      return currentFcc6;
      break;
    case 7:
      return currentFcc7;
      break;
    case 8:
      return currentFcc8;
      break;
    case 9:
      return currentFcc9;
      break;
    case 10:
      return currentFcc10;
      break;
    case 11:
      return currentFcc11;
      break;
    case 12:
      return currentFcc12;
      break;
    case 13:
      return currentFcc13;
      break;
    case 14:
      return currentFcc14;
      break;
    case 15:
      return currentFcc15;
      break;
    default:
      break;
  }
}


int getTotalFcc(int i){
  switch(i) {
    case 1:
      return totalFcc1;
      break;
    case 2:
      return totalFcc2;
      break;
    case 3:
      return totalFcc3;
      break;
    case 4:
      return totalFcc4;
      break;
    case 5:
      return totalFcc5;
      break;
    case 6:
      return totalFcc6;
      break;
    case 7:
      return totalFcc7;
      break;
    case 8:
      return totalFcc8;
      break;
    case 9:
      return totalFcc9;
      break;
    case 10:
      return totalFcc10;
      break;
    case 11:
      return totalFcc11;
      break;
    case 12:
      return totalFcc12;
      break;
    case 13:
      return totalFcc13;
      break;
    case 14:
      return totalFcc14;
      break;
    case 15:
      return totalFcc15;
      break;
    default:
      break;
  }
}

int getTotalFccTemp(int i){
  switch(i) {
    case 1:
      return totalFcc1temp;
      break;
    case 2:
      return totalFcc2temp;
      break;
    case 3:
      return totalFcc3temp;
      break;
    case 4:
      return totalFcc4temp;
      break;
    case 5:
      return totalFcc5temp;
      break;
    case 6:
      return totalFcc6temp;
      break;
    case 7:
      return totalFcc7temp;
      break;
    case 8:
      return totalFcc8temp;
      break;
    case 9:
      return totalFcc9temp;
      break;
    case 10:
      return totalFcc10temp;
      break;
    case 11:
      return totalFcc11temp;
      break;
    case 12:
      return totalFcc12temp;
      break;
    case 13:
      return totalFcc13temp;
      break;
    case 14:
      return totalFcc14temp;
      break;
    case 15:
      return totalFcc15temp;
      break;
    default:
      break;
  }
}

void increaseTotalFcc(int i){
  switch(i) {
    case 1:
      totalFcc1++;
      totalFcc1temp++;
      break;
    case 2:
      totalFcc2++;
      totalFcc2temp++;
      break;
    case 3:
      totalFcc3++;
      totalFcc3temp++;
      break;
    case 4:
      totalFcc4++;
      totalFcc4temp++;
      break;
    case 5:
      totalFcc5++;
      totalFcc5temp++;
      break;
    case 6:
      totalFcc6++;
      totalFcc6temp++;
      break;
    case 7:
      totalFcc7++;
      totalFcc7temp++;
      break;
    case 8:
      totalFcc8++;
      totalFcc8temp++;
      break;
    case 9:
      totalFcc9++;
      totalFcc9temp++;
      break;
    case 10:
      totalFcc10++;
      totalFcc10temp++;
      break;
    case 11:
      totalFcc11++;
      totalFcc11temp++;
      break;
    case 12:
      totalFcc12++;
      totalFcc12temp++;
      break;
    case 13:
      totalFcc13++;
      totalFcc13temp++;
      break;
    case 14:
      totalFcc14++;
      totalFcc14temp++;
      break;
    case 15:
      totalFcc15++;
      totalFcc15temp++;
      break;
    default:
      break;
  }
}

int getInterestNum(){
  return interestNum;
}

int getDataNum(){
  return dataNum;
}

void increaseInterestNum(){
  interestNum++;
}

void increaseDataNum(){
  dataNum++;
}

int getServiceNum(){
  return serviceNum;
}

void increaseServiceNum(){
  serviceNum++;
}

void increaseAllFcc(){
  allFcc++;
}

int getAllFcc(){
  return allFcc;
}

void resetFcc(){
  allFcc = 0;
  functionCallCount1 = totalFcc1temp;
  totalFcc1temp = 0;
  functionCallCount2 = totalFcc2temp;
  totalFcc2temp = 0;
  functionCallCount3 = totalFcc3temp;
  totalFcc3temp = 0;
  functionCallCount4 = totalFcc4temp;
  totalFcc4temp = 0;
  functionCallCount5 = totalFcc5temp;
  totalFcc5temp = 0;
  functionCallCount6 = totalFcc6temp;
  totalFcc6temp = 0;
  functionCallCount7 = totalFcc7temp;
  totalFcc7temp = 0;
  functionCallCount8 = totalFcc8temp;
  totalFcc8temp = 0;
  functionCallCount9 = totalFcc9temp;
  totalFcc9temp = 0;
  functionCallCount10 = totalFcc10temp;
  totalFcc10temp = 0;
  functionCallCount11 = totalFcc11temp;
  totalFcc11temp = 0;
  functionCallCount12 = totalFcc12temp;
  totalFcc12temp = 0;
  functionCallCount13 = totalFcc13temp;
  totalFcc13temp = 0;
  functionCallCount14 = totalFcc14temp;
  totalFcc14temp = 0;
  functionCallCount15 = totalFcc15temp;
  totalFcc15temp = 0;
}

void increaseTotalHops(int i){
  totalHops += i;
}

void increaseTotalServiceTime(int i){
  totalServiceTime += i;
}

int getTotalServiceTime(){
  return totalServiceTime;
}

void setChoiceType(const char* type){
	std::string strType = type;
	if(strType == "siraiwaNDN"){
		choiceType = 0;
	}else if(strType == "roundRobin"){
		choiceType = 1;
	}else if(strType == "duration"){
		choiceType = 2;
	}else if(strType == "randChoice"){
		choiceType = 3;
	}else if(strType == "fibControl"){
		choiceType = 4;
	}
}

int getChoiceType(){
	return choiceType;
}

void increaseTotalSend(){
	totalSend++;
}

int getTotalSend(){
	return totalSend;
}

void setAverageServiceTime(double time){
	averageServiceTime = time;
}

double getAverageServiceTime(){
	return averageServiceTime;
}

void printResult(){
	std::cout << "AverageServiceTime: " << averageServiceTime << std::endl;
	std::cout << "ServiceNum: " << serviceNum << std::endl;
}

int getWeight(){
	//LordFirst->100 HopFirst->0
	return 2;
}

} // namespace ns3

