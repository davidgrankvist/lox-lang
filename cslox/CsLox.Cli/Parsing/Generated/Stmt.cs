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
        TResult VisitIfStmt(IfStmt stmt);
        TResult VisitWhileStmt(WhileStmt stmt);
        TResult VisitFunDeclarationStmt(FunDeclarationStmt stmt);
        TResult VisitReturnStmt(ReturnStmt stmt);
        TResult VisitClassStmt(ClassStmt stmt);
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
    public class IfStmt : Stmt
    {
        public IfStmt(Expr Condtition, Stmt IfSt, Stmt ElseSt)
        {
            this.Condtition = Condtition;
            this.IfSt = IfSt;
            this.ElseSt = ElseSt;
        }

        public readonly Expr Condtition;
        public readonly Stmt IfSt;
        public readonly Stmt ElseSt;

        public override TResult Accept<TResult>(IVisitor<TResult> visitor)
        {
            return visitor.VisitIfStmt(this);
        }
    }
    public class WhileStmt : Stmt
    {
        public WhileStmt(Expr Condtition, Stmt Body)
        {
            this.Condtition = Condtition;
            this.Body = Body;
        }

        public readonly Expr Condtition;
        public readonly Stmt Body;

        public override TResult Accept<TResult>(IVisitor<TResult> visitor)
        {
            return visitor.VisitWhileStmt(this);
        }
    }
    public class FunDeclarationStmt : Stmt
    {
        public FunDeclarationStmt(Token Identifier, List<Token> Parameters, List<Stmt> Body)
        {
            this.Identifier = Identifier;
            this.Parameters = Parameters;
            this.Body = Body;
        }

        public readonly Token Identifier;
        public readonly List<Token> Parameters;
        public readonly List<Stmt> Body;

        public override TResult Accept<TResult>(IVisitor<TResult> visitor)
        {
            return visitor.VisitFunDeclarationStmt(this);
        }
    }
    public class ReturnStmt : Stmt
    {
        public ReturnStmt(Token KeywordToken, Expr Expression)
        {
            this.KeywordToken = KeywordToken;
            this.Expression = Expression;
        }

        public readonly Token KeywordToken;
        public readonly Expr Expression;

        public override TResult Accept<TResult>(IVisitor<TResult> visitor)
        {
            return visitor.VisitReturnStmt(this);
        }
    }
    public class ClassStmt : Stmt
    {
        public ClassStmt(Token Identifier, List<Stmt.FunDeclarationStmt> Methods)
        {
            this.Identifier = Identifier;
            this.Methods = Methods;
        }

        public readonly Token Identifier;
        public readonly List<Stmt.FunDeclarationStmt> Methods;

        public override TResult Accept<TResult>(IVisitor<TResult> visitor)
        {
            return visitor.VisitClassStmt(this);
        }
    }

}