/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2016 Regents of the University of California.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 *
 * ndn-cxx library is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * ndn-cxx library is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received copies of the GNU General Public License and GNU Lesser
 * General Public License along with ndn-cxx, e.g., in COPYING.md file.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 */

#ifndef NDN_INTEREST_HPP
#define NDN_INTEREST_HPP

#include <string>
#include "name.hpp"
#include "selectors.hpp"
#include "util/time.hpp"
#include "tag-host.hpp"
#include "link.hpp"

namespace ndn {

class Data;

/** @var const unspecified_duration_type DEFAULT_INTEREST_LIFETIME;
 *  @brief default value for InterestLifetime
 */
const time::milliseconds DEFAULT_INTEREST_LIFETIME = time::milliseconds(4000);
const time::milliseconds DEFAULT_SERVICETIME = time::milliseconds(0);
const time::milliseconds DEFAULT_FUNCTIONTIME = time::milliseconds(0);

/** @brief represents an Interest packet
 */
class Interest : public TagHost, public enable_shared_from_this<Interest>
{
public:
  class Error : public tlv::Error
  {
  public:
    explicit
    Error(const std::string& what)
      : tlv::Error(what)
    {
    }
  };

  /** @brief Create a new Interest with an empty name (`ndn:/`)
   *  @warning In certain contexts that use Interest::shared_from_this(), Interest must be created
   *           using `make_shared`. Otherwise, .shared_from_this() will throw an exception.
   */
  Interest();

  /** @brief Create a new Interest with the given name
   *  @param name The name for the interest.
   *  @note This constructor allows implicit conversion from Name.
   *  @warning In certain contexts that use Interest::shared_from_this(), Interest must be created
   *           using `make_shared`. Otherwise, .shared_from_this() will throw an exception.
   */
  Interest(const Name& name);

  /** @brief Create a new Interest with the given name and interest lifetime
   *  @param name             The name for the interest.
   *  @param interestLifetime The interest lifetime in time::milliseconds, or -1 for none.
   *  @warning In certain contexts that use Interest::shared_from_this(), Interest must be created
   *           using `make_shared`. Otherwise, .shared_from_this() will throw an exception.
   */
  Interest(const Name& name, const time::milliseconds& interestLifetime);

  /** @brief Create from wire encoding
   *  @warning In certain contexts that use Interest::shared_from_this(), Interest must be created
   *           using `make_shared`. Otherwise, .shared_from_this() will throw an exception.
   */
  explicit
  Interest(const Block& wire);

  /**
   * @brief Fast encoding or block size estimation
   */
  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& encoder) const;

  /**
   * @brief Encode to a wire format
   */
  const Block&
  wireEncode() const;

  /**
   * @brief Decode from the wire format
   */
  void
  wireDecode(const Block& wire);

  /**
   * @brief Check if already has wire
   */
  bool
  hasWire() const
  {
    return m_wire.hasWire();
  }

  /**
   * @brief Encode the name according to the NDN URI Scheme
   *
   * If there are interest selectors, this method will append "?" and add the selectors as
   * a query string.  For example, "/test/name?ndn.ChildSelector=1"
   */
  std::string
  toUri() const;

public: // Link and forwarding hint

   /**
   * @brief Check whether the Interest contains a Link object
   * @return True if there is a link object, otherwise false
   */
  bool
  hasLink() const;

  /**
   * @brief Get the link object for this interest
   * @return The link object if there is one contained in this interest
   * @throws Interest::Error if there is no link object contained in the interest
   * @throws tlv::Error if the incorporated link object is malformed
   */
  const Link&
  getLink() const;

  /**
   * @brief Set the link object for this interest
   * @param link The link object that will be included in this interest (in wire format)
   * @post !hasSelectedDelegation()
   */
  void
  setLink(const Block& link);

  /**
   * @brief Delete the link object for this interest
   * @post !hasLink()
   */
  void
  unsetLink();

  /**
   * @brief Check whether the Interest includes a selected delegation
   * @return True if there is a selected delegation, otherwise false
   */
  bool
  hasSelectedDelegation() const;

  /**
   * @brief Get the name of the selected delegation
   * @return The name of the selected delegation
   * @throw Error SelectedDelegation is not set.
   */
  Name
  getSelectedDelegation() const;

  /**
   * @brief Set the selected delegation
   * @param delegationName The name of the selected delegation
   * @throw Error Link is not set.
   * @throw std::invalid_argument @p delegationName does not exist in Link.
   */
  void
  setSelectedDelegation(const Name& delegationName);

  /**
   * @brief Set the selected delegation
   * @param delegationIndex The index of the selected delegation
   * @throw Error Link is not set.
   * @throw std::out_of_range @p delegationIndex is out of bound in Link.
   */
  void
  setSelectedDelegation(size_t delegationIndex);

   /**
   * @brief Unset the selected delegation
   */
  void
  unsetSelectedDelegation();

public: // matching
  /** @brief Check if Interest, including selectors, matches the given @p name
   *  @param name The name to be matched. If this is a Data name, it shall contain the
   *              implicit digest component
   */
  bool
  matchesName(const Name& name) const;

  /**
   * @brief Check if Interest can be satisfied by @p data.
   *
   * This method considers Name, MinSuffixComponents, MaxSuffixComponents,
   * PublisherPublicKeyLocator, and Exclude.
   * This method does not consider ChildSelector and MustBeFresh.
   *
   * @todo recognize implicit digest component
   */
  bool
  matchesData(const Data& data) const;

public: // Name and guiders
  const Name&
  getName() const
  {
    return m_name;
  }

  Interest&
  setName(const Name& name)
  {
    m_name = name;
    m_wire.reset();
    return *this;
  }

  const Name&
  getFunctionFullName() const
  {
	  return m_functionFullName;
  }

  void
  setFunctionFullName(const Name& name)
  {
	  m_functionFullName = name;
	  m_wire.reset();
  }

  void
  setFunctionFullName(const Name& name) const
    {
  	  m_functionFullName = name;
  	  m_wire.reset();
    }

  void
  addFunctionFullName(Name name) const
  {
	  std::string oldFunc = m_functionFullName.toUri();
	  std::string newFunc = name.toUri();
	  newFunc += oldFunc;
	  //std::cout << "newFunc: " << newFunc << ", name: " << name.toUri() <<std::endl;
	  shared_ptr<Name> addFunc = make_shared<Name>(Name(newFunc));
	  this->setFunctionFullName(*addFunc);
	  m_wire.reset();
  }

  const Name&
  getFunctionNextName() const
  {
	  return m_functionNextName;
  }

  void
  setFunctionNextName(const Name& name)
  {
	  m_functionNextName = name;
	  m_wire.reset();
  }

  void
  setFunctionNextName(const Name& name) const
    {
  	  m_functionNextName = name;
  	  m_wire.reset();
    }

  const Name&
  getFunction() const
  {
    return m_functionName;
  }

  void
  setFunction(const Name& functionName)
  const
  {
    m_functionName = functionName;
    m_wire.reset();
    //return *this;
  }

  void
  removeHeadFunction(const Interest& interest)
  const
  {
    std::string funcStr = interest.getFunction().toUri();
    int pos = funcStr.find("/", 1);
    if(pos == -1 && funcStr.size() > 1){
      funcStr.erase(1, funcStr.size()-1);
      Name newFunc(funcStr);
      interest.setFunction(newFunc);
    }
    else if(pos != -1){
      funcStr.erase(1, pos);
      Name newFunc(funcStr);
      interest.setFunction(newFunc);
    }
    m_wire.reset();
  }

  //defined by yamaguchi
  void
  replaceHeadFunction(const Interest& interest, shared_ptr<std::string> str)
  const
  {
	std::string funcStr = interest.getFunction().toUri();
	int pos = funcStr.find("/", 1);
	if(pos == -1 && funcStr.size() > 1){
		std::string headFunc = *str;
		Name newFunc(headFunc);
		interest.setFunction(newFunc);
	}
	else if(pos != -1){
		std::string headFunc = *str;
		funcStr.erase(1,pos);
		headFunc += funcStr;
		Name newFunc(headFunc);
		interest.setFunction(newFunc);
	}
	m_wire.reset();
  }

  const time::milliseconds&
  getInterestLifetime() const
  {
    return m_interestLifetime;
  }

  Interest&
  setInterestLifetime(const time::milliseconds& interestLifetime)
  {
    m_interestLifetime = interestLifetime;
    m_wire.reset();
    return *this;
  }

  const time::milliseconds&
  getServiceTime() const
  {
    return m_serviceTime;
  }

  void
  setServiceTime(const time::milliseconds& serviceTime) const
  {
    m_serviceTime = serviceTime;
    m_wire.reset();
    //return *this;
  }

  const time::milliseconds&
  getFunctionTime() const
  {
    return m_functionTime;
  }

  //Interest&
  void
  setFunctionTime(const time::milliseconds& functionTime) const
  {
    m_functionTime = functionTime;
    m_wire.reset();
    //return *this;
  }

  int
  getFunctionFlag() const
  {
    return m_functionFlag;
  }

  void
  setFunctionFlag(const int functionFlag) const
  {
    m_functionFlag = functionFlag;
    m_wire.reset();
  }

  /** @brief Check if Nonce set
   */
  bool
  hasNonce() const
  {
    return m_nonce.hasWire();
  }

  /** @brief Get Interest's nonce
   *
   *  If nonce was not set before this call, it will be automatically assigned to a random value
   */
  uint32_t
  getNonce() const;

  /** @brief Set Interest's nonce
   *
   *  If wire format already exists, this call simply replaces nonce in the
   *  existing wire format, without resetting and recreating it.
   */
  Interest&
  setNonce(uint32_t nonce);

  /** @brief Refresh nonce
   *
   *  It's guaranteed that new nonce value differs from the existing one.
   *
   *  If nonce is already set, it will be updated to a different random value.
   *  If nonce is not set, this method does nothing.
   */
  void
  refreshNonce();

public: // Selectors
  /**
   * @return true if Interest has any selector present
   */
  bool
  hasSelectors() const
  {
    return !m_selectors.empty();
  }

  const Selectors&
  getSelectors() const
  {
    return m_selectors;
  }

  Interest&
  setSelectors(const Selectors& selectors)
  {
    m_selectors = selectors;
    m_wire.reset();
    return *this;
  }

  int
  getMinSuffixComponents() const
  {
    return m_selectors.getMinSuffixComponents();
  }

  Interest&
  setMinSuffixComponents(int minSuffixComponents)
  {
    m_selectors.setMinSuffixComponents(minSuffixComponents);
    m_wire.reset();
    return *this;
  }

  int
  getMaxSuffixComponents() const
  {
    return m_selectors.getMaxSuffixComponents();
  }

  Interest&
  setMaxSuffixComponents(int maxSuffixComponents)
  {
    m_selectors.setMaxSuffixComponents(maxSuffixComponents);
    m_wire.reset();
    return *this;
  }

  const KeyLocator&
  getPublisherPublicKeyLocator() const
  {
    return m_selectors.getPublisherPublicKeyLocator();
  }

  Interest&
  setPublisherPublicKeyLocator(const KeyLocator& keyLocator)
  {
    m_selectors.setPublisherPublicKeyLocator(keyLocator);
    m_wire.reset();
    return *this;
  }

  const Exclude&
  getExclude() const
  {
    return m_selectors.getExclude();
  }

  Interest&
  setExclude(const Exclude& exclude)
  {
    m_selectors.setExclude(exclude);
    m_wire.reset();
    return *this;
  }

  int
  getChildSelector() const
  {
    return m_selectors.getChildSelector();
  }

  Interest&
  setChildSelector(int childSelector)
  {
    m_selectors.setChildSelector(childSelector);
    m_wire.reset();
    return *this;
  }

  int
  getMustBeFresh() const
  {
    return m_selectors.getMustBeFresh();
  }

  Interest&
  setMustBeFresh(bool mustBeFresh)
  {
    m_selectors.setMustBeFresh(mustBeFresh);
    m_wire.reset();
    return *this;
  }

public: // EqualityComparable concept
  bool
  operator==(const Interest& other) const
  {
    return wireEncode() == other.wireEncode();
  }

  bool
  operator!=(const Interest& other) const
  {
    return !(*this == other);
  }

private:
  Name m_name;
  mutable Name m_functionNextName;
  mutable Name m_functionFullName;
  mutable Name m_functionName;
  Selectors m_selectors;
  mutable Block m_nonce;
  time::milliseconds m_interestLifetime;
  mutable time::milliseconds m_serviceTime;
  mutable time::milliseconds m_functionTime;

  mutable Block m_link;
  mutable shared_ptr<Link> m_linkCached;
  size_t m_selectedDelegationIndex;
  mutable Block m_wire;
  mutable int m_functionFlag;
};

std::ostream&
operator<<(std::ostream& os, const Interest& interest);

inline std::string
Interest::toUri() const
{
  std::ostringstream os;
  os << *this;
  return os.str();
}

} // namespace ndn

#endif // NDN_INTEREST_HPP
