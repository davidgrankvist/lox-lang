using CsLox.Cli.Parsing.Generated;

namespace CsLox.Cli.Interpreting.Development;
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

    private string Parens(string s)
    {
        return "(" + s + ")";
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

    public string VisitIfStmt(Stmt.IfStmt stmt)
    {
        var result = "";

        var cs = Parens(VisitExpr(stmt.Condtition));
        var sts = VisitStmt(stmt.IfSt);
        var ifs = "if " + cs + " " + sts;
        result += ifs;

        if (stmt.ElseSt != null)
        {
            var els = " else " + VisitStmt(stmt.ElseSt);
            result += els;
        }

        return Parens(result);
    }

    public string VisitLogicalExpr(Expr.LogicalExpr expr)
    {
        var left = VisitExpr(expr.Left);
        var right = VisitExpr(expr.Right);

        return Parens($"{expr.Operator.Text} {left} {right}");
    }

    public string VisitWhileStmt(Stmt.WhileStmt stmt)
    {
        var condition = VisitExpr(stmt.Condtition);
        var body = VisitStmt(stmt.Body);
        return Parens("while " + condition + " " + body);
    }

    public string VisitCallExpr(Expr.CallExpr expr)
    {
        var calle = VisitExpr(expr.Callee);
        var args = "";
        for (var i = 0; i < expr.Arguments.Count; i++)
        {
            var arg = expr.Arguments[i];
            args += VisitExpr(arg) + (i == expr.Arguments.Count - 1 ? "" : ", ");
        }

        return calle + Parens(args);
    }

    public string VisitFunDeclarationStmt(Stmt.FunDeclarationStmt stmt)
    {
        return VisitFunDeclarationStmtInternal(stmt);
    }

    private string VisitFunDeclarationStmtInternal(Stmt.FunDeclarationStmt stmt, bool method = false)
    {
        var prefix = method ? "" : "fun ";
        var result = prefix + stmt.Identifier.Text + " ";

        var ps = "";
        for (var i = 0; i < stmt.Parameters.Count; i++)
        {
            var p = stmt.Parameters[i];
            ps += p.Text + (i == stmt.Parameters.Count - 1 ? "" : ", ");
        }
        result += Parens(ps);

        var sts = "";
        foreach (var st in stmt.Body)
        {
            var s = " " + VisitStmt(st);
            sts += s;
        }
        result += Parens(sts);

        return result;
    }

    public string VisitReturnStmt(Stmt.ReturnStmt stmt)
    {
        return "return " + (stmt.Expression == null ? "" : VisitExpr(stmt.Expression));
    }

    public string VisitClassStmt(Stmt.ClassStmt stmt)
    {
        var result = "class " + stmt.Identifier.Text;
        if (stmt.SuperClass != null)
        {
            result += " < " + stmt.SuperClass.Identifier.Text;
        }

        foreach (var m in stmt.Methods)
        {
            var ms = VisitFunDeclarationStmtInternal(m, true);
            result += " " + ms;
        }

        return Parens(result);
    }

    public string VisitPropertyAccessExpr(Expr.PropertyAccessExpr expr)
    {
        return VisitExpr(expr.Object) + "." + expr.Identifier.Text;
    }

    public string VisitPropertyAssignmentExpr(Expr.PropertyAssignmentExpr expr)
    {
        return VisitExpr(expr.Object) + " = " + VisitExpr(expr.Value);
    }

    public string VisitThisExpr(Expr.ThisExpr expr)
    {
        return "this";
    }

    public string VisitSuperExpr(Expr.SuperExpr expr)
    {
        return "super." + expr.Method.Text;
    }
}
