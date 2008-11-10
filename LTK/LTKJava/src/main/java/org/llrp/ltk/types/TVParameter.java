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


/**
 * TV Parameter do not encode length as the length is implicitly given by the
 * type TV parameter have type values from 0 to 127 TV Parameters encode the
 * length of a parameter when encoded. The binary encoding Is always: Reserved
 * (Bit set to 1) Parameter Type (7 Bits) | Parameter Value
 *
 * @author gasserb
 */
public abstract class TVParameter extends LLRPParameter {
    protected static final int PARAMETERTYPELENGTH = 8;

    /**
     * decodeBinary should be called from Constructor Taking binary encoded
     * parameter as argument.
     *
     * @param bits to be decoded
     *
     * @throws LLRPException
     *             in case of any error or unexpected behaviour
     *
     */
    public void decodeBinary(LLRPBitList bits) {
        // 7 bits only for type!!
        // very first bit is always set to 1
        SignedShort tN = new SignedShort(bits.subList(1, 7));

        if (!tN.equals(getTypeNum())) {
            // LLRPMessage.logger.error("incorrect type. Expected
            // "+getTypeNum().toShort()+" message indicates "+tN.toShort());
            throw new IllegalArgumentException("incorrect type. Expected " +
                getTypeNum().toShort() + " message indicates " + tN.toShort());
        }

        //decodeBinarySpecific is called for parameter specific decoding. Each parameter must have implemented it
        decodeBinarySpecific(bits.subList(PARAMETERTYPELENGTH,
                bits.length() - PARAMETERTYPELENGTH));
    }

    /**
     * encode parameter
     *
     * @return LLRPBitList
     *
     */
    public LLRPBitList encodeBinary() {
        LLRPBitList le = getTypeNum().encodeBinary();

        // type Number is saved as a short, but we need only the last 8 bits
        LLRPBitList result = le.subList(le.length() - PARAMETERTYPELENGTH,
                PARAMETERTYPELENGTH);

        // first bit must always be set to 1
        result.set(0);
        // call parameter specific encoding method
        result.append(encodeBinarySpecific());

        return result;
    }

    /**
     * function to be implemented
     *
     * @param binary
     *            binary representation of this parameter
     */
    protected abstract void decodeBinarySpecific(LLRPBitList binary);

    /**
     * protected method to force subclasses to implement their specific encoding
     *
     * @return LLRPBitList
     */
    protected abstract LLRPBitList encodeBinarySpecific();
}
