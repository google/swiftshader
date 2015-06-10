//TODO: copyrights

#ifndef __PROPERTYLIST_H__
#define __PROPERTYLIST_H__

/**
* \brief Assign a value of a given type to the return value
* \param type type of the argument
* \param _value value to assign
*/
#define SIMPLE_ASSIGN(type, _value) do {    \
    value_length = sizeof(type);            \
    type##_var = (type)_value;              \
    value = & type##_var;                   \
} while (0);

/**
* \brief Assign a string to the return value
* \param string the string to assign, as a constant
*/
#define STRING_ASSIGN(string) do {          \
    static const char str[] = string;       \
    value_length = sizeof(str);             \
    value = (void *)str;                    \
} while (0);

/**
* \brief Assign a memory buffer to the return value
* \note the buffer must remain valid after the end of the \c info() call
* \param size size of the buffer
* \param buf buffer (of type <tt>void *</tt> for instance)
*/
#define MEM_ASSIGN(size, buf) do {          \
    value_length = size;                    \
    value = (void *)buf;                    \
} while (0);

#endif
