
using CsLox.Cli.Scanning;

namespace CsLox.Cli.Interpreting.Runtime;
internal class LoxClass : ICallableFunction
{
    public Token Identifier { get; }
    private readonly LoxClass superClass;
    private readonly Dictionary<string, LoxFunction> methods;

    public LoxClass(Token identifier, LoxClass superClass, Dictionary<string, LoxFunction> methods)
    {
        Identifier = identifier;
        this.superClass = superClass;
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
        LoxFunction result = null;
        if (methods.TryGetValue(name, out var method))
        {
            result = method;
        }
        else if (superClass != null)
        {
            result = superClass.FindMethod(name);
        }
        return result;
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
