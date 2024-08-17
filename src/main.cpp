#include <cstdint>
#include <string>

const std::string reg_strs[]{"AL", "CL", "DL", "BL", "AH", "CH", "DH", "BH"};
const std::string reg_wstrs[]{"AX", "CX", "DX", "BX", "SP", "BP", "SI", "DI"};

//
// Lots of enums and structures that aren't needed, but I like.
//
enum op : uint8_t
{
    MOV,
    NONE
};

enum direction : uint8_t
{
    RM,
    REG
};

enum operation : uint8_t
{
    BYTE,
    WORD
};

enum mod : uint8_t
{
    MEM_MODE_NO_DISPLACEMENT = 0x0,
    MEM_MODE_8B_DISPACEMENT = 0x1,
    MEM_MODE_16B_DISPLACEMENT = 0x2,
    REGISTER_TO_REGISTER = 0x3
};

// unused for now
enum reg : uint8_t
{
    AL = 0x000,
    CL = 0x001,
    DL = 0x002, // 010
    BL = 0x003, // 011
    AH = 0x004, // 100
    CH = 0x005, // 101
    DH = 0x006, // 110
    BH = 0x007, // 111
};

// unused for now
enum regwide : uint8_t
{
    AX = 0x000,
    CX = 0x001,
    DX = 0x002,
    BX = 0x003,
    SP = 0x004,
    BP = 0x005,
    SI = 0x006,
    DI = 0x007
};

struct Instruction
{
    op op;
    direction direction;
    operation operation;
};

// Least creative name.
struct ModRegRM
{
    std::string reg_reg;
    std::string rm_reg;
    mod mod;
};

std::string bit_val_to_reg(uint8_t val, operation oper)
{
    if (oper == operation::BYTE)
        return reg_strs[val];
    return reg_wstrs[val];
}

Instruction read_isn_byte(uint8_t byte)
{
    op isn{};
    direction direction{};
    operation operation{};

    if (((byte >> 2) & 0x3F) == 0x22)
        isn = op::MOV;

    if (((byte & 0x02) >> 1) == 0)
        direction = direction::RM;
    else
        direction = direction::REG;

    if ((byte & 0x01) == 0)
        operation = operation::BYTE;
    else
        operation = operation::WORD;

    Instruction ret;
    ret.op = isn;
    ret.direction = direction;
    ret.operation = operation;
    return ret;
}

ModRegRM parse_second_byte(uint8_t byte, Instruction* isn)
{
    ModRegRM r;

    // MOD
    if (((byte & 0xC0) >> 6) == 0x3)
        r.mod = mod::REGISTER_TO_REGISTER;

    // REG
    // AL/AX
    if (((byte & 0x38) >> 3) == 0)
        r.reg_reg = bit_val_to_reg(0, isn->operation);
    // CL/CX
    if (((byte & 0x38) >> 3) == 1)
        r.reg_reg = bit_val_to_reg(1, isn->operation);
    // DL/DX
    if (((byte & 0x38) >> 3) == 2)
        r.reg_reg = bit_val_to_reg(2, isn->operation);
    // BL/BX
    if (((byte & 0x38) >> 3) == 3)
        r.reg_reg = bit_val_to_reg(3, isn->operation);
    // AH/SP
    if (((byte & 0x38) >> 3) == 4)
        r.reg_reg = bit_val_to_reg(4, isn->operation);
    // CH/BP
    if (((byte & 0x38) >> 3) == 5)
        r.reg_reg = bit_val_to_reg(5, isn->operation);
    // DH/SI
    if (((byte & 0x38) >> 3) == 6)
        r.reg_reg = bit_val_to_reg(6, isn->operation);
    // BH/DI
    if (((byte & 0x38) >> 3) == 7)
        r.reg_reg = bit_val_to_reg(7, isn->operation);

    // RM
    // AL/AX
    if ((byte & 0x7) == 0)
        r.rm_reg = bit_val_to_reg(0, isn->operation);
    // CL/CX
    if ((byte & 0x7) == 1)
        r.rm_reg = bit_val_to_reg(1, isn->operation);
    // DL/DX
    if ((byte & 0x7) == 2)
        r.rm_reg = bit_val_to_reg(2, isn->operation);
    // BL/BX
    if ((byte & 0x7) == 3)
        r.rm_reg = bit_val_to_reg(3, isn->operation);
    // AH/SP
    if ((byte & 0x7) == 4)
        r.rm_reg = bit_val_to_reg(4, isn->operation);
    // CH/BP
    if ((byte & 0x7) == 5)
        r.rm_reg = bit_val_to_reg(5, isn->operation);
    // DH/SI
    if ((byte & 0x7) == 6)
        r.rm_reg = bit_val_to_reg(6, isn->operation);
    // BH/DI
    if ((byte & 0x7) == 7)
        r.rm_reg = bit_val_to_reg(7, isn->operation);

    return r;
}

int main()
{
    FILE* input_bin;
    long sz_bin;
    char bin_bytes[4096];

    input_bin = fopen("../../input/38_many_mov", "r");
    /*input_bin = fopen("../../input/37_single_mov", "r");*/
    fseek(input_bin, 0L, SEEK_END);
    sz_bin = ftell(input_bin);
    rewind(input_bin);

    if (input_bin == NULL)
    {
        printf("Can't open file. Exiting.\n");
        exit(0);
    }

    fgets(bin_bytes, sz_bin, input_bin);

    for (int i = 0; i < sz_bin; i += 2)
    {
        Instruction isn = read_isn_byte((uint8_t)bin_bytes[i]);
        ModRegRM mrrm = parse_second_byte((uint8_t)bin_bytes[i + 1], &isn);

        printf("MOV ");
        if (isn.direction == direction::RM)
            printf("%s,%s", mrrm.rm_reg.c_str(), mrrm.reg_reg.c_str());
        else
            printf("%s,%s", mrrm.reg_reg.c_str(), mrrm.rm_reg.c_str());
        printf("\n");
    }
}
