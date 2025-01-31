#ifndef dev_h
#define dev_h

#include "ops.h"

void disas_ops(Ops* ops, const char* name);
int disas_op_at(Ops* ops, int pos);
void print_val(Val val);

#endif
