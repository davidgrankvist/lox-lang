using CsLox.CodeGen.Generation;

namespace CsLox.CodeGen;
internal static class GrammarConstants
{
    public static readonly List<ClassPayload> Grammar = [
            // expressions
            new ClassPayload(ClassPayloadType.Expression, "Literal", ["object Value"]),
            new ClassPayload(ClassPayloadType.Expression, "Group", ["Expr Expression"]),
            new ClassPayload(ClassPayloadType.Expression, "Unary", ["Token Operator", "Expr Expression"]),
            new ClassPayload(ClassPayloadType.Expression, "Binary", ["Expr Left", "Token Operator", "Expr Right"]),
            new ClassPayload(ClassPayloadType.Expression, "Variable", ["Token Identifier"]),
            new ClassPayload(ClassPayloadType.Expression, "Assignment", ["Token Identifier", "Expr Expression"]),
            // statements
            new ClassPayload(ClassPayloadType.Statement, "Expression", ["Expr Expression"]),
            new ClassPayload(ClassPayloadType.Statement, "Program", ["List<Stmt> Statements"]),
            new ClassPayload(ClassPayloadType.Statement, "Print", ["Expr Expression"]),
            new ClassPayload(ClassPayloadType.Statement, "Declaration", ["Token Identifier", "Expr Expression"]),
        ];
}
