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

#ifndef NDN_CXX_LP_FIELDS_HPP
#define NDN_CXX_LP_FIELDS_HPP

#include "detail/field-decl.hpp"

#include "sequence.hpp"
#include "cache-policy.hpp"
#include "nack-header.hpp"
#include "name.hpp"

#include <boost/mpl/set.hpp>

namespace ndn {
namespace lp {
constexpr bool isTag = true;
constexpr bool notTag = false;

typedef detail::FieldDecl<field_location_tags::Header,
                          Sequence,
                          tlv::Sequence, notTag> SequenceField;
BOOST_CONCEPT_ASSERT((Field<SequenceField>));

typedef detail::FieldDecl<field_location_tags::Header,
                          uint64_t,
                          tlv::FragIndex, notTag> FragIndexField;
BOOST_CONCEPT_ASSERT((Field<FragIndexField>));

typedef detail::FieldDecl<field_location_tags::Header,
                          uint64_t,
                          tlv::FragCount, notTag> FragCountField;
BOOST_CONCEPT_ASSERT((Field<FragCountField>));

typedef detail::FieldDecl<field_location_tags::Header,
                          NackHeader,
                          tlv::Nack, notTag> NackField;
BOOST_CONCEPT_ASSERT((Field<NackField>));

typedef detail::FieldDecl<field_location_tags::Header,
                          uint64_t,
                          tlv::NextHopFaceId, notTag> NextHopFaceIdField;
BOOST_CONCEPT_ASSERT((Field<NextHopFaceIdField>));

typedef detail::FieldDecl<field_location_tags::Header,
                          CachePolicy,
                          tlv::CachePolicy, notTag> CachePolicyField;
BOOST_CONCEPT_ASSERT((Field<CachePolicyField>));

typedef detail::FieldDecl<field_location_tags::Header,
                          uint64_t,
                          tlv::IncomingFaceId, notTag> IncomingFaceIdField;
BOOST_CONCEPT_ASSERT((Field<IncomingFaceIdField>));

typedef detail::FieldDecl<field_location_tags::Header,
                          uint64_t,
                          tlv::CongestionMark, notTag> CongestionMarkField;
BOOST_CONCEPT_ASSERT((Field<CongestionMarkField>));

typedef detail::FieldDecl<field_location_tags::Header,
                          uint64_t,
                          tlv::HopCountTag, isTag> HopCountTagField;
BOOST_CONCEPT_ASSERT((Field<HopCountTagField>));

//added 2019/10/26

typedef detail::FieldDecl<field_location_tags::Header,
                          uint64_t,
                          tlv::PartialHopTag, isTag> PartialHopTagField;
BOOST_CONCEPT_ASSERT((Field<PartialHopTagField>));

typedef detail::FieldDecl<field_location_tags::Header,
                          uint64_t,
                          tlv::CountTag, isTag> CountTagField;
BOOST_CONCEPT_ASSERT((Field<CountTagField>));

typedef detail::FieldDecl<field_location_tags::Header,
                          Name,
                          tlv::FunctionNameTag, isTag> FunctionNameTagField;
BOOST_CONCEPT_ASSERT((Field<FunctionNameTagField>));

typedef detail::FieldDecl<field_location_tags::Header,
                          Name,
                          tlv::PreviousFunctionTag, isTag> PreviousFunctionTagField;
BOOST_CONCEPT_ASSERT((Field<PreviousFunctionTagField>));

typedef detail::FieldDecl<field_location_tags::Header,
                          Name,
                          tlv::PitFunctionNameTag, isTag> PitFunctionNameTagField;
BOOST_CONCEPT_ASSERT((Field<PitFunctionNameTagField>));
//


/**
 * The value of the wire encoded field is the data between the provided iterators. During
 * encoding, the data is copied from the Buffer into the wire buffer.
 */
typedef detail::FieldDecl<field_location_tags::Fragment,
                          std::pair<Buffer::const_iterator, Buffer::const_iterator>,
                          tlv::Fragment, notTag> FragmentField;
BOOST_CONCEPT_ASSERT((Field<FragmentField>));

/**
 * \brief set of all field declarations
 */
typedef boost::mpl::set<
  FragmentField,
  SequenceField,
  FragIndexField,
  FragCountField,
  NackField,
  NextHopFaceIdField,
  CachePolicyField,
  IncomingFaceIdField,
  CongestionMarkField,
  HopCountTagField,
  PartialHopTagField,
  CountTagField,
  FunctionNameTagField,
  PreviousFunctionTagField,
  PitFunctionNameTagField
  > FieldSet;

} // namespace lp
} // namespace ndn

#endif // NDN_CXX_LP_FIELDS_HPP
