/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014-2015,  Regents of the University of California,
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


#define BOOST_TEST_MODULE storage test

#include <boost/test/unit_test.hpp>
#include "../../daemon/table/cs.hpp"
#include <ndn-cxx/security/key-chain.hpp>
#include "../../daemon/utils/parameterconfiguration.hpp"
#include "../../daemon/table/storage/storagemananger.hpp"

#include "tests/test-common.hpp"

namespace nfd {
namespace tests {

class StorageFixture : protected BaseFixture
{
protected:

  StorageFixture() : BaseFixture()
  {
    //here we can add common settings..
    ParameterConfiguration::getInstance ()->setParameter ("MaxMMStorageEntries",5);
    ParameterConfiguration::getInstance ()->setParameter ("MaxDiskStorageEntries",10);
    ParameterConfiguration::getInstance ()->setParameter ("MaxCsEntries",15);

    m_cs.setLimit ((ParameterConfiguration::getInstance ()->getParameter ("MaxCsEntries")));
  }

  Name insert(uint32_t id, const Name& name)
  {
    shared_ptr<Data> data = makeData(name);
    data->setFreshnessPeriod(time::milliseconds(99999));
    data->setContent(reinterpret_cast<const uint8_t*>(&id), sizeof(id));
    data->wireEncode();

    m_cs.insert(*data);

    return data->getFullName();
  }

  Interest& startInterest(const Name& name)
  {
    m_interest = make_shared<Interest>(name);
    return *m_interest;
  }

  void find(const std::function<void(uint32_t)>& check)
  {
    m_cs.find(*m_interest,
              [&] (const Interest& interest, const Data& data) {
                  const Block& content = data.getContent();
                  uint32_t found = *reinterpret_cast<const uint32_t*>(content.value());
                  check(found); },
              bind([&] { check(0); }));
  }

protected:
  Cs m_cs;
  shared_ptr<Interest> m_interest;
};

BOOST_FIXTURE_TEST_SUITE(StorageTest, StorageFixture)

// find miss, then insert
BOOST_AUTO_TEST_CASE(StorageTest)
{
  BOOST_REQUIRE(m_cs.getLimit () == ParameterConfiguration::getInstance ()->getParameter ("MaxCsEntries"));
  BOOST_REQUIRE(m_cs.size () == 0);
  BOOST_REQUIRE(nfd::cs::StorageMananger::getInstance()->storedEntries() == 0);

  insert(0, "/ndn/packet0/v0");

  BOOST_REQUIRE(m_cs.size () == 1);
  BOOST_REQUIRE(nfd::cs::StorageMananger::getInstance()->storedEntries() == 1);

  insert(1, "/ndn/packet0/v1");
  insert(2, "/ndn/packet0/v1");

  BOOST_REQUIRE(m_cs.size () == 3);
  BOOST_REQUIRE(nfd::cs::StorageMananger::getInstance()->storedEntries() == 3);

  insert(3, "/ndn/packet0/v2");
  insert(3, "/ndn/packet0/v2");

  BOOST_REQUIRE(m_cs.size () == 4);
  BOOST_REQUIRE(nfd::cs::StorageMananger::getInstance()->storedEntries() == 4);

  insert(4, "/ndn/packet1");
  insert(4, "/ndn/packet2");

  BOOST_REQUIRE(m_cs.size () == 6);
  BOOST_REQUIRE(nfd::cs::StorageMananger::getInstance()->storedEntries() == 6);

  insert(5, "/ndn");

  BOOST_REQUIRE(m_cs.size () == 7);
  BOOST_REQUIRE(nfd::cs::StorageMananger::getInstance()->storedEntries() == 7);

  insert(6, "/ndn/packet3");
  insert(7, "/ndn/packet4");
  insert(8, "/ndn/packet5");
  insert(9, "/ndn/packet6");
  insert(10, "/ndn/packet7");
  insert(10, "/ndn/packet8");
  insert(12, "/ndn/packet9");
  insert(13, "/ndn/packet10");

  BOOST_REQUIRE(m_cs.size () == 15);
  BOOST_REQUIRE(nfd::cs::StorageMananger::getInstance()->storedEntries() == 15);

  insert(14, "/ndn/packet11");
  insert(15, "/ndn/packet12");
  insert(16, "/ndn/packet13");

  BOOST_REQUIRE(m_cs.size () == 15);
  BOOST_REQUIRE(nfd::cs::StorageMananger::getInstance()->storedEntries() == 15);

  //some more intense test

  for(int i = 0; i < 1000; i++)
  {
    insert(i, "/ndn/auto/packet");
  }

  BOOST_REQUIRE(m_cs.size () == 15);
  BOOST_REQUIRE(nfd::cs::StorageMananger::getInstance()->storedEntries() == 15);
}

BOOST_AUTO_TEST_SUITE_END()


} // namespace tests
} // namespace nfd
