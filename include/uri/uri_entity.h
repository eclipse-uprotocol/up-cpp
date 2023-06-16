/**
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#ifndef URI_ENTITY_H_
#define URI_ENTITY_H_

#include <algorithm>
#include <cctype>
#include <optional>
#include <string>

namespace uri_datamodel {
class uri_entity {
 public:
  uri_entity() : uri_entity("", "") {}

  uri_entity(const std::string& name, const std::string& version)
      : name(name), version(version) {}

  static uri_entity fromName(const std::string& name) {
    return uri_entity{name, ""};
  }

  static uri_entity empty() {
    static const auto EMPTY = uri_datamodel::uri_entity("", "");
    return EMPTY;
  }

  [[nodiscard]] bool isEmpty() const {
    bool whiteSpacesOnly = std::all_of(name.begin(), name.end(), isspace);
    return whiteSpacesOnly && !getVersion().has_value();
  }

  [[nodiscard]] std::string getName() const { return name; }

  [[nodiscard]] std::optional<std::string> getVersion() const {
    bool whiteSpacesOnly = std::all_of(version.begin(), version.end(), isspace);
    return whiteSpacesOnly ? std::optional<std::string>()
                           : std::optional<std::string>(version);
  }

  bool operator==(const uri_entity& o) const {
    if (this == &o) {
      return true;
    }

    return ((name == o.name) && (version == o.version));
  }

  [[nodiscard]] std::string tostring() const {
    bool whiteSpacesOnly = std::all_of(version.begin(), version.end(), isspace);
    return std::string("uEntity{") + "name='" + name + '\'' + ", version='" +
           (whiteSpacesOnly ? "latest" : version) + '\'' + '}';
  }

 private:
  const std::string name;
  const std::string version;
};
}  // namespace uri_datamodel

#endif  // URI_ENTITY_H_
