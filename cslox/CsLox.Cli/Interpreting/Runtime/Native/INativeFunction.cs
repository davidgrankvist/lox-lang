namespace CsLox.Cli.Interpreting.Runtime.Native;
internal interface INativeFunction : ICallableFunction
{
    public string Name { get; }
}
