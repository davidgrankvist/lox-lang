
using CsLox.Cli.Scanning;

namespace CsLox.Cli.Interpreting.Runtime;
internal class LoxInstance
{
    private readonly LoxClass clazz;
    private readonly Dictionary<string, object> fields = [];

    public LoxInstance(LoxClass clazz)
    {
        this.clazz = clazz;
    }

    public object Get(Token identifier)
    {
        if (fields.TryGetValue(identifier.Text, out var field))
        {
            return field;
        }

        var method = clazz.FindMethod(identifier.Text);
        if (method != null)
        {
            return method.Bind(this);
        }

        throw new RuntimeError(identifier, "Unable to access undefined property");
    }

    public void Set(Token identifier, object value)
    {
        fields[identifier.Text] = value;
    }

    public override string ToString()
    {
        return $"<{clazz.Identifier.Text} instance>";
    }
}
