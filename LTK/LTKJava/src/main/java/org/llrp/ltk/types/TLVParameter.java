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

import org.llrp.ltk.exceptions.LLRPException;


/**
 *  * TLV parameter have type values from 128 to 1024
 *
 * @author gasserb
 */
public abstract class TLVParameter extends LLRPParameter {
    public static Logger logging = Logger.getLogger(TLVParameter.class);
    public final int reservedLength = 6;
    public final int typeNumberLength = 10;
    public BitList reserved = new BitList(reservedLength);

    /**
     * create parameter from bitlist
     *
     * @param bits
     *
     * @throws LLRPException in case of any error or unexpected behaviour
     */
    public final void decodeBinary(LLRPBitList binary) {
        SignedShort tN = new SignedShort(binary.subList(reservedLength,
                    typeNumberLength));

        if (!tN.equals(getTypeNum())) {
            //			LLRPMessage.logger.error("incorrect type. Expected "+getTypeNum().toShort()+" message indicates "+tN.toShort());
            throw new LLRPException("incorrect type. Expected " +
                getTypeNum().toShort() + " message indicates " + tN.toShort());
        }

        int byteLength = new UnsignedShort(binary.subList(reservedLength +
                    typeNumberLength, UnsignedShort.length())).toInteger();
        bitLength = new UnsignedShort(8 * byteLength);

        if (bitLength.toShort() != binary.length()) {
            //			LLRPMessage.logger.error("incorrect length. Expected "+bits.length()+" message indicates "+bitLength.toShort());
            throw new LLRPException("incorrect length");
        }

        int headLength = reservedLength + typeNumberLength +
            UnsignedShort.length();
        decodeBinarySpecific(binary.subList(headLength,
                binary.length() - headLength));
    }

    /**
     * create binary representation of this binary
     *
     * @return LLRPBitList
     *
     * @throws LLRPException
     */
    public final LLRPBitList encodeBinary() {
        LLRPBitList result = new LLRPBitList();
        result.append(reserved.encodeBinary());

        LLRPBitList le = getTypeNum().encodeBinary();
        result.append(le.subList(reservedLength, typeNumberLength));
        result.append(bitLength.encodeBinary());
        result.append(encodeBinarySpecific());
        finalizeEncode(result);

        return result;
    }

    /**
     * decoding function to be implemented by each parameter
     *
     * @param binary binary representation of the parameter
     */
    protected abstract void decodeBinarySpecific(LLRPBitList binary);

    /**
     * protected method to force subclasses to implement their specific encoding
     *
     * @return
     */
    protected abstract LLRPBitList encodeBinarySpecific();

    /**
     * finalize encoding by setting length of parameter
     * @param result
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
                result.set(reservedLength + typeNumberLength + i);
            } else {
                result.clear(reservedLength + typeNumberLength + i);
            }
        }
    }
}
