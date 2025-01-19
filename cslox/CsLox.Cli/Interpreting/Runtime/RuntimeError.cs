using CsLox.Cli.Scanning;

namespace CsLox.Cli.Interpreting.Runtime;

internal class RuntimeError : Exception
{
    public readonly Token Token;

    public RuntimeError(Token token, string message) : base(message)
    {
        Token = token;
    }
}
