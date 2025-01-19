using CsLox.Cli.Parsing.Generated;

namespace CsLox.Cli.Interpreting.Runtime;
internal class LoxFunction : ICallableFunction
{
    private readonly Stmt.FunDeclarationStmt declaration;
    private readonly InterpreterEnvironment closure;

    public int Arity => declaration.Parameters.Count;

    public LoxFunction(Stmt.FunDeclarationStmt declaration, InterpreterEnvironment closure)
    {
        this.declaration = declaration;
        this.closure = closure;
    }

    public object Call(Interpreter interpreter, List<object> arguments)
    {
        var env = new InterpreterEnvironment(closure);
        for (var i = 0; i < arguments.Count; i++)
        {
            var parameter = declaration.Parameters[i];
            var argument = arguments[i];
            env.Declare(parameter.Text, argument);
        }

        try
        {
            interpreter.ExecuteBlock(declaration.Body, env);
        } catch (Return ret)
        {
            return ret.Value;
        }

        return null;
    }
}
