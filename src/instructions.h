#pragma once
#include <stdint.h>

/*  [11:8] opcode | [7:4] funct | [3:2] src_addr | [1:0] dst_addr */

/* Primary opcode field (4 bits) */
typedef enum Opcode {
    OP_MOV = 0,
    OP_CMP = 1,
    OP_ADD = 2,
    OP_SUB = 2,
    OP_LEA = 4,
    OP_CLR = 5,
    OP_NOT = 5,
    OP_INC = 5,
    OP_DEC = 5,
    OP_JMP = 9,
    OP_BNE = 9,
    OP_JSR = 9,
    OP_RED = 12,
    OP_PRN = 13,
    OP_RTS = 14,
    OP_STOP = 15
} opcode_t;

/* Function Type (4 bits) */
typedef enum FuncT {
    FUNCT_NONE = -1,

    FUNCT_ADD = 10,
    FUNCT_SUB = 11,

    FUNCT_CLR = 10,
    FUNCT_NOT = 11,
    FUNCT_INC = 12,
    FUNCT_DEC = 13,

    FUNCT_JMP = 10,
    FUNCT_BNE = 11,
    FUNCT_JSR = 12
} funct_t;

/* Addressing Type (2 bits) */
typedef enum AddressingType {
    ADDR_TYPE_NONE = -1,
    ADDR_TYPE_IMMEDIATE = 0,
    ADDR_TYPE_DIRECT = 1,
    ADDR_TYPE_RELATIVE = 2,
    ADDR_TYPE_REGISTER_DIRECT = 3
} addressing_type_t;


struct Instruction {
    opcode_t opcode;
    funct_t funct;
    uint8_t source_address; /* (2 bits) */
    uint8_t dest_address; /* (2 bits) */
} instruction_t;

#define STOP_INSTRUCTION "stop"
#define MACRO_END_INSTRUCTION "mcroend"
#define MACRO_INSTRUCTION "mcro"
