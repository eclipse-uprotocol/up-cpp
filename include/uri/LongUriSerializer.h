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

#include "UUri.h"
#include "UAuthority.h"
#include "UEntity.h"
#include "UResource.h"

namespace uprotocol::uri {

/**
 * UUri Serializer that serializes a UUri to a string (long format) per
 * https://github.com/eclipse-uprotocol/uprotocol-spec/blob/main/basics/uri.adoc
 */
class LongUriSerializer {
public:

    /**
     * Support for serializing {@link UUri} objects into their String format.
     * @param Uri {@link UUri} object to be serialized to the String format.
     * @return Returns the String format of the supplied {@link UUri} that can be used as a sink or a source in a uProtocol publish communication.
     */
    static std::string serialize(const UUri& upUri) {
      if (upUri.isEmpty()) {
        return "";
      }

      std::string sb;
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

    /**
     * Deserialize a String into a UUri object.
     * @param uProtocolUri A long format uProtocol URI.
     * @return Returns an UUri data object.
     */
    static UUri deserialize(std::string const& uProtocolUri) {
      if (uProtocolUri.empty()) {
        return UUri::empty();
      }

      static const std::regex schemaPattern("up:", std::regex_constants::icase);

      std::string uri = std::regex_replace(uProtocolUri, schemaPattern, std::string(""),
                                          std::regex_constants::format_first_only);

      std::replace(uri.begin(), uri.end(), '\\', '/');

      bool isLocal = (uri.find("//") != 0);  // local if does not start with "//"

      const auto uriParts = split(uri, "/");
      auto numberOfPartsInUri = uriParts.size();

      if (numberOfPartsInUri == 0 || numberOfPartsInUri == 1) {
        // if split result is 0 or 1 (it means there is no valid address) return
        return isLocal ? UUri::empty()
                      : UUri(UAuthority::longRemote("", ""), UEntity::empty(), UResource::empty());
      }

      std::string useName;
      std::string useVersion;
      UResource uResource;
      UAuthority uAuthority;

      if (isLocal) {
        uAuthority = UAuthority::local();
        useName = uriParts[1];
        if (numberOfPartsInUri > 2) {
          useVersion = uriParts[2];

          uResource = (numberOfPartsInUri > 3) ? parseFromString(uriParts[3])
                                              : UResource::empty();
        } else {
          uResource = UResource::empty();
        }
      } else {
        auto authorityParts = split(uriParts[2], ".");
        std::string device = authorityParts[0];
        std::string domain;
        if (authorityParts.size() > 1) {
          domain = authorityParts[1];
        }
        uAuthority = UAuthority::longRemote(device, domain);

        if (uriParts.size() > 3) {
          useName = uriParts[3];
          if (numberOfPartsInUri > 4) {
            useVersion = uriParts[4];
            uResource = numberOfPartsInUri > 5 ? parseFromString(uriParts[5])
                                              : UResource::empty();
          } else {
            uResource = UResource::empty();
          }
        } else {
          return UUri{uAuthority, UEntity::empty(), UResource::empty()};
        }
      }

      std::optional<uint32_t> versionInt = std::nullopt;
      try {
          versionInt = useVersion.empty() ? std::nullopt
                                      : std::optional<uint32_t>(std::stoi(useVersion));
      } catch (const std::invalid_argument& e) {
          spdlog::error("Error parsing version: {}", e.what());
      }
      return UUri{uAuthority, UEntity::longFormat(useName, versionInt), uResource};
    }

    /**
     * Indicates if the supplied String is a valid uProtocol URI.
     * @param uProtocolUri A String that represents a uProtocol URI.
     * @return Returns true if the supplied String is a valid uProtocol URI.
     */
    static bool isValidupUri(const std::string& uProtocolUri) {
      return !deserialize(uProtocolUri).isEmpty();
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

    /**
     * Trim from start of a string (in place).
     * @param s String to be trimmed.
     **/
    static inline void ltrim(std::string& s) {
      s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
                return !std::isspace(ch);
              }));
    }

    /**
     * Trim from end of a string (in place)
     * @param s String to be trimmed.
     **/
    static inline void rtrim(std::string& s) {
      s.erase(std::find_if(s.rbegin(), s.rend(),
                          [](unsigned char ch) { return !std::isspace(ch); })
                  .base(),
              s.end());
    }

    /**
     * Trim start and end of string (in place)
     * @param s String to be trimmed.
     **/
    static inline void trim(std::string& s) {
      rtrim(s);
      ltrim(s);
    }

    /**
     * Trim from start (copying)
     * @param s String to be trimmed.
     * @return Returns the trimmed string.
     **/
    static inline std::string ltrim_copy(std::string s) {
      ltrim(s);
      return s;
    }

    /**
     * Trim from end (copying)
     * @param s String to be trimmed.
     * @return Returns the trimmed string.
     **/
    static inline std::string rtrim_copy(std::string s) {
      rtrim(s);
      return s;
    }

    /**
     * Trim from both ends (copying)
     * @param s String to be trimmed.
     * @return Returns the trimmed string.
     **/
    static inline std::string trim_copy(std::string s) {
      trim(s);
      return s;
    }

    /**
     * Create the resource part of the Uri from a resource object.
     * @param uResource  Resource representing a resource or an RPC method.
    */
    static std::string buildupResourcePartOfUri(const UResource& uResource) {
      if (uResource.isEmpty()) {
        return "";
      }

      std::string sb("/");
      sb.append(uResource.getName());
      if (uResource.getInstance().has_value()) {
        sb.append(".").append(uResource.getInstance().value());
      }
      if (uResource.getMessage().has_value()) {
        sb.append("#").append(uResource.getMessage().value());
      }

      return sb;
    }

    /**
     * Create the service part of the uProtocol URI from an  software entity object.
     * @param use  Software Entity representing a service or an application.
     */
    static std::string buildupSoftwareEntityPartOfUri(const UEntity& use) {
      std::string sb = trim_copy(use.getName());
      sb.append("/");
      if (use.getVersion().has_value()) {
        sb.append(std::to_string(use.getVersion().value()));
      }

      return sb;
    }

    /**
     * Create the authority part of the uProtocol URI from an  authority object.
     * @param Authority represents the deployment location of a specific  Software Entity in the Ultiverse.
     * @return Returns the String representation of the  Authority in the uProtocol URI.
     */
    static std::string buildAuthorityPartOfUri(const UAuthority& upAuthority) {
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

    /**
     * Static factory method for creating a UResource using a string that contains
     * name + instance + message.
     * @param resourceString String that contains the UResource information.
     * @return Returns a UResource object.
     */
    static UResource parseFromString(const std::string& resourceString) {
      auto parts = split(resourceString, "#");
      std::string nameAndInstance = parts[0];
      auto nameAndInstanceParts = split(nameAndInstance, ".");
      std::string resourceName = nameAndInstanceParts[0];
      std::string resourceInstance = nameAndInstanceParts.size() > 1 ? nameAndInstanceParts[1] : "";
      std::string resourceMessage = parts.size() > 1 ? parts[1] : "";
      return UResource::longFormat(resourceName, resourceInstance, resourceMessage);
    }
}; // class LongUriSerializer

} // namespace uri_factory

#endif // up_URI_FACTORY_H_
