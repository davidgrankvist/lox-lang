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
            new ClassPayload(ClassPayloadType.Expression, "Logical", ["Expr Left", "Token Operator", "Expr Right"]),
            new ClassPayload(ClassPayloadType.Expression, "Call", ["Expr Callee", "Token LeftParen", "List<Expr> Arguments"]),
            // statements
            new ClassPayload(ClassPayloadType.Statement, "Expression", ["Expr Expression"]),
            new ClassPayload(ClassPayloadType.Statement, "Program", ["List<Stmt> Statements"]),
            new ClassPayload(ClassPayloadType.Statement, "Print", ["Expr Expression"]),
            new ClassPayload(ClassPayloadType.Statement, "Declaration", ["Token Identifier", "Expr Expression"]),
            new ClassPayload(ClassPayloadType.Statement, "Block", ["List<Stmt> Statements"]),
            new ClassPayload(ClassPayloadType.Statement, "If", ["Expr Condtition", "Stmt IfSt", "Stmt ElseSt"]),
            new ClassPayload(ClassPayloadType.Statement, "While", ["Expr Condtition", "Stmt Body"]),
            new ClassPayload(ClassPayloadType.Statement, "FunDeclaration", ["Token Identifier", "List<Token> Parameters", "List<Stmt> Body"]),
            new ClassPayload(ClassPayloadType.Statement, "Return", ["Token KeywordToken", "Expr Expression"]),
        ];
}
