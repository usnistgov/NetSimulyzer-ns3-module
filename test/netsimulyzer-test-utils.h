#pragma once

#include <ns3/core-module.h>
#include <ns3/netsimulyzer-module.h>
#include <ns3/network-module.h>
#include <ns3/test.h>

#include <string>
#include <string_view>
#include <vector>


namespace ns3::test {
class NetSimulyzerTestCase : public TestCase {
protected:
  void RequiredFields(const std::vector<std::string> &fields,
                      const nlohmann::json &element,
                      const std::string &elementName);

    void CheckColor(const nlohmann::json &color, const netsimulyzer::Color3 &check);

public:
  explicit NetSimulyzerTestCase(const std::string &name);
};

inline void
NetSimulyzerTestCase::RequiredFields(const std::vector<std::string> &fields,
                                     const nlohmann::json &element,
                                     const std::string &elementName) {
  for (const auto &field : fields) {
    NS_TEST_ASSERT_MSG_EQ(element.contains(field), true,
                          "Element '" << elementName
                                      << "' must contain field  '" << field
                                      << "'");
  }
}

inline void NetSimulyzerTestCase::CheckColor(const nlohmann::json &color, const netsimulyzer::Color3 &check)
{
    RequiredFields({"red", "green", "blue"}, color, "color");

    NS_TEST_ASSERT_MSG_EQ(color["red"].get<uint8_t>(), check.red, "Event red component should match");
    NS_TEST_ASSERT_MSG_EQ(color["green"].get<uint8_t>(), check.green, "Event green component should match");
    NS_TEST_ASSERT_MSG_EQ(color["blue"].get<uint8_t>(), check.blue, "Event blue component should match");

}

inline NetSimulyzerTestCase::NetSimulyzerTestCase(const std::string &name)
    : TestCase(name) {}
} // namespace ns3::test
