// DO NOT EDIT - THIS CLASS IS AUTO-GENERATED
// See the CsLox.CodeGen project

using CsLox.Cli.Scanning;

namespace CsLox.Cli.Parsing.Generated;

public abstract class Stmt
{
    public interface IVisitor<TResult>
    {
        TResult VisitExpressionStmt(ExpressionStmt stmt);
        TResult VisitProgramStmt(ProgramStmt stmt);
        TResult VisitPrintStmt(PrintStmt stmt);
        TResult VisitDeclarationStmt(DeclarationStmt stmt);
        TResult VisitBlockStmt(BlockStmt stmt);
    }

    public abstract TResult Accept<TResult>(IVisitor<TResult> visitor);

    public class ExpressionStmt : Stmt
    {
        public ExpressionStmt(Expr Expression)
        {
            this.Expression = Expression;
        }

        public readonly Expr Expression;

        public override TResult Accept<TResult>(IVisitor<TResult> visitor)
        {
            return visitor.VisitExpressionStmt(this);
        }
    }
    public class ProgramStmt : Stmt
    {
        public ProgramStmt(List<Stmt> Statements)
        {
            this.Statements = Statements;
        }

        public readonly List<Stmt> Statements;

        public override TResult Accept<TResult>(IVisitor<TResult> visitor)
        {
            return visitor.VisitProgramStmt(this);
        }
    }
    public class PrintStmt : Stmt
    {
        public PrintStmt(Expr Expression)
        {
            this.Expression = Expression;
        }

        public readonly Expr Expression;

        public override TResult Accept<TResult>(IVisitor<TResult> visitor)
        {
            return visitor.VisitPrintStmt(this);
        }
    }
    public class DeclarationStmt : Stmt
    {
        public DeclarationStmt(Token Identifier, Expr Expression)
        {
            this.Identifier = Identifier;
            this.Expression = Expression;
        }

        public readonly Token Identifier;
        public readonly Expr Expression;

        public override TResult Accept<TResult>(IVisitor<TResult> visitor)
        {
            return visitor.VisitDeclarationStmt(this);
        }
    }
    public class BlockStmt : Stmt
    {
        public BlockStmt(List<Stmt> Statements)
        {
            this.Statements = Statements;
        }

        public readonly List<Stmt> Statements;

        public override TResult Accept<TResult>(IVisitor<TResult> visitor)
        {
            return visitor.VisitBlockStmt(this);
        }
    }

}