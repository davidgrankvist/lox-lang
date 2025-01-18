using CsLox.Cli.Errors;
using CsLox.Cli.Parsing.Generated;
using CsLox.Cli.Scanning;

namespace CsLox.Cli.Parsing;
internal class Parser
{
    private readonly Reporter reporter;

    private List<Token> tokens;
    private int current = 0;

    public Parser(Reporter reporter)
    {
        this.reporter = reporter;
    }

    public Stmt? Parse(List<Token> tokens)
    {
        this.tokens = tokens;
        current = 0;

        if (tokens.Count == 0)
        {
            return null;
        }

        Stmt? ast = null;
        try
        {
            ast = ParseProgram();
        }
        catch (Exception ex)
        {
            if (ex is not ParseError)
            {
                reporter.Error(current >= tokens.Count ? Previous() : Peek(), "Unexpected parsing error: " + ex.Message);
            }
        }

        return ast;
    }

    private Stmt ParseProgram()
    {
        List<Stmt> stmts = [];

        while (!IsDone())
        {
            var stmt = ParseStatement();
            stmts.Add(stmt);
        }

        return new Stmt.ProgramStmt(stmts);
    }


    private Stmt ParseStatement()
    {
        if (Match(TokenType.Print))
        {
            var expr = ParseExpression();
            Consume(TokenType.Semicolon, "Expected ';'");
            return new Stmt.PrintStmt(expr);
        }

        return ParseExpressionStatement();
    }

    private Stmt ParseExpressionStatement()
    {
        var expr = ParseExpression();

        Consume(TokenType.Semicolon, "Expected ';'");

        return new Stmt.ExpressionStmt(expr);
    }

    private Expr ParseExpression()
    {
        return ParseBinary();
    }

    private Expr ParseBinary()
    {
        return ParseEquality();
    }

    private Expr ParseEquality()
    {
        var expr = ParseComparison();

        while (Match(TokenType.EqualEqual, TokenType.BangEqual))
        {
            var op = Previous();
            var otherExpr = ParseComparison();
            expr = new Expr.BinaryExpr(expr, op, otherExpr);
        }

        return expr;
    }

    private Expr ParseComparison()
    {
        var expr = ParseTerm();

        while (Match(TokenType.Greater, TokenType.GreaterEqual, TokenType.Less, TokenType.LessEqual))
        {
            var op = Previous();
            var otherExpr = ParseTerm();
            expr = new Expr.BinaryExpr(expr, op, otherExpr);
        }

        return expr;
    }

    private Expr ParseTerm()
    {
        var expr = ParseFactor();

        while (Match(TokenType.Plus, TokenType.Minus))
        {
            var op = Previous();
            var otherExpr = ParseFactor();
            expr = new Expr.BinaryExpr(expr, op, otherExpr);
        }

        return expr;
    }

    private Expr ParseFactor()
    {
        var expr = ParseUnary();

        while (Match(TokenType.Star, TokenType.Slash))
        {
            var op = Previous();
            var otherExpr = ParseUnary();
            expr = new Expr.BinaryExpr(expr, op, otherExpr);
        }

        return expr;
    }
    private Expr ParseUnary()
    {
        if (Match(TokenType.Bang, TokenType.Minus))
        {
            var op = Previous();
            var expr = ParseUnary();
            return new Expr.UnaryExpr(op, expr);
        }

        return ParsePrimary();
    }

    private Expr ParsePrimary()
    {
        if (Match(TokenType.Nil))
        {
            return new Expr.LiteralExpr(null);
        }

        if (Match(TokenType.True))
        {
            return new Expr.LiteralExpr(true);
        }

        if (Match(TokenType.False))
        {
            return new Expr.LiteralExpr(false);
        }

        if (Match(TokenType.String, TokenType.Number))
        {
            var literal = Previous();
            return new Expr.LiteralExpr(literal.Value);
        }

        if (Match(TokenType.ParenStart))
        {
            var expr = ParseExpression();
            Consume(TokenType.ParenEnd, "Expected ')'");
            return new Expr.GroupExpr(expr);
        }

        throw ReportAndCreateError(Peek(), "Unexpected expression");
    }

    private bool IsDone()
    {
        return current >= tokens.Count || Peek().Type == TokenType.Eof;
    }

    private Token Peek()
    {
        return tokens[current];
    }

    private Token Previous()
    {
        return tokens[current - 1];
    }

    private Token Advance()
    {
        if (IsDone())
        {
            return Previous();
        }
        return tokens[current++];
    }

    private bool Check(TokenType type)
    {
        return !IsDone() && Peek().Type == type;
    }

    private bool Match(params TokenType[] alternatives)
    {
        foreach (var type in alternatives)
        {
            if (Check(type))
            {
                Advance();
                return true;
            }
        }

        return false;
    }

    private Token Consume(TokenType type, string message)
    {
        if (Check(type))
        {
            return Advance();
        }

        throw ReportAndCreateError(Peek(), message);
    }

    private ParseError ReportAndCreateError(Token token, string message)
    {
        reporter.Error(token, message);
        return new ParseError();
    }

    private class ParseError : Exception
    {
    }
}
