using System.Globalization;

using CsLox.Cli.Errors;

namespace CsLox.Cli.Scanning;
internal class Scanner
{
    private readonly Reporter reporter;

    private string program;
    private char[] chars;
    private readonly List<Token> tokens = [];

    private int current;
    private int line = 1;
    private int column = 1;
    private int tokenStart;

    private static readonly Dictionary<string, TokenType> KeyWords = new Dictionary<string, TokenType>
    {
        {  "true", TokenType.True },
        {  "false", TokenType.False },
        {  "and", TokenType.And },
        {  "or", TokenType.Or },
        {  "nil", TokenType.Nil },
        {  "var", TokenType.Var },
        {  "fun", TokenType.Fun },
        {  "class", TokenType.Class },
        {  "print", TokenType.Print },
    };

    public Scanner(Reporter reporter)
    {
        this.reporter = reporter;
    }

    public List<Token> Scan(string program)
    {
        current = 0;
        line = 1;
        column = 1;
        this.program = program;
        chars = program.ToCharArray();
        tokens.Clear();

        while (!IsDone())
        {
            tokenStart = current;
            ScanToken();
        }

        tokens.Add(new Token(TokenType.Eof, null, line, column));
        return tokens;
    }

    private void ScanToken()
    {
        var c = Advance();
        switch (c)
        {
            // whitespace
            case ' ':
            case '\r':
            case '\t':
                break;
            case '\n':
                NextLine();
                break;
            // single character
            case '(':
                AddToken(TokenType.ParenStart);
                break;
            case ')':
                AddToken(TokenType.ParenEnd);
                break;
            case '{':
                AddToken(TokenType.CurlyStart);
                break;
            case '}':
                AddToken(TokenType.CurlyEnd);
                break;
            case '+':
                AddToken(TokenType.Plus);
                break;
            case '-':
                AddToken(TokenType.Minus);
                break;
            case '*':
                AddToken(TokenType.Star);
                break;
            case ';':
                AddToken(TokenType.Semicolon);
                break;
            case ',':
                AddToken(TokenType.Comma);
                break;
            case '.':
                AddToken(TokenType.Dot);
                break;
            // two characters
            case '!':
                AddToken(Match('=') ? TokenType.BangEqual : TokenType.Bang);
                break;
            case '=':
                AddToken(Match('=') ? TokenType.EqualEqual : TokenType.Equal);
                break;
            case '<':
                AddToken(Match('=') ? TokenType.LessEqual : TokenType.Less);
                break;
            case '>':
                AddToken(Match('=') ? TokenType.GreaterEqual : TokenType.Greater);
                break;
            case '/':
                if (Match('/'))
                {
                    ScanComment();
                }
                else
                {
                    AddToken(TokenType.Slash);
                }
                break;
            // multiple characters
            case '"':
                ScanString();
                break;
            default:
                if (IsDigit(c))
                {
                    ScanNumber();
                }
                else if (IsAlpha(c))
                {
                    ScanKeywordOrIdentifier();
                }
                else
                {
                    reporter.Error(line, column, $"Unexpected character: {c}");
                }
                break;

        }
    }

    private void AddToken(TokenType tokenType, object? value = null)
    {
        tokens.Add(new Token(tokenType, ScanRawToken(), line, column, value));
    }

    private void ScanComment()
    {
        while (Peek() != '\n' && !IsDone())
        {
            Advance();
        }
    }

    private void ScanString()
    {
        while (Peek() != '"' && !IsDone())
        {
            var c = Peek();
            if (Peek() == '\n')
            {
                NextLine();
            }
            Advance();
        }

        if (IsDone())
        {
            var c = Peek();
            reporter.Error(line, column, "Unterminated string.");
            return;
        }

        Advance();

        var str = program.Substring(tokenStart + 1, current - tokenStart - 2);
        AddToken(TokenType.String, str);
    }

    private void ScanNumber()
    {
        while (IsDigit(Peek()))
        {
            Advance();
        }

        if (Peek() == '.' && IsDigit(PeekNext()))
        {
            Advance();

            while (IsDigit(Peek()))
            {
                Advance();
            }
        }

        var num = double.Parse(ScanRawToken(), CultureInfo.InvariantCulture);
        AddToken(TokenType.Number, num);
    }

    private string ScanRawToken()
    {
        return program.Substring(tokenStart, current - tokenStart);
    }

    private void ScanKeywordOrIdentifier()
    {
        while (IsAlphaNumeric(Peek()))
        {
            Advance();
        }

        var id = ScanRawToken();
        TokenType tokenType;
        if (KeyWords.TryGetValue(id, out var keyword))
        {
            tokenType = keyword;
        }
        else
        {
            tokenType = TokenType.Identifier;
        }
        AddToken(tokenType, id);

    }

    private bool IsDigit(char c)
    {
        return c >= '0' && c <= '9';
    }

    private bool IsAlpha(char c)
    {
        return c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z';
    }

    private bool IsAlphaNumeric(char c)
    {
        return IsDigit(c) || IsAlpha(c);
    }

    private char Advance()
    {
        column++;
        return chars[current++];
    }

    private bool Match(char expected)
    {
        if (IsDone())
        {
            return false;
        }
        if (Peek() != expected)
        {
            return false;
        }

        Advance();
        return true;
    }

    private char Peek()
    {
        return current >= chars.Length ? '\0' : chars[current];
    }

    private char PeekNext()
    {
        return current + 1 >= chars.Length ? '\0' : chars[current + 1];
    }

    private bool IsDone()
    {
        return current >= chars.Length;
    }

    private void NextLine()
    {
        line++;
        column = 1;
    }
}
