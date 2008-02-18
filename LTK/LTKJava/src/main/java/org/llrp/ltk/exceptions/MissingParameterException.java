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
 * See the License for the specific language governing permissions
 * and limitations under the License.
 */
package org.llrp.ltk.exceptions;


/**
 * IllegalBitList Exception is thrown whenever a LLRPBitList is not in
 * an expected format - this mainly happens during decoding.
 *
 * @author Basil Gasser - ETH Zurich
 */
public class MissingParameterException extends RuntimeException {
    /**
         * Creates a new MissingParameterException object.
         *
         * @param message with information
         */
    public MissingParameterException(final String message) {
        super(message);
    }
}
