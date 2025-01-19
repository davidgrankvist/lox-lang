namespace CsLox.Cli.Interpreting.Runtime.Native;
internal class ClockFunction : INativeFunction
{
    public string Name => "clock";

    public int Arity { get; } = 0;

    public object Call(Interpreter interpreter, List<object> arguments)
    {
        return DateTime.UtcNow.Ticks / 1e7;
    }
}
