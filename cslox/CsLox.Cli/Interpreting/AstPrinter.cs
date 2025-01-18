using CsLox.Cli.Parsing.Generated;

namespace CsLox.Cli.Interpreting;
internal class AstPrinter : Expr.IVisitor<string>, Stmt.IVisitor<string>
{
    private const string Indent = "    ";

    private string VisitExpr(Expr expr)
    {
        return expr.Accept(this);
    }

    private string VisitStmt(Stmt stmt)
    {
        return stmt.Accept(this);
    }

    public void Print(Stmt ast)
    {
        var s = VisitStmt(ast);
        Console.Write(s);
    }

    public string VisitBinaryExpr(Expr.BinaryExpr expr)
    {
        var left = VisitExpr(expr.Left);
        var right = VisitExpr(expr.Right);

        return Parens($"{expr.Operator.Text} {left} {right}");
    }

    public string VisitExpressionStmt(Stmt.ExpressionStmt stmt)
    {
        var es = VisitExpr(stmt.Expression);
        return Parens("stmt " + es);
    }

    public string VisitGroupExpr(Expr.GroupExpr expr)
    {
        var es = VisitExpr(expr.Expression);
        return Parens(es);
    }

    public string VisitLiteralExpr(Expr.LiteralExpr expr)
    {
        return expr.Value.ToString();
    }

    public string VisitProgramStmt(Stmt.ProgramStmt stmt)
    {
        var result = "";
        foreach (var st in stmt.Statements)
        {
            var s = Environment.NewLine + Indent + VisitStmt(st);
            result += s;
        }
        return result + Environment.NewLine;
    }

    public string VisitUnaryExpr(Expr.UnaryExpr expr)
    {
        var es = VisitExpr(expr.Expression);
        return Parens(expr.Operator.Text + " " + es);
    }

    public string VisitPrintStmt(Stmt.PrintStmt stmt)
    {
        return Parens("print " + VisitExpr(stmt.Expression));
    }

    public string VisitDeclarationStmt(Stmt.DeclarationStmt stmt)
    {
        var es = VisitExpr(stmt.Expression);
        return Parens("var " + stmt.Identifier.Text + " = " + es);
    }

    public string VisitVariableExpr(Expr.VariableExpr expr)
    {
        return expr.Identifier.Text;
    }

    public string VisitAssignmentExpr(Expr.AssignmentExpr expr)
    {
        var es = VisitExpr(expr.Expression);
        return Parens(expr.Identifier.Text + " = " + es);
    }

    public string VisitBlockStmt(Stmt.BlockStmt stmt)
    {
        var result = "block";
        foreach (var st in stmt.Statements)
        {
            var s = " " + VisitStmt(st);
            result += s;
        }
        return Parens(result);
    }

    private string Parens(string s)
    {
        return "(" + s + ")";
    }
}
