

using CsLox.Cli.Scanning;

namespace CsLox.Cli.Interpreting.Runtime;
internal class InterpreterEnvironment
{
    private readonly Dictionary<string, object> variables = [];

    public readonly InterpreterEnvironment Parent;

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

    public void Assign(Token Identifier, object value)
    {
        if (variables.ContainsKey(Identifier.Text))
        {
            Declare(Identifier.Text, value);
        }
        else if (Parent != null)
        {
            Parent.Assign(Identifier, value);
        }
        else
        {
            throw new RuntimeError(Identifier, "Undefined variable");
        }
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
        else
        {
            throw new RuntimeError(Identifier, "Undefined identifier");
        }
    }

    public object GetAt(Token identifier, int distance)
    {
        return Ancestor(distance).Get(identifier);
    }

    public object GetThis()
    {
        return variables["this"];
    }

    public object GetThisAt(int distance)
    {
        return Ancestor(distance).GetThis();
    }

    private InterpreterEnvironment Ancestor(int distance)
    {
        var current = this;
        for (var i = 0; i < distance; i++)
        {
            current = current.Parent;
        }

        return current;
    }

    public void AssignAt(Token identifier, object ev, int distance)
    {
        Ancestor(distance).Assign(identifier, ev);
    }
}
