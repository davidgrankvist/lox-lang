#include "ops.h"
#include "dev.h"
#include "vm.h"

int main() {
    init_vm();

    Ops ops;
    init_ops(&ops);

    // 5.3 + 2 = 7.3
    int i_constant = append_const(&ops, 5.3);
    append_op(&ops, OP_CONST, 1);
    append_op(&ops, i_constant, 1);
    i_constant = append_const(&ops, 2);
    append_op(&ops, OP_CONST, 1);
    append_op(&ops, i_constant, 1);
    append_op(&ops, OP_ADD, 1);

    // 7.3 - 1 = 6.3
    i_constant = append_const(&ops, 1);
    append_op(&ops, OP_CONST, 1);
    append_op(&ops, i_constant, 1);
    append_op(&ops, OP_SUBTRACT, 1);

    // 6.3 / 2 = 3.15
    i_constant = append_const(&ops, 2);
    append_op(&ops, OP_CONST, 1);
    append_op(&ops, i_constant, 1);
    append_op(&ops, OP_DIVIDE, 1);

    // 3.15 * 3 = 9.45
    i_constant = append_const(&ops, 3);
    append_op(&ops, OP_CONST, 1);
    append_op(&ops, i_constant, 1);
    append_op(&ops, OP_MULTIPLY, 1);

    // -9.45
    append_op(&ops, OP_NEGATE, 1);

    append_op(&ops, OP_RETURN, 1);

    interpret(&ops);

    free_vm();
    free_ops(&ops);
    return 0;
}
