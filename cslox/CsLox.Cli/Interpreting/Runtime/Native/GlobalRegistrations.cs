namespace CsLox.Cli.Interpreting.Runtime.Native;

internal static class GlobalRegistrations
{
    public static void Register(InterpreterEnvironment environment)
    {
        List<INativeFunction> globalFunctions = [
            new ClockFunction(),
        ];
        foreach (var fun in globalFunctions)
        {
            environment.Declare(fun.Name, fun);
        }
    }
}
