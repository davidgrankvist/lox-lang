using System.Globalization;

using CsLox.Cli.Errors;
using CsLox.Cli.Parsing.Generated;
using CsLox.Cli.Scanning;

namespace CsLox.Cli.Interpreting;
internal class Interpreter : Expr.IVisitor<object>, Stmt.IVisitor<object>
{
    private readonly Reporter reporter;
    private readonly bool debugMode;
    private InterpreterEnvironment environment;

    public Interpreter(Reporter reporter, bool debugMode = false)
    {
        this.reporter = reporter;
        this.debugMode = debugMode;
        environment = new InterpreterEnvironment();
    }

    public void Run(Stmt ast)
    {
        if (debugMode)
        {
            var printer = new AstPrinter();
            printer.Print(ast);
        }

        try
        {
            VisitStmt(ast);
        }
        catch (Exception ex)
        {
            if (ex is RuntimeError re)
            {
                reporter.Error(re.Token, re.Message);
            }
            else
            {
                reporter.Error(new Token(TokenType.Eof, "", 1, 1), "Unexpected runtime error: " + ex.Message);
            }
        }
    }

    private object VisitExpr(Expr expr)
    {
        return expr.Accept(this);
    }

    private object VisitStmt(Stmt stmt)
    {
        return stmt.Accept(this);
    }

    public object VisitBinaryExpr(Expr.BinaryExpr expr)
    {
        object result = null;

        var left = VisitExpr(expr.Left);
        var right = VisitExpr(expr.Right);
        var op = expr.Operator;
        switch(op.Type)
        {
            // numbers - operators
            case TokenType.Plus:
                AssertAreNumberOperands(op, left, right);
                result = (double)left + (double)right;
                break;
            case TokenType.Minus:
                AssertAreNumberOperands(op, left, right);
                result = (double)left - (double)right;
                break;
            case TokenType.Star:
                AssertAreNumberOperands(op, left, right);
                result = (double)left * (double)right;
                break;
            case TokenType.Slash:
                AssertAreNumberOperands(op, left, right);
                result = (double)left / (double)right;
                break;
            // numbers - conditions
            case TokenType.Greater:
                AssertAreNumberOperands(op, left, right);
                result = (double)left > (double)right;
                break;
            case TokenType.GreaterEqual:
                AssertAreNumberOperands(op, left, right);
                result = (double)left >= (double)right;
                break;
            case TokenType.Less:
                AssertAreNumberOperands(op, left, right);
                result = (double)left < (double)right;
                break;
            case TokenType.LessEqual:
                AssertAreNumberOperands(op, left, right);
                result = (double)left <= (double)right;
                break;
            // equality
            case TokenType.EqualEqual:
                result = AreEqual(left, right);
                break;
            case TokenType.BangEqual:
                result = !AreEqual(left, right);
                break;
            default:
                throw new RuntimeError(op, "Unsupported binary operator");
        }

        return result;
    }

    public object VisitExpressionStmt(Stmt.ExpressionStmt stmt)
    {
        return VisitExpr(stmt.Expression);
    }

    public object VisitGroupExpr(Expr.GroupExpr expr)
    {
        return VisitExpr(expr.Expression);
    }

    public object VisitLiteralExpr(Expr.LiteralExpr expr)
    {
        return expr.Value;
    }

    public object VisitProgramStmt(Stmt.ProgramStmt stmt)
    {
        foreach (var st in stmt.Statements)
        {
            VisitStmt(st);
        }

        return null;
    }

    public object VisitUnaryExpr(Expr.UnaryExpr expr)
    {
        object result = null;
        var ev = VisitExpr(expr.Expression);
        switch (expr.Operator.Type)
        {
            case TokenType.Bang:
                result = !IsTruthy(ev);
                break;
            case TokenType.Minus:
                AssertIsNumberOperand(expr.Operator, ev);
                result = -(double)ev;
                break;
            default:
                throw new RuntimeError(expr.Operator, "Unsupported unary operator");
        }

        return result;
    }

    public object VisitPrintStmt(Stmt.PrintStmt stmt)
    {
        var toPrint = Stringify(VisitExpr(stmt.Expression));
        Console.WriteLine(toPrint);

        return null;
    }

    public object VisitDeclarationStmt(Stmt.DeclarationStmt stmt)
    {
        var ev = stmt.Expression == null ? null : VisitExpr(stmt.Expression);
        environment.Declare(stmt.Identifier.Text, ev);

        return ev;
    }

    public object VisitAssignmentExpr(Expr.AssignmentExpr expr)
    {
        var ev = VisitExpr(expr.Expression);
        environment.Assign(expr.Identifier, ev);

        return ev;
    }

    public object VisitVariableExpr(Expr.VariableExpr expr)
    {
        return environment.Get(expr.Identifier);
    }

    public object VisitBlockStmt(Stmt.BlockStmt stmt)
    {
        environment = new InterpreterEnvironment(environment);

        foreach (var st in stmt.Statements)
        {
            VisitStmt(st);
        }

        environment = environment.Parent;
        return null;
    }

    public object VisitIfStmt(Stmt.IfStmt stmt)
    {
        var c = VisitExpr(stmt.Condtition);
        if (IsTruthy(c))
        {
            VisitStmt(stmt.IfSt);
        }
        else if (stmt.ElseSt != null)
        {
            VisitStmt(stmt.ElseSt);
        }

        return null;
    }

    public object VisitLogicalExpr(Expr.LogicalExpr expr)
    {
        object result = null;

        var left = VisitExpr(expr.Left);
        var op = expr.Operator;

        if (op.Type == TokenType.Or)
        {
            if (IsTruthy(left))
            {
                return left;
            }
        }
        else
        {
            if (!IsTruthy(left))
            {
                return left;
            }
        }

        var right = VisitExpr(expr.Right);
        return right;
    }

    private void AssertIsNumberOperand(Token op, object operand)
    {
        if (operand is double)
        {
            return;
        }

        throw new RuntimeError(op, "Operand must be a number");
    }

    private void AssertAreNumberOperands(Token op, object left, object right)
    {
        if (left is double && right is double)
        {
            return;
        }

        throw new RuntimeError(op, "Operands must be numbers");
    }

    private bool IsTruthy(object obj)
    {
        if (obj == null)
        {
            return false;
        }

        if (obj is bool b)
        {
            return b;
        }

        return true;
    }

    private bool AreEqual(object a, object b)
    {
        if (a == null && b == null)
        {
            return true;
        }

        if (a == null)
        {
            return false;
        }

        return a.Equals(b);
    }

    private string Stringify(object a)
    {
        if (a == null)
        {
            return "nil";
        }
        else if (a is double d)
        {
            return d.ToString(CultureInfo.InvariantCulture);
        }
        else if (a is bool b)
        {
            return b.ToString().ToLower();
        }
        else 
        {
            return a.ToString();
        }
    }
}
