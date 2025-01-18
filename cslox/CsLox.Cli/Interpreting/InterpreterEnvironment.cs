using CsLox.Cli.Scanning;

namespace CsLox.Cli.Interpreting;
internal class InterpreterEnvironment
{
    private readonly Dictionary<string, object> variables = [];

    private InterpreterEnvironment Parent;

    public InterpreterEnvironment()
    {
    }

    public InterpreterEnvironment(InterpreterEnvironment parent)
    {
        Parent = parent;
    }

    public void Declare(string name, object value)
    {
        variables[name] = value;
    }

    public object Get(Token Identifier)
    {
        if (variables.TryGetValue(Identifier.Text, out var v))
        {
            return v;
        }
        else if (Parent != null)
        {
            return Parent.Get(Identifier);
        }
        throw new RuntimeError(Identifier, "Undefined variable");
    }
}
