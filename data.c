#include <data.h>
#include <stdio.h>
#include <stdlib.h>

int convertCharToNumber(char ch) {
    if (ch >= '0' && ch <= '9') {
        return ch - '0';
    } else if (ch >= 'A' && ch <= 'F') {
        return ch - 'A' + 10;
    } else {
        return -1;
    }
}

char convertNumberToChar(int n) {
    if (n >= 0 && n <= 9) {
        return (char)(n + '0');
    } else if (n >= 10 && n <= 15) {
        return (char)(n - 10 + 'A');
    } else {
        return 0;
    }
}

static int data_to_int(const Data src) {
    unsigned int uval = 0;
    DataNode* node = src.data;
    
    while (node != NULL) {
        int digit = convertCharToNumber(node->number);
        uval = uval * src.base + (unsigned int)digit;
        node = node->next;
    }
    
    if (src.number_bits < 32) {
        unsigned int mask = ((unsigned int)1 << src.number_bits) - 1;
        uval &= mask;
    }
    
    if (src.sign == 1) {
        unsigned int sign_bit = (unsigned int)1 << (src.number_bits - 1);
        if ((uval & sign_bit) != 0) {
            unsigned int extension_mask = ~(((unsigned int)1 << src.number_bits) - 1);
            return (int)(uval | extension_mask);
        }
    }
    
    return (int)uval;
}

Data convert_int_to_data(int number, unsigned char base, unsigned char number_bits) {
    Data new_data;
    new_data.base = base;
    new_data.sign = 1;
    new_data.number_bits = number_bits;
    new_data.len = 0;
    new_data.data = NULL;
    
    unsigned int mask = 0xFFFFFFFFU;
    if (number_bits < 32) {
        mask = ((unsigned int)1 << number_bits) - 1;
    }
    
    unsigned int uval = (unsigned int)number & mask;
    
    if (uval == 0) {
        DataNode* zero_node = (DataNode*)malloc(sizeof(DataNode));
        zero_node->number = '0';
        zero_node->next = NULL;
        new_data.data = zero_node;
        new_data.len = 1;
        return new_data;
    }
    
    unsigned int temp = uval;
    char digits[64];
    int count = 0;
    
    while (temp != 0) {
        int digit_val = (int)(temp % base);
        temp /= base;
        digits[count++] = convertNumberToChar(digit_val);
    }
    
    DataNode* head = NULL;
    DataNode* tail = NULL;
    
    for (int i = count - 1; i >= 0; i--) {
        DataNode* new_node = (DataNode*)malloc(sizeof(DataNode));
        new_node->number = digits[i];
        new_node->next = NULL;
        
        if (head == NULL) {
            head = new_node;
            tail = new_node;
        } else {
            tail->next = new_node;
            tail = new_node;
        }
    }
    
    new_data.data = head;
    new_data.len = (unsigned char)count;
    
    return new_data;
}

Data convert_to_base_n(Data src, unsigned char n) {
    int value = data_to_int(src);
    Data new_data = convert_int_to_data(value, n, src.number_bits);
    new_data.sign = src.sign;
    
    return new_data;
}

Data left_shift(Data src, int n) {
    Data new_data;
    
    int value = data_to_int(src);
    
    unsigned int mask = 0xFFFFFFFFU;
    if (src.number_bits < 32) {
        mask = ((1U << src.number_bits) - 1U);
    }
    
    unsigned int uval = (unsigned int)value & mask;
    uval <<= n;
    uval &= mask;
    
    new_data = convert_int_to_data((int)uval, 2, src.number_bits);
    new_data.sign = src.sign;
    
    return new_data;
}

Data right_shift(Data src, int n) {
    Data new_data;
    
    int value = data_to_int(src);
    
    unsigned int mask = 0xFFFFFFFFU;
    if (src.number_bits < 32) {
        mask = ((1U << src.number_bits) - 1U);
    }
    
    int result;
    
    if (src.sign == 0) {
        unsigned int uval = (unsigned int)value & mask;
        uval >>= n;
        result = (int)uval;
    } else {
        result = value >> n;
        result &= (int)mask;
    }
    
    new_data = convert_int_to_data(result, 2, src.number_bits);
    new_data.sign = src.sign;
    
    return new_data;
}
