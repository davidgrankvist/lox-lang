// DO NOT EDIT - THIS CLASS IS AUTO-GENERATED
// See the CsLox.CodeGen project

namespace CsLox.Cli.Parsing.Generated;

public abstract class Stmt
{
    public interface IVisitor<TResult>
    {
        TResult VisitExpressionStmt(ExpressionStmt stmt);
    }

    protected abstract TResult Accept<TResult>(IVisitor<TResult> visitor);

    public class ExpressionStmt : Stmt
    {
        public ExpressionStmt(Expr Expression)
        {
            this.Expression = Expression;
        }

        public readonly Expr Expression;

        protected override TResult Accept<TResult>(IVisitor<TResult> visitor)
        {
            return visitor.VisitExpressionStmt(this);
        }
    }

}