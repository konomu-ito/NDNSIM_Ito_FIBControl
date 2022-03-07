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
#include <vector>

#include "fib.hpp"
#include "pit-entry.hpp"
#include "measurements-entry.hpp"

#include <boost/concept/assert.hpp>
#include <boost/concept_check.hpp>
#include <type_traits>

namespace nfd {
namespace fib {

const unique_ptr<Entry> Fib::s_emptyEntry = make_unique<Entry>(Name());

// http://en.cppreference.com/w/cpp/concept/ForwardIterator
BOOST_CONCEPT_ASSERT((boost::ForwardIterator<Fib::const_iterator>));
// boost::ForwardIterator follows SGI standard http://www.sgi.com/tech/stl/ForwardIterator.html,
// which doesn't require DefaultConstructible
#ifdef HAVE_IS_DEFAULT_CONSTRUCTIBLE
static_assert(std::is_default_constructible<Fib::const_iterator>::value,
		"Fib::const_iterator must be default-constructible");
#else
BOOST_CONCEPT_ASSERT((boost::DefaultConstructible<Fib::const_iterator>));
#endif // HAVE_IS_DEFAULT_CONSTRUCTIBLE

static inline bool
nteHasFibEntry(const name_tree::Entry& nte)
{
	return nte.getFibEntry() != nullptr;
}

Fib::Fib(NameTree& nameTree)
: m_nameTree(nameTree)
, m_nItems(0)
, m_fcc(0)
{
}

template<typename K>
const Entry&
Fib::findLongestPrefixMatchImpl(const K& key) const
{
	name_tree::Entry* nte = m_nameTree.findLongestPrefixMatch(key, &nteHasFibEntry);
	if (nte != nullptr) {
		return *nte->getFibEntry();
	}
	return *s_emptyEntry;
}

const Entry&
Fib::findLongestPrefixMatch(const Name& prefix) const
{
	return this->findLongestPrefixMatchImpl(prefix);
}

const Entry&
Fib::findLongestPrefixMatch(const pit::Entry& pitEntry) const
{
	return this->findLongestPrefixMatchImpl(pitEntry);
}

//2020/11/18
fib::Entry*
Fib::selectFunction(const Name& prefix) const
{
	fib::Entry* fibEntrY = nullptr;
	int indicatorNum = 3;
	char indicator = 'a';
	std::string headFunction = prefix.toUri();
	auto separator = std::string("/");
	auto separator_length = separator.length();

	auto pos = headFunction.find(separator,separator_length);
	if(pos != std::string::npos)
		headFunction.erase(pos, headFunction.size() - pos);


	for(int i = 0; i < indicatorNum; i++){
		Name exFunctionName = Name(headFunction + indicator);
		//std::cout << "Candidate:" << exFunctionName.toUri() << std::endl;
		fib::Entry* entry = this->findLongestPrefixMatchFunction(exFunctionName);
		//std::cout<<"the cost is " << entry->getCost() << std::endl;
		if(fibEntrY == nullptr){
			fibEntrY = entry;
			//std::cout << "Selected:" << exFunctionName.toUri() << std::endl;
		}else if(entry->getCost() < fibEntrY->getCost()){
			fibEntrY = entry;
			//std::cout << "Selected2:" << exFunctionName.toUri() << std::endl;
		}
		indicator++;
	}
	//std::cout << "FCC:" << fibEntrY->getFcc() << std::endl;
	fibEntrY->setFcc(fibEntrY->getFcc() + 1);
	return fibEntrY;
}

//ADDED Longest Prefix Match for Function Chaining
fib::Entry*
Fib::findLongestPrefixMatchFunction(const Name& prefix) const
{
	nfd::name_tree::Entry* nameTreeEntry = m_nameTree.findLongestPrefixMatch(prefix);
	if (static_cast<bool>(nameTreeEntry)) {
		fib::Entry* fibEntry = nameTreeEntry->getFibEntry();
		if(fibEntry->getPrefix().toUri()!="/"){
			return fibEntry;
		}
	}
	return nullptr;
}

//added 2020/11/25
void
Fib::resetFcc(){
	m_fcc = 0;
}

void
Fib::increaseFcc(){
	m_fcc++;
}

int
Fib::getFcc(){
	return m_fcc;
}


const Entry&
Fib::findLongestPrefixMatch(const measurements::Entry& measurementsEntry) const
{
	return this->findLongestPrefixMatchImpl(measurementsEntry);
}

Entry*
Fib::findExactMatch(const Name& prefix)
{
	name_tree::Entry* nte = m_nameTree.findExactMatch(prefix);
	if (nte != nullptr)
		return nte->getFibEntry();

	return nullptr;
}

std::pair<Entry*, bool>
Fib::insert(const Name& prefix)
{
	name_tree::Entry& nte = m_nameTree.lookup(prefix);
	Entry* entry = nte.getFibEntry();
	if (entry != nullptr) {
		return std::make_pair(entry, false);
	}

	nte.setFibEntry(make_unique<Entry>(prefix));
	++m_nItems;
	return std::make_pair(nte.getFibEntry(), true);
}

void
Fib::erase(name_tree::Entry* nte, bool canDeleteNte)
{
	BOOST_ASSERT(nte != nullptr);

	nte->setFibEntry(nullptr);
	if (canDeleteNte) {
		m_nameTree.eraseIfEmpty(nte);
	}
	--m_nItems;
}

void
Fib::erase(const Name& prefix)
{
	name_tree::Entry* nte = m_nameTree.findExactMatch(prefix);
	if (nte != nullptr) {
		this->erase(nte);
	}
}

void
Fib::erase(const Entry& entry)
{
	name_tree::Entry* nte = m_nameTree.getEntry(entry);
	if (nte == nullptr) { // don't try to erase s_emptyEntry
		BOOST_ASSERT(&entry == s_emptyEntry.get());
		return;
	}
	this->erase(nte);
}

void
Fib::removeNextHop(Entry& entry, const Face& face)
{
	entry.removeNextHop(face);

	if (!entry.hasNextHops()) {
		name_tree::Entry* nte = m_nameTree.getEntry(entry);
		this->erase(nte, false);
	}
}

Fib::Range
Fib::getRange() const
{
	return m_nameTree.fullEnumerate(&nteHasFibEntry) |
			boost::adaptors::transformed(name_tree::GetTableEntry<Entry>(&name_tree::Entry::getFibEntry));
}

} // namespace fib
} // namespace nfd
