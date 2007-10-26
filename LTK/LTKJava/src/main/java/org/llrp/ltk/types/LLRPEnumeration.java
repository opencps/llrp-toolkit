package org.llrp.ltk.types;


/**
 * LLRPEnumeration for parameters that allow only
 * certain values.
 *
 */
public interface LLRPEnumeration {
    /**
     * check if a value is allowed for an enumeration.
    * @param value  to set
     * @return boolean
    */
    public boolean isValidValue(final Integer value);

    /**
     * get value of a name representing a value
     * if no value can be found, return -1.
     * @param name for value
     * @return int
     */
    public int getValue(final String name);

    /**
     * get name of a value
     * if no name can be found for given value
     * return empty string.
     * @param value of name
     * @return String
     */
    public String getName(int value);

    /**
             * check if the name stands for an allowed value
            * of this enumeration.
            * @param name to check
             * @return boolean
     */
    public boolean isValidName(final String name);
}
