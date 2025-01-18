using CsLox.Cli.Errors;
using CsLox.Cli.Parsing.Generated;

namespace CsLox.Cli.Interpreting;
internal class Interpreter
{
    private readonly Reporter reporter;

    public Interpreter(Reporter reporter)
    {
        this.reporter = reporter;
    }

    public void Run(Stmt ast)
    {
        reporter.Error(0, 0, "Unable to interpret anything, sorry");
    }
}
