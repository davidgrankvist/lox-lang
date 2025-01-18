// DO NOT EDIT - THIS CLASS IS AUTO-GENERATED
// See the CsLox.CodeGen project

namespace CsLox.Cli.Parsing.Generated;

public abstract class Expr
{
    public interface IVisitor<TResult>
    {
        TResult VisitLiteralExpr(LiteralExpr expr);
    }

    protected abstract TResult Accept<TResult>(IVisitor<TResult> visitor);

    public class LiteralExpr : Expr
    {
        public LiteralExpr(object Value)
        {
            this.Value = Value;
        }

        public readonly object Value;

        protected override TResult Accept<TResult>(IVisitor<TResult> visitor)
        {
            return visitor.VisitLiteralExpr(this);
        }
    }

}