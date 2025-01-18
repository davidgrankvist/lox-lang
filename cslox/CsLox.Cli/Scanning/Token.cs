namespace CsLox.Cli.Scanning;
public readonly struct Token
{
    public Token(TokenType type, string? text, int line, int column, object? value = null)
    {
        Line = line;
        Column = column;
        Text = text ?? string.Empty;
        Type = type;
        Value = value;
    }

    public readonly int Line;
    public readonly int Column;
    public readonly string Text;
    public readonly TokenType Type;
    public readonly object? Value;
}
