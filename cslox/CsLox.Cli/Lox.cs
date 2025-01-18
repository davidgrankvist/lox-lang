using CsLox.Cli.Errors;
using CsLox.Cli.Interpreting;
using CsLox.Cli.Parsing;
using CsLox.Cli.Parsing.Generated;

namespace CsLox.Cli.Scanning;
internal class Lox
{
    private readonly Reporter reporter;
    private readonly Scanner scanner;
    private readonly Parser parser;
    private readonly Interpreter interpreter;

    public Lox()
    {
        reporter = new Reporter();
        scanner = new Scanner(reporter);
        parser = new Parser(reporter);
        interpreter = new Interpreter(reporter);
    }

    public void Run(string program)
    {
        var tokens = scanner.Scan(program);

        if (reporter.HasError)
        {
            reporter.Output();
            return;
        }

        Stmt? ast = null;
        if (!reporter.HasError)
        {
            ast = parser.Parse(tokens);
        }

        if (!reporter.HasError && ast != null)
        {
            interpreter.Run(ast);
        }

        if (reporter.HasError)
        {
            reporter.Output();
        }
    }
}
