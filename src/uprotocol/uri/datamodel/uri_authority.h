/**
 * Copyright (c) 2023 General Motors GTO LLC
 *
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
#ifndef URI_AUTHORITY_H_
#define URI_AUTHORITY_H_

#include <algorithm>
#include <cctype>
#include <optional>
#include <string>

namespace uri_datamodel {
class uri_authority {
 public:
  uri_authority() : uri_authority("", "", false) {}

  static uri_authority local() { return empty(); }

  static uri_authority remote(const std::string& device,
                              const std::string& domain) {
    return uri_authority{device, domain, true};
  }

  static uri_authority empty() {
    static const auto EMPTY = uri_datamodel::uri_authority("", "", false);
    return EMPTY;
  }

  [[nodiscard]] bool isRemote() const {
    return getDomain().has_value() || getDevice().has_value();
  }

  [[nodiscard]] bool isLocal() const {
    return !getDomain().has_value() && !getDevice().has_value();
  }

  [[nodiscard]] std::optional<std::string> getDevice() const {
    bool whiteSpacesOnly = std::all_of(device.begin(), device.end(), isspace);
    return whiteSpacesOnly ? std::optional<std::string>()
                           : std::optional<std::string>(device);
  }

  [[nodiscard]] std::optional<std::string> getDomain() const {
    bool whiteSpacesOnly = std::all_of(domain.begin(), domain.end(), isspace);
    return whiteSpacesOnly ? std::optional<std::string>()
                           : std::optional<std::string>(domain);
  }

  [[nodiscard]] bool isMarkedRemote() const { return markedRemote; }

  bool operator==(const uri_authority& o) const {
    if (this == &o) {
      return true;
    }

    return ((markedRemote == o.markedRemote) && (device == o.device) &&
            (domain == o.domain));
  }

  [[nodiscard]] std::string tostring() const {
    return std::string("uAuthority{") + "device='" +
           (device.empty() ? "null" : device) + '\'' + ", domain='" +
           (domain.empty() ? "null" : domain) + '\'' +
           ", markedRemote=" + (markedRemote ? "true" : "false") + '}';
  }

 private:
  uri_authority(const std::string& device, const std::string& domain,
                bool markedRemote)
      : markedRemote(markedRemote) {
    if (!device.empty()) {
      auto current = device;
      std::transform(current.begin(), current.end(), current.begin(),
                     [](unsigned char c) { return std::tolower(c); });
      this->device = std::move(current);
    }
    if (!domain.empty()) {
      auto current = domain;
      std::transform(current.begin(), current.end(), current.begin(),
                     [](unsigned char c) { return std::tolower(c); });
      this->domain = std::move(current);
    }
  }

  std::string device;
  std::string domain;
  bool markedRemote;
};
}  // namespace uri_datamodel

#endif  // URI_AUTHORITY_H_
