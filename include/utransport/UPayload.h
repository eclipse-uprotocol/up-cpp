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
 */

#ifndef _UPAYLOAD_H_
#define _UPAYLOAD_H_

/**
 * The UPayload contains the clean Payload information at its raw serialized structure of a byte[]
 */

namespace uprotocol 
{
    namespace Utransport
    {
public class UPayload {

    private static final UPayload EMPTY = new UPayload(new byte[0]);

    private final byte[] data;


    /**
     * Create a UPayload.
     * @param data A byte array of the actual data.
     * @param representationHint Hint regarding what the bytes represent such as simple binary, a String or maybe a special type of String.
     */
    public UPayload(byte[] data) {
        this.data = data;
    }

    /**
     * The actual serialized or raw data, which can be deserialized or simply used as is using the hint.
     * @return Returns the actual serialized or raw data, which can be deserialized or simply used as is using the hint.
     */
    public byte[] data() {
        return this.data;
    }

    
    /**
     * @return Returns an empty representation of UPayload.
     */
    public static UPayload empty() {
        return EMPTY;
    }

    /**
     * @return Returns true if the data in the UPayload is empty.
     */
    public boolean isEmpty() {
        return this.data == null || this.data.length == 0;
    }

    
    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        UPayload uPayload = (UPayload) o;
        return Arrays.equals(data, uPayload.data);
    }

    @Override
    public int hashCode() {
        int result = Arrays.hashCode(data);
        return result;
    }

    @Override
    public String toString() {
        return "UPayload{" +
                "data=" + Arrays.toString(data) +
                '}';
    }
}

#endif /* _UPAYLOAD_H_ */