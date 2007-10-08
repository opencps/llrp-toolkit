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

import org.llrp.ltk.exceptions.LLRPException;


/**
 * TV Parameter do not encode length as the length is implicitly given by the type
 * TV parameter have type values from 0 to 127
 *
 * @author gasserb
 */
public abstract class TVParameter extends LLRPParameter {
    private final int parameterTypeLength = 8;
    protected LLRPBitList bits;

    /**
     * create parameter from bitlist
     *
     * @param bits
     *
     * @throws LLRPException in case of any error or unexpected behaviour
     */
    public void decodeBinary(LLRPBitList bits) {
        this.bits = bits.clone();

        // 8 bits only for type!!
        SignedShort tN = new SignedShort(bits.subList(0, 8));

        if (!tN.equals(getTypeNum())) {
            //			LLRPMessage.logger.error("incorrect type. Expected "+getTypeNum().toShort()+" message indicates "+tN.toShort());
            throw new LLRPException("incorrect type. Expected " +
                getTypeNum().toShort() + " message indicates " + tN.toShort());
        }

        decodeBinarySpecific(bits.subList(parameterTypeLength,
                bits.length() - parameterTypeLength));
    }

    /**
     * encode parameter
     *
     * @return LLRPBitList
     *
     * @throws LLRPException
     */
    public LLRPBitList encodeBinary() {
        LLRPBitList result = new LLRPBitList();
        LLRPBitList le = getTypeNum().encodeBinary();
        // type Number is saved as a short, but we need only the last 8 bits
        result.append(le.subList(le.length() - parameterTypeLength,
                parameterTypeLength));
        result.append(encodeBinarySpecific());
        bits = result;

        return bits;
    }

    /**
     * function to be implemented
     *
     * @param binary binary representation of this parameter
     */
    protected abstract void decodeBinarySpecific(LLRPBitList binary);

    /**
     * protected method to force subclasses to implement their specific encoding
     *
     * @return
     */
    protected abstract LLRPBitList encodeBinarySpecific();
}
