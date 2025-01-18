using CsLox.Cli.Scanning;

namespace CsLox.Cli.Errors;
internal class Reporter
{
    private readonly List<(Token, string)> errors = [];

    public bool HasError => errors.Count > 0;

    public void Error(Token token, string message)
    {
        errors.Add((token, message));
    }

    public void Error(int line, int column, string message)
    {
        // dummy token
        Error(new Token(TokenType.Eof, "", line, column), message);
    }

    public void Reset()
    {
        errors.Clear();
    }

    public void Output()
    {
        foreach (var (token, message) in errors)
        {
            Console.Error.WriteLine($"Error at {token.Line},{token.Column} {token.Text} - {message}");
        }
        Reset();
    }
}
