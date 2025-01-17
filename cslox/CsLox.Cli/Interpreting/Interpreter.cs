using CsLox.Cli.Errors;
using CsLox.Cli.Parsing;

namespace CsLox.Cli.Interpreting;
internal class Interpreter
{
    private readonly Reporter reporter;

    public Interpreter(Reporter reporter)
    {
        this.reporter = reporter;
    }

    public void Run(AstNode ast)
    {

    }
}
