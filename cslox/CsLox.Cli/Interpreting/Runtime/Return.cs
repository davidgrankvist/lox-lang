using CsLox.Cli.Scanning;

namespace CsLox.Cli.Interpreting.Runtime;
internal class Return : Exception
{
    public readonly object Value;

    public Return(object value)
    {
        Value = value;
    }
}
