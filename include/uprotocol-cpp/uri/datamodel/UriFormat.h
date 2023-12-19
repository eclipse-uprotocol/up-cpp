/*
 * Copyright (c) 2023 General Motors GTO LLC
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 * SPDX-FileType: SOURCE
 * SPDX-FileCopyrightText: 2023 General Motors GTO LLC
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _URI_FORMAT_H_
#define _URI_FORMAT_H_

namespace uprotocol::uri {

/**
 * Interface used to provide hints as to how the Uri part or Uri itself is formatted meaning does it contain
 * long format, micro format or both (resolved) information.
 */
class UriFormat {
public:
    virtual ~UriFormat() = default;

    /**
     * Supporting empty Uri parts enables avoiding null values in the data model, and can indicate the absence of a Uri Part.
     * @return Returns true if the Uri part is empty.
     */
    virtual bool isEmpty() const = 0;

    /**
     * Returns true if the Uri part contains both names and ids (numbers) corresponding to the data inside its belly.
     * isResolved true means that the Uri part can be serialized to both long and micro uri format.
     * @return Returns true if the Uri part contains both names and ids (numbers), long and micro representations.
     */
    virtual bool isResolved() const = 0;

    /**
     * Returns true if the Uri part contains names so that it can be serialized to long format.
     * @return Returns true if the Uri part contains names so that it can be serialized to long format.
     */
    virtual bool isLongForm() const = 0;

    /**
     * Returns true if the Uri part contains the id's which will allow the Uri part to be serialized into micro form.
     * @return Returns true if the Uri part can be serialized into micro form.
     */
    virtual bool isMicroForm() const = 0;

}; // class UriFormat

} // namespace uprotocol::uri

#endif // _URI_FORMAT_H_
