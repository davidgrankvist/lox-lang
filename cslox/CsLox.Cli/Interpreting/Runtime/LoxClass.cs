
using CsLox.Cli.Scanning;

namespace CsLox.Cli.Interpreting.Runtime;
internal class LoxClass : ICallableFunction
{
    public Token Identifier { get; }
    private readonly Dictionary<string, LoxFunction> methods;

    public LoxClass(Token identifier, Dictionary<string, LoxFunction> methods)
    {
        Identifier = identifier;
        this.methods = methods;
    }

    public int Arity { get
        {
            var constructor = GetConstructor();
            if (constructor == null)
            {
                return 0;
            }
            else
            {
                return constructor.Arity;
            }
        }
    }

    public object Call(Interpreter interpreter, List<object> arguments)
    {
        var instance = new LoxInstance(this);
        var constructor = GetConstructor();
        if (constructor != null)
        {
            constructor.Bind(instance).Call(interpreter, arguments);
        }
        return instance;
    }

    public LoxFunction FindMethod(string name)
    {
        if (methods.TryGetValue(name, out var method))
        {
            return method;
        }
        return null;
    }

    private LoxFunction GetConstructor()
    {
        return FindMethod("init");
    }

    public override string ToString()
    {
        return $"<class {Identifier.Text}>";
    }
}
