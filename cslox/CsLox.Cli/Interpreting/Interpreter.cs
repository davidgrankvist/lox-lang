﻿using System.Globalization;

using CsLox.Cli.Errors;
using CsLox.Cli.Interpreting.Development;
using CsLox.Cli.Interpreting.Runtime;
using CsLox.Cli.Interpreting.Runtime.Native;
using CsLox.Cli.Parsing.Generated;
using CsLox.Cli.Scanning;

namespace CsLox.Cli.Interpreting;
internal class Interpreter : Expr.IVisitor<object>, Stmt.IVisitor<object>
{
    private readonly Reporter reporter;
    private readonly bool debugMode;

    private readonly InterpreterEnvironment globals;
    private InterpreterEnvironment environment;
    private readonly Dictionary<Expr, int> locals = [];

    public Interpreter(Reporter reporter, bool debugMode = false)
    {
        this.reporter = reporter;
        this.debugMode = debugMode;
        globals = new InterpreterEnvironment();
        GlobalRegistrations.Register(globals);
        environment = globals;
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
            // numbers/strings - operators
            case TokenType.Plus:
                if (left is string ls && right is string rs)
                {
                    result = ls + rs;
                }
                else if (left is double ld && right is double rd)
                {
                    result = ld + rd;
                }
                else
                {
                    throw new RuntimeError(op, "Operands must be numbers or strings");
                }
                break;
            // numbers - operators
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

        if (locals.TryGetValue(expr, out var distance))
        {
            environment.AssignAt(expr.Identifier, ev, distance);
        }
        else
        {
            globals.Assign(expr.Identifier, ev);
        }

        return ev;
    }

    public object VisitVariableExpr(Expr.VariableExpr expr)
    {
        return VisitScopedVariable(expr, expr.Identifier);
    }

    private object VisitScopedVariable(Expr expr, Token identifier)
    {
        if (locals.TryGetValue(expr, out var distance))
        {
            return environment.GetAt(identifier, distance);
        }
        else
        {
            return globals.Get(identifier);
        }
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

    public object VisitWhileStmt(Stmt.WhileStmt stmt)
    {
        object condition;
        while (IsTruthy(condition = VisitExpr(stmt.Condtition)))
        {
            VisitStmt(stmt.Body);
        }

        return null;
    }

    public object VisitCallExpr(Expr.CallExpr expr)
    {
        var callee = VisitExpr(expr.Callee);
        if (callee is not ICallableFunction fun)
        {
            throw new RuntimeError(expr.LeftParen, "Can only call functions and methods");
        }

        if (expr.Arguments.Count != fun.Arity)
        {
            throw new RuntimeError(expr.LeftParen, $"Expected {fun.Arity} arguments, but received {expr.Arguments.Count}");
        }

        var args = new List<object>();
        foreach (var arg in expr.Arguments)
        {
            args.Add(VisitExpr(arg));
        }

        return fun.Call(this, args);
    }

    public object VisitFunDeclarationStmt(Stmt.FunDeclarationStmt stmt)
    {
        var fun = new LoxFunction(stmt, environment);
        environment.Declare(stmt.Identifier.Text, fun);

        return null;
    }
    public object VisitReturnStmt(Stmt.ReturnStmt stmt)
    {
        object val = null;
        if (stmt.Expression != null)
        {
            val = VisitExpr(stmt.Expression);
        }
        throw new Return(val);
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

    internal void ExecuteBlock(List<Stmt> body, InterpreterEnvironment env)
    {
        environment = env;

        try
        {
            foreach (var st in body)
            {
                VisitStmt(st);
            }
        }
        finally
        {
            environment = env.Parent;
        }
    }

    internal void ResolveLocalVariable(Expr expr, int depth)
    {
        locals[expr] = depth;
    }
}
