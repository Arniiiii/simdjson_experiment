#include "simdjson/simdjson.h"
#include <iostream>
#include <string_view>
int main(void) {
  std::string_view strv("{\"serverTime\":1499827319559}");
  simdjson::padded_string_view pstrv(strv, strv.size());
  simdjson::ondemand::parser parser;
  simdjson::ondemand::document time = parser.iterate(pstrv);
  std::cout << time["serverTime"] << '\n';
}

