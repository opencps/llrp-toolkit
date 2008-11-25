/*
 * Copyright 2007 ETH Zurich
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 *
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and limitations under the License.
 */
package org.llrp.ltk.types;

import org.jdom.Element;


/**
 * Enumerations that can hold 4 values (2 bits) at most
 *
 * @author Basil Gasser - ETH Zurich
 */
public class TwoBitEnumeration extends SignedByte {
    private static final int LENGTH = 2;

    /**
         * Creates a new TwoBitEnumeration object.
         */
    public TwoBitEnumeration() {
        super(0);
    }

    /**
     * Creates a new TwoBitEnumeration object from jdom element - used for xml decoding
     *
     * @param element to be decoded
     */
    public TwoBitEnumeration(Element element) {
        decodeXML(element);
    }

    /**
         * Creates a new TwoBitEnumeration object.
         *
         * @param value to set
         */
    public TwoBitEnumeration(int value) {
        super(value);
    }

    /**
     * overwritte encode
     *
     * @return LLRPBitList of length 2
     */
    public LLRPBitList encodeBinary() {
        LLRPBitList result = new LLRPBitList(Integer.toBinaryString(value));

        if (result.length() < LENGTH) {
            result.pad(LENGTH - result.length());
        }

        return result.subList(result.length() - LENGTH, LENGTH);
    }

    /**
     * number of bits to represent this type
     *
     * @return Integer
     */
    public static int length() {
        return LENGTH;
    }
}
