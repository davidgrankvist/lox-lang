namespace CsLox.Cli.Interpreting.Runtime;
internal interface ICallableFunction
{
    int Arity { get; }

    object Call(Interpreter interpreter, List<object> arguments);
}
