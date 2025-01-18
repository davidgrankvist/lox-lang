using CsLox.Cli.Errors;
using CsLox.Cli.Parsing.Generated;
using CsLox.Cli.Scanning;

namespace CsLox.Cli.Parsing;
internal class Parser
{
    private readonly Reporter reporter;

    public Parser(Reporter reporter)
    {
        this.reporter = reporter;
    }

    public Stmt? Parse(List<Token> tokens)
    {
        reporter.Error(0, 0, "Unable to parse anything, sorry");
        return null;
    }
}
