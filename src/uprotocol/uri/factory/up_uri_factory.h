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
#ifndef UP_URI_FACTORY_H_
#define UP_URI_FACTORY_H_

#include <spdlog/spdlog.h>

#include <algorithm>
#include <cctype>
#include <locale>
#include <optional>
#include <regex>
#include <string>
#include <vector>

#include "up_uri.h"
#include "uri_authority.h"
#include "uri_entity.h"
#include "uri_resource.h"

namespace uri_factory {
class up_uri_factory {
 public:
  static std::string buildUProtocolUri(const uri_datamodel::up_uri& upUri) {
    if (upUri.isEmpty()) {
      return uri_datamodel::up_uri::SCHEME;
    }

    std::string sb(uri_datamodel::up_uri::SCHEME);

    sb.append(buildAuthorityPartOfUri(upUri.getUAuthority()));

    if (upUri.getUAuthority().isMarkedRemote()) {
      sb.append("/");
    }

    if (upUri.getUEntity().isEmpty()) {
      return sb;
    }

    sb.append(buildupSoftwareEntityPartOfUri(upUri.getUEntity()));

    sb.append(buildupResourcePartOfUri(upUri.getUResource()));

    return sb;
  }

  static std::string buildUProtocolUri(
      const uri_datamodel::uri_authority& uAuthority,
      const uri_datamodel::uri_entity& uEntity,
      const uri_datamodel::uri_resource& uResource) {
    return buildUProtocolUri(
        uri_datamodel::up_uri(uAuthority, uEntity, uResource));
  }

  static std::string buildUriForRpc(
      uri_datamodel::uri_authority const& uAuthority,
      uri_datamodel::uri_entity const& uEntitySource) {
    std::string sb(uri_datamodel::up_uri::SCHEME);

    sb.append(buildAuthorityPartOfUri(uAuthority));

    if (uAuthority.isMarkedRemote()) {
      sb.append("/");
    }
    sb.append(buildupSoftwareEntityPartOfUri(uEntitySource));
    sb.append("/rpc.response");

    return sb;
  }

  static std::string buildMethodUri(
      uri_datamodel::uri_authority const& uAuthority,
      uri_datamodel::uri_entity const& uEntity, std::string const& methodName) {
    std::string sb(uri_datamodel::up_uri::SCHEME);

    sb.append(buildAuthorityPartOfUri(uAuthority));

    if (uAuthority.isMarkedRemote()) {
      sb.append("/");
    }
    sb.append(buildupSoftwareEntityPartOfUri(uEntity));

    sb.append(buildupResourcePartOfUri(
        uri_datamodel::uri_resource::forRpc(methodName)));

    return sb;
  }

  static uri_datamodel::up_uri parseFromUri(std::string const& uProtocolUri) {
    if (uProtocolUri.empty()) {
      return uri_datamodel::up_uri::empty();
    }

    static const std::regex schemaPattern("up:", std::regex_constants::icase);

    std::string uri =
        std::regex_replace(uProtocolUri, schemaPattern, std::string(""),
                           std::regex_constants::format_first_only);

    std::replace(uri.begin(), uri.end(), '\\', '/');

    bool isLocal = (uri.find("//") != 0);  // local if does not start with "//"

    const auto uriParts = split(uri, "/");
    auto numberOfPartsInUri = uriParts.size();

    if (numberOfPartsInUri == 0 || numberOfPartsInUri == 1) {
      // if split result is 0 or 1 (it means there is no valid address) return
      return isLocal ? uri_datamodel::up_uri::empty()
                     : uri_datamodel::up_uri(
                           uri_datamodel::uri_authority::remote("", ""),
                           uri_datamodel::uri_entity::empty(),
                           uri_datamodel::uri_resource::empty());
    }

    std::string useName;
    std::string useVersion;

    uri_datamodel::uri_resource uResource;
    uri_datamodel::uri_authority uAuthority;

    if (isLocal) {
      uAuthority = uri_datamodel::uri_authority::local();
      useName = uriParts[1];
      if (numberOfPartsInUri > 2) {
        useVersion = uriParts[2];

        uResource = numberOfPartsInUri > 3
                        ? buildupResource(uriParts[3])
                        : uri_datamodel::uri_resource::empty();
      } else {
        uResource = uri_datamodel::uri_resource::empty();
      }
    } else {
      auto authorityParts = split(uriParts[2], ".");
      std::string device = authorityParts[0];
      std::string domain;
      if (authorityParts.size() > 1) {
        domain = authorityParts[1];
      }
      uAuthority = uri_datamodel::uri_authority::remote(device, domain);

      if (uriParts.size() > 3) {
        useName = uriParts[3];
        if (numberOfPartsInUri > 4) {
          useVersion = uriParts[4];

          uResource = numberOfPartsInUri > 5
                          ? buildupResource(uriParts[5])
                          : uri_datamodel::uri_resource::empty();
        } else {
          uResource = uri_datamodel::uri_resource::empty();
        }
      } else {
        return uri_datamodel::up_uri{uAuthority,
                                     uri_datamodel::uri_entity::empty(),
                                     uri_datamodel::uri_resource::empty()};
      }
    }

    return uri_datamodel::up_uri{
        uAuthority, uri_datamodel::uri_entity(useName, useVersion), uResource};
  }

  static bool isValidupUri(const std::string& uProtocolUri) {
    return !parseFromUri(uProtocolUri).isEmpty();
  }

 private:
  /**
   * Splits this string around matches of the given string delimiter.
   * Trailing empty strings are therefore not included in the resulting array.
   **/
  static std::vector<std::string> split(std::string str,
                                        const std::string_view& delimiter) {
    std::vector<std::string> vec;

    size_t pos{};
    std::string token;
    while ((pos = str.find(delimiter)) != std::string::npos) {
      token = str.substr(0, pos);
      vec.push_back(token);
      str.erase(0, pos + delimiter.length());
    }

    vec.push_back(str);

    return vec;
  }

  // trim from start (in place)
  static inline void ltrim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
              return !std::isspace(ch);
            }));
  }

  // trim from end (in place)
  static inline void rtrim(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
                         [](unsigned char ch) { return !std::isspace(ch); })
                .base(),
            s.end());
  }

  // trim from both ends (in place)
  static inline void trim(std::string& s) {
    rtrim(s);
    ltrim(s);
  }

  // trim from start (copying)
  static inline std::string ltrim_copy(std::string s) {
    ltrim(s);
    return s;
  }

  // trim from end (copying)
  static inline std::string rtrim_copy(std::string s) {
    rtrim(s);
    return s;
  }

  // trim from both ends (copying)
  static inline std::string trim_copy(std::string s) {
    trim(s);
    return s;
  }

  static std::string buildupResourcePartOfUri(
      const uri_datamodel::uri_resource& uResource) {
    if (uResource.isEmpty()) {
      return "";
    }

    std::string sb = std::string("/").append(uResource.getName());
    if (uResource.getInstance().has_value()) {
      sb.append(".").append(uResource.getInstance().value());
    }
    if (uResource.getMessage().has_value()) {
      sb.append("#").append(uResource.getMessage().value());
    }

    return sb;
  }

  static std::string buildupSoftwareEntityPartOfUri(
      const uri_datamodel::uri_entity& use) {
    std::string sb = trim_copy(use.getName());
    sb.append("/");
    if (use.getVersion().has_value()) {
      sb.append(use.getVersion().value());
    }

    return sb;
  }

  static std::string buildAuthorityPartOfUri(
      const uri_datamodel::uri_authority& upAuthority) {
    if (upAuthority.isLocal()) {
      return "/";
    }

    std::string partialURI("//");
    const std::optional<std::string> maybeDevice = upAuthority.getDevice();
    const std::optional<std::string> maybeDomain = upAuthority.getDomain();

    if (maybeDevice.has_value()) {
      partialURI.append(maybeDevice.value());
      if (maybeDomain.has_value()) {
        partialURI.append(".");
      }
    }
    if (maybeDomain.has_value()) {
      partialURI.append(maybeDomain.value());
    }

    return partialURI;
  }

  static uri_datamodel::uri_resource buildupResource(
      const std::string& resourceString) {
    auto parts = split(resourceString, "#");
    std::string nameAndInstance = parts[0];
    auto nameAndInstanceParts = split(nameAndInstance, ".");
    std::string resourceName = nameAndInstanceParts[0];
    std::string resourceInstance =
        nameAndInstanceParts.size() > 1 ? nameAndInstanceParts[1] : "";
    std::string resourceMessage = parts.size() > 1 ? parts[1] : "";

    return uri_datamodel::uri_resource{resourceName, resourceInstance,
                                       resourceMessage};
  }
};
}  // namespace uri_factory

#endif  // up_URI_FACTORY_H_
