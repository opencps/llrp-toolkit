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

import org.apache.log4j.Logger;


/**
 * TLVParameter have type values from 128 to 1024 TLVParameters encode the
 * length of a parameter when encoded. The binary encoding Is always: Reserved(6
 * Bits) | Parameter Type (10 Bits) | Parameter Length (16 Bits) | Parameters
 *
 * @author gasserb
 */
public abstract class TLVParameter extends LLRPParameter {
    private static Logger logging = Logger.getLogger(TLVParameter.class);
    protected final int RESERVEDLENGTH = 6;
    protected final int TYPENUMBERLENGTH = 10;
    private BitList reserved = new BitList(RESERVEDLENGTH);

    /**
     * decodeBinary should be called from Constructor Taking binary encoded
     * parameter as argument
     *
     * @param binary list to be decoded
     *
     * @throws IllegalArgumentException
     *             in case of any error or unexpected behaviour
     */
    public final void decodeBinary(LLRPBitList binary) {
        SignedShort tN = new SignedShort(binary.subList(RESERVEDLENGTH,
                    TYPENUMBERLENGTH));

        if (!tN.equals(getTypeNum())) {
            logging.error("incorrect type. Expected " + getTypeNum().toShort() +
                " message indicates " + tN.toShort());
            throw new IllegalArgumentException("incorrect type. Expected " +
                getTypeNum().toShort() + " message indicates " + tN.toShort());
        }

        int byteLength = new UnsignedShort(binary.subList(RESERVEDLENGTH +
                    TYPENUMBERLENGTH, UnsignedShort.length())).toInteger();
        bitLength = new UnsignedShort(8 * byteLength);

        if (bitLength.toShort() != binary.length()) {
            logging.error("incorrect length. Expected " + binary.length() +
                " message indicates " + bitLength.toShort());
            throw new IllegalArgumentException("incorrect length");
        }

        int headLength = RESERVEDLENGTH + TYPENUMBERLENGTH +
            UnsignedShort.length();
        // decodeBinarySpecific is called for parameter specific decoding. Each
        // parameter must have implemented it
        decodeBinarySpecific(binary.subList(headLength,
                binary.length() - headLength));
    }

    /**
     * create binary encoding of this parameter. Returns a LLRPBitList which can
     * concatenated with binary encoding of other parameters to form a binary
     * encoded messages
     *
     * @return LLRPBitList
     *
     */
    public final LLRPBitList encodeBinary() {
        //
        LLRPBitList result = new LLRPBitList();
        result.append(reserved.encodeBinary());

        // encode everything that each parameter has
        LLRPBitList le = getTypeNum().encodeBinary();
        result.append(le.subList(RESERVEDLENGTH, TYPENUMBERLENGTH));
        result.append(bitLength.encodeBinary());
        // call parameter specific encoding method
        result.append(encodeBinarySpecific());
        // call finalize to set length correctly
        finalizeEncode(result);

        return result;
    }

    /**
     * decoding function to be implemented by each parameter
     *
     * @param binary
     *            binary representation of the parameter
     */
    protected abstract void decodeBinarySpecific(LLRPBitList binary);

    /**
     * protected method to force subclasses to implement their specific encoding
     *
     * @return LLRPBitList
     */
    protected abstract LLRPBitList encodeBinarySpecific();

    /**
     * finalize encoding by setting length of parameter Must be called at very
     * end since length can not be known before message is completly encoded
     *
     * @param result to be finalized
     */
    private void finalizeEncode(LLRPBitList result) {
        int lengthBits = result.length();
        int lengthBytes = lengthBits / 8;

        if ((lengthBits % 8) > 0) {
            lengthBytes++;
        }

        LLRPBitList binLength = new UnsignedShort(lengthBytes).encodeBinary();

        for (int i = 0; i < UnsignedShort.length(); i++) {
            if (binLength.get(i)) {
                result.set(RESERVEDLENGTH + TYPENUMBERLENGTH + i);
            } else {
                result.clear(RESERVEDLENGTH + TYPENUMBERLENGTH + i);
            }
        }
    }
}
