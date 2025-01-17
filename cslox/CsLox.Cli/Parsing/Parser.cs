using CsLox.Cli.Errors;
using CsLox.Cli.Scanning;

namespace CsLox.Cli.Parsing;
internal class Parser
{
    private readonly Reporter reporter;

    public Parser(Reporter reporter)
    {
        this.reporter = reporter;
    }

    public AstNode Parse(List<Token> tokens)
    {
        return new AstNode();
    }
}
