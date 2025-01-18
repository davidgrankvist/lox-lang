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

}