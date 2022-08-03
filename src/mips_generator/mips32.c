#include "mips32.h"

struct RegDesc
{ // the register descriptor
    const char *name;
    char var[8];
} regs[NUM_REGS];

struct VarDesc
{ // the variable descriptor
    char var[8];
    Register reg;
    int offset; // the offset from stack
    VarDesc *next;
} * vars;

/* the output file descriptor, may not be explicitly used */
FILE *fd;

#define _tac_kind(tac) (((tac)->code).kind)
#define _tac_quadruple(tac) (((tac)->code).tac)
#define _reg_name(reg) regs[reg].name

int arg_offset = 0;
int param_offset = 0;

int local_offset;
int reg_flag = 0;

void _mips_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfprintf(fd, fmt, args);
    va_end(args);
    fputs("\n", fd);
}

void _mips_iprintf(const char *fmt, ...)
{
    va_list args;
    fputs("  ", fd); // `iprintf` stands for indented printf
    va_start(args, fmt);
    vfprintf(fd, fmt, args);
    va_end(args);
    fputs("\n", fd);
}

void init_new_stackframe()
{
    local_offset = -8;
    vars->next = NULL;
}

VarDesc *get_var(const char *var)
{
    VarDesc *cur = vars;
    while (cur != NULL)
    {
        if (!strcmp(cur->var, var))
            return cur;
        cur = cur->next;
    }
    return NULL;
}

/**
 * @brief binds a param to its location in the memory
 *
 * @param param the name of the parameter
 * @param offset the offset from the address of $fp (should be positive or 0)
 */
void bind_param(const char *param, int offset)
{
    VarDesc *var_desc = (VarDesc *)malloc(sizeof(VarDesc));
    strcpy(var_desc->var, param);
    var_desc->next = vars->next;
    vars->next = var_desc;
    var_desc->offset = offset;
}

Register get_register(tac_opd *opd)
{
    assert(opd->kind == OP_VARIABLE);
    char *var = opd->char_val;
    VarDesc *var_desc = get_var(var);
    Register reg = reg_flag ? t1 : t0;
    reg_flag = 1 - reg_flag;
    _mips_iprintf("lw %s, %d($fp)", _reg_name(reg), var_desc->offset);
    return reg;
}

VarDesc *new_local_var(const char *var)
{
    VarDesc *var_desc = (VarDesc *)malloc(sizeof(VarDesc));
    strcpy(var_desc->var, var);
    var_desc->next = vars->next;
    vars->next = var_desc;
    local_offset -= 4;
    _mips_iprintf("addi $sp, $fp, %d", local_offset);
    var_desc->offset = local_offset;
}

Register get_register_w(tac_opd *opd)
{
    assert(opd->kind == OP_VARIABLE);
    char *var = opd->char_val;
    VarDesc *var_desc = get_var(var);
    if (var_desc == NULL)
        var_desc = new_local_var(var);
    strcpy(regs[s0].var, var);
    return s0;
}

void spill_register(Register reg)
{
    VarDesc *var_desc = get_var(regs[reg].var);
    if (var_desc == NULL)
        return;
    _mips_iprintf("sw %s, %d($fp)", _reg_name(reg), var_desc->offset);
}

/* PARAM: a pointer to `struct tac_node` instance
   RETURN: the next instruction to be translated */
tac *emit_label(tac *label)
{
    assert(_tac_kind(label) == LABEL_TAC);
    _mips_printf("label%d:", _tac_quadruple(label).labelno->int_val);
    return label->next;
}

tac *emit_function(tac *function)
{
    _mips_printf("%s:", _tac_quadruple(function).funcname);
    _mips_iprintf("sw $ra, -4($sp)");
    _mips_iprintf("sw $fp, -8($sp)");
    _mips_iprintf("move $fp, $sp");
    _mips_iprintf("subu $sp, $sp, 8");
    param_offset = 0;
    init_new_stackframe();
    return function->next;
}

tac *emit_assign(tac *assign)
{
    Register x, y;

    x = get_register_w(_tac_quadruple(assign).left);
    if (_tac_quadruple(assign).right->kind == OP_CONSTANT)
    {
        _mips_iprintf("li %s, %d", _reg_name(x),
                      _tac_quadruple(assign).right->int_val);
    }
    else
    {
        y = get_register(_tac_quadruple(assign).right);
        _mips_iprintf("move %s, %s", _reg_name(x), _reg_name(y));
    }
    spill_register(s0);
    return assign->next;
}

tac *emit_add(tac *add)
{
    Register x, y, z;

    x = get_register_w(_tac_quadruple(add).left);
    if (_tac_quadruple(add).r1->kind == OP_CONSTANT)
    {
        y = get_register(_tac_quadruple(add).r2);
        _mips_iprintf("addi %s, %s, %d", _reg_name(x),
                      _reg_name(y),
                      _tac_quadruple(add).r1->int_val);
    }
    else if (_tac_quadruple(add).r2->kind == OP_CONSTANT)
    {
        y = get_register(_tac_quadruple(add).r1);
        _mips_iprintf("addi %s, %s, %d", _reg_name(x),
                      _reg_name(y),
                      _tac_quadruple(add).r2->int_val);
    }
    else
    {
        y = get_register(_tac_quadruple(add).r1);
        z = get_register(_tac_quadruple(add).r2);
        _mips_iprintf("add %s, %s, %s", _reg_name(x),
                      _reg_name(y),
                      _reg_name(z));
    }
    spill_register(s0);
    return add->next;
}

tac *emit_sub(tac *sub)
{
    Register x, y, z;

    x = get_register_w(_tac_quadruple(sub).left);
    if (_tac_quadruple(sub).r1->kind == OP_CONSTANT)
    {
        y = get_register(_tac_quadruple(sub).r2);
        _mips_iprintf("neg %s, %s", _reg_name(y), _reg_name(y));
        _mips_iprintf("addi %s, %s, %d", _reg_name(x),
                      _reg_name(y),
                      _tac_quadruple(sub).r1->int_val);
    }
    else if (_tac_quadruple(sub).r2->kind == OP_CONSTANT)
    {
        y = get_register(_tac_quadruple(sub).r1);
        _mips_iprintf("addi %s, %s, -%d", _reg_name(x),
                      _reg_name(y),
                      _tac_quadruple(sub).r2->int_val);
    }
    else
    {
        y = get_register(_tac_quadruple(sub).r1);
        z = get_register(_tac_quadruple(sub).r2);
        _mips_iprintf("sub %s, %s, %s", _reg_name(x),
                      _reg_name(y),
                      _reg_name(z));
    }
    spill_register(s0);
    return sub->next;
}

tac *emit_mul(tac *mul)
{
    Register x, y, z;

    x = get_register_w(_tac_quadruple(mul).left);
    if (_tac_quadruple(mul).r1->kind == OP_CONSTANT)
    {
        y = get_register_w(_tac_quadruple(mul).r1);
        z = get_register(_tac_quadruple(mul).r2);
        _mips_iprintf("lw %s, %d", _reg_name(y),
                      _tac_quadruple(mul).r1->int_val);
    }
    else if (_tac_quadruple(mul).r2->kind == OP_CONSTANT)
    {
        y = get_register(_tac_quadruple(mul).r1);
        z = get_register_w(_tac_quadruple(mul).r2);
        _mips_iprintf("lw %s, %d", _reg_name(z),
                      _tac_quadruple(mul).r2->int_val);
    }
    else
    {
        y = get_register(_tac_quadruple(mul).r1);
        z = get_register(_tac_quadruple(mul).r2);
    }
    _mips_iprintf("mul %s, %s, %s", _reg_name(x),
                  _reg_name(y),
                  _reg_name(z));
    spill_register(s0);
    return mul->next;
}

tac *emit_div(tac *div)
{
    Register x, y, z;

    x = get_register_w(_tac_quadruple(div).left);
    if (_tac_quadruple(div).r1->kind == OP_CONSTANT)
    {
        y = get_register_w(_tac_quadruple(div).r1);
        z = get_register(_tac_quadruple(div).r2);
        _mips_iprintf("lw %s, %d", _reg_name(y),
                      _tac_quadruple(div).r1->int_val);
    }
    else if (_tac_quadruple(div).r2->kind == OP_CONSTANT)
    {
        y = get_register(_tac_quadruple(div).r1);
        z = get_register_w(_tac_quadruple(div).r2);
        _mips_iprintf("lw %s, %d", _reg_name(z),
                      _tac_quadruple(div).r2->int_val);
    }
    else
    {
        y = get_register(_tac_quadruple(div).r1);
        z = get_register(_tac_quadruple(div).r2);
    }
    _mips_iprintf("div %s, %s", _reg_name(y), _reg_name(z));
    _mips_iprintf("mflo %s", _reg_name(x));
    spill_register(s0);
    return div->next;
}

tac *emit_addr(tac *addr)
{
    Register x, y;

    x = get_register_w(_tac_quadruple(addr).left);
    y = get_register(_tac_quadruple(addr).right);
    _mips_iprintf("move %s, %s", _reg_name(x), _reg_name(y));
    spill_register(s0);
    return addr->next;
}

tac *emit_fetch(tac *fetch)
{
    Register x, y;

    x = get_register_w(_tac_quadruple(fetch).left);
    y = get_register(_tac_quadruple(fetch).raddr);
    _mips_iprintf("lw %s, 0(%s)", _reg_name(x), _reg_name(y));
    spill_register(s0);
    return fetch->next;
}

tac *emit_deref(tac *deref)
{
    Register x, y;

    x = get_register(_tac_quadruple(deref).laddr);
    y = get_register(_tac_quadruple(deref).right);
    _mips_iprintf("sw %s, 0(%s)", _reg_name(y), _reg_name(x));
    return deref->next;
}

tac *emit_goto(tac *goto_)
{
    _mips_iprintf("j label%d", _tac_quadruple(goto_).labelno->int_val);
    return goto_->next;
}

Register get_if_reg(tac_opd *if_opd)
{
    assert(if_opd->kind == OP_VARIABLE || if_opd->kind == OP_CONSTANT);
    Register reg;
    if (if_opd->kind == OP_CONSTANT)
    {
        reg = t4;
        _mips_iprintf("li %s, %d", _reg_name(reg), if_opd->int_val);
    }
    else
    {
        reg = get_register(if_opd);
    }
    return reg;
}

tac *emit_iflt(tac *iflt)
{
    Register x, y;
    x = get_if_reg(_tac_quadruple(iflt).c1);
    y = get_if_reg(_tac_quadruple(iflt).c2);
    _mips_iprintf("blt %s, %s, label%d", _reg_name(x), _reg_name(y), _tac_quadruple(iflt).labelno->int_val);
    return iflt->next;
}

tac *emit_ifle(tac *ifle)
{
    Register x, y;
    x = get_if_reg(_tac_quadruple(ifle).c1);
    y = get_if_reg(_tac_quadruple(ifle).c2);
    _mips_iprintf("ble %s, %s, label%d", _reg_name(x), _reg_name(y), _tac_quadruple(ifle).labelno->int_val);
    return ifle->next;
}

tac *emit_ifgt(tac *ifgt)
{
    Register x, y;
    x = get_if_reg(_tac_quadruple(ifgt).c1);
    y = get_if_reg(_tac_quadruple(ifgt).c2);
    _mips_iprintf("bgt %s, %s, label%d", _reg_name(x), _reg_name(y), _tac_quadruple(ifgt).labelno->int_val);
    return ifgt->next;
}

tac *emit_ifge(tac *ifge)
{
    Register x, y;
    x = get_if_reg(_tac_quadruple(ifge).c1);
    y = get_if_reg(_tac_quadruple(ifge).c2);
    _mips_iprintf("bge %s, %s, label%d", _reg_name(x), _reg_name(y), _tac_quadruple(ifge).labelno->int_val);
    return ifge->next;
}

tac *emit_ifne(tac *ifne)
{
    Register x, y;
    x = get_if_reg(_tac_quadruple(ifne).c1);
    y = get_if_reg(_tac_quadruple(ifne).c2);
    _mips_iprintf("bne %s, %s, label%d", _reg_name(x), _reg_name(y), _tac_quadruple(ifne).labelno->int_val);
    return ifne->next;
}

tac *emit_ifeq(tac *ifeq)
{
    Register x, y;
    x = get_if_reg(_tac_quadruple(ifeq).c1);
    y = get_if_reg(_tac_quadruple(ifeq).c2);
    _mips_iprintf("beq %s, %s, label%d", _reg_name(x), _reg_name(y), _tac_quadruple(ifeq).labelno->int_val);
    return ifeq->next;
}

tac *emit_return(tac *return_)
{
    if (_tac_quadruple(return_).var->kind == OP_VARIABLE){
        Register ret = get_register(_tac_quadruple(return_).var);
        _mips_iprintf("move $v0, %s", _reg_name(ret));
    }
    else{
        _mips_iprintf("li $v0, %d", _tac_quadruple(return_).var->int_val);
    }
    _mips_iprintf("addi $sp, $fp, -8");
    _mips_iprintf("lw $ra, -4($fp)");
    _mips_iprintf("lw $fp, -8($fp)");
    _mips_iprintf("jr $ra");
    return return_->next;
}

tac *emit_dec(tac *dec)
{
    /* NO NEED TO IMPLEMENT */
    return dec->next;
}

tac *emit_arg(tac *arg)
{
    arg_offset -= 4;
    Register x = get_register(_tac_quadruple(arg).var);
    _mips_iprintf("sw %s, %d($sp)", _reg_name(x), arg_offset);
    return arg->next;
}

tac *emit_call(tac *call)
{
    _mips_iprintf("subu $sp, $sp, %d", -arg_offset);
    _mips_iprintf("jal %s", _tac_quadruple(call).funcname);
    _mips_iprintf("addi $sp, $sp, %d", -arg_offset);

    Register ret = get_register_w(_tac_quadruple(call).ret);
    _mips_iprintf("move %s, $v0", _reg_name(ret));

    spill_register(s0);
    arg_offset = 0;
    return call->next;
}

tac *emit_param(tac *param)
{
    bind_param(_tac_quadruple(param).p->char_val, param_offset);
    param_offset += 4;
    return param->next;
}

tac *emit_read(tac *read)
{
    Register x = get_register_w(_tac_quadruple(read).p);

    _mips_iprintf("addi $sp, $sp, -4");
    _mips_iprintf("sw $ra, 0($sp)");
    _mips_iprintf("jal read");
    _mips_iprintf("lw $ra, 0($sp)");
    _mips_iprintf("addi $sp, $sp, 4");
    _mips_iprintf("move %s, $v0", _reg_name(x));
    spill_register(s0);
    return read->next;
}

tac *emit_write(tac *write)
{
    Register x = get_register(_tac_quadruple(write).p);

    _mips_iprintf("move $a0, %s", _reg_name(x));
    _mips_iprintf("addi $sp, $sp, -4");
    _mips_iprintf("sw $ra, 0($sp)");
    _mips_iprintf("jal write");
    _mips_iprintf("lw $ra, 0($sp)");
    _mips_iprintf("addi $sp, $sp, 4");
    return write->next;
}

void emit_preamble()
{
    _mips_printf("# SPL compiler generated assembly");
    _mips_printf(".data");
    _mips_printf("_prmpt: .asciiz \"Enter an integer: \"");
    _mips_printf("_eol: .asciiz \"\\n\"");
    _mips_printf(".globl main");
    _mips_printf(".text");
}

void emit_read_function()
{
    _mips_printf("read:");
    _mips_iprintf("li $v0, 4");
    _mips_iprintf("la $a0, _prmpt");
    _mips_iprintf("syscall");
    _mips_iprintf("li $v0, 5");
    _mips_iprintf("syscall");
    _mips_iprintf("jr $ra");
}

void emit_write_function()
{
    _mips_printf("write:");
    _mips_iprintf("li $v0, 1");
    _mips_iprintf("syscall");
    _mips_iprintf("li $v0, 4");
    _mips_iprintf("la $a0, _eol");
    _mips_iprintf("syscall");
    _mips_iprintf("move $v0, $0");
    _mips_iprintf("jr $ra");
}

static tac *(*emitter[])(tac *) = {
    emit_label, emit_function, emit_assign,
    emit_add, emit_sub, emit_mul, emit_div,
    emit_addr, emit_fetch, emit_deref, emit_goto,
    emit_iflt, emit_ifle, emit_ifgt, emit_ifge, emit_ifne, emit_ifeq,
    emit_return, emit_dec, emit_arg, emit_call, emit_param,
    emit_read, emit_write};

tac *emit_code(tac *head)
{
    tac *(*tac_emitter)(tac *);
    tac *tac_code = head;
    emit_preamble();
    emit_read_function();
    emit_write_function();
    while (tac_code != NULL)
    {
        if (_tac_kind(tac_code) != NONE_TAC)
        {
            tac_emitter = emitter[_tac_kind(tac_code)];
            tac_code = tac_emitter(tac_code);
        }
        else
        {
            tac_code = tac_code->next;
        }
    }
}

/* translate a TAC list into mips32 assembly
   output the textual assembly code to _fd */
void mips32_gen(tac *head, FILE *_fd)
{
    regs[zero].name = "$zero";
    regs[at].name = "$at";
    regs[v0].name = "$v0";
    regs[v1].name = "$v1";
    regs[a0].name = "$a0";
    regs[a1].name = "$a1";
    regs[a2].name = "$a2";
    regs[a3].name = "$a3";
    regs[t0].name = "$t0";
    regs[t1].name = "$t1";
    regs[t2].name = "$t2";
    regs[t3].name = "$t3";
    regs[t4].name = "$t4";
    regs[t5].name = "$t5";
    regs[t6].name = "$t6";
    regs[t7].name = "$t7";
    regs[s0].name = "$s0";
    regs[s1].name = "$s1";
    regs[s2].name = "$s2";
    regs[s3].name = "$s3";
    regs[s4].name = "$s4";
    regs[s5].name = "$s5";
    regs[s6].name = "$s6";
    regs[s7].name = "$s7";
    regs[t8].name = "$t8";
    regs[t9].name = "$t9";
    regs[k0].name = "$k0";
    regs[k1].name = "$k1";
    regs[gp].name = "$gp";
    regs[sp].name = "$sp";
    regs[fp].name = "$fp";
    regs[ra].name = "$ra";
    vars = (struct VarDesc *)malloc(sizeof(struct VarDesc));
    vars->next = NULL;
    fd = _fd;
    emit_code(head);
}
