﻿namespace CsLox.Cli.Scanning;
public enum TokenType
{
    // literals
    Number,
    String,
    True,
    False,
    // operators
    Plus,
    Minus,
    Star,
    Slash,
    Bang,
    Equal,
    EqualEqual,
    BangEqual,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,
    // groups
    ParenStart,
    ParenEnd,
    CurlyStart,
    CurlyEnd,
    // keywords
    And,
    Or,
    Nil,
    Var,
    Fun,
    Class,
    // special
    Identifier,
    Semicolon,
    Comma,
    Dot,
    Eof,
}
