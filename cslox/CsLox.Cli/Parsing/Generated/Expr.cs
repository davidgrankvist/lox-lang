// DO NOT EDIT - THIS CLASS IS AUTO-GENERATED
// See the CsLox.CodeGen project

using CsLox.Cli.Scanning;

namespace CsLox.Cli.Parsing.Generated;

public abstract class Expr
{
    public interface IVisitor<TResult>
    {
        TResult VisitLiteralExpr(LiteralExpr expr);
        TResult VisitGroupExpr(GroupExpr expr);
        TResult VisitUnaryExpr(UnaryExpr expr);
        TResult VisitBinaryExpr(BinaryExpr expr);
        TResult VisitVariableExpr(VariableExpr expr);
        TResult VisitAssignmentExpr(AssignmentExpr expr);
        TResult VisitLogicalExpr(LogicalExpr expr);
        TResult VisitCallExpr(CallExpr expr);
    }

    public abstract TResult Accept<TResult>(IVisitor<TResult> visitor);

    public class LiteralExpr : Expr
    {
        public LiteralExpr(object Value)
        {
            this.Value = Value;
        }

        public readonly object Value;

        public override TResult Accept<TResult>(IVisitor<TResult> visitor)
        {
            return visitor.VisitLiteralExpr(this);
        }
    }
    public class GroupExpr : Expr
    {
        public GroupExpr(Expr Expression)
        {
            this.Expression = Expression;
        }

        public readonly Expr Expression;

        public override TResult Accept<TResult>(IVisitor<TResult> visitor)
        {
            return visitor.VisitGroupExpr(this);
        }
    }
    public class UnaryExpr : Expr
    {
        public UnaryExpr(Token Operator, Expr Expression)
        {
            this.Operator = Operator;
            this.Expression = Expression;
        }

        public readonly Token Operator;
        public readonly Expr Expression;

        public override TResult Accept<TResult>(IVisitor<TResult> visitor)
        {
            return visitor.VisitUnaryExpr(this);
        }
    }
    public class BinaryExpr : Expr
    {
        public BinaryExpr(Expr Left, Token Operator, Expr Right)
        {
            this.Left = Left;
            this.Operator = Operator;
            this.Right = Right;
        }

        public readonly Expr Left;
        public readonly Token Operator;
        public readonly Expr Right;

        public override TResult Accept<TResult>(IVisitor<TResult> visitor)
        {
            return visitor.VisitBinaryExpr(this);
        }
    }
    public class VariableExpr : Expr
    {
        public VariableExpr(Token Identifier)
        {
            this.Identifier = Identifier;
        }

        public readonly Token Identifier;

        public override TResult Accept<TResult>(IVisitor<TResult> visitor)
        {
            return visitor.VisitVariableExpr(this);
        }
    }
    public class AssignmentExpr : Expr
    {
        public AssignmentExpr(Token Identifier, Expr Expression)
        {
            this.Identifier = Identifier;
            this.Expression = Expression;
        }

        public readonly Token Identifier;
        public readonly Expr Expression;

        public override TResult Accept<TResult>(IVisitor<TResult> visitor)
        {
            return visitor.VisitAssignmentExpr(this);
        }
    }
    public class LogicalExpr : Expr
    {
        public LogicalExpr(Expr Left, Token Operator, Expr Right)
        {
            this.Left = Left;
            this.Operator = Operator;
            this.Right = Right;
        }

        public readonly Expr Left;
        public readonly Token Operator;
        public readonly Expr Right;

        public override TResult Accept<TResult>(IVisitor<TResult> visitor)
        {
            return visitor.VisitLogicalExpr(this);
        }
    }
    public class CallExpr : Expr
    {
        public CallExpr(Expr Callee, Token LeftParen, List<Expr> Arguments)
        {
            this.Callee = Callee;
            this.LeftParen = LeftParen;
            this.Arguments = Arguments;
        }

        public readonly Expr Callee;
        public readonly Token LeftParen;
        public readonly List<Expr> Arguments;

        public override TResult Accept<TResult>(IVisitor<TResult> visitor)
        {
            return visitor.VisitCallExpr(this);
        }
    }

}