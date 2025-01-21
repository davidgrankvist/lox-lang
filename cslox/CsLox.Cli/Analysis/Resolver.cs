using CsLox.Cli.Errors;
using CsLox.Cli.Interpreting;
using CsLox.Cli.Parsing.Generated;
using CsLox.Cli.Scanning;

namespace CsLox.Cli.Analysis;
internal class Resolver : Expr.IVisitor<object>, Stmt.IVisitor<object>
{
    private readonly Reporter reporter;
    private readonly Interpreter interpreter;

    private readonly Stack<Dictionary<string, bool>> scopes = [];
    private FunctionType currentFunction = FunctionType.None;
    private ClassType currentClass = ClassType.None;

    public Resolver(Reporter reporter, Interpreter interpreter)
    {
        this.reporter = reporter;
        this.interpreter = interpreter;
    }

    public void Resolve(Stmt ast)
    {
        VisitStatement(ast);
    }

    private object VisitExpression(Expr expr)
    {
        return expr.Accept(this);
    }

    private object VisitStatement(Stmt stmt)
    {
        return stmt.Accept(this);
    }

    private void VisitStatements(List<Stmt> stmts)
    {
        foreach (var stmt in stmts)
        {
            VisitStatement(stmt);
        }
    }

    private void BeginScope()
    {
        scopes.Push(new Dictionary<string, bool>());
    }

    private void EndScope()
    {
        scopes.Pop();
    }

    private void Define(Token identifier)
    {
        if (scopes.Count == 0)
        {
            return;
        }

        scopes.Peek()[identifier.Text] = true;
    }

    private void Declare(Token identifier)
    {
        if (scopes.Count == 0)
        {
            return;
        }

        if (scopes.Peek().ContainsKey(identifier.Text))
        {
            reporter.Error(identifier, "A variable with the same name exists in this scope");
        }

        scopes.Peek()[identifier.Text] = false;
    }

    public object VisitAssignmentExpr(Expr.AssignmentExpr expr)
    {
        VisitExpression(expr.Expression);
        VisitLocalVariable(expr, expr.Identifier.Text);
        return null;
    }

    public object VisitBinaryExpr(Expr.BinaryExpr expr)
    {
        VisitExpression(expr.Left);
        VisitExpression(expr.Right);
        return null;
    }

    public object VisitBlockStmt(Stmt.BlockStmt stmt)
    {
        BeginScope();
        foreach (var st in stmt.Statements)
        {
            VisitStatement(st);
        }
        EndScope();
        return null;
    }

    public object VisitCallExpr(Expr.CallExpr expr)
    {
        VisitExpression(expr.Callee);
        foreach (var arg in expr.Arguments)
        {
            VisitExpression(arg);
        }
        return null;
    }

    public object VisitDeclarationStmt(Stmt.DeclarationStmt stmt)
    {
        Declare(stmt.Identifier);
        if (stmt.Expression != null)
        {
            VisitExpression(stmt.Expression);
        }
        Define(stmt.Identifier);

        return null;
    }

    public object VisitExpressionStmt(Stmt.ExpressionStmt stmt)
    {
        VisitExpression(stmt.Expression);
        return null;
    }

    public object VisitFunDeclarationStmt(Stmt.FunDeclarationStmt stmt)
    {
        Declare(stmt.Identifier);
        Define(stmt.Identifier);
        VisitFunction(stmt, FunctionType.Function);
        return null;
    }

    private void VisitFunction(Stmt.FunDeclarationStmt stmt, FunctionType ft)
    {
        FunctionType curr = currentFunction;
        currentFunction = ft;
        BeginScope();
        foreach (var token in stmt.Parameters)
        {
            Declare(token);
            Define(token);
        }
        VisitStatements(stmt.Body);
        EndScope();
        currentFunction = curr;
    }

    public object VisitGroupExpr(Expr.GroupExpr expr)
    {
        VisitExpression(expr.Expression);
        return null;
    }

    public object VisitIfStmt(Stmt.IfStmt stmt)
    {
        VisitExpression(stmt.Condtition);
        VisitStatement(stmt.IfSt);
        if (stmt.ElseSt != null)
        {
            VisitStatement(stmt.ElseSt);
        }
        return null;
    }

    public object VisitLiteralExpr(Expr.LiteralExpr expr)
    {
        return null;
    }

    public object VisitLogicalExpr(Expr.LogicalExpr expr)
    {
        VisitExpression(expr.Left);
        VisitExpression(expr.Right);
        return null;
    }

    public object VisitPrintStmt(Stmt.PrintStmt stmt)
    {
        VisitExpression(stmt.Expression);
        return null;
    }

    public object VisitProgramStmt(Stmt.ProgramStmt stmt)
    {
        foreach (var st in stmt.Statements)
        {
            VisitStatement(st);
        }
        return null;
    }

    public object VisitReturnStmt(Stmt.ReturnStmt stmt)
    {
        if (currentFunction == FunctionType.None)
        {
            reporter.Error(stmt.KeywordToken, "Return is only allowed inside functions");
        }
        else if (stmt.Expression != null)
        {
            if (currentFunction == FunctionType.Initializer)
            {
                reporter.Error(stmt.KeywordToken, "Can't return a value from the init method");
            }
            VisitExpression(stmt.Expression);
        }
        return null;
    }

    public object VisitUnaryExpr(Expr.UnaryExpr expr)
    {
        VisitExpression(expr.Expression);
        return null;
    }

    public object VisitVariableExpr(Expr.VariableExpr expr)
    {
        if (scopes.Count > 0 && scopes.Peek().TryGetValue(expr.Identifier.Text, out var isDefined) && !isDefined)
        {
            reporter.Error(expr.Identifier, "Unable to read a variable in its initializer");
        }
        VisitLocalVariable(expr, expr.Identifier.Text);
        return null;
    }

    private void VisitLocalVariable(Expr expr, string name)
    {
        var i = scopes.Count - 1;
        foreach (var scope in scopes)
        {
            if (scope.ContainsKey(name))
            {
                interpreter.ResolveLocalVariable(expr, scopes.Count - 1 - i);
                return;
            }
            i--;
        }
    }

    public object VisitWhileStmt(Stmt.WhileStmt stmt)
    {
        VisitExpression(stmt.Condtition);
        BeginScope();
        VisitStatement(stmt.Body);
        EndScope();
        return null;
    }

    public object VisitClassStmt(Stmt.ClassStmt stmt)
    {
        var currClass = currentClass;
        currentClass = ClassType.Class;
        Declare(stmt.Identifier);
        Define(stmt.Identifier);

        BeginScope();
        scopes.Peek().Add("this", true);

        foreach (var st in stmt.Methods)
        {
            var fun = st.Identifier.Text == "init" ? FunctionType.Initializer : FunctionType.Method;
            VisitFunction(st, fun);
        }

        EndScope();
        currentClass = currClass;
        return null;
    }

    public object VisitPropertyAccessExpr(Expr.PropertyAccessExpr expr)
    {
        VisitExpression(expr.Object);
        return null;
    }

    public object VisitPropertyAssignmentExpr(Expr.PropertyAssignmentExpr expr)
    {
        VisitExpression(expr.Object);
        VisitExpression(expr.Value);
        return null;
    }

    public object VisitThisExpr(Expr.ThisExpr expr)
    {
        if (currentClass == ClassType.Class)
        {
            VisitLocalVariable(expr, expr.Keyword.Text);
        }
        else
        {
            reporter.Error(expr.Keyword, "Can only access 'this' within a class");
        }
        return null;
    }
}
