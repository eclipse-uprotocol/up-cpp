// Copyright (c) 2024 Contributors to the Eclipse Foundation                    
//                                                                              
// See the NOTICE file(s) distributed with this work for additional                
// information regarding copyright ownership.                                   
//                                                                              
// Licensed under the Apache License, Version 2.0 (the "License");                 
// you may not use this file except in compliance with the License.                
// You may obtain a copy of the License at                                      
//                                                                              
//     http://www.apache.org/licenses/LICENSE-2.0                               
//                                                                              
// Unless required by applicable law or agreed to in writing, software             
// distributed under the License is distributed on an "AS IS" BASIS,               
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.        
// See the License for the specific language governing permissions and            
// limitations under the License.                                               
//                                                                              
// SPDX-FileType: SOURCE                                                        
// SPDX-FileCopyrightText: 2024 Contributors to the Eclipse Foundation          
// SPDX-License-Identifier: Apache-2.0

#ifndef UP_CPP_DATAMODEL_SERIALIZER_UURI_H
#define UP_CPP_DATAMODEL_SERIALIZER_UURI_H

#include <up-core-api/uri.pb.h>
#include <string>

/// @brief Collection of interfaces for converting uprotocol::v1::UUri objects
///        between protobuf and alternative representations.
namespace uprotocol::datamodel::serializer::uri {

/// @brief Converts to and from a human-readable string representation of UUri
///        according to the UUri spec.
struct AsString {
    [[nodiscard]] std::string serialize(const v1::UUri&);
    [[nodiscard]] v1::UUri deserialize(const std::string&);
};

} // namespace uprotocol::datamodel::serializer::uri

#endif // UP_CPP_DATAMODEL_SERIALIZER_UURI_H
