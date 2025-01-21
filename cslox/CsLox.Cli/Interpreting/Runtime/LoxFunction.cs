using CsLox.Cli.Parsing.Generated;

namespace CsLox.Cli.Interpreting.Runtime;
internal class LoxFunction : ICallableFunction
{
    private readonly Stmt.FunDeclarationStmt declaration;
    private readonly InterpreterEnvironment closure;
    private readonly bool isConstructor;

    public int Arity => declaration.Parameters.Count;

    public LoxFunction(Stmt.FunDeclarationStmt declaration, InterpreterEnvironment closure, bool isConstructor)
    {
        this.declaration = declaration;
        this.closure = closure;
        this.isConstructor = isConstructor;
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
            if (isConstructor)
            {
                return closure.GetThis();
            }

            return ret.Value;
        }

        if (isConstructor)
        {
            return closure.GetThis();
        }
        return null;
    }

    public LoxFunction Bind(LoxInstance instance)
    {
        var env = new InterpreterEnvironment(closure);
        env.Declare("this", instance);
        return new LoxFunction(declaration, env, isConstructor);
    }

    public override string ToString()
    {
        return $"<fn {declaration.Identifier.Text}>";
    }
}
